VPATH = src:lib

all: client server

client: client.o ConexaoRawSocket.o
	gcc -Wall obj/client.o obj/ConexaoRawSocket.o -o ./bin/client

server: server.o ConexaoRawSocket.o
	gcc -Wall obj/server.o obj/ConexaoRawSocket.o -o ./bin/server

server.o: server.c  ConexaoRawSocket.o
	gcc -c src/server.c -o obj/server.o

client.o: client.c ConexaoRawSocket.c 
	gcc -c src/client.c -o obj/client.o

ConexaoRawSocket.o: ConexaoRawSocket.h
	gcc -c src/ConexaoRawSocket.c -o obj/ConexaoRawSocket.o

clean:
	-rm -f ./obj/* ./bin/* 