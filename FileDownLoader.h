#ifndef __FD__
#define __FD__

#include <string>
#include <pthread.h>

#define FD_REPORT_PROGRESS_ON_CONSOLE  1 
#define FD_DNLOAD_PARTIAL_REQ   2

typedef struct efsm_ efsm_t;

class FD {

    private:
    protected:
        FD();
    public:
        virtual ~FD();

        int sockfd;
        pthread_mutex_t bytes_downloaded_mutex;
        int bytes_downloaded; // if you take it atomic int, then no need of mutex
        int file_size;
        int low_byte;
        int high_byte;
        int flags;
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
        virtual void Pause () = 0;
        virtual void FDReset() = 0;
        virtual void ProgressBar() = 0;
        void SetByteRange (int, int);
        int GetFileSize();
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
        virtual void FDReset() final;
        virtual void Pause () final;
        virtual void ProgressBar() final;
} ;

#endif 