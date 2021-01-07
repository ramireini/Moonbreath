#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_ROOM_COUNT 512
#define MAX_DUNGEON_PASSAGE_COUNT 8

typedef enum
{
    TileID_None,
    
    TileID_StoneWall1,
    TileID_StoneWall2,
    TileID_StoneWall3,
    TileID_StoneWall4,
    TileID_StoneWall5,
    
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
    
    TileID_StoneStaircaseUp,
    TileID_StoneStaircaseDown,
    TileID_ExitDungeon,
    
    TileID_RedBlood1,
    TileID_RedBlood2,
    TileID_RedBlood3,
    TileID_RedBlood4,
    TileID_RedBlood5,
    TileID_RedBlood6,
    TileID_RedBlood7,
    
    TileID_GreenBlood1,
    TileID_GreenBlood2,
    TileID_GreenBlood3,
    TileID_GreenBlood4,
    TileID_GreenBlood5,
    TileID_GreenBlood6,
    TileID_GreenBlood7
} TileID;

typedef enum
{
    RoomType_None,
    
    RoomType_Rect,
    RoomType_DoubleRect,
    RoomType_Automaton,
    
    RoomType_Count
} RoomType;

typedef enum
{
    CorridorType_None,
    
    CorridorType_Turn,
    CorridorType_Zigzag,
    CorridorType_Diagonal,
    
    CorridorType_Count
} CorridorType;

typedef enum
{
    PassageType_None,
    
    PassageType_Up,
    PassageType_Down
} PassageType;

typedef struct
{
    PassageType type;
    v2u pos;
} Passage;

typedef struct
{
    TileID id;
    TileID remains_id;
    
    b32 is_seen;
    b32 has_been_seen;
    b32 is_occupied;
} Tile;

typedef struct
{
    b32 found;
    b32 index;
} RoomIndex;

typedef struct
{
    b32 found;
    v4u rect;
} RemainsSource;

typedef struct
{
    b32 success;
    v4u rect;
} CreatedRoom;

typedef struct
{
    u32 width;
    u32 array[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
} Pathfind;

typedef struct
{
    u32 width;
    Tile *array;
} Tiles;

typedef struct
{
    u32 count;
    v4u array[MAX_DUNGEON_ROOM_COUNT];
    u32 enemy_count[MAX_DUNGEON_ROOM_COUNT];
    u32 item_count[MAX_DUNGEON_ROOM_COUNT];
} Rooms;

typedef struct
{
    u32 level;
    u32 width;
    u32 height;
    
    Pathfind pathfind;
    Tiles tiles;
    Rooms rooms;
    
    u32 room_type_chances[RoomType_Count];
    u32 corridor_type_chances[CorridorType_Count];
    u32 item_type_chances[ItemType_Count];
    u32 potion_chances[Potion_Count];
    u32 scroll_chances[Scroll_Count];
    
    u32 enemy_count;
    u32 item_count;
    u32 room_enemy_count;
    u32 room_item_count;
    
    Passage passages[MAX_DUNGEON_PASSAGE_COUNT];
    u32 min_distance_between_passages;
    u32 up_passage_count;
    u32 down_passage_count;
    
    v2u rect_room_size;
    v2u double_rect_room_size;
    v2u automaton_room_size;
} Dungeon;

internal b32 is_tile_traversable(Tiles tiles, v2u pos);