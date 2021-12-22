all: chatclient chatserver

chatclient: chatclient.c chatlinker.c login.c menu.c
	gcc -g -w chatclient.c  chatlinker.c login.c menu.c -o chatclient

chatserver: chatserver.c chatlinker.c login.c menu.c
	gcc -g chatserver.c  chatlinker.c login.c menu.c -o chatserver
.PHONY:clean
clean:
	rm -f chatclient chatserver
