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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/socket.h"
#include "../lib/network.h"
#include "../lib/backup.h"
#include "../lib/utils.h"

void process_command(char files_names[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int type_flag, int sockfd);
char *current_dir;

int main()
{
    int sockfd = create_socket("eno1");

    char input[100];
    char *token;
    char files_names[100][MAX_FILE_NAME_SIZE];
    const char delimiter[] = " \n";
    current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
    get_current_directory(current_dir, MAX_DATA_SIZE);

    printf("__________________Client Terminal__________________\n");
    printf("--> backup <file_name> \n");
    printf("--> backup -m <file_name_1> <file_name_2> ... <file_name_n> \n");
    printf("--> restore <file_name> \n");
    printf("--> restore -m <file_name_1> <file_name_2> ... <file_name_n> \n");


    while (1) 
    {
        printf("client: ");
        fgets(input, sizeof(input), stdin);

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }

        printf("\nTOKEN: %s\n", token);

        if (strcmp(token, "backup") == 0) // OR?
            process_command(files_names, token, delimiter, BACKUP, sockfd);
        else if(strcmp(token, "restore") == 0)
            process_command(files_names, token, delimiter, RESTORE,sockfd);
        else if(strcmp(token, "ssdir") == 0)
            process_command(files_names, token, delimiter, SET_SERVER_DIR,sockfd);
        else 
            printf("--> Unsupported command: %s\n", token);
    }

    return 0;
}

void process_command(char files_names[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int type_flag, int sockfd)
{
    token = strtok(NULL, delimiter);

    if (token == NULL)
    {
        printf("--> Please provide arguments for the command.\n");
    }

    // Check if it is a multi-file command
    if (strcmp(token, "-m") == 0)
    {
        printf("--> Multi-file command.\n");

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
            printf("--> Please provide arguments for the command.\n");
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
    else
    {
        printf("Single-file command: %s\n", token); // can be a "cd"
        if(type_flag == BACKUP)
            send_single_file(token, token, sockfd); // (token, token)?
        else if(type_flag == RESTORE)
            restore_single_file(token, current_dir, sockfd); 
        else if(type_flag == SET_SERVER_DIR)
            set_server_directory(token, sockfd);
    }
}