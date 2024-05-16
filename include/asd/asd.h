#ifndef ASD_ASD_H
#define ASD_ASD_H

#include <netinet/in.h>
#include <stddef.h>

typedef enum {
  ASD_ACK,
  ASD_TEST,
  ASD_RUN,
  ASD_STOP,
} AsdMsgType;

typedef struct {
  AsdMsgType type;
  size_t cmd_len;
} AsdHeader;

typedef struct {
  AsdHeader header;
  char *cmd;
} AsdMsg;

/* Send ASD_ACK message to destination */
int asd_send_ack(int sfd, struct sockaddr *dest_addr);

/* Send any ASD message */
int asd_send_command(AsdHeader header, char *cmd, int sfd,
                     struct sockaddr *dest_addr);

/* Wait for an ASD message and return it */
AsdMsg *asd_recv_command(int sfd, struct sockaddr *recv_addr);

#endif