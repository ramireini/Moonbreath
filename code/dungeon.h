#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_ROOMS 512

typedef enum
{
    TileID_None,
    
    TileID_StoneWall1,
    TileID_StoneWall2,
    TileID_StoneWall3,
    TileID_StoneWall4,
    TileID_StoneWall5,
    TileID_StoneWall6,
    
    TileID_StoneWallTorch1,
    TileID_StoneWallTorch2,
    TileID_StoneWallTorch3,
    TileID_StoneWallTorch4,
    TileID_StoneWallTorch5,
    
    TileID_StoneWallGrate1,
    TileID_StoneWallGrate2,
    
    TileID_StoneWallVines1,
    TileID_StoneWallVines2,
    TileID_StoneWallVines3,
    TileID_StoneWallVines4,
    TileID_StoneWallVines5,
    
    TileID_StoneWallBanner1,
    TileID_StoneWallBanner2,
    TileID_StoneWallBanner3,
    
    TileID_StoneFloor1,
    TileID_StoneFloor2,
    TileID_StoneFloor3,
    TileID_StoneFloor4,
    
    TileID_StoneFloorGrate1,
    TileID_StoneFloorGrate2,
    TileID_StoneFloorGrate3,
    TileID_StoneFloorGrate4,
    TileID_StoneFloorGrate5,
    TileID_StoneFloorGrate6,
    
    TileID_StoneDoorClosed,
    TileID_StoneDoorOpen,
    
    TileID_StonePathUp,
    TileID_StonePathDown,
    TileID_Escape,
    
    TileID_RedBloodPuddleSmall1,
    TileID_RedBloodPuddleSmall2,
    
    TileID_RedBloodPuddleMedium1,
    TileID_RedBloodPuddleMedium2,
    
    TileID_RedBloodPuddleLarge1,
    TileID_RedBloodPuddleLarge2,
    
    TileID_RedBloodSplatter1,
    TileID_RedBloodSplatter2,
    TileID_RedBloodSplatter3,
    
    TileID_GreenBloodPuddleSmall1,
    TileID_GreenBloodPuddleSmall2,
    
    TileID_GreenBloodPuddleMedium1,
    TileID_GreenBloodPuddleMedium2,
    
    TileID_GreenBloodPuddleLarge1,
    TileID_GreenBloodPuddleLarge2,
    
    TileID_GreenBloodSplatter1,
    TileID_GreenBloodSplatter2,
    TileID_GreenBloodSplatter3
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

internal b32 is_inside_dungeon(Dungeon *dungeon, v2u pos);
internal b32 is_tile_traversable(TileData tiles, v2u pos);