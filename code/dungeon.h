#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_SIZE_SQUARED (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE)

#define MAX_DUNGEON_ROOM_COUNT 256
#define MAX_DUNGEON_TRAP_COUNT 128
#define MAX_DUNGEON_PASSAGE_COUNT 8

typedef enum
{
    DungeonTileID_None,
    
    DungeonTileID_StoneWall1,
    DungeonTileID_StoneWall2,
    DungeonTileID_StoneWall3,
    DungeonTileID_StoneWall4,
    DungeonTileID_StoneWall5,
    
    DungeonTileID_StoneWallTorch1,
    DungeonTileID_StoneWallTorch2,
    DungeonTileID_StoneWallTorch3,
    DungeonTileID_StoneWallTorch4,
    DungeonTileID_StoneWallTorch5,
    
    DungeonTileID_StoneWallGrate1,
    DungeonTileID_StoneWallGrate2,
    
    DungeonTileID_StoneWallVines1,
    DungeonTileID_StoneWallVines2,
    DungeonTileID_StoneWallVines3,
    DungeonTileID_StoneWallVines4,
    DungeonTileID_StoneWallVines5,
    
    DungeonTileID_StoneFloor1,
    DungeonTileID_StoneFloor2,
    DungeonTileID_StoneFloor3,
    DungeonTileID_StoneFloor4,
    
    DungeonTileID_StoneFloorGrate1,
    DungeonTileID_StoneFloorGrate2,
    DungeonTileID_StoneFloorGrate3,
    DungeonTileID_StoneFloorGrate4,
    DungeonTileID_StoneFloorGrate5,
    DungeonTileID_StoneFloorGrate6,
    
    DungeonTileID_StoneDoorClosed,
    DungeonTileID_StoneDoorOpen,
    
    DungeonTileID_StoneStaircaseUp,
    DungeonTileID_StoneStaircaseDown,
    DungeonTileID_ExitDungeon,
    
    DungeonTileID_EntityStatus,
    DungeonTileID_EntityAlerted,
    DungeonTileID_EntityInvisible,
    
    DungeonTileID_FootstepsUp,
    DungeonTileID_FootstepsDown,
    DungeonTileID_FootstepsLeft,
    DungeonTileID_FootstepsRight,
    
    DungeonTileID_FootstepsUpLeft,
    DungeonTileID_FootstepsUpRight,
    DungeonTileID_FootstepsDownLeft,
    DungeonTileID_FootstepsDownRight,
    
    DungeonTileID_SpikeTrap,
    DungeonTileID_SwordTrap,
    DungeonTileID_ArrowTrap,
    DungeonTileID_MagicTrap,
    DungeonTileID_BindTrap,
    DungeonTileID_ShaftTrap,
    DungeonTileID_SummonTrap,
    DungeonTileID_TeleportTrap,
    
    // Red Blood
    DungeonTileID_RedBloodGroundSmall1,
    DungeonTileID_RedBloodGroundSmall2,
    DungeonTileID_RedBloodGroundSmall3,
    
    DungeonTileID_RedBloodGroundMedium1,
    DungeonTileID_RedBloodGroundMedium2,
    
    DungeonTileID_RedBloodGroundLarge1,
    DungeonTileID_RedBloodGroundLarge2,
    
    DungeonTileID_RedBloodWallUp1,
    DungeonTileID_RedBloodWallUp2,
    DungeonTileID_RedBloodWallUp3,
    
    DungeonTileID_RedBloodWallDown1,
    DungeonTileID_RedBloodWallDown2,
    DungeonTileID_RedBloodWallDown3,
    
    DungeonTileID_RedBloodWallLeft1,
    DungeonTileID_RedBloodWallLeft2,
    DungeonTileID_RedBloodWallLeft3,
    
    DungeonTileID_RedBloodWallRight1,
    DungeonTileID_RedBloodWallRight2,
    DungeonTileID_RedBloodWallRight3,
    
    // Green Blood
    DungeonTileID_GreenBloodGroundSmall1,
    DungeonTileID_GreenBloodGroundSmall2,
    DungeonTileID_GreenBloodGroundSmall3,
    
    DungeonTileID_GreenBloodGroundMedium1,
    DungeonTileID_GreenBloodGroundMedium2,
    
    DungeonTileID_GreenBloodGroundLarge1,
    DungeonTileID_GreenBloodGroundLarge2,
    
    DungeonTileID_GreenBloodWallUp1,
    DungeonTileID_GreenBloodWallUp2,
    DungeonTileID_GreenBloodWallUp3,
    
    DungeonTileID_GreenBloodWallDown1,
    DungeonTileID_GreenBloodWallDown2,
    DungeonTileID_GreenBloodWallDown3,
    
    DungeonTileID_GreenBloodWallLeft1,
    DungeonTileID_GreenBloodWallLeft2,
    DungeonTileID_GreenBloodWallLeft3,
    
    DungeonTileID_GreenBloodWallRight1,
    DungeonTileID_GreenBloodWallRight2,
    DungeonTileID_GreenBloodWallRight3,
    
    DungeonTileID_Water1,
        DungeonTileID_Water2,
        DungeonTileID_Water3
} DungeonTileID;

