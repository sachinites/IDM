#ifndef __E_FSM__
#define __E_FSM__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>

typedef uint8_t state_id_t;
typedef uint8_t fsm_event_id_t;

#define TT_TABLE_MAX_SIZE   10

typedef struct efsm_state_ efsm_state_t;
typedef struct efsm_ efsm_t;

typedef bool (*action_fn)(efsm_t *);
#define FSM_NO_ACTION ((action_fn)NULL)
#define FSM_NO_STATE_TRANSITION ((efsm_state_t *)NULL)
#define STATE_EVENT_TT_ENTRY(action_fn, next_state) {action_fn, next_state}

typedef struct transition_table_entry_ {

    action_fn action_fn_cbk;
    efsm_state_t *next_state;
}  transition_table_entry_t; 

typedef struct transition_table_ {

    const transition_table_entry_t (*tte_array)[TT_TABLE_MAX_SIZE];

} transition_table_t;

struct efsm_state_ {

    state_id_t id;
    bool final_state;
    bool (*entry_fn)(efsm_t *);
    bool (*exit_fn)(efsm_t *);
    transition_table_t trans_table;
};

struct efsm_ {

    efsm_state_t *initial_state;
    int event_triggered;
    efsm_state_t *current_state;

    void *user_data;

    /* Helper Callbacks for logging */
    const char * (*state_print)(state_id_t);
    const char *(*event_print)(int event);

    /* Event Submission Named Queue*/   
    int pipefd[2]; 

    /* Zero semaphore to implement synchornous calls */
    sem_t zsem; 
};

/* APIs */
efsm_t *efsm_new (void *user_data);

void efsm_destroy (efsm_t *fsm);

void 
efsm_start_event_listener (efsm_t *efsm);

void
efsm_start_event_listener (efsm_t *efsm);

void efsm_fire_event (efsm_t *efsm, int event);
void efsm_fire_event_synchronous (efsm_t *efsm, int event);

void *
efsm_get_user_data (efsm_t * efsm);

#endif 
