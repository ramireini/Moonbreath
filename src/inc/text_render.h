#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_GREEN 0x1CF002FF
#define TEXT_COLOR_ORANGE 0xA47B55FF
#define TEXT_COLOR_YELLOW 0xFDD500FF
#define TEXT_COLOR_BLUE 0x0060C8FF

void render_text(char *str, int x, int y, int text_color, font_t *font_struct, ...);
font_t* create_font_atlas(TTF_Font *font);

#endif // TEXT_RENDER_H