#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>

#include "EventFSM/fsm.h"
#include "FileDownLoader.h"
#include "FileDownloaderStates.h"

static std::string protocol_delimiter = "://";
static std::string path_delimiter = "/";

HTTP_FD::HTTP_FD(std::string url) {

    this->sockfd = -1;
    this->read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);
    this->read_buffer_size = HTTP_READ_BUFFER_SIZE;
    this->current_byte = 0;
    this->file_size = 0;
    this->low_byte = 0;
    this->high_byte = 0;

    // url = "http://mirror2.internetdownloadmanager.com/idman642build20.exe";
    size_t protocol_pos = url.find(protocol_delimiter);
    size_t server_start_pos = protocol_pos + protocol_delimiter.length();
    size_t path_start_pos = url.find(path_delimiter, server_start_pos);
    this->server_name = url.substr(server_start_pos, path_start_pos - server_start_pos); // "mirror2.internetdownloadmanager.com";
    this->file_path = url.substr(path_start_pos);   // "/idman642build20.exe";
    printf ("Constructor : Server = %s   File-Path = %s\n", 
        this->server_name.c_str(), this->file_path.c_str());   
    this->fsm = NULL;
    this->connector_thread = NULL;
    this->downloader_thread = NULL;
}

HTTP_FD::~HTTP_FD() {

    assert (this->sockfd == -1);
    assert (this->read_buffer == NULL);
    assert (this->connector_thread == NULL);    
    assert (this->downloader_thread == NULL);
    printf ("HTTP FD Deleted\n");
}


static void *
server_head_connect_fn (void *arg) {
    
        HTTP_FD *fd = (HTTP_FD *)arg;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        int portno = HTTP_DEFAULT_PORT;
    
        fd->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (fd->sockfd < 0) {
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
            return NULL;
        }
    
        server = gethostbyname(fd->server_name.c_str());
        if (server == NULL) {
            printf ("Failed to resolve host-name : %s\n", fd->server_name.c_str()); 
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
            return NULL;
        }
    
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
        serv_addr.sin_port = htons(portno);

        int n_tries = HEAD_CONNECT_TRIES;

        while (n_tries && 
            (connect(fd->sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)) {
            printf ("Head Connect to Server Failed, Retrying %d...\n", n_tries);
            sleep(HEAD_CONNECT_RETRY_INTERVAL);
            n_tries--;
        }

        if (n_tries == 0) {
            printf ("Head Connect exhausted all its attempts, Could not connect to Server\n");
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_CONNECT_FAILED);
            return NULL;
        }

        efsm_fire_event (fd->fsm, DNLOAD_EVENT_CONNECT_SUCCESS);
        return NULL;
}

bool
HTTP_FD::HeadConnectServer( ) {

    assert (this->sockfd == -1);
    assert (this->connector_thread == NULL);
    assert (this->downloader_thread == NULL);
    assert (this->server_name.length() > 0); 

    this->connector_thread = (pthread_t *)calloc (1, sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create (this->connector_thread, &attr, server_head_connect_fn, (void *)this);
    return true;
}

static void * 
http_send_head_request (void *arg) {

    char head_request[1024];

	FD *fd = (FD *)arg;
	
    snprintf (head_request, sizeof (head_request), 
        "HEAD %s HTTP/1.1\r\n"
        "Host: %s\r\n\r\n", 
        fd->file_path.c_str(), fd->server_name.c_str());

	int n = write (fd->sockfd, head_request, strlen(head_request));
	if (n < 0) {
		efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
		return NULL;
	}

	char *read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);
	n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE);
	if (n < 0) {
		efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
        free (read_buffer);
		return NULL;
	}

	printf ("\nHead Response: %s\n", read_buffer);

    if (strstr (read_buffer, "HTTP/1.1 200 OK") == NULL) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        free (read_buffer);
        return NULL;
    }

    char *content_length_str = strstr(read_buffer, "Content-Length:");

    if (content_length_str == NULL) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        free (read_buffer);
        return NULL;
    }

    sscanf(content_length_str, "Content-Length: %d", &fd->file_size);
    free (read_buffer);
	efsm_fire_event (fd->fsm, DNLOAD_EVENT_FINISHED);
	return NULL;
}

