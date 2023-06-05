VPATH = src:lib
OBJ_DIR := obj
BIN_DIR := bin
FLAGS := -Wall -Wextra -std=c99 


all: client server

debug: FLAGS += -DDEBUG -g
debug: all

client: client.o socket.o communication.o backup.o
		gcc $(FLAGS) obj/log.o obj/backup.o obj/client.o obj/socket.o obj/communication.o -o $(BIN_DIR)/client -lm

server: server.o socket.o communication.o backup.o log.o
		gcc $(FLAGS) obj/server.o obj/socket.o obj/communication.o obj/backup.o obj/log.o -o $(BIN_DIR)/server -lm

server.o: server.c
		gcc $(FLAGS) -c src/server.c -o obj/server.o

client.o: client.c | $(OBJ_DIR) $(BIN_DIR)
		gcc $(FLAGS) -c src/client.c -o obj/client.o 

socket.o: socket.h
		gcc $(FLAGS) -c src/socket.c -o obj/socket.o

communication.o: communication.h
		gcc $(FLAGS) -c src/communication.c -o obj/communication.o

backup.o: backup.h log.o
		gcc $(FLAGS) -c src/backup.c -o obj/backup.o  -lm

log.o: log.h
		gcc $(FLAGS) -c src/log.c -o obj/log.o 

$(OBJ_DIR) $(BIN_DIR):
		mkdir -p $@

clean:
		-rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
