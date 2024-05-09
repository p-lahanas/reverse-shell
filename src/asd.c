#include <rtp/asd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

AsdPacket *asd_create_packet(msg_type type, char *command) {
  AsdPacket *pack = (AsdPacket *)calloc(1, sizeof(AsdPacket));

  pack->header.msg_type = type;
  pack->header.command_size =
      (sizeof(char) * strnlen(command, MAX_COMMAND_LEN));
  pack->command = command;

  return pack;
}

/* Send an asd packet and wait for an acknowledgement
    Returns: 0 on success, otherwise -1 */
int asd_send_packet(msg_type type, char *command, int sfd,
                    struct sockaddr_in dest_addr) {
  int nbytes;
  AsdPacket *pack;

  /* Create our packet */
  pack = asd_create_packet(type, command);

  /* Send the header */
  nbytes = sendto(sfd, pack, sizeof(AsdHeader), 0,
                  (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  printf("sent %d bytes\n", nbytes);
  if (nbytes == -1) {
    return -1;
  }
  printf("sent %d bytes\n", nbytes);
  /* Send the rest of the packet */
  nbytes = sendto(sfd, pack->command, pack->header.command_size, 0,
                  (struct sockaddr *)&dest_addr, sizeof(dest_addr));

  if (nbytes == -1) {
    return -1;
  }

  free(pack);

  return 0;
}

/* Receives a packet and stores the */
// int asd_receive_packet(AsdPacket *pack, char *buff, ) {}
