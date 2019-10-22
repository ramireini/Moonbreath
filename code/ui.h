#define LOWERCASE_ALPHABET_START 97

typedef struct
{
    char str[256];
    v4u color;
} console_text_t;

internal void add_console_text(char *str, v4u color, ...);