rm -f EventFSM/*.o
rm -f *.o
rm -f *exe
g++ -g -c EventFSM/fsm.cpp -o EventFSM/fsm.o -fpermissive
g++ -g -c FileDownLoader.c -o FileDownLoader.o -fpermissive
g++ -g -c FileDownloaderFSM.c -o FileDownloaderFSM.o -fpermissive
g++ -g -c FileDownloaderFSMTest.c -o FileDownloaderFSMTest.o -fpermissive
g++ -g EventFSM/fsm.o FileDownLoader.o FileDownloaderFSM.o FileDownloaderFSMTest.o -o exe -lrt -lpthread

