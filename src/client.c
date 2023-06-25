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

#define DEST "127.0.0.1"
#define PORT 27015

void backup_command(char files_to_backup[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int sockfd);

int main() {
    int sockfd = create_socket("lo");

    char input[100];
    char *token;
    char files_to_backup[100][MAX_FILE_NAME_SIZE];
    const char delimiter[] = " \n";

    printf("__________________Client Terminal__________________\n");
    printf("--> backup <file_name> \n");
    printf("--> backup -m <file_name_1> <file_name_2> ... <file_name_n> \n");

    while (1) {
        printf("client: ");
        fgets(input, sizeof(input), stdin);

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }

        if (strcmp(token, "backup") == 0) {
            backup_command(files_to_backup, token, delimiter, sockfd);
        } else {
            printf("--> Unsupported command: %s\n", token);
        }
    }

    return 0;
}

void backup_command(char files_to_backup[][MAX_FILE_NAME_SIZE], char *token, const char delimiter[], int sockfd){
    token = strtok(NULL, delimiter);

    if (token == NULL) {
        printf("--> Please provide arguments for the backup command.\n");
    }

    if (strcmp(token, "-m") == 0) {
        printf("--> Multi-file backup.\n");

        /* Save all files to backup in a matrix */
        int i = 0;
        token = strtok(NULL, delimiter);
        while (token != NULL) {
            strcpy(files_to_backup[i], token);
            token = strtok(NULL, delimiter);
            i++;
        }

        if (i == 0) 
            printf("--> Please provide arguments for the backup command.\n");
        else
            send_multiple_files(files_to_backup, i, sockfd);

    } else {
        printf("Single-file backup: %s\n", token);
        send_single_file(token, sockfd);
    }
}