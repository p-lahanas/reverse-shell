#include "asd.h"

#include <stdlib.h>
#include <string.h>

struct AsdPacket *create_asd_packet(msg_type type, char *command) {
  struct AsdPacket *pack =
      (struct AsdPacket *)calloc(1, sizeof(struct AsdPacket));

  pack->msg_type = type;

  if (command != NULL) {
    strncpy(pack->command, command, MAX_COMMAND);
  }

  return pack;
}
