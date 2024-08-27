#include <stdlib.h>
#include <memory.h>
#include "EventFSM/fsm.h"
#include "FileDownloaderStates.h"
#include "FileDownLoader.h"

extern efsm_t *
file_downloader_new_efsm (FD *file_downloader_instance) ;

int
main (int argc, char *argv[]) {

    FD *fd = new HTTP_FD();
    efsm_t *efsm = file_downloader_new_efsm(fd);
    efsm_start_event_listener(efsm);

    /* Now Fire Evevents on FSM */
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    efsm_fire_event(efsm, DNLOAD_EVENT_CONNECT_SUCCESS);
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    efsm_fire_event(efsm, DNLOAD_EVENT_PAUSE);
    efsm_fire_event(efsm, DNLOAD_EVENT_RESUME);
    efsm_fire_event(efsm, DNLOAD_EVENT_CONNECT_SUCCESS);
    efsm_fire_event(efsm, DNLOAD_EVENT_START);
    efsm_fire_event(efsm, DNLOAD_EVENT_FINISHED);
    efsm_fire_event(efsm, DNLOAD_EVENT_CLEANUP);
    
    pthread_exit(0);
    return 0;
}
