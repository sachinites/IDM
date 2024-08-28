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

    if (this->sockfd != -1) {
        close (this->sockfd);
    }
    free (this->read_buffer);
    assert (this->connector_thread == NULL);    
    assert (this->downloader_thread == NULL);
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
		return NULL;
	}

	printf ("\nHead Response: %s\n", read_buffer);
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
