#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "fsm.h"

static void
efsm_state_exit (efsm_t *efsm) {

    state_id_t state_id = efsm->current_state->id;

    if (efsm->state_print) {
        printf ("Exiting state %s\n", efsm->state_print(efsm->current_state->id));
    }
    else {
        printf ("Exiting state %d\n", efsm->current_state->id);
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

    /* Execute theuser defined action to be performed on state entry */
    if (state->entry_fn) {
        state->entry_fn(efsm);
    }

    efsm->current_state = state;
}

efsm_t *
efsm_new (void *user_data) {

    efsm_t *efsm;
    efsm = (efsm_t *)calloc (1, sizeof(efsm_t)); 
    efsm->user_data = user_data; 
    return efsm;
}

static void
efsm_execute (efsm_t *efsm, int event) {

    efsm_state_t *next_state;
    action_fn action_fn_cbk;

    if (!efsm->current_state) {
        efsm->current_state = efsm->initial_state;
    }

    if (efsm->event_print)
        printf ("Executing Event %s", efsm->event_print(event));
    else
	    printf ("Eecuting Event %d", event);

    if (efsm->state_print) 
        printf (" on state %s\n", efsm->state_print(efsm->current_state->id));
    else
        printf (" on state %d\n", efsm->current_state->id);

   efsm->event_triggered = event;
   action_fn_cbk = efsm->current_state->trans_table.tte_array[0][event].action_fn_cbk;
   next_state = efsm->current_state->trans_table.tte_array[0][event].next_state;

   if (next_state) {
   	    efsm_state_exit (efsm);
        efsm_state_enter (efsm, next_state);
   }

   if (action_fn_cbk) {
        action_fn_cbk(efsm);
   }
}

static void *
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

void 
efsm_destroy (efsm_t *efsm) {

}

void *
efsm_get_user_data (efsm_t * efsm) {

    return efsm->user_data;
}
