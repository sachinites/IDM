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
        virtual void FileDownloadConnectServer() = 0;
        virtual void FileDownload() = 0;
        virtual void CleanupDnloadResources() = 0;
        virtual void AssembleChunks() = 0;
        virtual void Cancel () = 0;
        virtual void Pause () = 0;
        virtual void ProgressBar() = 0;
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
        virtual void FileDownloadConnectServer() final;
        virtual void FileDownload() final;
        virtual void CleanupDnloadResources() final;
        virtual void AssembleChunks() final;
        virtual void Cancel () final;
        virtual void Pause () final;
        virtual void ProgressBar() final;
} ;

#endif 