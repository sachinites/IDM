
#ifndef FILE_DOWNLOADER_STATES_H
#define FILE_DOWNLOADER_STATES_H

typedef enum DownLoader_STATES_ {
	
	/* Downloader has been initialized with all required information
	 * such as file name, server name, protocol to use ( HTTP/HTTPS/FTP)
	 */
	DNLOAD_STATE_INIT,
	/* Downloader is downloading the file */ 
	DNLOAD_STATE_IN_PROGRESS,
	/* downloader has suspended downloading the file */
	DNLOAD_STATE_SUSPENDED,
	/* downloader is in attempt to establish TCP connection with the server */
	DNLOAD_STATE_CONNECTION_IN_PROGRESS,
	/* Downloader could not connect to server */
	DNLOAD_STATE_CONNECTION_FAILED,
	/* downloade connection with the server is successful */
	DNLOAD_STATE_CONNECTED,
	/* downloder has finished downloading the file */
	DNLOAD_STATE_DNLOAD_FINISHED,
	/* downloader has cleaned up all its resources */
	DNLOAD_STATE_DONE,
	/* Used for Array index */
	DNLOAD_STATE_MAX

} DNLOADER_STATE_T;

typedef enum DoanLoader_EVENTS_ {

	/* Events issued by User on Dnloader */
	DNLOAD_EVENT_CANCEL,
	DNLOAD_EVENT_PAUSE,
	DNLOAD_EVENT_RESUME,
	DNLOAD_EVENT_START,
	DNLOAD_EVENT_CLEANUP,
	DNLOAD_EVENT_RECONNECT,
	/* Events Issues by Dnloader itself */
	DNLOAD_EVENT_FINISHED,
	DNLOAD_EVENT_LOST_CONNECTION,
	DNLOAD_EVENT_CONNECT_SUCCESS,
	DNLOAD_EVENT_MAX

} DNLOAD_EVENTS_T;

static const char *
file_downloader_state_tostring (state_id_t state_id) {

	DNLOADER_STATE_T state = (DNLOADER_STATE_T)state_id;

	switch (state) {

		case DNLOAD_STATE_INIT:
			return "DNLOAD_STATE_INIT";
		case DNLOAD_STATE_IN_PROGRESS:
			return "DNLOAD_STATE_IN_PROGRESS";
		case DNLOAD_STATE_SUSPENDED:
			return "DNLOAD_STATE_SUSPENDED";
		case DNLOAD_STATE_CONNECTION_IN_PROGRESS:
			return "DNLOAD_STATE_CONNECTION_IN_PROGRESS";
		case DNLOAD_STATE_CONNECTION_FAILED:
			return "DNLOAD_STATE_CONNECTION_FAILED";
		case DNLOAD_STATE_CONNECTED:
			return "DNLOAD_STATE_CONNECTED";
		case DNLOAD_STATE_DNLOAD_FINISHED:
			return "DNLOAD_STATE_DNLOAD_FINISHED";
		case DNLOAD_STATE_DONE:
			return "DNLOAD_STATE_DONE";
		case DNLOAD_STATE_MAX:
			return NULL;
		default:;
	}
	return NULL;
}

static const char *
file_downloader_event_tostring (int event) {

	switch (event) {

		case DNLOAD_EVENT_CANCEL:
			return "DNLOAD_EVENT_CANCEL";
		case DNLOAD_EVENT_PAUSE:
			return "DNLOAD_EVENT_PAUSE";
		case DNLOAD_EVENT_RESUME:
			return "DNLOAD_EVENT_RESUME";
		case DNLOAD_EVENT_START:
			return "DNLOAD_EVENT_START";
		case DNLOAD_EVENT_CLEANUP:
			return "DNLOAD_EVENT_CLEANUP";
		case DNLOAD_EVENT_RECONNECT:
			return "DNLOAD_EVENT_RECONNECT";
		case DNLOAD_EVENT_FINISHED:
			return "DNLOAD_EVENT_FINISHED";
		case DNLOAD_EVENT_LOST_CONNECTION:
			return "DNLOAD_EVENT_LOST_CONNECTION";
		case DNLOAD_EVENT_CONNECT_SUCCESS:
			return "DNLOAD_EVENT_CONNECT_SUCCESS";
		case DNLOAD_EVENT_MAX:
			return "DNLOAD_EVENT_MAX";
		default:;
	}
	return NULL;
}

/* Transition Table */

/* Initial State : DNLOAD_STATE_INIT
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources that were initialized
 * DNLOAD_EVENT_PAUSE      	NA
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	DNLOAD_STATE_CONNECTION_IN_PROGRESS        try to establish connection with server
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE	      		   Free all resources that were initialized
 * DNLOAD_EVENT_RECONNECT  	NA					
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_IN_PROGRESS
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources 
 * DNLOAD_EVENT_PAUSE           DNLOAD_STATE_SUSPENDED			   Suspend the downloder thread	
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	NA
 * DNLOAD_STATE_CLEANUP    	NA
 * DNLOAD_EVENT_RECONNECT  	NA					
 * DNLOAD_EVENT_FINISHED   	DNLOAD_STATE_DNLOAD_FINISHED		   Cancel the downloader thread
 * DNLOAD_EVENT_LOST_CONNECTION DNLOAD_STATE_CONNECTION_FAILED		   cancel the downloder thread
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_SUSPENDED
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources 
 * DNLOAD_EVENT_PAUSE           NA
 * DNLOAD_EVENT_RESUME     	DNLOAD_STATE_CONNECTION_IN_PROGRESS	   connect to server again
 * DNLOAD_EVENT_START      	DNLOAD_STATE_CONNECTION_IN_PROGRESS	   connect to server again
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	NA					
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_CONNECTION_IN_PROGRESS
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources 
 * DNLOAD_EVENT_PAUSE           DNLOAD_STATE_SUSPENDED 			   Suspend the downloder thread
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	NA
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	NA					
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS DNLOAD_STATE_CONNECTED
 *
 * Initial State : DNLOAD_STATE_CONNECTION_FAILED
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources 
 * DNLOAD_EVENT_PAUSE           NA
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	NA
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	DNLOAD_STATE_CONNECTION_IN_PROGRESS	   try reconnect to server					
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_CONNECTED
 *
 * DNLOAD_EVENT_CANCEL     	DNLOAD_STATE_DONE          		   Free all resources 
 * DNLOAD_EVENT_PAUSE           NA
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	DNLOAD_STATE_IN_PROGRESS		   start downloading the file
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	NA
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_DNLOAD_FINISHED
 *
 * DNLOAD_EVENT_CANCEL     	NA
 * DNLOAD_EVENT_PAUSE           NA
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	DNLOAD_STATE_INIT			   Free old resourced, ready do download file again
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	NA
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 *
 * Initial State : DNLOAD_STATE_DONE
 *
 * DNLOAD_EVENT_CANCEL     	NA
 * DNLOAD_EVENT_PAUSE           NA
 * DNLOAD_EVENT_RESUME     	NA
 * DNLOAD_EVENT_START      	NA
 * DNLOAD_STATE_CLEANUP    	DNLOAD_STATE_DONE			   Free all resources
 * DNLOAD_EVENT_RECONNECT  	NA
 * DNLOAD_EVENT_FINISHED   	NA
 * DNLOAD_EVENT_LOST_CONNECTION NA
 * DNLOAD_EVENT_CONNECT_SUCCESS NA
 */ 











#endif 
