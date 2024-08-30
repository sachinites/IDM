#ifndef __FD_FSM_ACTION__
#define __FD_FSM_ACTION__

#include <stdbool.h>
typedef struct efsm_ efsm_t;

// DNLOAD_STATE_INIT
bool fd_action_state_init_action_start_state_head_connect_in_progress (efsm_t *efsm) ;

// DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS
bool fd_action_state_head_connect_in_progress_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_head_connect_in_progress_action_pause_state_head_connection_failed (efsm_t *efsm) ;
bool fd_action_state_head_connect_in_progress_action_error_state_error (efsm_t *efsm) ;
bool fd_action_state_head_connect_in_progress_action_success_state_head_connected (efsm_t *efsm) ;
bool fd_action_state_head_connect_in_progress_action_connect_failed_state_head_connection_failed (efsm_t *efsm) ;

// DNLOAD_STATE_HEAD_CONNECTED
bool fd_action_state_head_connected_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_head_connected_action_start_state_head_get_response_await (efsm_t *efsm) ;

// DNLOAD_STATE_HEAD_CONNECTION_FAILED
bool fd_action_state_head_connection_failed_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_head_connection_failed_action_reconnect_state_head_connect_in_progress (efsm_t *efsm) ;

// DNLOAD_STATE_HEAD_GET_RESPONSE_AWAIT
bool fd_action_state_head_get_response_await_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_head_get_response_await_action_error_state_error (efsm_t *efsm) ;
bool fd_action_state_head_get_response_await_action_finished_state_fd_connect_in_progress (efsm_t *efsm) ;
bool fd_action_state_head_get_response_await_action_lost_connection_state_head_connection_failed (efsm_t *efsm) ;

// DNLOAD_STATE_FD_CONNECT_IN_PROGRESS
bool fd_action_state_fd_connect_in_progress_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_fd_connect_in_progress_action_pause_state_init (efsm_t *efsm );
bool fd_action_state_fd_connect_in_progress_action_pause_state_fd_connection_failed (efsm_t *efsm) ;
bool fd_action_state_fd_connect_in_progress_action_error_state_error (efsm_t *efsm) ;
bool fd_action_state_fd_connect_in_progress_action_success_state_fd_connected (efsm_t *efsm) ;
bool fd_action_state_fd_connect_in_progress_action_failed_state_fd_connection_failed (efsm_t *efsm) ;

// DNLOAD_STATE_FD_CONNECTED
bool fd_action_state_fd_connected_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_fd_connected_action_start_state_fd_in_progress (efsm_t *efsm) ;

// DNLOAD_STATE_FD_CONNECTION_FAILED
bool fd_action_state_fd_connection_failed_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_fd_connection_failed_action_error_state_error (efsm_t *efsm) ;
bool fd_action_state_fd_connection_failed_action_reconnect_state_fd_connect_in_progress (efsm_t *efsm) ;

// DNLOAD_STATE_FD_IN_PROGRESS
bool fd_action_state_fd_in_progress_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_fd_in_progress_action_pause_state_fd_suspended (efsm_t *efsm) ;
bool fd_action_state_fd_in_progress_action_error_state_error (efsm_t *efsm) ;
bool fd_action_state_fd_in_progress_action_finished_state_fd_finished (efsm_t *efsm) ;
bool fd_action_state_fd_in_progress_action_lost_connection_state_fd_connection_failed(efsm_t *efsm) ;

// DNLOAD_STATE_FD_SUSPENDED
bool fd_action_state_fd_suspended_action_cancel_state_cancelled (efsm_t *efsm) ;
bool fd_action_state_fd_suspended_action_resume_state_fd_connect_in_progress (efsm_t *efsm) ;

// DNLOAD_STATE_FD_FINISHED

// DNLOAD_STATE_ERROR

#endif 
