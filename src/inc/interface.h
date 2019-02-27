#ifndef INTERAFACE_H
#define INTERAFACE_H

#define MESSAGE_COUNT 9

typedef struct
{
  char msg[256];
  SDL_Color color;
} console_message_t;

extern console_message_t messages[MESSAGE_COUNT];

void add_console_msg(char *msg, SDL_Color c, ...);
void render_interface();

#endif // INTERAFACE_H