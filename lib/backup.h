#ifndef BACKUP_H
#define BACKUP_H

int backup_single_file(const char *src_path, int socket);
void backup_multiple_files(const char *src_dir, int socket);
void restore_single_file(const char *src_path, int socke);
void restore_multiple_files(const char *src_dir, int socket);
void set_server_directory(const char *dir);
void cd_local(const char *dir);
int verify_file_md5(const char *file_path);

#endif
