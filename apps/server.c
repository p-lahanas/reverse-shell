#include <asd/asd.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVPORT 5432

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
  serv_addr.sin_addr.s_addr = INADDR_ANY; // Accept any IP
  memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

  // bind socket to the specified IP and port
  if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  AsdMsg *pack;
  ssize_t nbytes;

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);

  int run = 1;
  while (run) {
    pack = asd_recv_command(sfd, (struct sockaddr *)&recv_addr);
    if (pack == NULL) {
      fprintf(stderr, "Receive Error\n");
      continue;
    }

    switch (pack->header.type) {

    case ASD_STOP:
      run = 0;
      break;

    case ASD_TEST:
      break;

    case ASD_RUN:
      /* Blindly run the command */
      if (system(pack->cmd) == -1) {
        fprintf(stderr, "Command failed: %s\n", pack->cmd);
      }
      break;

    case ASD_ACK:
      break;
    }
    /* Free packet and cmd buffer */
    free(pack->cmd);
    free(pack);

    /* Send out an ack */
    if (asd_send_ack(sfd, (struct sockaddr *)&recv_addr) != 0) {
      fprintf(stderr, "Error sending ACK\n");
      continue;
    }
  }

  close(sfd);

  return 0;
}
