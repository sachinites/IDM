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

#include "EventFSM/fsm.h"
#include "FileDownLoader.h"
#include "FileDownloaderStates.h"


HTTP_FD::HTTP_FD(std::string url) {

    this->sockfd = -1;
    this->read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);
    this->read_buffer_size = HTTP_READ_BUFFER_SIZE;
    this->current_byte = 0;
    this->file_size = 0;
    this->low_byte = 0;
    this->high_byte = 0;
    /* From URL, derive the server name and file path */
    this->server_name = "mirror2.internetdownloadmanager.com";
    this->file_path = "/idman642build20.exe";
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
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_CONNECT_FAILED);
            return NULL;
        }
    
        server = gethostbyname(fd->server_name.c_str());
        if (server == NULL) {
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_CONNECT_FAILED);
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

    /* We dont have Skt fd, it means we are not connected to server */
    assert (this->sockfd == -1);
    assert (this->connector_thread == NULL);
    assert (this->downloader_thread == NULL);

    this->connector_thread = (pthread_t *)calloc (1, sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create (this->connector_thread, &attr, server_head_connect_fn, (void *)this);
    return true;
}

static void * 
http_send_head_request (void *arg) {

	FD *fd = (FD *)arg;
	char *head_request = "HEAD /idman642build20.exe HTTP/1.1\r\nHost: mirror2.internetdownloadmanager.com\r\n\r\n";
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

    HTTP_FD *fd = (HTTP_FD *)arg;

    char *get_request = "GET /idman642build20.exe HTTP/1.1\r\nHost: mirror2.internetdownloadmanager.com\r\n\r\n";

    int n = write (fd->sockfd, get_request, strlen(get_request));

    if (n < 0) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
        return NULL;
    }

    FILE *fp = fopen ("idman642build20.exe", "wb");

    if (fp == NULL) {
        efsm_fire_event (fd->fsm, DNLOAD_EVENT_ERROR);
        return NULL;
    }

    char *read_buffer = (char *)calloc (1, HTTP_READ_BUFFER_SIZE);

    if (fd->current_byte == 0) {

        n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE);
        if (n < 0) {
            efsm_fire_event (fd->fsm, DNLOAD_EVENT_LOST_CONNECTION);
            fclose (fp);
            free(read_buffer);
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
    }

    /* Move the FD to the end of the file*/
    fseek (fp, 0, SEEK_END);

    while ((n = read (fd->sockfd, read_buffer, HTTP_READ_BUFFER_SIZE)) > 0) {
        fwrite (read_buffer, 1, n, fp);
        fd->current_byte += n;
    }

    fclose (fp);
    free(read_buffer);

    if (fd->current_byte != fd->file_size) {
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
    remove ("idman642build20.exe");
}