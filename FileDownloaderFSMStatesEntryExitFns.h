#ifndef  _FILEDOWNLOADERFSMSTATESENTRYEXITFNS_H
#define _FILEDOWNLOADERFSMSTATESENTRYEXITFNS_H

typedef struct efsm_ efsm_t;

bool 
file_dnloader_DNLOAD_STATE_HEAD_CONNECT_IN_PROGRESS_exit_fn(efsm_t *efsm) ;

bool 
file_dnloader_DNLOAD_STATE_HEAD_GET_RESPONSE_AWAIT_exit_fn(efsm_t *efsm) ;

bool 
file_downloader_DNLOAD_STATE_FD_CONNECT_IN_PROGRESS_exit_fn(efsm_t *efsm) ;

#endif //  _FILEDOWNLOADERFSMSTATESENTRYEXITFNS_H
