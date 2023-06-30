#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include "../lib/socket.h"
#include "../lib/network.h"
#include "../lib/backup.h"
#include "../lib/utils.h"
#include "../lib/log.h"
#define PATH_MAX 100

void change_directory(char *token, const char delimiter[]);
void process_command(char files_names[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int type_flag, int sockfd);
void print_commands();
char *current_dir;



int main()
{
    int sockfd = create_socket("enp3s0");

    char input[100];
    char *token;
    char files_names[MAX_NUM_FILES][MAX_FILE_NAME_SIZE];
    const char delimiter[] = " \n";
    current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
    get_current_directory(current_dir, MAX_DATA_SIZE);

    #include <stdio.h>

    printf("Welcome to the Backup System!\n\n");
    printf("Available commands:\n");
    print_commands();
    printf("\n");

    while (1) 
    {
        printf("\nclient:%s$ ",current_dir);
        fgets(input, sizeof(input), stdin);

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }
        if (strcmp(token, "backup") == 0)
        {
            process_command(files_names, token, delimiter, BACKUP, sockfd);
        }
        else if(strcmp(token, "restore") == 0)
        {
            process_command(files_names, token, delimiter, RESTORE,sockfd);
        }
        else if(strcmp(token, "ssdir") == 0)
        {
            process_command(files_names, token, delimiter, SET_SERVER_DIR,sockfd);
        }
        else if(strcmp(token, "verify") == 0)
        {
            process_command(files_names, token, delimiter, VERIFY, sockfd);
        }
        else if(strcmp(token, "cd") == 0)
        {
            change_directory(token, delimiter);
            get_current_directory(current_dir, MAX_DATA_SIZE);
        }
        else if(strcmp(token, "ls") == 0)
        {
            list_files(current_dir);
        }
        else if(strcmp(token, "help") == 0)
        {
            print_commands();
        }
        else if(strcmp(token, "exit") == 0)
        {
            printf("Goodbye!\n");
            return 0;
        }
        else 
        {
            printf("--> Unsupported command: %s\n", token);
        }
    }

    return 0;
}


void change_directory(char *token, const char delimiter[]) 
{
    token = strtok(NULL, delimiter);

    if(chdir(token) != 0)
    {
        printf(" The directory doesn't exist!\n");
    }
}


void process_command(char files_names[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int type_flag, int sockfd)
{
    token = strtok(NULL, delimiter);

    if (token == NULL)
    {
        printf("--> Please provide arguments for the command.\n");
        return;
    }

    // Check if it is a multi-file command
    if (strcmp(token, "-m") == 0)
    {
        /* Save all files to backup in a matrix */
        int i = 0;
        token = strtok(NULL, delimiter);
        while (token != NULL) {
            strcpy(files_names[i], token);
            token = strtok(NULL, delimiter);
            i++;
        }

        if (i == 0)
        {
            printf("Please provide arguments for the command.\n");
        }
        else if(type_flag == BACKUP)
        {
            send_multiple_files(files_names, i, sockfd);
        }
        else if(type_flag == RESTORE)
        {
            restore_multiple_files(files_names, i, sockfd);
        }
    } 
    else if (strcmp(token, "-w") == 0)
    {
        /* Save all files to backup in a matrix */
        token = strtok(NULL, delimiter);

        if(token == NULL)
        {
            printf("Please provide arguments for the command.\n");
            return;
        }

        int files_quantity = get_files(token, files_names);

        if(files_quantity == 0)
        {
            printf("Couldn't find any files!");
            return;
        }

        if(type_flag == RESTORE)
        {
            restore_multiple_files(files_names, files_quantity, sockfd);
        }
        else if (type_flag == BACKUP)
        {
            send_multiple_files(files_names, files_quantity, sockfd);
        }
    }
    else
    {
        if(type_flag == BACKUP)
            send_single_file(token, sockfd); // (token, token)?
        else if(type_flag == RESTORE)
            restore_single_file(token, sockfd); 
        else if(type_flag == VERIFY)
            verify_file_md5(token, sockfd);
        else if(type_flag == SET_SERVER_DIR)
            set_server_directory(token, sockfd);
    }
}


void print_commands()
{
    printf("- backup <filename>: Backup single file.\n");
    printf("- backup -m <filename> <filename> ... : Backup multiples files.\n");
    printf("- backup -w *.txt ... : Backup all the .txt on the current dir. Works with other files too!\n");
    printf("- restore <filename>: Restore single file.\n");
    printf("- restore -m <filename> <filename> ...: Restore multiples files.\n");
    printf("- restore -w *.txt ... : Restore all the .txt on the current dir server.\n");
    printf("- cd <comand>: cd local!\n");
    printf("- ssdir <comand>: cd command to server!\n");
    printf("- help: show available commands!\n");
    printf("- exit: Exit the program.\n");
}