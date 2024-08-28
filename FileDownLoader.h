#ifndef __FD__
#define __FD__

#include <string>
#include <pthread.h>

typedef struct efsm_ efsm_t;

class FD {

    private:
    protected:
        FD();
    public:
        virtual ~FD();

        int sockfd;
        char *read_buffer;
        int read_buffer_size;
        int current_byte;
        int file_size;
        int low_byte;
        int high_byte;
        std::string server_name;
        std::string file_path;
        efsm_t *fsm;
        pthread_t *connector_thread;
        pthread_t *downloader_thread;
        virtual bool HeadConnectServer() = 0;
        virtual void HeadSendGetRequest() = 0;
        void Reset();
};

#define HTTP_READ_BUFFER_SIZE   4096
#define HEAD_CONNECT_TRIES      5
#define HTTP_DEFAULT_PORT       80
#define HEAD_CONNECT_RETRY_INTERVAL 5

class HTTP_FD : public FD { 

    private:
    protected:
    public:

        HTTP_FD(std::string url);
        virtual ~HTTP_FD();

        virtual bool HeadConnectServer( ) final;
        virtual void HeadSendGetRequest() final;
} ;

#endif 