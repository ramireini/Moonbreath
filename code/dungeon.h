#define MAX_DUNGEON_LEVELS 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_SIZE_SQUARED (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE)

#define MAX_DUNGEON_ROOM_COUNT 256
#define MAX_DUNGEON_TRAP_COUNT 128
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
    
    TileID_SpikeTrap,
    TileID_SwordTrap,
    TileID_ArrowTrap,
    TileID_MagicTrap,
    TileID_BindTrap,
    TileID_ShaftTrap,
    TileID_SummonTrap,
    TileID_TeleportTrap,
    
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
    RoomType_Circle,
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

typedef enum
{
    TrapType_None,
    
    TrapType_Spike,
    TrapType_Sword,
    TrapType_Arrow,
    TrapType_Magic,
    TrapType_Bind,
    TrapType_Shaft,
    TrapType_Summon,
    TrapType_Teleport,
        
        TrapType_Count
} TrapType;

typedef struct
{
    b32 found;
    u32 value;
} RoomIndex;

typedef struct
{
    b32 exists;
    v4u rect;
} RemainsSource;

typedef struct
{
    b32 success;
    
    RoomType type;
    v4u rect;
    
    u32 enemy_count;
    u32 item_count;
} Room;

typedef struct
{
    u32 width;
    u32 array[MAX_DUNGEON_SIZE_SQUARED];
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
    PassageType type;
    v2u pos;
    v2u dest_pos;
} Passage;

typedef struct
{
    u32 up_count;
    u32 down_count;
    
    Passage array[MAX_DUNGEON_PASSAGE_COUNT];
} PassageState;

typedef struct
{
    TrapType type;
    char *name;
    char description[128];
    
    v2u pos;
    v4u tile_src;
    
    // Each shaft will drop you a certain amount of dungeon levels and to a particular
    // position. The same data is used when you fall down the same shaft.
    b32 is_shaft_set;
    u32 shaft_depth;
    v2u shaft_destination;
} Trap;

typedef struct
{
    u32 count;
    Trap array[MAX_DUNGEON_TRAP_COUNT];
} TrapState;

typedef struct
{
    // TODO(rami): Water!
    // Could do a automaton room and have that be water, just need to make sure that dungeon
    // rooms are still all reachable.
    // OR
    // Put water inside of rooms, as in, for all tiles, if we are off from the wall by X
    // amount of tiles, put water there. This would create a puddle in the middle of the room.
    
    // TODO(rami): I think we want something like a dungeon spec which is used to generate
    // the dungeon level. How and what the dungeon generates depends on the spec.
    
    b32 ready_for_pathfinding;
    u32 level;
    
    u32 width;
    u32 height;
    u32 size;
    v4u rect;
    
    Tiles tiles;
    
    u32 rooms_count;
    Room rooms[MAX_DUNGEON_ROOM_COUNT];
    
    PassageState passages;
    TrapState traps;
    
    u32 enemy_count;
    u32 item_count;
    
    u32 item_curse_chance;
    u32 item_chances[ItemType_Count];
    u32 potion_chances[Potion_Count];
    u32 scroll_chances[Scroll_Count];
    
    u32 room_chances[RoomType_Count];
    u32 corridor_chances[CorridorType_Count];
    
    u32 room_create_retry_count;
    u32 room_items_max_count;
    u32 room_enemies_max_count;
    
    v2u room_size_rect;
    v2u room_size_double_rect;
    v2u room_size_circle;
    v2u room_size_automaton;
    
    v2u spike_trap_value;
    v2u sword_trap_value;
    v2u arrow_trap_value;
    v2u magic_trap_value;
    v2u bind_trap_value;
    v2u shaft_trap_value;
    } Dungeon;

typedef struct
{
    u32 current_level;
    Dungeon levels[MAX_DUNGEON_LEVELS];
} DungeonState;

internal b32 is_tile_occupied(Tiles tiles, v2u pos);
internal b32 is_tile_traversable(Tiles tiles, v2u pos);
internal Dungeon *get_dungeon_from_level(DungeonState *dungeons, u32 index);