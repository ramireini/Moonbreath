#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32
#define START_ASCII_GLYPH 32

// TODO(rami): If you need to test a font
// !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~

typedef enum
{
    FontName_Classic,
    FontName_ClassicOutlined,
    FontName_Alkhemikal,
    FontName_Monaco,
    FontName_DosVga,
    
    FontName_Count
} FontName;

typedef enum
{
    FontType_None,
    
    FontType_BMP,
    FontType_TTF
} FontType;

typedef enum
{
    Color_Black,
    Color_White,
    
    Color_LightGray,
    Color_DarkGray,
    
    Color_LightRed,
    Color_DarkRed,
    
    Color_LightGreen,
    Color_DarkGreen,
    
    Color_LightBlue,
    Color_DarkBlue,
    
    Color_LightBrown,
    Color_DarkBrown,
    
    Color_Cyan,
    Color_Yellow,
    Color_Purple,
    Color_Orange
} Color;

typedef struct
{
    u32 x, y, w, h;
    u32 glyph_advance;
} GlyphMetrics;

typedef struct
{
    SDL_Texture *tex;
    u32 w, h;
} Texture;

typedef struct
{
    // TTF fonts will use the glyph_advance found in glyph_metrics_t for each glyph.
    // BMP fonts will use the shared_glyph_advance value.
    
    b32 success;
    FontType type;
    u32 size;
    
    SDL_Texture *atlas;
    GlyphMetrics metrics[95];
    u32 shared_glyph_advance;
} Font;

typedef struct
{
    Texture tilemap;
    Texture tileset;
    Texture item_tileset;
    Texture wearable_item_tileset;
    Texture sprite_sheet;
    Texture ui;
    
    v4u health_bar_outside;
    v4u health_bar_inside;
    v4u log_window;
    v4u inventory_window;
    v4u inventory_selected_slot;
    v4u inventory_equipped_slot;
    v4u item_window;
    
    Font *fonts[FontName_Count];
} Assets;

internal v4u get_color_value(Color color);