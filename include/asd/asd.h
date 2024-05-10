#ifndef ASD_H
#define ASD_H

#include <netinet/in.h>
#include <stddef.h>
#include <sys/types.h>

#define ASD_MAX_CMD 500  /* Max cmd supported */
#define ASD_MAX_RETRY 5  /* Max. number of retransmissions */
#define ASD_TIMEOUT 5000 /* Timeout for the retransmission (ms) */

typedef enum {
  ASD_ACK,
  ASD_TEST,
  ASD_RUN,
  ASD_STOP,
} asd_msg_type;

typedef struct {
  asd_msg_type type;
  char cmd[ASD_MAX_CMD];
} AsdPacket;

ssize_t asd_send_ack(int sfd, struct sockaddr_in dest_addr);
ssize_t asd_send_packet(asd_msg_type type, char *cmd, size_t cmd_len, int sfd,
                        struct sockaddr_in dest_addr);

#endif