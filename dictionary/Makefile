CC=gcc
CFLAGS=-Wall 

all:client server

client:client.c client_dict.c client_dict.h
	${CC} ${CFLAGS} client.c client_dict.c -g -o $@

server:server.c server_dict.c server_dict.h
	${CC} ${CFLAGS} server.c server_dict.c -g -o $@ -lsqlite3

.PHONY:clean
clean:
	-rm client server sql.db
