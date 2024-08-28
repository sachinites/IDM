#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include "FileDownloaderFSMStatesEntryExitFns.h"
#include "EventFSM/fsm.h"
#include "FileDownLoader.h"

bool 
file_dnloader_DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS_exit_fn(efsm_t *efsm) {

    printf ("%s() Called ...\n", __FUNCTION__);

    FD *fd = (FD *)efsm_get_user_data(efsm);
    
    if (fd->connector_thread) {
        pthread_cancel(*fd->connector_thread);
        pthread_join(*fd->connector_thread, NULL);
        free(fd->connector_thread);
        fd->connector_thread = NULL;
    }

    return true;
}

bool
file_dnloader_DNLOAD_STATE_HEAD_GET_RESPONSE_AWAIT_exit_fn(efsm_t *efsm) {

    printf ("%s() Called ...\n", __FUNCTION__);

    FD *fd = (FD *)efsm_get_user_data(efsm);
    
    if (fd->downloader_thread) {
        pthread_cancel(*fd->downloader_thread);
        pthread_join(*fd->downloader_thread, NULL);
        free(fd->downloader_thread);
        fd->downloader_thread = NULL;
    }

    if (fd->read_buffer) {
        free(fd->read_buffer);
        fd->read_buffer = NULL;
    }

    return true;
}