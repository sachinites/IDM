#include <stdio.h>
#include "fsm.h"

bool
dhcp_client_fn_process_offer(efsm_t *efsm) {

    printf ("%s() Called...\n", __FUNCTION__);
    return true;
}

bool
dhcp_client_fn_process_ack(efsm_t *efsm) {

    printf ("%s() Called...\n", __FUNCTION__);
    return true;
}

bool
dhcp_client_fn_send_discover(efsm_t *efsm) {

    printf ("%s() Called...\n", __FUNCTION__);
    return true;
}

bool
dhcp_client_fn_send_request(efsm_t *efsm) {

    printf ("%s() Called...\n", __FUNCTION__);
    return true;
}