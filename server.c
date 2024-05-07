#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "asd.h"

#define SERVPORT 5432

#define BUF_SIZE 200

int main(int argc, char *argv[]) {
  int sfd = socket(PF_INET, SOCK_DGRAM, 0);

  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // define server address
  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVPORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;  // Accept any IP
  memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

  // bind socket to the specified IP and port
  if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  char buf[BUF_SIZE];
  ssize_t nbytes;
  int sbytes;

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);

  struct AsdPacket recv_pack;
  struct AsdPacket *ack;

  int run = 1;
  while (run) {
    nbytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&recv_addr,
                      &addr_len);

    /* Received nothing so skip this iteration */
    if (nbytes == -1) {
      continue;
    }
    memcpy(&recv_pack, buf, sizeof(struct AsdPacket));

    switch (recv_pack.msg_type) {
      case ASD_STOP:
        run = 0;
        break;

      case ASD_TEST:
        break;

      case ASD_RUN:
        /* Blindly run the command */
        system(recv_pack.command);
        break;
    }

    /* Send out an ack */
    ack = create_asd_packet(ASD_ACK, NULL);
    sbytes = sendto(sfd, ack, sizeof(struct AsdPacket), 0,
                    (struct sockaddr *)&recv_addr, addr_len);

    if (sbytes == -1) {
      perror("sendto");
      close(sfd);
      exit(EXIT_FAILURE);
    }
  }

  close(sfd);

  return 0;
}
