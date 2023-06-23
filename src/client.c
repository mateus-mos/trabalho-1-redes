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
#include "../lib/communication.h"
#include "../lib/backup.h"

#define DEST "127.0.0.1"
#define PORT 27015

void backup_command(char files_to_backup[][100], char *token, const char delimiter[], int sockfd);

int main() {
    int sockfd = create_socket("eno1");

    char input[100];
    char *token;
    char files_to_backup[100][100];
    const char delimiter[] = " \n";

    printf("__________________Client Terminal__________________\n");
    printf("_> backup <file_name> \n");
    printf("_> backup -m <file_name_1> <file_name_2> ... <file_name_n> \n");


    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }

        if (strcmp(token, "backup") == 0) {
            backup_command(files_to_backup, token, delimiter, sockfd);
        } else {
            printf("_> Unsupported command: %s\n", token);
        }
    }

    return 0;
}

void backup_command(char files_to_backup[][100], char *token, const char delimiter[], int sockfd){
    token = strtok(NULL, delimiter);

    if (token == NULL) {
        printf("_> Please provide arguments for the backup command.\n");
    }

    if (strcmp(token, "-m") == 0) {
        printf("_> Multi-file backup.\n");

        /* Save all files to backup in a matrix */
        int i = 0;
        token = strtok(NULL, delimiter);
        while (token != NULL) {
            strcpy(files_to_backup[i], token);
            token = strtok(NULL, delimiter);
            i++;
        }

        if (i == 0) 
            printf("_> Please provide arguments for the backup command.\n");
        else
            backup_multiple_files(files_to_backup, i, sockfd);

    } else {
        printf("Single-file backup: %s\n", token);
        backup_single_file(token, sockfd);
    }
}

//int main() {
//        
//    printf("Creating socket...\n");
//    int sockfd = create_socket("lo");
//    printf("Socket created!\n");
//
//    char command[50];
//    char option[5];
//    char file_name[30];
//
//    printf("---------- Client Terminal ----------\n");
//    printf("$: ");
//
//    fgets(command, sizeof(command), stdin);
//
//    while (strcmp(command, "exit"))
//    {    
//        if (sscanf(command, "%*s %s %[^\n]", option, file_name) == 2 && strcmp(option, "-s") == 0) 
//        {
//            printf("Comando válido!\n");
//            printf("Nome do arquivo: %s\n", file_name);
//            backup_single_file(file_name, sockfd);
//        } 
//        else 
//        {
//            printf("Comando inválido!\n");
//        }   
//
//        printf("$: ");
//        fgets(command, sizeof(command), stdin);
//    }
//    
//    /* Printar um terminal */
//    /* Receber input de um comando */
//    /* Executar um comando */
//    /* Esperar outro comando*/
//
//    /* Comandos:
//     * backup <file>
//     * backup <dir>
//     * restore <file>
//     * restore <dir>
//     * set_server_dir <dir>
//     * cd_local <dir>
//     * verify_file_md5 <file>
//     * exit
//    */
//   /* Printar erros:
//    * Digitar comando errado
//    * Arquivo não existe
//    * Diretório não existe
//    * Diretório não pode ser acessado
//    * Servidor indisponível
//   */
//
//    // backup_single_file("helloworld.txt",sockfd);
//    close(sockfd);
//
//    return 0;
//}