/* Send HEAD GET Request in a separate thread */
void 
HTTP_FD::HeadSendGetRequest() {
    
    assert (this->sockfd != -1);
    assert (this->connector_thread == NULL);
    assert (this->downloader_thread == NULL);

    this->downloader_thread = (pthread_t *)calloc (1, sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create (this->downloader_thread, &attr, http_send_head_request, (void *)this);        
}

void 
HTTP_FD::FileDownloadConnectServer() {

    this->HeadConnectServer();
}


static void * 
http_file_download_thread_fn (void *arg) {

    char get_request[1024];

    HTTP_FD *fd = (HTTP_FD *)arg;

    snprintf(get_request, sizeof (get_request), 
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Range : bytes=%d-%d\r\n"
        "Connection: close\r\n"
        "\r\n", 
        fd->file_path.c_str(), fd->server_name.c_str(),
        fd->current_byte, fd->file_size);

    int n = write (fd->sockfd, get_request, strlen(get_request));

    if (n < 0) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
        return NULL;
    }

    char file_name[64];
    sscanf (fd->file_path.c_str(), "/%s", file_name);
    FILE *fp = fopen (file_name, "wb");

    if (fp == NULL) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        return NULL;
    }

    char *read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);

    if (fd->current_byte == 0) {

        n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE);
        if (n < 0) {
            fclose (fp);
            free(read_buffer);
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
            return NULL;
        }

        char *header_end_marker = strstr(read_buffer, "\r\n\r\n");

        if (header_end_marker == NULL) {
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
            fclose (fp);
            free(read_buffer); 
            return NULL;
        }

        int header_end = header_end_marker - read_buffer + 4;   // End of headers
        fwrite(read_buffer + header_end, 1, n - header_end, fp);
        fd->current_byte += n - header_end;
        printf ("Downloading ... \n");
        fd->ProgressBar();
    }

    /* Move the FD to the end of the file*/
    fseek (fp, 0, SEEK_END);

    while ((n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE)) > 0) {
        fwrite (read_buffer, 1, n, fp);
        fd->current_byte += n;
        fd->ProgressBar();
        if (fd->current_byte == fd->file_size) {
            printf ("\n");
            break;
        }
    }

    fclose (fp);
    free (read_buffer);

    if (n < 0) {
        printf ("\nError : File Download aborted, errno = %d\n", errno);
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        return NULL;
    }

    if (fd->current_byte != fd->file_size) {
        printf ("\nError : File Size Downloaded : %d, Actual file size : %d\n", 
            fd->current_byte, fd->file_size); 
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        return NULL;
    }

    efsm_fire_event (fd->fsm, DNLOAD_EVENT_FINISHED);
    return NULL;
}

void 
HTTP_FD::FileDownload() {

    assert (this->sockfd != -1);
    assert (this->connector_thread == NULL);
    assert (this->downloader_thread == NULL);

    this->downloader_thread = (pthread_t *)calloc (1, sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create (this->downloader_thread, &attr, http_file_download_thread_fn, (void *)this);                
}

void 
HTTP_FD::CleanupDnloadResources() {

    if (this->sockfd != -1) {
        close (this->sockfd);
        this->sockfd = -1;
    }

    if (this->connector_thread) {
        pthread_cancel(*this->connector_thread);
        pthread_join(*this->connector_thread, NULL);
        free(this->connector_thread);
        this->connector_thread = NULL;
    }

    if (this->downloader_thread) {
        pthread_cancel(*this->downloader_thread);
        pthread_join(*this->downloader_thread, NULL);
        free(this->downloader_thread);
        this->downloader_thread = NULL;
    }

    if (this->read_buffer) {
        free(this->read_buffer);
        this->read_buffer = NULL;
    }

    this->read_buffer_size = 0;
    this->current_byte = 0;
    this->file_size = 0;
    this->low_byte = 0;
    this->high_byte = 0;
}

void
HTTP_FD::AssembleChunks() {

    printf ("%s() Called ...\n", __FUNCTION__);
}

void
HTTP_FD::Cancel () {

    this->CleanupDnloadResources();
    remove (this->file_path.c_str());
}

void 
HTTP_FD::Pause() {

    assert (this->downloader_thread);
    pthread_cancel(*this->downloader_thread);
    pthread_join(*this->downloader_thread, NULL);
    free(this->downloader_thread);
    this->downloader_thread = NULL;

    if (this->sockfd != -1) {
        close (this->sockfd);
        this->sockfd = -1;
    }

    if (this->read_buffer) {
        free(this->read_buffer);
        this->read_buffer = NULL;
    }

    this->read_buffer_size = 0;

    // preserve below stats 

    //this->current_byte = 0;
    //this->file_size = 0;
    //this->low_byte = 0;
    //this->high_byte = 0;
}

void 
HTTP_FD::ProgressBar () {

    int i = (int )((this->current_byte * 100 ) / this->file_size);
    std:: string progress = std::to_string(i) + "% " + "[" + std::string(i, '*') + std::string(100-i, ' ') + "] " + std::to_string(i) + "%";
    std::cout << "\r\033[F"  << "\n" << progress << std::flush;
}