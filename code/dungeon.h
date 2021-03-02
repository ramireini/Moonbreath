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
    
    TileID_StatusMark,
    TileID_QuestionMark,
    
    TileID_FootstepsUp,
    TileID_FootstepsDown,
    TileID_FootstepsLeft,
    TileID_FootstepsRight,
    
    TileID_FootstepsUpLeft,
    TileID_FootstepsUpRight,
    TileID_FootstepsDownLeft,
    TileID_FootstepsDownRight,
    
    // Red Blood
    TileID_RedBloodGroundSmall1,
    TileID_RedBloodGroundSmall2,
    TileID_RedBloodGroundSmall3,
    
    TileID_RedBloodGroundMedium1,
    TileID_RedBloodGroundMedium2,
    
    TileID_RedBloodGroundLarge1,
    TileID_RedBloodGroundLarge2,
    
    TileID_RedBloodWallUp1,
    TileID_RedBloodWallUp2,
    TileID_RedBloodWallUp3,
    
    TileID_RedBloodWallDown1,
    TileID_RedBloodWallDown2,
    TileID_RedBloodWallDown3,
    
    TileID_RedBloodWallLeft1,
    TileID_RedBloodWallLeft2,
    TileID_RedBloodWallLeft3,
    
    TileID_RedBloodWallRight1,
    TileID_RedBloodWallRight2,
    TileID_RedBloodWallRight3,
    
    // Green Blood
    TileID_GreenBloodGroundSmall1,
    TileID_GreenBloodGroundSmall2,
    TileID_GreenBloodGroundSmall3,
    
    TileID_GreenBloodGroundMedium1,
    TileID_GreenBloodGroundMedium2,
    
    TileID_GreenBloodGroundLarge1,
    TileID_GreenBloodGroundLarge2,
    
    TileID_GreenBloodWallUp1,
    TileID_GreenBloodWallUp2,
    TileID_GreenBloodWallUp3,
    
    TileID_GreenBloodWallDown1,
    TileID_GreenBloodWallDown2,
    TileID_GreenBloodWallDown3,
    
    TileID_GreenBloodWallLeft1,
    TileID_GreenBloodWallLeft2,
    TileID_GreenBloodWallLeft3,
    
    TileID_GreenBloodWallRight1,
    TileID_GreenBloodWallRight2,
    TileID_GreenBloodWallRight3,
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
    RandomChanceType_Normal,
    RandomChanceType_ItemType,
    RandomChanceType_Potion,
        RandomChanceType_Scroll
} RandomChanceType;

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
    b32 found;
    u32 value;
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
    u32 enemy_count;
    u32 item_count;
} Room;

typedef struct
{
    u32 width;
    u32 array[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
} PathfindMap;

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
    u32 width;
    Tile *array;
} Tiles;

typedef struct
{
    u32 level;
    u32 width;
    u32 height;
    
    Tiles tiles;
    PathfindMap pathfind;
    
    u32 room_count;
    Room rooms[MAX_DUNGEON_ROOM_COUNT];
    
    u32 enemy_count;
    u32 max_room_enemy_count;
    u32 min_enemy_tile_distance;
    
    u32 item_count;
    u32 max_room_item_count;
    u32 min_item_tile_distance;
    
    u32 up_passage_count;
    u32 down_passage_count;
    u32 min_passage_tile_distance;
    Passage passages[MAX_DUNGEON_PASSAGE_COUNT];
    
    // TODO(rami): Dungeon traps
    //Trap traps[MAX_DUNGEON_TRAP_COUNT];
    
    u32 cursed_item_chance;
    u32 room_type_chances[RoomType_Count];
    u32 corridor_type_chances[CorridorType_Count];
    u32 item_type_chances[ItemType_Count];
    u32 potion_chances[Potion_Count];
    u32 scroll_chances[Scroll_Count];
    
    v2u rect_room_size;
    v2u double_rect_room_size;
    v2u automaton_room_size;
} Dungeon;

internal b32 is_tile_traversable(Tiles tiles, v2u pos);