typedef enum
{
    DungeonTileType_Floor,
    DungeonTileType_Water,
    DungeonTileType_Torch,
    DungeonTileType_Door
} DungeonTileType;

typedef enum
{
    DungeonRandomPosType_Traversable,
    DungeonRandomPosType_TraversableUnoccupied,
    DungeonRandomPosType_TraversableRect,
    DungeonRandomPosType_TraversableUnoccupiedRect,
        DungeonRandomPosType_Feature
} DungeonRandomPosType;

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
    DungeonTileID tile;
    DungeonTileID remains;
    
    b32 is_occupied;
    b32 is_seen;
    b32 has_been_seen;
    } DungeonTile;

typedef struct
{
    u32 width;
    DungeonTile *array;
} DungeonTiles;

typedef enum
{
    DungeonTrapFlag_HasBeenSeen = (1 << 1)
} DungeonTrapFlag;

// Dungeon trap ideas:
// Summoning multiple enemies
// Traps with random effects
struct DungeonTrap
{
    u32 flags;
    
    DungeonTrapType type;
    String32 name;
    String128 description;
    char select_letter;
    
    v2u pos;
    v4u tile_src;
    
    // Each shaft will drop you a certain amount of dungeon levels and to a particular
    // position. The same data is used when you fall down the same shaft.
    b32 is_shaft_set;
    u32 shaft_depth;
    v2u shaft_dest;
};

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
    v2u size;
    u32 area;
    
    // Flood
    f32 flood_traversable_min;
    
    // Water
    b32 has_water;
    f32 water_min_total_area;
    u32 water_placement_min_floor_count;
    
    // Torches and Doors
    u32 torch_count;
    u32 door_count;
    u32 door_min_spacing;
    
    // Room
    u32 room_retry_count;
    
    f32 automaton_room_min_valid_area;
    u32 automaton_room_init_chance;
    u32 automaton_room_step_count;
    
    u32 room_max_items;
    u32 room_max_enemies;
    
    u32 room_type_chances[DungeonRoomType_Count];
    u32 corridor_type_chances[DungeonCorridorType_Count];
    
    v2u rect_room_size;
    v2u double_rect_room_size;
    v2u circle_room_size;
    v2u automaton_room_size;
    v2u automaton_water_size;
    
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
    Dungeon levels[MAX_DUNGEON_LEVEL];
} Dungeons;

internal void dungeon_automaton_room_step(Random *random, DungeonTiles src_tiles, DungeonTiles dest_tiles, v4u rect);
internal void set_dungeon_pos_wall(Random *random, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_floor(Random *random, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_water(Random *random, DungeonTiles tiles, v2u pos);
internal u32 get_dungeon_pos_trap_count(DungeonTiles tiles, DungeonTraps *traps, v2u pos);
internal b32 is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_occupied(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_traversable(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_floor(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_passage(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile);
internal b32 is_pos_inside_dungeon(v2u dungeon_size, v2u pos);
internal b32 is_dungeon_pos_water(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_traversable_and_unoccupied(DungeonTiles tiles, v2u pos);
internal b32 can_place_dungeon_feature_on_pos(Dungeon *dungeon, ItemState *items, v2u pos);
internal v2u get_random_dungeon_pos(Random *random, v2u dungeon_size);
internal v2u get_random_dungeon_rect_pos(Random *random, v4u rect);
internal v4u get_dungeon_tileset_rect(DungeonTileID tile);
internal Dungeon *get_dungeon_from_level(Dungeons *dungeons, u32 level);