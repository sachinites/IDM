
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "EventFSM/fsm.h"
#include "FileDownLoader.h"
#include "FileDownloaderStates.h"
#include "FileDownLoaderFSMActions.h"

#define printf(a, ...) 

bool fd_action_state_init_action_start_state_head_connect_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->HeadConnectServer();
	return true;
}

bool fd_action_state_head_connect_in_progress_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
    return true;
}

bool fd_action_state_head_connect_in_progress_action_pause_state_head_connection_failed (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_head_connect_in_progress_action_error_state_error (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	//delete fd;
	return true;
}

bool fd_action_state_head_connect_in_progress_action_success_state_head_connected (efsm_t *efsm) {

	//printf ("Head Connection Successful\n");
	FD *fd = (FD *)efsm_get_user_data(efsm);
	assert (fd->connector_thread);
	pthread_cancel 	(*(fd->connector_thread));
	pthread_join (*(fd->connector_thread), NULL);
	free (fd->connector_thread);
	fd->connector_thread = NULL;
	efsm_fire_event (efsm, DNLOAD_EVENT_START);
	return true;
}

bool fd_action_state_head_connect_in_progress_action_connect_failed_state_head_connection_failed (efsm_t *efsm) {

	//printf ("Head Connection Failed\n");
	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	return true;
}

bool fd_action_state_head_connected_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_head_connected_action_start_state_head_get_response_await (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->HeadSendGetRequest();
	return true;
}

bool fd_action_state_head_connection_failed_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_head_connection_failed_action_reconnect_state_head_connect_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->HeadConnectServer();
	return true;
}

bool fd_action_state_head_get_response_await_action_cancel_state_cancelled (efsm_t *efsm) {

	return true;
}

bool fd_action_state_head_get_response_await_action_error_state_error (efsm_t *efsm) {

	/* Head Response from the server is not appropriate*/
	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_head_get_response_await_action_finished_state_fd_connect_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	int file_size = fd->file_size;
	fd->CleanupDnloadResources();
	fd->file_size = file_size;
	fd->FileDownloadConnectServer();
	return true;
}

bool fd_action_state_head_get_response_await_action_lost_connection_state_head_connection_failed (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	efsm_fire_event (efsm, DNLOAD_EVENT_RECONNECT);
	return true;
}

bool fd_action_state_fd_connect_in_progress_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_fd_connect_in_progress_action_pause_state_init (efsm_t *efsm ) {
	
	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	return true;
}

bool fd_action_state_fd_connect_in_progress_action_error_state_error (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_fd_connect_in_progress_action_success_state_fd_connected (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	assert (fd->connector_thread);
	pthread_cancel 	(*(fd->connector_thread));
	pthread_join (*(fd->connector_thread), NULL);
	free (fd->connector_thread);
	fd->connector_thread = NULL;
	efsm_fire_event (efsm, DNLOAD_EVENT_START);
	return true;
}

bool fd_action_state_fd_connect_in_progress_action_failed_state_fd_connection_failed (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	return true;
}

bool fd_action_state_fd_connected_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_fd_connected_action_start_state_fd_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->FileDownload();
	return true;
}

bool fd_action_state_fd_connection_failed_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_fd_connection_failed_action_error_state_error (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_fd_connection_failed_action_reconnect_state_fd_connect_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->CleanupDnloadResources();
	fd->FileDownloadConnectServer();
	return true;
}

bool fd_action_state_fd_in_progress_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	return true;
}

bool fd_action_state_fd_in_progress_action_pause_state_fd_suspended (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Pause();
	return true;
}

bool fd_action_state_fd_in_progress_action_error_state_error (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_fd_in_progress_action_finished_state_fd_finished (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->AssembleChunks();
	close (fd->sockfd);
	fd->sockfd = -1;
	assert (fd->downloader_thread);
	pthread_cancel(*fd->downloader_thread);
    pthread_join(*fd->downloader_thread, NULL);
    free(fd->downloader_thread);
    fd->downloader_thread = NULL;
	return true;
}

bool fd_action_state_fd_in_progress_action_lost_connection_state_fd_connection_failed (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	assert (fd->downloader_thread);
	pthread_cancel (*(fd->downloader_thread));
	pthread_join (*(fd->downloader_thread), NULL);
	free (fd->downloader_thread);
	fd->downloader_thread = NULL;
	assert (fd->sockfd != -1);
	close (fd->sockfd);
	fd->sockfd = -1;
	efsm_fire_event (efsm, DNLOAD_EVENT_RECONNECT);
	return true;
}

bool fd_action_state_fd_suspended_action_cancel_state_cancelled (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->Cancel();
	//delete fd;
	return true;
}

bool fd_action_state_fd_suspended_action_resume_state_fd_connect_in_progress (efsm_t *efsm) {

	FD *fd = (FD *)efsm_get_user_data(efsm);
	fd->FileDownloadConnectServer();
	return true;
}
