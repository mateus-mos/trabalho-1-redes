#ifndef UTILS_H
#define UTILS_H
#define _POSIX_C_SOURCE 2
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "../lib/backup.h"

#define READ_BUFFER_SIZE 100
#define MD5_DIGEST_LENGTH 100

long long int get_file_size(char *path);
char* uint8ArrayToString(uint8_t* array, size_t length);
void get_current_directory(char *buffer, size_t size); 
char *concatenate_strings(const char* str1, const char* str2); 
int file_exists(const char *full_path_to_file);
void save_file_info(char *file_name, char *file_path, char *output_file); 
char *get_file_path(char *log_file, char *file_name);
void list_files(const char* directory);
int file_to_md5(const char* path, char* md5); 
int get_files(const char* pattern, char file_names[][MAX_FILE_NAME_SIZE]);
int create_new_directory(char* directory_name);


#endif  // UTILS_H