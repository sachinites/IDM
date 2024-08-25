
#include <stdbool.h>

typedef struct efsm_ efsm_t;

extern bool fd_action_free_all_resources(efsm_t *efsm) {
    return true;
}

extern bool fd_action_connect(efsm_t *efsm) {
    return true;
}
