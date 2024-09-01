#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include "EventFSM/fsm.h"
#include "FileDownloaderStates.h"
#include "FileDownLoader.h"
#include "Cursor/cursor.h"

extern efsm_t *
file_downloader_new_efsm () ;

static std::string protocol_delimiter = "://";
static std::string path_delimiter = "/";

#define MB (1024 * 1024 )
#define CHUNK_SIZE  2    // chunk size in MB

bool 
multithreaded_http_downloader (std::string url) {

    size_t protocol_pos = url.find(protocol_delimiter);
    size_t server_start_pos = protocol_pos + protocol_delimiter.length();
    size_t path_start_pos = url.find(path_delimiter, server_start_pos);
    std::string server_name = url.substr(server_start_pos, path_start_pos - server_start_pos); 
    std::string file_path = url.substr(path_start_pos);

    printf ("Constructor : Server = %s   File-Path = %s\n",  
        server_name.c_str(), file_path.c_str());   
    
    /* Checking if the server supports partial file downloads*/
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int portno = HTTP_DEFAULT_PORT;

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        printf ("Error : Socket Creation Failed\n");
        return false;
    }

    server = gethostbyname(server_name.c_str());
    if (server == NULL) {
        printf("Failed to resolve host-name : %s\n", server_name.c_str());
        close (sockfd);
        return false;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)) {
        printf("Error : Head Connect to Server Failed\n");
        close(sockfd);
        return false;
    }

    printf ("Connection Successful, Sending HTTP HEAD Request : \n");

    char head_request[1024];

    snprintf (head_request, sizeof (head_request), 
        "HEAD %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        file_path.c_str(), server_name.c_str());    

	int n = write (sockfd, head_request, strlen(head_request));
	
    if (n < 0) {
		printf ("Error : Sending HTTP HEAD Request Failed\n");
        close (sockfd);
		return false;
	}    

    char *response = head_request;   // Using ame buffer for response

    n = read (sockfd, response, sizeof (head_request));
	if (n < 0) {
		printf ("Error : Read Error on Socket\n");
        close (sockfd);
		return false;
	}

	printf ("\nHead Response: \n%s", response);

    if (strstr (response, "HTTP/1.1 200 OK") == NULL) {
        printf ("Error : Head Response not Ok\n");
        close ( sockfd);
        return false;
    }    

    if (strstr (response, "Accept-Ranges: bytes") == NULL) {
        printf ("Error : Server do not support Partial Downloads\n");
        close (sockfd);
        return false;
    }

    char *content_length_str = strstr(response, "Content-Length:");

    if (content_length_str == NULL) {
        printf ("Error : Server did not notify us Size of File\n");
        close (sockfd);
        return false;
    }

    int file_size;
    sscanf(content_length_str, "Content-Length: %d", &file_size);

    printf ("File Size is : %d\n", file_size);
    close(sockfd);

   /* Now, Launch Multiple HTTP File downloaders */

   /* Number of threads to be launched. Each thread is responsible to download 2MB of file*/
    n = file_size / (CHUNK_SIZE * MB );   
    int remaining_bytes = file_size % (CHUNK_SIZE * MB);

    int i ;

    FD **fd_array = (FD **)calloc (n, sizeof (FD *));

    for (i = 0; i < n - 1; i++) {

        fd_array[i] = new HTTP_FD (url);
        fd_array[i]->SetByteRange (i * CHUNK_SIZE * MB,  ((i + 1) * CHUNK_SIZE * MB) - 1);
    }

    fd_array[n - 1] = new HTTP_FD (url);
    fd_array[n - 1]->SetByteRange ( (n - 1) * CHUNK_SIZE * MB,  (n * CHUNK_SIZE * MB) + remaining_bytes - 1);

    efsm_t **efsm_array = (efsm_t **) calloc (n, sizeof (efsm_t *));
    
    for (i = 0; i < n; i++) {

        efsm_array[i] = file_downloader_new_efsm();
        fd_array[i]->fsm = efsm_array[i];
        efsm_array[i]->user_data = (void *)fd_array[i];
        efsm_start_event_listener(efsm_array[i]);
        efsm_fire_event(efsm_array[i], DNLOAD_EVENT_START);
    }

    /* Pole All the FileDownloaders of their status : once per second */
    FD *fd; int j;
    bool cancel ;
    std::string progress;
    int *bytes_downloaded = (int *)calloc (n, sizeof (int));

     printf ("Downloading ....\n\r");

    save_cursor_position();

    while (true) {

        cancel = true;

        // Lock mutexes and fetch the bytes_downloaded values
        for (int i = 0; i < n; i++) {

            fd = fd_array[i];
            pthread_mutex_lock(&fd->bytes_downloaded_mutex);
            bytes_downloaded[i] = fd->bytes_downloaded;
            pthread_mutex_unlock(&fd->bytes_downloaded_mutex);
        }

        // Update progress for each chunk
        for (int i = 0; i < n; i++) {

            j = (int )((bytes_downloaded[i] * 100 ) /fd_array[i]->GetFileSize());

            progress = "chunk: " + std::to_string(i) + " " + std::to_string(j) + "% " + "[" + std::string(j, '*') + std::string(100-j, ' ') + "] " + std::to_string(j) + "%";
            clear_line();
            printf ("%s\r", progress.c_str());
            fflush(stdout);
            move_cursor_down(1);
            move_cursor_to_start () ;

            if (j != 100) cancel = false;
        }

        if (cancel) break;

        restore_cursor_position();
        usleep(500000);
    }

    /* Cleanup Everything */
    efsm_t *efsm;

    for (i = 0 ; i < n; i++) {

        fd = fd_array[i];
        efsm = efsm_array[i];

        fd->CleanupDnloadResources();
        delete fd;
        efsm->user_data = NULL;
        efsm_destroy(efsm);
    }

    free (fd_array);
    free(efsm_array);
    free (bytes_downloaded);
    printf ("\n Download Done !\n");
    exit(0);
    return true;
}





/* 
 Ex :  ./exe  http://mirror2.internetdownloadmanager.com/idman642build20.exe
*/
int
main (int argc, char **argv) {

    multithreaded_http_downloader (argv[1]);
    pthread_exit(0);

    /* Create a File Downloader Instance*/
    //FD *fd = new HTTP_FD("http://mirror2.internetdownloadmanager.com/idman642build20.exe");
    FD *fd = new HTTP_FD(argv[1]);
    //fd->SetByteRange (100, 3000);
    fd->flags |= FD_REPORT_PROGRESS_ON_CONSOLE;

    /* Create FSM instance which will drive our FD instance state machine*/
    efsm_t *efsm = file_downloader_new_efsm();

    /* Bind the FD and its FSM together */
    fd->fsm = efsm;
    efsm->user_data = (void *)fd;

    /* Start the event Listener thread on FSM*/
    efsm_start_event_listener(efsm);

    /* Now Fire Evevents on FSM */
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    #if 0
    efsm_fire_event(efsm, DNLOAD_EVENT_CONNECT_SUCCESS);
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    efsm_fire_event(efsm, DNLOAD_EVENT_PAUSE);
    efsm_fire_event(efsm, DNLOAD_EVENT_RESUME);
    efsm_fire_event(efsm, DNLOAD_EVENT_CONNECT_SUCCESS);
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    efsm_fire_event(efsm, DNLOAD_EVENT_FINISHED);
    efsm_fire_event(efsm, DNLOAD_EVENT_ERROR);
    #endif
    pthread_exit(0);
    return 0;
}



