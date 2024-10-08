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

#define printf(a, ...) 

static std::string protocol_delimiter = "://";
static std::string path_delimiter = "/";

HTTP_FD::HTTP_FD(std::string url) {

    this->sockfd = -1;
    pthread_mutex_init (&this->bytes_downloaded_mutex, NULL);
    this->bytes_downloaded = 0;

    this->file_size = 0;
    this->low_byte = 0;
    this->high_byte = 0;
    this->flags = 0;
    // url = "http://mirror2.internetdownloadmanager.com/idman642build20.exe";
    size_t protocol_pos = url.find(protocol_delimiter);
    size_t server_start_pos = protocol_pos + protocol_delimiter.length();
    size_t path_start_pos = url.find(path_delimiter, server_start_pos);
    this->server_name = url.substr(server_start_pos, path_start_pos - server_start_pos); // "mirror2.internetdownloadmanager.com";
    this->file_path = url.substr(path_start_pos);   // "/idman642build20.exe";
    //printf ("Constructor : Server = %s   File-Path = %s\n", 
     //   this->server_name.c_str(), this->file_path.c_str());   
    this->fsm = NULL;
    this->connector_thread = NULL;
    this->downloader_thread = NULL;
}

HTTP_FD::~HTTP_FD() {

    assert (this->sockfd == -1);
    assert (this->connector_thread == NULL);    
    assert (this->downloader_thread == NULL);
    pthread_mutex_destroy (&this->bytes_downloaded_mutex);
    //printf ("HTTP FD Deleted\n");
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
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
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

	//printf ("\nHead Response: %s\n", read_buffer);

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

    int file_size;
    bool partial_req = false;
    char get_request[1024];
    HTTP_FD *fd = (HTTP_FD *)arg;

    /* Range not specified, download the entire file */
    if (fd->low_byte == 0 && fd->high_byte == 0) {

        snprintf(get_request, sizeof (get_request), 
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n", 
            fd->file_path.c_str(), fd->server_name.c_str());
            file_size = fd->file_size;
    }
    else
    {
        snprintf(get_request, sizeof(get_request),
                 "GET %s HTTP/1.1\r\n"
                 "Host: %s\r\n"
                 "Range: bytes=%d-%d\r\n"
                 "Connection: close\r\n"
                 "\r\n",
                 fd->file_path.c_str(), fd->server_name.c_str(),
                 fd->low_byte, fd->high_byte);
        file_size = fd->high_byte - fd->low_byte + 1;
    }

    if (fd->flags & FD_DNLOAD_PARTIAL_REQ) partial_req = true;

   // printf ("HTTP GET REQUEST SENT: \n%s", get_request);

    int n = write (fd->sockfd, get_request, strlen(get_request));

    if (n < 0) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
        return NULL;
    }

    const char *temp = strrchr(fd->file_path.c_str(), '/');
    const char *file_name = temp ? temp + 1 : fd->file_path.c_str();
    FILE *fp = NULL;

    if (partial_req) fp = fopen (file_name, "r+b");
    else fp = fopen (file_name, "wb");

    if (!fp) {
        efsm_fire_event(fd->fsm, DNLOAD_EVENT_ERROR);
        return NULL;
    }

    fseek (fp, 0, fd->low_byte);

    char *read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);
    
    if (fd->bytes_downloaded == 0) {

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

        //printf ("HTTP GET RESPONSE:\n\n");
        //fwrite (read_buffer, 1, header_end, stdout);

        /* Analyzing GET Resonse*/
        char *response = (char *)calloc (1, header_end + 1);
        memcpy (response, read_buffer, header_end);
        response[header_end] = '\0';

        if (strstr (response, "HTTP/1.1 400 Bad Request")) {
            fclose (fp);
            free(read_buffer); 
            free (response);
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
            return NULL;
        }

        if (partial_req) {
            if (strstr (response, "HTTP/1.1 206 Partial Content") == NULL ) {
                printf ("Error : Server Do not Support Partial Content\n");
                fclose (fp);
                free(read_buffer); 
                free (response);
                efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
                return NULL;                
            }
         }
        free (response);

        fwrite(read_buffer + header_end, 1, n - header_end, fp);
        pthread_mutex_lock (&fd->bytes_downloaded_mutex);
        fd->bytes_downloaded += (n - header_end);
        pthread_mutex_unlock (&fd->bytes_downloaded_mutex);
        fd->ProgressBar();
    }

    /* Move the FD to the end of the file*/
    fseek (fp, 0, fd->low_byte + fd->bytes_downloaded);

    while ((n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE)) > 0) {
        fwrite (read_buffer, 1, n, fp);
        pthread_mutex_lock (&fd->bytes_downloaded_mutex);
        fd->bytes_downloaded += n;
        pthread_mutex_unlock (&fd->bytes_downloaded_mutex);
        fd->ProgressBar();
        if (fd->bytes_downloaded == file_size) {
            //printf ("\n");
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

    if (fd->bytes_downloaded != file_size) {
        printf ("\nError : File Size Downloaded : %d, Actual file size : %d\n", 
            fd->bytes_downloaded, file_size); 
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

    if (! (this->flags & FD_DNLOAD_PARTIAL_REQ)) {

        const char *temp = strrchr(file_path.c_str(), '/');
        const char *file_name = temp ? temp + 1 : file_path.c_str();
        remove (file_name);
    }
}

void 
HTTP_FD::Pause() {

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
    
}

void 
HTTP_FD::ProgressBar () {

    if (this->flags & FD_REPORT_PROGRESS_ON_CONSOLE) {
        int file_size = this->GetFileSize();
        int i = (int )((this->bytes_downloaded * 100 ) /file_size);
        std:: string progress = std::to_string(i) + "% " + "[" + std::string(i, '*') + std::string(100-i, ' ') + "] " + std::to_string(i) + "%";
        std::cout << "\r\033[F"  << "\n" << progress << std::flush;
    }

}

void 
HTTP_FD::FDReset () {

    this->CleanupDnloadResources();
    this->bytes_downloaded = 0;
}