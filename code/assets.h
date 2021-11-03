#define FONT_ATLAS_WIDTH 1920
#define FONT_ATLAS_HEIGHT 1080
#define MAX_FONT_METRICS_COUNT 95
#define FIRST_FONT_GLYPH ' '
#define MAX_HEALTHBAR_SRC_COUNT 5

// Font test
// !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~

typedef enum
{
    Color_None,
    
    Color_Black,
    Color_White,
    
    Color_LightGray,
    Color_DarkGray,
    
    Color_LightRed,
    Color_DarkRed,
    
    Color_Green,
    Color_LightGreen,
    Color_DarkGreen,
    
    Color_LightBlue,
    Color_DarkBlue,
    
    Color_LightBrown,
    Color_DarkBrown,
    
    Color_Yellow,
    Color_LightYellow,
    Color_EntityView,
    
    Color_Cyan,
    Color_Purple,
    Color_Orange,
    
    Color_DebugBackgroundLight,
    Color_DebugBackgroundDark,
    Color_AlmostWhite,
    
    //
    
    Color_WindowBorder,
    Color_WindowBackground,
    Color_WindowShadow,
    
    Color_MouseHighlightBackground,
    Color_MouseHighlightBorder
} Color;

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
    FontType type;
    u32 bmp_advance;
    u32 size;
    
    SDL_Texture *atlas;
    GlyphMetrics metrics[MAX_FONT_METRICS_COUNT];
} Font;

typedef struct
{
    u32 stat_and_log_window_h;
    
    Texture tilemap;
    Texture tileset;
    
    v4u healthbar_src[MAX_HEALTHBAR_SRC_COUNT];
    Font fonts[FontName_Count];
} Assets;