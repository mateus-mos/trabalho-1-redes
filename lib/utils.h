#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

long long int get_file_size(char *path);
char* uint8ArrayToString(uint8_t* array, size_t length);
void get_current_directory(char *buffer, size_t size); 
char *concatenate_strings(const char* str1, const char* str2); 
int file_exists(const char *full_path_to_file);

#endif  // UTILS_H