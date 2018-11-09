file: main.o file.o
	gcc file.o main.o -o file
main.o: main.c
	gcc -c main.c -o main.o
file.o: file.c file.h
	gcc -c file.c -o file.o
clean:
	rm *.o file
