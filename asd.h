#define ASD_ACK 0
#define ASD_TEST 1
#define ASD_RUN 2
#define ASD_STOP 3

#define MAX_COMMAND 200

typedef unsigned short int msg_type;

struct AsdPacket {
  msg_type msg_type;
  char command[MAX_COMMAND];
};
struct AsdPacket *create_asd_packet(msg_type type, char *command);