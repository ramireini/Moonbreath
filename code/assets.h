#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32

#define START_ASCII_GLYPH 32

// TODO(rami): If you need to test a font
// render_text("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~", V2u(100, 100), color_white, fonts[font_name]);

typedef enum
{
    font_classic,
    font_classic_outlined,
    font_alkhemikal,
    font_monaco,
    font_dos_vga,
    
    font_total
} game_font;

typedef enum
{
    tex_tilemap,
    tex_game_tileset,
    tex_item_tileset,
    tex_wearable_item_tileset,
    tex_sprite_sheet,
    tex_inventory_win,
    tex_inventory_item_win,
    tex_inventory_selected_item,
    tex_interface_bottom_win,
    tex_health_bar_outside,
    tex_health_bar_inside,
    
    tex_total
} game_texture;

typedef enum
{
    font_none,
    font_bmp,
    font_ttf
} font_type;

typedef struct
{
    u32 x, y, w, h;
    u32 glyph_advance;
} glyph_metrics_t;

typedef struct
{
    b32 success;
    
    font_type type;
    char *path; // TODO(rami): We might not need this.
    u32 size;
    
    // NOTE(rami):
    // TTF fonts will use the glyph_advance found in glyph_metrics_t for each glyph.
    // BMP fonts will use the shared_glyph_advance_value.
    
    SDL_Texture *atlas;
    glyph_metrics_t metrics[95];
    u32 shared_glyph_advance;
} font_t;