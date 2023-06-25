#include "../lib/utils.h"

/* Get the size of a file in bytes. 
 * 
 * @param path The path of the file.
*/
long long int get_file_size(char *path) 
{
    struct stat st;
    if(stat(path, &st) == -1) 
    {
        perror("Could not get file size!");
        return -1;
    }
    return st.st_size;
}

/**
 * @brief  an array of uint8_t to a string.
 * 
 * @param array The array to be converted.
 * @param length The length of the array.
 * 
 * @return The converted string.
*/
char* uint8ArrayToString(uint8_t* array, size_t length) {
    char* str = (char*)malloc((length + 1) * sizeof(char));  // Allocate memory for the string
    
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    
    for (size_t i = 0; i < length; i++) {
        str[i] = (char)array[i];  // Convert each element to char and store in the string
    }
    
    str[length] = '\0';  // Add the null-terminating character
    
    return str;
}

/**
 * @brief  Get the current directory.
*/
void get_current_directory(char *buffer, size_t size) {
    if (getcwd(buffer, size) == NULL) {
        perror("Error getting current directory");
    }
    // add "/" to the end of the path
    strcat(buffer, "/");
}

/**
 * @brief  Check if a file exists.
 * 
 * @return 1 if the file exists, 0 otherwise.
*/
int file_exists(const char *full_path_to_file) {
    FILE *file = fopen(full_path_to_file, "r");
    if (file != NULL) {
        fclose(file);
        return 1;  // File exists
    }
    return 0;  // File does not exist
}

/**
 * @brief  Concatenate two strings.
 * 
 * @return The concatenated string.
*/
char *concatenate_strings(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = (char*)malloc(len1 + len2 + 1); // +1 for the null terminator

    if (result == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}