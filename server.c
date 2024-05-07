#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVPORT 5432

#define BUF_SIZE 500

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
    close(sfd);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  char buf[BUF_SIZE];
  ssize_t nbytes;
  struct sockaddr_in recv_addr;
  socklen_t addr_len;

  while (1) {
    nbytes = recvfrom(sfd, buf, BUF_SIZE - 1, 0, (struct sockaddr *)&recv_addr,
                      &addr_len);

    if (nbytes == -1) {
      continue;
    }

    printf("Packet is of length: %ld bytes\n", nbytes);
    buf[nbytes] = '\0';
    printf("packet contains \"%s\"\n", buf);
    fflush(stdout);
    break;
  }
  close(sfd);

  return 0;
}
