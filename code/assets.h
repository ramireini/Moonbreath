#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32
#define START_ASCII_GLYPH 32

// TODO(rami): If you need to test a font
// !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~

typedef enum
{
    font_classic,
    font_classic_outlined,
    font_alkhemikal,
    font_monaco,
    font_dos_vga,
    
    font_total
} font;

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
} tex;

typedef enum
{
    font_type_none,
    
    font_type_bmp,
    font_type_ttf
} font_type;

typedef struct
{
    u32 x, y, w, h;
    u32 glyph_advance;
} glyph_metrics_t;

typedef struct
{
    SDL_Texture *tex;
    u32 w, h;
} texture_t;

typedef struct
{
    b32 success;
    font_type type;
    u32 size;
    
    // NOTE(rami):
    // TTF fonts will use the glyph_advance found in glyph_metrics_t for each glyph.
    // BMP fonts will use the shared_glyph_advance value.
    
    SDL_Texture *atlas;
    glyph_metrics_t metrics[95];
    u32 shared_glyph_advance;
} font_t;

typedef struct
{
    texture_t tilemap;
    texture_t tileset;
    texture_t item_tileset;
    texture_t wearable_item_tileset;
    texture_t sprite_sheet;
    texture_t ui;
    
    v4u health_bar_outside;
    v4u health_bar_inside;
    v4u log_window;
    v4u inventory_window;
    v4u inventory_selected_slot;
    v4u inventory_equipped_slot;
    v4u item_window;
    
    font_t *fonts[font_total];
} assets_t;