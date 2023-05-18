VPATH = src:lib

all: client server

client: client.o socket.o
	gcc -Wall obj/client.o obj/socket.o -o ./bin/client

server: server.o socket.o
	gcc -Wall obj/server.o obj/socket.o -o ./bin/server

server.o: server.c  socket.o
	gcc -c src/server.c -o obj/server.o

client.o: client.c socket.c 
	gcc -c src/client.c -o obj/client.o

socket.o: socket.h
	gcc -c src/socket.c -o obj/socket.o

clean:
	-rm -f ./obj/* ./bin/* 