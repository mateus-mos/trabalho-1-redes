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
#include "../lib/socket.h"
#include "../lib/communication.h"
#include "../lib/backup.h"

#define DEST "127.0.0.1"
#define PORT 27015

int main() {
        
    printf("Creating socket...\n");
    int sockfd = create_socket("lo");
    printf("Socket created!\n");

    // char command[100];
    // printf("---------- Client Terminal ----------\n");
    // printf("$: ");
    // scanf("%99s", command);
    
    // while (strcmp(command, "exit")){
    //     if(!strcmp(command,"cd")){
            
    //     } else if (!strcmp(command,"backup")){
            
    //     } else if (!strcmp(command,"restore")){
            
    //     } else if(!strcmp(command,"set server dir")){
            
    //     }

    //     printf("$: ");
    //     scanf("%99s", command);

    // }
    
    /* Printar um terminal */
    /* Receber input de um comando */
    /* Executar um comando */
    /* Esperar outro comando*/

    /* Comandos:
     * backup <file>
     * backup <dir>
     * restore <file>
     * restore <dir>
     * set_server_dir <dir>
     * cd_local <dir>
     * verify_file_md5 <file>
     * exit
    */
   /* Printar erros:
    * Digitar comando errado
    * Arquivo não existe
    * Diretório não existe
    * Diretório não pode ser acessado
    * Servidor indisponível
   */

    backup_single_file("/home/mateus/Documents/BCC/trabalho-1-redes/helloworld.txt", sockfd);

    close(sockfd);

    return 0;
}