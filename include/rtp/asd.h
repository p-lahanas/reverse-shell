#ifndef ASD_H
#define ASD_H

#include <netinet/in.h>

#define MAX_COMMAND_LEN 50
typedef enum {
  ASD_ACK,
  ASD_TEST,
  ASD_RUN,
  ASD_STOP,
} msg_type;

typedef struct {
  msg_type msg_type;
  size_t command_size;
} AsdHeader;

typedef struct {
  AsdHeader header;
  char *command;
} AsdPacket;

AsdPacket *asd_create_packet(msg_type type, char *command);

int asd_send_packet(msg_type type, char *command, int sfd,
                    struct sockaddr_in dest_addr);

#endif