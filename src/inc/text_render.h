#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

#define ATLAS_WIDTH 1376
#define ATLAS_HEIGHT 32

#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_RED 0xFF0000FF
#define TEXT_COLOR_BLUE 0x0080FFFF
#define TEXT_COLOR_GREEN 0x00B300FF
#define TEXT_COLOR_YELLOW 0xFFFF00FF
#define TEXT_COLOR_ORANGE 0xFFA500FF
#define TEXT_COLOR_BROWN 0xE7A56AFF

void render_text(char *str, int32 str_x, int32 str_y, int32 str_color, font_t *font, ...);

// Returns a malloc'd struct, REMEMBER TO FREE!
font_t* create_ttf_font_atlas(TTF_Font *font, int32 space_in_px);
font_t* create_bmp_font_atlas(char *path, int32  glyph_w, int32 glyph_h, int32 glyphs_per_row, int32 space_in_px, int32 shared_advance_in_px);

#endif // TEXT_RENDER_H