CC=gcc
.PHONY:clean mv
mv:all
	mv client Client/
	mv server Server/
all:client server

server:server.c server_fun.c
	${CC} server_fun.c server.c -g -o $@
client:client.c client_fun.c
	${CC} client.c client_fun.c -g -o $@ -lpthread
clean:
	-rm Client/client Server/server
