#ifndef __FD__
#define __FD__

class FD {

    private:
    protected:
        FD();
    public:
        virtual ~FD();
};

class HTTP_FD : public FD { 

    private:
    protected:
    public:
        int sockfd;
        char *read_buffer;
        int read_buffer_size;
        int current_byte;
        int file_size;
        int low_byte;
        int high_byte;
        char server_name[128];
        char file_path[512];

        HTTP_FD();
        virtual ~HTTP_FD();
} ;

#endif 