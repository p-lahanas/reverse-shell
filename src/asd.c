#include <asd/asd.h>

#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define RTP_MAX_PACK 500 /* Max RTP packet size */
#define RTP_MAX_RETRY 5  /* Max number of retransmissions */
#define RTP_TIMEOUT 5000 /* Timeout for the retransmission (ms) */

typedef enum { RTP_DATA, RTP_FIN, RTP_ACK } RtpMsgType;

typedef struct {
  RtpMsgType type;
  size_t dlen;
} RtpHeader;

typedef struct {
  RtpHeader header;
  void *data;
} RtpPacket;

/* Creates a serial packet with RtpHeader and data
    Returns: pointer to the packet on success, on error returns NULL */
void *rtp_create_serial_packet(RtpHeader header, void *data) {
  void *pack = calloc(1, sizeof(RtpHeader) + header.dlen);
  if (pack == NULL) {
    perror("calloc failed");
    return NULL;
  }

  memcpy(pack, &header, sizeof(RtpHeader));
  memcpy(pack + sizeof(RtpHeader), data, header.dlen);

  return pack;
}

/* Send an RTP acknowledgment (just a header)
  Return: 0 on success, otherwise -1 */
int rtp_send_ack(int sfd, struct sockaddr dest_addr) {
  int nbytes;
  RtpHeader head;

  /* Populate our packet */
  head.type = RTP_ACK;
  head.dlen = 0;

  nbytes =
      sendto(sfd, &head, sizeof(RtpHeader), 0, &dest_addr, sizeof(dest_addr));

  /* No bytes sent */
  if (nbytes == -1) {
    return -1;
  }

  return 0;
}

/* Sends and rtp packet and waits for an RTP_ACK
  Returns 0 on success -1 on failure */
int rtp_send_packet(RtpHeader header, void *data, int sfd,
                    struct sockaddr dest_addr) {
  void *pack = rtp_create_serial_packet(header, data);
  if (pack == NULL) {
    return -1;
  }

  int err = 0;

  /* Send */
  int nbytes;
  int attempts = 0;

  /* Receive */
  int rbytes;
  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  RtpHeader resp;

  /* Overhead to poll our sfd for an ack */
  struct pollfd fds[1];
  fds[0].fd = sfd;
  fds[0].events = POLLIN;
  int sfd_event;

  /* Send the packet and wait for ack */
  while (attempts < RTP_MAX_RETRY) {
    nbytes = sendto(sfd, pack, sizeof(RtpHeader) + header.dlen, 0, &dest_addr,
                    sizeof(dest_addr));

    /* No bytes sent */
    if (nbytes == -1) {
      return -1;
    }

    /* Wait for an ack (timeout 5 seconds) */
    sfd_event = poll(fds, 1, RTP_TIMEOUT); // Timeout in milliseconds
    if (sfd_event == -1) {
      return -1; // Error in poll()
    } else if (sfd_event == 0) {
      /* No ack */
      attempts++;
      fprintf(stderr, "Destination Host Unreachable");
      if (attempts < RTP_MAX_RETRY) {
        fprintf(stderr, "...trying again (attempt: %d)", attempts + 1);
      }
      fprintf(stderr, "\n");
      continue;
    } else {
      /* Received a packet make sure it's an ack */
      rbytes = recvfrom(sfd, &resp, sizeof(RtpHeader), 0,
                        (struct sockaddr *)&recv_addr, &addr_len);
      if (resp.type != RTP_ACK || rbytes == -1) {
        err = -1;
      }
      break;
    }
  }
  if (attempts >= RTP_MAX_RETRY) {
    err = -1;
  }

  free(pack);
  return err;
}

/* Ensure the caller frees the RtpPacket */
RtpPacket *rtp_receive_packet(int sfd, struct sockaddr *recv_addr) {
  socklen_t addr_len = sizeof(struct sockaddr);
  ssize_t nbytes;

  RtpHeader head;
  RtpPacket *pack;
  void *buf = calloc(1, RTP_MAX_PACK);

  if (buf == NULL) {
    return NULL;
  }

  nbytes = recvfrom(sfd, buf, RTP_MAX_PACK, 0, recv_addr, &addr_len);

  if (nbytes == -1) {
    return NULL;
  }

  /* Grab our header */
  memcpy(&head, buf, sizeof(RtpHeader));

  /* Create a packet & data buffer */
  pack = calloc(1, sizeof(RtpPacket));
  void *data = calloc(1, head.dlen);
  if (pack == NULL || data == NULL) {
    return NULL;
  }

  /* Read in our data and free buf*/
  memcpy(data, buf + sizeof(RtpHeader), head.dlen);
  free(buf);

  /* Update our fields */
  pack->data = data;
  pack->header.dlen = head.dlen;
  pack->header.type = head.type;

  /* Send back and RTP_ACK */
  rtp_send_ack(sfd, *recv_addr);

  return pack;
}

