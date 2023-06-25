#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

long long int get_file_size(char *path);
char* uint8ArrayToString(uint8_t* array, size_t length);

#endif  // UTILS_H