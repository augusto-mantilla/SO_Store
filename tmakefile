#The compiler to use will be CC represents the variable we will use.
CC=gcc
#CFLAGS will be the options we will pass to the compiler.
CFLAGS=-c -Wall

all: file

file: main.o file.o
	$(CC) file.o main.o -o file
main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o
file.o: file.c file.h
	$(CC) $(CFLAGS) file.c -o file.o
clean:
	rm *.o file
