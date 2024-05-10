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
  fprintf(stderr, "Usage: client [flag] \n");

  fprintf(stderr,
          "Option (only use ONE flag at a time):\n"
          "  -r <command>  Send a run packet with the corresponding <command>\n"
          "  -t            Send a test packet\n"
          "  -s Send a stop packet\n"
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

  int send_err;

  if (tflag) {
    send_err = asd_send_packet(ASD_TEST, NULL, 0, sfd, serv_addr);
  } else if (sflag) {
    send_err = asd_send_packet(ASD_STOP, NULL, 0, sfd, serv_addr);
  } else {
    send_err = asd_send_packet(ASD_RUN, command, strnlen(command, ASD_MAX_CMD),
                               sfd, serv_addr);
  }

  if (send_err != 0) {
    perror("Error sending packet");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  printf("Received Ack\n");

  close(sfd);

  return 0;
}
