VPATH = src:lib
OBJ_DIR := obj
BIN_DIR := bin
FLAGS := -Wall -Wextra -Werror -std=c99


all: client server

debug: FLAGS += -DDEBUG -g
debug: all

client: client.o socket.o communication.o
		gcc $(FLAGS) obj/client.o obj/socket.o obj/communication.o -o $(BIN_DIR)/client

server: server.o socket.o communication.o
		gcc $(FLAGS) obj/server.o obj/socket.o obj/communication.o -o $(BIN_DIR)/server

server.o: server.c
		gcc $(FLAGS) -c src/server.c -o obj/server.o

client.o: client.c | $(OBJ_DIR) $(BIN_DIR)
		gcc $(FLAGS) -c src/client.c -o obj/client.o

socket.o: socket.h
		gcc $(FLAGS) -c src/socket.c -o obj/socket.o

communication.o: communication.h
		gcc $(FLAGS) -c src/communication.c -o obj/communication.o

$(OBJ_DIR) $(BIN_DIR):
		mkdir -p $@

clean:
		-rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
