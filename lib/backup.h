#ifndef BACKUP_H
#define BACKUP_H

#define MAX_FILE_NAME_SIZE 63 
#define BACKUP 0
#define RESTORE 1

int send_single_file(char *src_path, int socket);
int send_multiple_files(char files[][MAX_FILE_NAME_SIZE], int files_quantity, int socket);
void restore_single_file(char *src_path, int socke);
void restore_multiple_files(char files[][MAX_FILE_NAME_SIZE], int files_quantity, int socket);
void set_server_directory(char *dir);
int receive_file(char *file_path, int socket);
int receive_multiple_files(int socket);

void cd_local(char *dir);
int verify_file_md5(char *file_path);

#endif
