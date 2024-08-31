#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "EventFSM/fsm.h"
#include "FileDownloaderStates.h"
#include "FileDownLoader.h"

extern efsm_t *
file_downloader_new_efsm () ;

/* 
 Ex :  ./exe  http://mirror2.internetdownloadmanager.com/idman642build20.exe
*/
int
main (int argc, char **argv) {

    /* Create a File Downloader Instance*/
    //FD *fd = new HTTP_FD("http://mirror2.internetdownloadmanager.com/idman642build20.exe");
    FD *fd = new HTTP_FD(argv[1]);
    fd->SetByteRange (100, 3000);

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
