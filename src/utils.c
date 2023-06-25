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