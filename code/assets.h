#define FONT_ATLAS_WIDTH 1920
#define FONT_ATLAS_HEIGHT 1080
#define FONT_START_GLYPH 32

// Font test
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
    Color_None,
    
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
    u32 advance;
} GlyphMetrics;

typedef struct
{
    SDL_Texture *tex;
    u32 w, h;
} Texture;

typedef struct
{
    b32 success;
    FontType type;
    u32 size;
    
    SDL_Texture *atlas;
    GlyphMetrics metrics[95];
    u32 bmp_advance;
} Font;

typedef struct
{
    Font *fonts[FontName_Count];
    
    Texture tilemap;
    Texture tileset;
    Texture ui;
    
    v4u health_bar_outside;
    v4u health_bar_inside;
    v4u bottom_window;
    v4u item_window;
    v4u full_log_window;
    v4u inventory_window;
    v4u inventory_selected_slot;
    v4u inventory_equipped_slot;
    v4u item_ground_outline;
    v4u yellow_outline;
} Assets;

internal v4u get_color_value(Color color);