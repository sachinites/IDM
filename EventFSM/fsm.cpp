#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "fsm.h"

void 
efsm_state_timer_operation (wheel_timer_elem_t *timer, efsm_state_timer_op_t op) {

    switch (op) {

        case   EFSM_STATE_TIMER_START:
            timer_start(timer);
            break;
        case EFSM_STATE_TIMER_STOP:
            timer_stop(timer);
            break;
        case EFSM_STATE_TIMER_RESTART:
            timer_restart(timer);
            break;
        case EFSM_STATE_TIMER_DESTROY:
            timer_stop (timer);
            free(timer);
            break;
        default: ;
    }
}

static const char *TIMER_EXP_EVENT_STR = 
    "FSM_STATE_EVENT_TIMER_EXPIRY";

static void
efsm_state_exit (efsm_t *efsm) {

    state_id_t state_id = efsm->current_state->id;

    if (efsm->state_print) {
        printf ("Exiting state %s\n", efsm->state_print(efsm->current_state->id));
    }
    else {
        printf ("Exiting state %d\n", efsm->current_state->id);
    }

    wheel_timer_elem_t *expiry_timer = efsm->state_config_data[state_id] ? 
                            efsm->state_config_data[state_id]->expiry_timer : NULL;

    /* Executethe default action to  be performed on state exit */
    if (expiry_timer  && 
                (efsm->event_triggered != FSM_STATE_EVENT_TIMER_EXPIRY)) {
        efsm_state_timer_operation (expiry_timer, EFSM_STATE_TIMER_STOP);
    }

    /* Execute theuser defined action to be performed on state exit */
    if (efsm->current_state->exit_fn) {
        efsm->current_state->exit_fn(efsm);
    }

    efsm->current_state = NULL;
}

static void
efsm_state_enter (efsm_t *efsm, efsm_state_t *state) {

    assert(!efsm->current_state);

    if (efsm->state_print) {
        printf ("Entering state %s\n", efsm->state_print(state->id));
    }
    else {
        printf ("Entering state %d\n", state->id);
    }

    if (efsm->state_config_data[state->id] && 
            efsm->state_config_data[state->id]->start_expiry_timer_on_enter &&
            efsm->state_config_data[state->id]->expiry_timer) {
        efsm_state_timer_operation (
                efsm->state_config_data[state->id]->expiry_timer,
                EFSM_STATE_TIMER_START);
    }

    /* Execute theuser defined action to be performed on state entry */
    if (state->entry_fn) {
        state->entry_fn(efsm);
    }

    efsm->current_state = state;
}

efsm_state_t *
efsm_create_new_state (state_id_t id, 
                                        bool is_final,
                                        bool (*entry_fn)(efsm_t *),
                                        bool (*exit_fn)(efsm_t *), 
                                        transition_table_t *trans_table) {

    efsm_state_t *state = (efsm_state_t *)calloc (1, sizeof(efsm_state_t));
    state->id = id;
    state->final_state = is_final;
    state->entry_fn = entry_fn;
    state->exit_fn = exit_fn;
    state->trans_table.tte_array = NULL;
    return state;
}

efsm_t *
efsm_new (void *user_data, state_id_t max_state_id) {

    efsm_t *efsm;
    efsm = (efsm_t *)calloc (1, sizeof(efsm_t) + 
                        (sizeof(efsm->state_config_data[0]) * max_state_id)  );
    efsm->wt = init_wheel_timer (60, 1000, TIMER_MILLI_SECONDS);
    start_wheel_timer(efsm->wt);
    pthread_spin_init(&efsm->spinlock, PTHREAD_PROCESS_PRIVATE);
    efsm->user_data = user_data;
    efsm->transitioning = false;
    
    return efsm;
}

static void
efsm_execute (efsm_t *efsm, int event) {

    efsm_state_t *next_state;
    action_fn action_fn_cbk;

    pthread_spin_lock (&efsm->spinlock);

    if (!efsm->current_state) {
        efsm->current_state = efsm->initial_state;
    }

    if (efsm->event_print)
        printf ("Executing Event %s", 
            (event != FSM_STATE_EVENT_TIMER_EXPIRY) ? efsm->event_print(event) : TIMER_EXP_EVENT_STR);
    else if (event == FSM_STATE_EVENT_TIMER_EXPIRY)
        printf ("Executing Event %s", TIMER_EXP_EVENT_STR);
    else
        printf ("Executing Event %d", event);


    if (efsm->state_print) 
        printf (" on state %s\n", efsm->state_print(efsm->current_state->id));
    else
        printf (" on state %d\n", efsm->current_state->id);


    /* Dont invoke this API from within transition fn*/
    assert (efsm->transitioning == false);

   efsm->event_triggered = event;
   action_fn_cbk = efsm->current_state->trans_table.tte_array[0][event].action_fn_cbk;
   next_state = efsm->current_state->trans_table.tte_array[0][event].next_state;

   if (action_fn_cbk) {
        efsm->transitioning = true;
        action_fn_cbk(efsm);
        efsm->transitioning = false;
   }
   if (next_state) {
        efsm_state_exit (efsm);
        efsm_state_enter (efsm, next_state);
    }

    pthread_spin_unlock (&efsm->spinlock);
}

/* Called when state timer expires */
static void
fsm_state_timer_expiry_fn (void *arg, uint32_t arg_size) {

    if (!arg) return;
    efsm_t *efsm = (efsm_t *)arg;
    efsm_state_t *current_state = efsm->current_state;
    efsm_execute(efsm, FSM_STATE_EVENT_TIMER_EXPIRY);
}

wheel_timer_elem_t *
fsm_create_timer (efsm_t *efsm, uint16_t time_interval) {

    wheel_timer_elem_t *wt_elem = NULL;

    wt_elem = timer_create_new(
                                    efsm->wt, 
                                    fsm_state_timer_expiry_fn,
                                    (void *)efsm,
                                    sizeof(*efsm),
                                    time_interval,
                                    0);

    return wt_elem;
}

void
efsm_state_expiry_timer_config 
                                        (efsm_t *efsm,
                                         state_id_t state_id, 
                                         uint16_t expiry_interval, 
                                         bool start_on_enter) {

    state_config_data_t *state_config_data = 
        efsm->state_config_data[state_id];

    if (!state_config_data) {
        state_config_data = (state_config_data_t *)calloc (1, sizeof(state_config_data_t));
        efsm->state_config_data[state_id] = state_config_data;
    }

    state_config_data->expiry_time = expiry_interval;
    state_config_data->start_expiry_timer_on_enter = start_on_enter;
    state_config_data->expiry_timer = fsm_create_timer (efsm, expiry_interval);
}

static void
event_listen_fn ( void * arg ) {

	efsm_t *efsm = (efsm_t *)arg;
	int event;

	while (true) {

		read (efsm->pipefd[0], (char *)&event, sizeof(int));
		efsm_execute (efsm, event);
	}
}

void
efsm_start_event_listener (efsm_t *efsm) {

	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&thread, &attr, event_listen_fn, (void *)efsm);
}

void 
efsm_fire_event (efsm_t *efsm, int event){

	write (efsm->pipefd[1], &event, sizeof (int));
}
