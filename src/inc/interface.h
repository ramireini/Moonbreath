#ifndef INTERAFACE_H
#define INTERAFACE_H

#include <game.h>

#define MESSAGE_COUNT 9

extern console_message_t messages[MESSAGE_COUNT];

void add_console_msg(char *msg, uint32 msg_color, ...);
void render_interface();

#endif // INTERAFACE_H