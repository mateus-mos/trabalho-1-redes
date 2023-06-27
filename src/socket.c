#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../lib/socket.h"


int create_socket(char *device)
{
  int sock;
  struct ifreq ir;
  struct sockaddr_ll adress;
  struct packet_mreq mr;


  sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  	/*cria socket*/
  if (sock == -1) {
    perror("Error creating socket! Did you run as root?\n");
    exit(-1);
  }

  memset(&ir, 0, sizeof(struct ifreq));  	/*dispositivo eth0*/
  memcpy(ir.ifr_name, device, sizeof(&device));
  if (ioctl(sock, SIOCGIFINDEX, &ir) == -1) {
    printf("Erro no ioctl\n");
    exit(-1);
  }
	

  memset(&adress, 0, sizeof(adress)); 	/*IP do dispositivo*/
  adress.sll_family = AF_PACKET;
  adress.sll_protocol = htons(ETH_P_ALL);
  adress.sll_ifindex = ir.ifr_ifindex;
  if (bind(sock, (struct sockaddr *)&adress, sizeof(adress)) == -1) {
    printf("Erro no bind\n");
    exit(-1);
  }

  memset(&mr, 0, sizeof(mr));          /*Modo Promiscuo*/
  mr.mr_ifindex = ir.ifr_ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)	{
    printf("Erro ao fazer setsockopt\n");
    exit(-1);
  }

  return sock;
}
