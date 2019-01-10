#
#
#	Makefile for test programs
#
#


all: monitor simulador

monitor: monitor.o util.o util-stream-client.o file.o queue.o
	gcc -o monitor monitor.o util.o file.o util-stream-client.o -lm

simulador: simulador.o util.o util-stream-server.o file.o queue.o
	gcc -o simulador simulador.o queue.o file.o util.o util-stream-server.o -lpthread -lm

clean:
	rm -f *.o *~ monitor simulador


