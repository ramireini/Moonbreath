#ifndef INTERFACE_H
#define INTERFACE_H

#define CONSOLE_MESSAGE_COUNT 9
#define CONSOLE_MESSAGE_EMPTY "EMPTY"

#define LOWERCASE_ALPHABET_START 97

typedef struct
{
  char msg[256];
  iv4 color;
} message_t;

#endif // INTERFACE_H