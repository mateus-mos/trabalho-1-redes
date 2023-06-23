#ifndef BACKUP_H
#define BACKUP_H

int send_single_file(const char *src_path, int socket);
int send_multiple_files(const char files[][100], int files_quantity, int socket);
void restore_single_file(const char *src_path, int socke);
void restore_multiple_files(const char *src_dir, int socket);
void set_server_directory(const char *dir);
int receive_file(const char *file_path, int socket);
int receive_multiple_files(int socket);

void cd_local(const char *dir);
int verify_file_md5(const char *file_path);

#endif
