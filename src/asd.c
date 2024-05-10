#include <asd/asd.h>

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/* Populate the contents of an asd packet.
    Returns: 0 on success, otherwise -1 */
ssize_t asd_fill_packet(AsdPacket *pack, asd_msg_type type, char *cmd,
                        size_t cmd_len) {

  pack->type = type;
  if (cmd_len >= ASD_MAX_CMD) {
    fprintf(stderr, "Command size too large. Must be less than %d bytes\n",
            ASD_MAX_CMD);
    return -1;
  }

  if (cmd_len == 0) {
    return 0;
  }
  memcpy(pack->cmd, cmd, cmd_len);

  return 0;
}

/* Send an asd ack (does not wait for an ack)
    Returns: 0 on success, otherwise -1 */
ssize_t asd_send_ack(int sfd, struct sockaddr_in dest_addr) {
  int nbytes;

  AsdPacket *pack = calloc(1, sizeof(AsdPacket));

  /* Populate our packet */
  if (asd_fill_packet(pack, ASD_ACK, NULL, 0) != 0) {
    return -1;
  }

  nbytes = sendto(sfd, pack, sizeof(AsdPacket), 0,
                  (struct sockaddr *)&dest_addr, sizeof(dest_addr));

  /* No bytes sent */
  if (nbytes == -1) {
    return -1;
  }

  free(pack);

  return 0;
}

/* Send an asd packet and wait for an acknowledgement
    Returns: 0 on success, otherwise -1 */
ssize_t asd_send_packet(asd_msg_type type, char *cmd, size_t cmd_len, int sfd,
                        struct sockaddr_in dest_addr) {
  int err = 0;

  /* Send */
  int nbytes;
  int attempts = 0;

  /* Receive */
  int rbytes;
  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);

  /* Overhead to poll our sfd for an ack */
  struct pollfd fds[1];
  fds[0].fd = sfd;
  fds[0].events = POLLIN;
  int sfd_event;

  AsdPacket *pack = calloc(1, sizeof(AsdPacket));

  /* Populate our packet */
  if (asd_fill_packet(pack, type, cmd, cmd_len) != 0) {
    return -1;
  }

  while (attempts < ASD_MAX_RETRY) {
    nbytes = sendto(sfd, pack, sizeof(AsdPacket), 0,
                    (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    /* No bytes sent */
    if (nbytes == -1) {
      return -1;
    }

    /* Wait for an ack (timeout 5 seconds) */
    sfd_event = poll(fds, 1, ASD_TIMEOUT); // Timeout in milliseconds
    if (sfd_event == -1) {
      return -1; // Error in poll()
    } else if (sfd_event == 0) {
      /* No ack */
      attempts++;
      continue;
    } else {
      /* Received a packet make sure it's an ack */
      rbytes = recvfrom(sfd, pack, sizeof(AsdPacket), 0,
                        (struct sockaddr *)&recv_addr, &addr_len);
      if (pack->type != ASD_ACK) {
        err = -1;
      }
      break;
    }
  }

  free(pack);

  return err;
}
