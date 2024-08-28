
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "EventFSM/fsm.h"
#include "FileDownLoader.h"
#include "FileDownloaderStates.h"

/* Misc*/
extern bool fd_action_restart_downloader (efsm_t *efsm) {

	/* Releae all resources and then fire start event*/
	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Reset();
	return true;
}

/* Action Fn for State DNLOAD_STATE_INIT */
extern bool fd_action_state_init_action_start (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->HeadConnectServer();
    return true;
}

/* Action Fn for State DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS */
extern bool fd_action_state_head_connect_in_progress_action_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);

	if (fd->sockfd != -1) {
		close (fd->sockfd);
		fd->sockfd = -1;
	}

    return true;
}

extern bool fd_action_state_head_connect_in_progress_action_pause (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);

	if (fd->sockfd != -1) {
		close (fd->sockfd);
		fd->sockfd = -1;
	}

    return true;
}

extern bool fd_action_state_head_connect_in_progress_action_success (efsm_t *efsm) {

	printf ("Head Connect to Server is Succesful\n");
	efsm_fire_event (efsm, DNLOAD_EVENT_START);
	return true;
}

extern bool fd_action_state_head_connect_in_progress_action_failed (efsm_t *efsm) {

	printf ("Head Connect to Server is Failed\n");

	FD *fd = (FD *)efsm_get_user_data(efsm);

	if (fd->sockfd != -1) {
		close (fd->sockfd);
		fd->sockfd = -1;
	}

	return true;
}

/* Action Fn for State DNLOAD_STATE_HEAD_CONNECTED */

extern bool fd_action_state_head_connected_action_start (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->HeadSendGetRequest();
	return true;
}	

extern bool fd_action_state_head_connected_action_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	if (fd->sockfd != -1) {
		close (fd->sockfd);
		fd->sockfd = -1;
	}
	return true;
}


extern bool fd_action_free_all_resources(efsm_t *efsm) {
    return true;
}


extern bool fd_action_connect (efsm_t *efsm) {
    return true;
}

extern bool fd_action_file_download(efsm_t * efsm) {
	return true;
}

extern bool fd_action_reset_downloader(efsm_t * efsm) {
	return true;
}

extern bool fd_action_connected(efsm_t *efsm) {
	return true;
}

extern bool fd_action_suspended_dnloader_thread(efsm_t *efsm) {
	return true;
}
