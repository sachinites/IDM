
#include <stdbool.h>

typedef struct efsm_ efsm_t;

extern bool fd_action_free_all_resources(efsm_t *efsm) {
    return true;
}

extern bool fd_action_connect(efsm_t *efsm) {
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
