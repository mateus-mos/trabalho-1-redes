#include "../lib/utils.h"

int file_info_exists(const char *output_file, const char *file_name, const char *file_path);

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

int file_info_exists(const char *output_file, const char *file_name, const char *file_path) {
    FILE *fptr;
    char line[1024];
    int found = 0;

    fptr = fopen(output_file, "r");
    if (fptr == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fptr)) {
        if (strstr(line, file_name) || strstr(line, file_path)) {
            found = 1;
            break;
        }
    }

    fclose(fptr);
    return found;
}

void save_file_info(char *file_name, char *file_path, char *output_file) {
    FILE *fptr;

    if (file_info_exists(output_file, file_name, file_path)) {
        printf("File information already saved.\n");
        return;
    }

    // Open the output file in append mode
    fptr = fopen(output_file, "a");
    if (fptr == NULL) {
        printf("Error opening output file!\n");
        exit(1);
    }

    // Write the file name and file path to the output file
    fprintf(fptr, "%s:%s\n", file_name, file_path);

    // Close the output file
    fclose(fptr);

    printf("File information saved successfully.\n");
}

char *get_file_path(char *log_file, char *file_name) {
    FILE *fptr;
    char line[1024];
    char *found = NULL;

    fptr = fopen(log_file, "r");
    if (fptr == NULL) {
        printf("Error opening log file!\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), fptr)) {
        if ((found = strstr(line, file_name)) != NULL) {
            break;
        }
    }

    fclose(fptr);

    if (found) {
        char *delimiter = strchr(found, ':');
        if (delimiter) {
            char *file_path = (char *)malloc(strlen(delimiter + 1) + 1);
            strcpy(file_path, delimiter + 1);
            file_path[strlen(delimiter + 1)] = '\0';
            return file_path;
        }
    }

    return NULL;
}