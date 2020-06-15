#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_ROOMS 512

typedef enum
{
    tile_none,
    
    Tile_StoneWall1,
    Tile_StoneWall2,
    Tile_StoneWall3,
    Tile_StoneWall4,
    Tile_StoneWall5,
    Tile_StoneWall6,
    
    Tile_StoneWallTorch1,
    Tile_StoneWallTorch2,
    Tile_StoneWallTorch3,
    Tile_StoneWallTorch4,
    Tile_StoneWallTorch5,
    
    Tile_StoneWallGrate1,
    Tile_StoneWallGrate2,
    
    Tile_StoneWallVines1,
    Tile_StoneWallVines2,
    Tile_StoneWallVines3,
    Tile_StoneWallVines4,
    Tile_StoneWallVines5,
    
    Tile_StoneWallBanner1,
    Tile_StoneWallBanner2,
    Tile_StoneWallBanner3,
    
    Tile_StoneFloor1,
    Tile_StoneFloor2,
    Tile_StoneFloor3,
    Tile_StoneFloor4,
    
    Tile_StoneFloorGrate1,
    Tile_StoneFloorGrate2,
    Tile_StoneFloorGrate3,
    Tile_StoneFloorGrate4,
    Tile_StoneFloorGrate5,
    Tile_StoneFloorGrate6,
    
    Tile_StoneDoorClosed,
    Tile_StoneDoorOpen,
    
    Tile_StonePathUp,
    Tile_StonePathDown,
    Tile_Escape,
    
    Tile_RedBloodPuddle1,
    Tile_RedBloodPuddle2,
    Tile_RedBloodPuddle3,
    Tile_RedBloodSplatter1,
    Tile_RedBloodSplatter2,
    Tile_RedBloodSplatter3,
    
    Tile_GreenBloodPuddle1,
    Tile_GreenBloodPuddle2,
    Tile_GreenBloodPuddle3,
    Tile_GreenBloodSplatter1,
    Tile_GreenBloodSplatter2,
    Tile_GreenBloodSplatter3,
    
    //
    
    Tile_WallStart = Tile_StoneWall1,
    Tile_WallEnd= Tile_StoneWallBanner3,
    
    Tile_FloorStart = Tile_StoneFloor1,
    Tile_FloorEnd = Tile_StoneFloorGrate6
} TileID;

typedef enum
{
    RoomType_None,
    
    RoomType_Rect,
    RoomType_DoubleRect,
    RoomType_Automaton
} RoomType;

typedef enum
{
    DungeonType_None,
    
    DungeonType_Cavern
} DungeonType;

typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
    b32 is_occupied;
    
    TileID remains;
    TileID id;
} Tile;

typedef struct
{
    u32 width;
    Tile *array;
} TileData;

typedef struct
{
    u32 width;
    u32 array[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
} PathfindData;

typedef struct
{
    u32 count;
    v4u array[MAX_DUNGEON_ROOMS];
} Rooms;

typedef struct
{
    DungeonType type;
    u32 level;
    u32 width;
    u32 height;
    
    PathfindData pathfind;
    TileData tiles;
    Rooms rooms;
    
    b32 can_have_rect_rooms;
    u32 rect_min_size;
    u32 rect_max_size;
    
    b32 can_have_double_rect_rooms;
    u32 double_rect_min_size;
    u32 double_rect_max_size;
    
    b32 can_have_automaton_rooms;
    u32 automaton_min_size;
    u32 automaton_max_size;
} Dungeon;