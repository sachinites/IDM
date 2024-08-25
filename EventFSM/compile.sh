rm *o
rm *exe
g++ -g -c fsm.cpp -o fsm.o
g++ -g -c dhcp_client_pkt.cpp -o dhcp_client_pkt.o
g++ -g -c dhcp_client_fsm.cpp -o dhcp_client_fsm.o
cd libtimer
sh compile.sh
cd ..
g++ -g fsm.o dhcp_client_pkt.o dhcp_client_fsm.o libtimer/WheelTimer.o libtimer/timerlib.o libtimer/gluethread/glthread.o -o dhcp_client_fsm.exe -lrt -lpthread