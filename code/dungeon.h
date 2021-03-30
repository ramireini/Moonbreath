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
    RandomChanceType_Normal,
    RandomChanceType_ItemType,
    RandomChanceType_Potion,
        RandomChanceType_Scroll
} RandomChanceType;

typedef enum
{
    DungeonRoomFeatureType_Item,
    DungeonRoomFeatureType_Enemy
} DungeonRoomFeatureType;

typedef enum
{
    DungeonRoomType_None,
    
    DungeonRoomType_Rect,
    DungeonRoomType_DoubleRect,
    DungeonRoomType_Circle,
    DungeonRoomType_Automaton,
    
    DungeonRoomType_Count
} DungeonRoomType;

typedef enum
{
    DungeonCorridorType_None,
    
    DungeonCorridorType_Turn,
    DungeonCorridorType_Zigzag,
    DungeonCorridorType_Diagonal,
    
    DungeonCorridorType_Count
} DungeonCorridorType;

typedef enum
{
    DungeonPassageType_None,
    
    DungeonPassageType_Up,
    DungeonPassageType_Down
} DungeonPassageType;

typedef enum
{
    DungeonTrapType_None,
    
    DungeonTrapType_Spike,
    DungeonTrapType_Sword,
    DungeonTrapType_Arrow,
    DungeonTrapType_Magic,
    DungeonTrapType_Bind,
    DungeonTrapType_Shaft,
    DungeonTrapType_Summon,
    DungeonTrapType_Teleport,
        
    DungeonTrapType_Count
} DungeonTrapType;

typedef struct
{
    b32 found;
    u32 value;
} RoomIndex;

typedef struct
{
    b32 success;
    
    DungeonRoomType type;
    v4u rect;
    
    u32 enemy_count;
    u32 item_count;
} DungeonRoom;

typedef struct
{
    u32 width;
    u32 array[MAX_DUNGEON_SIZE_SQUARED];
} PathfindMap;

typedef struct
{
    TileID tile;
    TileID remains;
    
    b32 is_occupied;
    b32 is_seen;
    b32 has_been_seen;
    } DungeonTile;

typedef struct
{
    u32 width;
    DungeonTile *array;
} DungeonTiles;

typedef struct
{
    DungeonTrapType type;
    char *name;
    char description[128];
    
    v2u pos;
    v4u tile_src;
    
    // Each shaft will drop you a certain amount of dungeon levels and to a particular
    // position. The same data is used when you fall down the same shaft.
    b32 is_shaft_set;
    u32 shaft_depth;
    v2u shaft_destination;
} DungeonTrap;

typedef struct
{
    DungeonPassageType type;
    
    v2u pos;
    v2u dest_pos;
} DungeonPassage;

typedef struct
{
    u32 count;
    DungeonRoom array[MAX_DUNGEON_ROOM_COUNT];
} DungeonRooms;

typedef struct
{
    u32 count;
    DungeonTrap array[MAX_DUNGEON_TRAP_COUNT];
} DungeonTraps;

typedef struct
{
    u32 up_count;
    u32 down_count;
    
    DungeonPassage array[MAX_DUNGEON_PASSAGE_COUNT];
} DungeonPassages;

typedef struct
{
    // TODO(rami): How many rooms to create.
    // Exact room count
    // Until certain amount of level is filled
    
    // Room
    u32 room_retry_count;
    
    u32 room_max_items;
    u32 room_max_enemies;
    
    u32 room_type_chances[DungeonRoomType_Count];
    u32 corridor_type_chances[DungeonCorridorType_Count];
    
    v2u rect_room_size;
    v2u double_rect_room_size;
    v2u circle_room_size;
    v2u automaton_room_size;
    
    // Trap
    u32 trap_count;
    
    v2u spike_trap_value;
    v2u sword_trap_value;
    v2u arrow_trap_value;
    v2u magic_trap_value;
    v2u bind_trap_value;
    v2u shaft_trap_value;
    
    // Passage
    u32 down_passage_count;
    
    // Entity
    u32 enemy_count;
    
    // Item
    u32 item_count;
    u32 item_curse_chance;
    
    u32 item_type_chances[ItemType_Count];
    u32 potion_chances[Potion_Count];
    u32 scroll_chances[Scroll_Count];
    } DungeonSpec;

typedef struct
{
    // TODO(rami): Water!
    // Could do a automaton room and have that be water, just need to make sure that dungeon
    // rooms are still all reachable.
    // OR
    // Put water inside of rooms, as in, for all tiles, if we are off from the wall by X
    // amount of tiles, put water there. This would create a puddle in the middle of the room.
    
    b32 ready_for_pathfinding;
    
    u32 level;
    v2u size;
    u32 area;
    v4u rect;
    
    DungeonSpec spec;
    
    DungeonTiles tiles;
    DungeonRooms rooms;
    DungeonTraps traps;
    DungeonPassages passages;
    } Dungeon;

typedef struct
{
    u32 current_level;
    Dungeon levels[MAX_DUNGEON_LEVELS];
} Dungeons;

internal b32 is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_occupied(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_traversable(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_floor(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_passage(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, TileID tile);
internal v4u get_dungeon_tileset_rect(TileID tile);
internal Dungeon *get_dungeon_from_level(Dungeons *dungeons, u32 level);