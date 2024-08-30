rm -f EventFSM/*.o
rm -f *.o
rm -f *exe

# compile the FSM library
g++ -g -c EventFSM/fsm.cpp -o EventFSM/fsm.o -fpermissive

# Compile the state machine , Action Functions and Entry Exit Functions
g++ -g -c FileDownloaderFSM.cpp -o FileDownloaderFSM.o -fpermissive
g++ -g -c FileDownLoaderFSMActions.cpp -o FileDownLoaderFSMActions.o

# File Downloaders
g++ -g -c FileDownLoader.cpp -o FileDownLoader.o -fpermissive
g++ -g -c HTTP_FileDownLoader.cpp -o HTTP_FileDownLoader.o -fpermissive

# Make Executable
g++ -g -c FileDownloaderFSMTest.cpp -o FileDownloaderFSMTest.o -fpermissive
g++ -g EventFSM/fsm.o \
            FileDownloaderFSM.o \
            FileDownLoaderFSMActions.o \
            FileDownLoader.o \
            HTTP_FileDownLoader.o \
            FileDownloaderFSMTest.o -o exe -lrt -lpthread
