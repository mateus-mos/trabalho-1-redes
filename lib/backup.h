#ifndef BACKUP_H
#define BACKUP_H

#define MAX_NUM_FILES 1023
#define MAX_FILE_NAME_SIZE 63 
#define BACKUP 0
#define RESTORE 1
#define SET_SERVER_DIR 2
#define VERIFY 3

int send_single_file(char *file_name, int socket);

void restore_multiple_files(char files_names[][MAX_FILE_NAME_SIZE], int files_quantity, int socket);
int send_multiple_files(char files[][MAX_FILE_NAME_SIZE], int files_quantity, int socket);
void restore_single_file(char *file_name, int socket);
void set_server_directory(char *dir_name, int socket);
int receive_file(char *file_path,  int socket);
int receive_multiple_files(int socket);

void cd_local(char *dir);
void verify_file_md5(char *file_path, int socket);

#endif
