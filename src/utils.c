#include "../lib/utils.h"
#include <dirent.h>
#include <openssl/evp.h>
#define BUFSIZE 1024

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


int get_files(const char* pattern, char file_names[][MAX_FILE_NAME_SIZE]) {
    char command[MAX_FILE_NAME_SIZE + 50];
    FILE *fp;
    int count = 0;

    //creating the command string
    sprintf(command, "ls %s 2> /dev/null", pattern);

    //execute the command
    fp = popen(command, "r");
    if (fp == NULL) {
        //popen failed
        return 0;
    }

    //reading the output and storing in file_names
    while (fgets(file_names[count], MAX_FILE_NAME_SIZE, fp) != NULL && count < MAX_NUM_FILES) {
        //remove the newline at the end
        file_names[count][strcspn(file_names[count], "\n")] = 0; 
        count++;
    }
    
    //closing the file pointer
    pclose(fp);
    
    return count;
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

void list_files(const char* directory) {
    DIR* dir;
    struct dirent* entry;

    // Open the directory
    dir = opendir(directory);

    if (dir == NULL) {
        printf("Error opening directory.\n");
        return;
    }

    // Store file names in an array
    char files[100][256];
    int fileCount = 0;

    // Iterate over directory entries
    while ((entry = readdir(dir)) != NULL) {
        sprintf(files[fileCount], "%s", entry->d_name);
        fileCount++;
    }

    // Calculate number of columns
    int numColumns = 3;  // Adjust the number of columns as desired

    // Calculate number of rows
    int numRows = (fileCount + numColumns - 1) / numColumns;

    // Print files in columns
    for (int i = 0; i < numRows; i++) {
        for (int j = i; j < fileCount; j += numRows) {
            printf("%-30s", files[j]);  // Adjust the spacing as desired
        }
        printf("\n");
    }

    // Close the directory
    closedir(dir);
}

int file_to_md5(const char* path, char* md5) {
    FILE* file = fopen(path, "rb");
    if(!file) return -1;

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("md5");

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);

    unsigned char data[BUFSIZE];
    size_t bytes;
    while((bytes = fread(data, 1, BUFSIZE, file)) != 0)
        EVP_DigestUpdate(mdctx, data, bytes);

    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for(i = 0; i < md_len; i++)
        sprintf(&md5[i*2], "%02x", md_value[i]);

    fclose(file);
    return 0;
}

int create_new_directory(char* directory_name) {
    
    int result = mkdir(directory_name, 7777); // 0777 provides read, write, and execute permissions to everyone
    
    return result;
}