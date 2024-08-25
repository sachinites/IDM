rm *o
rm *exe
g++ -g -c timerlib.c -o timerlib.o
g++ -g -c timerlib_test.c -o timerlib_test.o
g++ -g timerlib.o timerlib_test.o -o exe -lrt
g++ -g -c WheelTimer.c -o WheelTimer.o
g++ -g -c WheelTimerDemo.c -o WheelTimerDemo.o
g++ -g -c gluethread/glthread.c -o gluethread/glthread.o
g++ -g WheelTimerDemo.o WheelTimer.o timerlib.o gluethread/glthread.o -o WheelTimerDemo.exe -lrt
