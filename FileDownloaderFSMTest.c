#include "EventFSM/fsm.h"
#include "FileDownloaderStates.h"

extern efsm_t *
file_downloader_new_efsm(void *user_data);

int
main (int argc, char *argv[]) {

    efsm_t *efsm = file_downloader_new_efsm(NULL);
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

    efsm_destroy (efsm);

    return 0;
}
