#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <moonbreath_mountain.h>

void render_text(char *str, int x, int y, int text_color, font_t *font_struct, ...);
font_t* create_font_atlas(TTF_Font *font);

#endif // TEXT_RENDER_H