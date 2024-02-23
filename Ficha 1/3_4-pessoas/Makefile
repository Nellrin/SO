CC = gcc
CFLAGS = -Wall -g

all: pessoas
	rm -f person.o pessoas.o

pessoas: pessoas.o person.o

person.o: person.c person.h

pessoas.o: pessoas.c person.h

clean:
	rm -f *.o pessoas file_pessoas pessoas.bin