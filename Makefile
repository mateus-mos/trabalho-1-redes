VPATH = src:lib
OBJ_DIR := obj
BIN_DIR := bin

all: client server

client: client.o socket.o
		gcc -Wall obj/client.o obj/socket.o -o $(BIN_DIR)/client

server: server.o socket.o
		gcc -Wall obj/server.o obj/socket.o -o $(BIN_DIR)/server

server.o: server.c socket.o
		gcc -c src/server.c -o obj/server.o

client.o: client.c socket.c | $(OBJ_DIR) $(BIN_DIR)
		gcc -c src/client.c -o obj/client.o

socket.o: socket.h
		gcc -c src/socket.c -o obj/socket.o

$(OBJ_DIR) $(BIN_DIR):
		mkdir -p $@

clean:
		-rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
