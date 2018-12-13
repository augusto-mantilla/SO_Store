#
#
#	Makefile for test programs
#
#


all: monitor simulador

monitor: monitor.o util.o util-stream-client.o
	gcc -o monitor monitor.o util.o util-stream-client.o 

simulador: simulador.o util.o util-stream-server.o
	gcc -o simulador simulador.o util.o util-stream-server.o 

clean:
	rm -f *.o *~ monitor simulador


