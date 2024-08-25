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
static const efsm_state_t file_downloader_states[] = {

    {DNLOAD_STATE_INIT, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_IN_PROGRESS, false,     
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_SUSPENDED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_CONNECTION_IN_PROGRESS, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_CONNECTION_FAILED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_CONNECTED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_DNLOAD_FINISHED, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL},

    {DNLOAD_STATE_DONE, false, 
    file_dnloader_default_state_entry_fn, file_dnloader_default_state_exit_fn, NULL}

};

extern bool fd_action_free_all_resources(efsm_t *efsm);
extern bool fd_action_connect(efsm_t *efsm);
extern bool fd_action_connected(efsm_t *efsm);
extern bool fd_action_suspended_dnloader_thread(efsm_t *efsm);
extern bool fd_action_file_download(efsm_t * efsm);
extern bool fd_action_reset_downloader(efsm_t * efsm);


static const transition_table_entry_t trans_table_fd_state_init[] = {

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
                STATE_EVENT_TT_ENTRY(fd_action_connect, 
                    &file_downloader_states[DNLOAD_STATE_CONNECTION_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                                

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_in_progress[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(fd_action_suspended_dnloader_thread,
                    &file_downloader_states[DNLOAD_STATE_SUSPENDED]),

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
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                                

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_suspended[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(
                    FSM_NO_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(fd_action_connect, 
                    &file_downloader_states[DNLOAD_STATE_CONNECTION_IN_PROGRESS]),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_connect, 
                    &file_downloader_states[DNLOAD_STATE_CONNECTION_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                                

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_connection_in_progress [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(fd_action_suspended_dnloader_thread,
                    &file_downloader_states[DNLOAD_STATE_SUSPENDED]),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(fd_action_connected,
                    &file_downloader_states[DNLOAD_STATE_CONNECTED]),                                

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_connection_connection_failed [] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(fd_action_connect, 
                    &file_downloader_states[DNLOAD_STATE_CONNECTION_IN_PROGRESS]),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                    

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_connection_connected[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_file_download,
                    &file_downloader_states[DNLOAD_STATE_IN_PROGRESS]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                    

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_dnload_finished[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(fd_action_reset_downloader,
                    &file_downloader_states[DNLOAD_STATE_INIT]),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                    

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

static const transition_table_entry_t trans_table_fd_state_done[] = {

                // DNLOAD_EVENT_CANCEL
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_PAUSE
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_RESUME
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_START
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CLEANUP
                STATE_EVENT_TT_ENTRY(fd_action_free_all_resources, 
                    &file_downloader_states[DNLOAD_STATE_DONE]),

                // DNLOAD_EVENT_RECONNECT
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_FINISHED
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_LOST_CONNECTION
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),

                // DNLOAD_EVENT_CONNECT_SUCCESS
                STATE_EVENT_TT_ENTRY(FSM_INVAID_ACTION, FSM_NO_STATE_TRANSITION),                    

                /* Timer Expiry Event in the last*/
                STATE_EVENT_TT_ENTRY(FSM_NO_ACTION, FSM_NO_STATE_TRANSITION)
};

efsm_t *
file_downloader_new_efsm(void *user_data);

efsm_t *
file_downloader_new_efsm(void *user_data) {

    efsm_state_t *state = &file_downloader_states[DNLOAD_STATE_INIT];
    state->trans_table.tte_array = &trans_table_fd_state_init;

    state = &file_downloader_states[DNLOAD_STATE_IN_PROGRESS];
    state->trans_table.tte_array = &trans_table_fd_state_in_progress;

    state = &file_downloader_states[DNLOAD_STATE_SUSPENDED];
    state->trans_table.tte_array = &trans_table_fd_state_suspended;

    state = &file_downloader_states[DNLOAD_STATE_CONNECTION_IN_PROGRESS];
    state->trans_table.tte_array = &trans_table_fd_state_connection_in_progress;

    state = &file_downloader_states[DNLOAD_STATE_CONNECTION_FAILED];
    state->trans_table.tte_array = &trans_table_fd_state_connection_connection_failed;

    state = &file_downloader_states[DNLOAD_STATE_CONNECTED];
    state->trans_table.tte_array = &trans_table_fd_state_connection_connected;

    state = &file_downloader_states[DNLOAD_STATE_DNLOAD_FINISHED];
    state->trans_table.tte_array = &trans_table_fd_state_dnload_finished;

    state = &file_downloader_states[DNLOAD_STATE_DONE];
    state->trans_table.tte_array = &trans_table_fd_state_done;

    /* Initiate Timers Here */
    efsm_t *efsm = efsm_new(user_data);
    efsm->initial_state = &file_downloader_states[DNLOAD_STATE_INIT];
    efsm->state_print = file_downloader_state_tostring;
    efsm->event_print =  file_downloader_event_tostring;
    assert (pipe(efsm->pipefd) != -1);
    return efsm;
}


