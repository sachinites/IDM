#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "EventFSM/fsm.h"
#include "FileDownloaderStates.h" 

static bool file_dnloader_default_state_entry_fn(efsm_t *efsm) { 
    printf ("%s() Called ...\n", __FUNCTION__);
    return true; 
}

static bool file_dnloader_default_state_exit_fn(efsm_t *efsm) { 
     printf ("%s() Called ...\n", __FUNCTION__);
    return true; 
};

/* Create FilwDownloader States*/
static efsm_state_t file_downloader_states[] = {

    {DNLOAD_STATE_INIT, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_HEAD_CONNECTED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_HEAD_CONNECTION_FAILED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_CONNECT_IN_PROGRESS, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_CONNECTED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_CONNECTION_FAILED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_IN_PROGRESS, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_SUSPENDED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_CANCELLED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},

    {DNLOAD_STATE_FD_FINISHED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}},            

    {DNLOAD_STATE_DONE, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, {NULL}}

};

extern bool fd_action_free_all_resources(efsm_t *efsm);
extern bool fd_action_connect(efsm_t *efsm);
extern bool fd_action_connected(efsm_t *efsm);
extern bool fd_action_suspended_dnloader_thread(efsm_t *efsm);
extern bool fd_action_file_download(efsm_t * efsm);
extern bool fd_action_reset_downloader(efsm_t * efsm);


static transition_table_entry_t trans_table_fd_state_init[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_connect, 
                    &file_downloader_states[DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)                  
};

static const transition_table_entry_t trans_table_fd_state_head_connect_in_progress[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(fd_action_suspended_dnloader_thread,
                    &file_downloader_states[DNLOAD_STATE_HEAD_CONNECTION_FAILED]),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(fd_action_file_download, 
                    &file_downloader_states[DNLOAD_STATE_HEAD_CONNECTED]),

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_INIT])
};

static const transition_table_entry_t trans_table_fd_state_head_connected[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_head_connection_failed [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_INIT]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, 
                    &file_downloader_states[DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_connection_connect_in_progress [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CANCELLED]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECTION_FAILED]),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECTED]),                 

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECTION_FAILED])
};

static const transition_table_entry_t trans_table_fd_state_connected[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CANCELLED]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_connection_failed [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CANCELLED]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_fd_in_progress[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CANCELLED]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_SUSPENDED]),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_FINISHED]),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_fd_suspended [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CANCELLED]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS]),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_fd_cancelled [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_fd_finished [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_INIT]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_fd_done [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),          

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),                

                // DNLOAD_EVENT_CONNECT_FAILED
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

/* This is one time fn only */
void 
file_downloader_fsm_init () {

    static bool fsm_init = false; 

    if (fsm_init ) return ;

    efsm_state_t *state = &file_downloader_states[DNLOAD_STATE_INIT];
    state->trans_table.tte_array = &trans_table_fd_state_init;

    state = &file_downloader_states[DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS];
    state->trans_table.tte_array = &trans_table_fd_state_head_connect_in_progress;

    state = &file_downloader_states[DNLOAD_STATE_HEAD_CONNECTED];
    state->trans_table.tte_array = &trans_table_fd_state_head_connected;

    state = &file_downloader_states[DNLOAD_STATE_HEAD_CONNECTION_FAILED];
    state->trans_table.tte_array = &trans_table_fd_state_head_connection_failed;

    state = &file_downloader_states[DNLOAD_STATE_FD_CONNECT_IN_PROGRESS];
    state->trans_table.tte_array = &trans_table_fd_state_connection_connect_in_progress;

    state = &file_downloader_states[DNLOAD_STATE_FD_CONNECTED];
    state->trans_table.tte_array = &trans_table_fd_state_connected;

    state = &file_downloader_states[DNLOAD_STATE_FD_CONNECTION_FAILED];
    state->trans_table.tte_array = &trans_table_fd_state_connection_failed;

    state = &file_downloader_states[DNLOAD_STATE_FD_IN_PROGRESS];
    state->trans_table.tte_array = &trans_table_fd_state_fd_in_progress;

    state = &file_downloader_states[DNLOAD_STATE_FD_SUSPENDED];
    state->trans_table.tte_array = &trans_table_fd_state_fd_suspended;

    state = &file_downloader_states[DNLOAD_STATE_FD_CANCELLED];
    state->trans_table.tte_array = &trans_table_fd_state_fd_cancelled;

    state = &file_downloader_states[DNLOAD_STATE_FD_FINISHED];
    state->trans_table.tte_array = &trans_table_fd_state_fd_finished;

    state = &file_downloader_states[DNLOAD_STATE_DONE];
    state->trans_table.tte_array = &trans_table_fd_state_fd_done;

    fsm_init = true;
}

class FD;

extern efsm_t *
file_downloader_new_efsm (FD *file_downloader_instance) ;

efsm_t *
file_downloader_new_efsm (FD *file_downloader_instance) {

    file_downloader_fsm_init ();
    
    efsm_t *efsm = efsm_new( (void *) file_downloader_instance);
    efsm->initial_state = &file_downloader_states[DNLOAD_STATE_INIT];
    efsm->state_print = file_downloader_state_tostring;
    efsm->event_print =  file_downloader_event_tostring;
    assert (pipe(efsm->pipefd) != -1);
    return efsm;
}