/* Sends data robustly
  Returns 0 on success otherwise, -1*/
int rtp_send_data(void *data, size_t dlen, int sfd, struct sockaddr dest_addr) {

  /* Figure out how many packets we need */
  size_t npackets = 1 + ((dlen - 1) / (RTP_MAX_PACK - sizeof(RtpHeader)));
  size_t remaining = dlen;
  void *pack;
  RtpHeader header;
  header.type = RTP_DATA;

  for (size_t i = 0; i < npackets; i++) {
    if (remaining < (RTP_MAX_PACK - sizeof(RtpHeader))) {
      header.dlen = remaining;
    } else {
      header.dlen = (RTP_MAX_PACK - sizeof(RtpHeader));
    }
    /* Create, send and wait for ack */
    if (rtp_send_packet(header, data + (dlen - remaining), sfd, dest_addr) ==
        -1) {
      return -1;
    }
    remaining -= header.dlen;
  }

  /* Send our FIN */
  header.type = RTP_FIN;
  header.dlen = 0;
  if (rtp_send_packet(header, NULL, sfd, dest_addr) == -1) {
    return -1;
  }

  return 0;
}

/* TODO: block until first packet then timeout if taking too long */
void *rtp_receive_data(int sfd, struct sockaddr *recv_addr) {
  /* Keep receiving packets until FIN */
  RtpPacket *pack;
  int run = 1;
  void *data;
  size_t dsize = 0;

  while (run) {
    pack = rtp_receive_packet(sfd, recv_addr);

    if (pack == NULL) {
      run = 0;
    }

    /* Check type */
    if (pack->header.type == RTP_FIN) {
      run = 0;
    }

    if (pack->header.type == RTP_DATA) {
      data = realloc(data, dsize + pack->header.dlen);
      memcpy(data + dsize, pack->data, pack->header.dlen);
      dsize += pack->header.dlen;
    }
    free(pack->data);
    free(pack);
  }
  return data;
}

/* Creates a serial packet with AsdHeader and command
    Returns: pointer to the packet on success, on error returns NULL */
void *asd_create_serial_packet(AsdHeader header, char *cmd) {
  void *pack = calloc(1, sizeof(AsdHeader) + header.cmd_len);
  if (pack == NULL) {
    perror("calloc failed");
    return NULL;
  }

  memcpy(pack, &header, sizeof(AsdHeader));
  memcpy(pack + sizeof(AsdHeader), cmd, header.cmd_len);

  return pack;
}

/* Send an asd ack (does not wait for an ack)
    Returns: 0 on success, otherwise -1 */
int asd_send_ack(int sfd, struct sockaddr *dest_addr) {
  AsdHeader pack;
  pack.type = ASD_ACK;
  pack.cmd_len = 0;

  return rtp_send_data(&pack, sizeof(AsdHeader), sfd, *dest_addr);
}

int asd_send_command(AsdHeader header, char *cmd, int sfd,
                     struct sockaddr *dest_addr) {
  int err = 0;
  void *pack = asd_create_serial_packet(header, cmd);

  err =
      rtp_send_data(pack, sizeof(AsdHeader) + header.cmd_len, sfd, *dest_addr);

  free(pack);
  return err;
}

/* Listen for an asd command.
  Returns: The received ASD packet, NULL if an error occured
  *Note must free the cmd buffer when finished */
AsdMsg *asd_recv_command(int sfd, struct sockaddr *recv_addr) {
  ssize_t nbytes;
  socklen_t addr_len = sizeof(struct sockaddr);
  AsdMsg *pack = calloc(1, sizeof(AsdMsg));
  if (pack == NULL) {
    return NULL;
  }

  void *data = rtp_receive_data(sfd, recv_addr);

  if (data == NULL) {
    return NULL;
  }

  /* Read in the header */
  memcpy(&pack->header, data, sizeof(AsdHeader));
  char *cmd = calloc(1, pack->header.cmd_len);
  memcpy(cmd, data + sizeof(AsdHeader), pack->header.cmd_len);

  pack->cmd = cmd;

  return pack;
}