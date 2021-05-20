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
    Color_Orange,
    
    Color_DebugBackgroundLight,
    Color_DebugBackgroundDark,
    Color_AlmostWhite,
    
    //
    
    Color_WindowBorder,
    Color_WindowBackground,
    Color_WindowShadow
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
    b32 is_valid;
    
    FontType type;
    u32 size;
    
    SDL_Texture *atlas;
    GlyphMetrics metrics[95];
    u32 bmp_advance;
} Font;

typedef struct
{
    Font fonts[FontName_Count];
    
    Texture tilemap;
    Texture tileset;
    Texture ui;
    
    v4u item_ground_outline_src;
    v4u yellow_outline_src;
    
    u32 stat_and_log_window_h;
} Assets;

internal v4u get_color_value(Color color);