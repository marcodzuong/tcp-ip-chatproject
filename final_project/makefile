all: chatclient chatserver

chatclient: chatclient.c chatlinker.c login.c menu.c utils.c
	gcc -g -w chatclient.c  chatlinker.c login.c menu.c utils.c -o chatclient

chatserver: chatserver.c chatlinker.c login.c menu.c utils.c
	gcc -g chatserver.c  chatlinker.c login.c menu.c utils.c -o chatserver
.PHONY:clean
clean:
	rm -f chatclient chatserver
