CFLAGS = -c -Wall
CC = gcc

all: main

main: main.o login.o
	${CC}  login.o main.o -o main

main.o: main.c
	${CC} ${CFLAGS} main.c

login.o: login.c
	${CC} ${CFLAGS} login.c

clean:
	rm -f *.o *~