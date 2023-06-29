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
char *current_dir;


int main()
{
    int sockfd = create_socket("enp3s0");

    char input[100];
    char *token;
    char files_names[100][MAX_FILE_NAME_SIZE];
    const char delimiter[] = " \n";
    current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
    get_current_directory(current_dir, MAX_DATA_SIZE);

    #include <stdio.h>

    printf("Welcome to the Backup System!\n\n");
    printf("Available commands:\n");
    printf("- backup <filename>: Backup single file.\n");
    printf("- backup -m <filename> <filename> ... : Backup multiples files.\n");
    printf("- restore <filename>: Restore single file.\n");
    printf("- restore -m <filename> <filename> ...: Restore multiples files.\n");
    printf("- help: Show available commands.\n");
    printf("- exit: Exit the program.\n");
    printf("\n");

    while (1) 
    {
        printf("client:%s$ ",current_dir);
        fgets(input, sizeof(input), stdin);

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }

        if (strcmp(token, "backup") == 0) // OR?
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
        else if(strcmp(token, "exit") == 0)
        {
            printf("Goodbye!\n");
            return 0;
        }
        else 
            printf("--> Unsupported command: %s\n", token);
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
            struct packet *packet = create_or_modify_packet(NULL, 0, 0, PT_RESTORE_FILES, NULL);
            if(send_packet_and_wait_for_response(packet, packet, PT_TIMEOUT, sockfd))
            {
                log_message("Timeout error!");
                return;
            }

            for(int j = 0; j < i; j++)
            {
                restore_single_file(files_names[j], current_dir, sockfd);
            }

            create_or_modify_packet(packet, 0, 0, PT_END_GROUP_FILES, NULL);
            if(send_packet_and_wait_for_response(packet, packet, PT_TIMEOUT, sockfd))
            {
                log_message("Timeout error!");
                return;
            }
            //restore_multiple_files(files_names, i, sockfd);
        }
    } 
    else
    {
        if(type_flag == BACKUP)
            send_single_file(token, sockfd); // (token, token)?
        else if(type_flag == RESTORE)
            restore_single_file(token, token, sockfd); 
        else if(type_flag == VERIFY)
            verify_file_md5(token, sockfd);
        else if(type_flag == SET_SERVER_DIR)
            set_server_directory(token, sockfd);
    }
}