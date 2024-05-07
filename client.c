#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVERPORT 5432

int main(int argc, char *argv[]) {
  int sfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVERPORT);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Loopback
  memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

  int nbytes;
  char data[255] = "Hello World!";
  nbytes = sendto(sfd, data, strlen(data), 0, (struct sockaddr *)&serv_addr,
                  sizeof(serv_addr));

  if (nbytes == -1) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }

  printf("sent %d bytes to %s\n", nbytes, inet_ntoa(serv_addr.sin_addr));
  close(sfd);

  return 0;
}
