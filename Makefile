VPATH = src:lib
OBJ_DIR := obj
BIN_DIR := bin
FLAGS := -Wall -Wextra -std=c99 


all: client server

debug: FLAGS += -DDEBUG -g
debug: all

client: client.o socket.o network.o backup.o utils.o
		gcc $(FLAGS) obj/log.o obj/backup.o obj/client.o obj/socket.o obj/network.o obj/utils.o -o $(BIN_DIR)/client -lm

server: server.o socket.o network.o backup.o log.o utils.o
		gcc $(FLAGS) obj/server.o obj/socket.o obj/network.o obj/backup.o obj/log.o obj/utils.o -o $(BIN_DIR)/server -lm

server.o: server.c
		gcc $(FLAGS) -c src/server.c -o obj/server.o

client.o: client.c | $(OBJ_DIR) $(BIN_DIR)
		gcc $(FLAGS) -c src/client.c -o obj/client.o 

socket.o: socket.h
		gcc $(FLAGS) -c src/socket.c -o obj/socket.o

network.o: network.h
		gcc $(FLAGS) -c src/network.c -o obj/network.o

backup.o: backup.h 
		gcc $(FLAGS) -c src/backup.c -o obj/backup.o  -lm

utils.o: utils.h
		gcc $(FLAGS) -c src/utils.c -o obj/utils.o

log.o: log.h
		gcc $(FLAGS) -c src/log.c -o obj/log.o 

$(OBJ_DIR) $(BIN_DIR):
		mkdir -p $@

clean:
		-rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
