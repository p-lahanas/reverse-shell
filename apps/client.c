#include <arpa/inet.h>
#include <asd/asd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVERPORT 5432

/* Command-line flags */
int rflag = 0; /* -r: Run packet */
int tflag = 0; /* -t: Test packet */
int sflag = 0; /* -s: Stock packet */

static void usage(void) {
  fprintf(stderr,
          "Usage: client [flag] \n"
          "Sends an ASD message to the server. Requires exactly ONE flag.\n\n");

  fprintf(stderr,
          "Option (only use ONE flag at a time):\n"
          "  -r <command>  Send a run packet with the corresponding <command>\n"
          "  -t            Send a test packet\n"
          "  -s            Send a stop packet\n"
          "\n");

  exit(EXIT_FAILURE);
}

void parse_args(int argc, char **argv);

int main(int argc, char *argv[]) {
  char ch, *command;

  while ((ch = getopt(argc, argv, "tsr:")) != -1) {
    switch (ch) {
    case 't':
      tflag = 1;
      break;
    case 's':
      sflag = 1;
      break;
    case 'r':
      rflag = 1;
      command = optarg;
      break;
    default:
      usage();
    }
  }

  /* Only want one argument at a time */
  int arg_sum = tflag + sflag + rflag;
  if (arg_sum == 0 || arg_sum > 1) {
    usage();
  }

  int sfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVERPORT);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Loopback
  memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
  AsdHeader head;

  int send_err;

  if (tflag) {
    head.type = ASD_TEST;
    head.cmd_len = 0;
    send_err = asd_send_command(head, NULL, sfd, (struct sockaddr *)&serv_addr);
  } else if (sflag) {
    head.type = ASD_STOP;
    head.cmd_len = 0;
    send_err = asd_send_command(head, NULL, sfd, (struct sockaddr *)&serv_addr);
  } else {
    head.type = ASD_RUN;
    head.cmd_len = strlen(command);
    send_err =
        asd_send_command(head, command, sfd, (struct sockaddr *)&serv_addr);
  }

  if (send_err != 0) {
    fprintf(stderr, "Error sending packet\n");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  AsdMsg *pack;
  pack = asd_recv_command(sfd, (struct sockaddr *)&recv_addr);
  if (pack == NULL || pack->header.type != ASD_ACK) {
    fprintf(stderr, "Didn't receive ACK from server\n");
    exit(EXIT_FAILURE);
  }

  printf("Received ACK from server\n");

  close(sfd);

  return 0;
}
