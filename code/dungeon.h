#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_SIZE_SQUARED (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE)

#define MAX_DUNGEON_ROOM_COUNT 256
#define MAX_DUNGEON_TRAP_COUNT 128
#define MAX_DUNGEON_PASSAGE_COUNT 8
#define TILE_SIZE 32

typedef enum
{
    DungeonTileID_None = 0,
    
    DungeonTileID_StoneWall1 = 1,
    DungeonTileID_StoneWall2 = 2,
    DungeonTileID_StoneWall3 = 3,
    DungeonTileID_StoneWall4 = 4,
    DungeonTileID_StoneWall5 = 5,
    
    DungeonTileID_StoneWallTorch1 = 6,
    DungeonTileID_StoneWallTorch2 = 7,
    DungeonTileID_StoneWallTorch3 = 8,
    DungeonTileID_StoneWallTorch4 = 9,
    DungeonTileID_StoneWallTorch5 = 10,
    
    DungeonTileID_StoneWallGrate1 = 11,
    DungeonTileID_StoneWallGrate2 = 12,
    
    DungeonTileID_StoneWallVines1 = 13,
    DungeonTileID_StoneWallVines2 = 14,
    DungeonTileID_StoneWallVines3 = 15,
    DungeonTileID_StoneWallVines4 = 16,
    DungeonTileID_StoneWallVines5 = 17,
    
    DungeonTileID_StoneFloor1 = 18,
    DungeonTileID_StoneFloor2 = 19,
    DungeonTileID_StoneFloor3 = 20,
    DungeonTileID_StoneFloor4 = 21,
    
    DungeonTileID_StoneFloorGrate1 = 22,
    DungeonTileID_StoneFloorGrate2 = 23,
    DungeonTileID_StoneFloorGrate3 = 24,
    DungeonTileID_StoneFloorGrate4 = 25,
    DungeonTileID_StoneFloorGrate5 = 26,
    DungeonTileID_StoneFloorGrate6 = 27,
    
    DungeonTileID_StoneDoorClosed = 28,
    DungeonTileID_StoneDoorOpen = 29,
    
    DungeonTileID_StoneStaircaseUp = 30,
    DungeonTileID_StoneStaircaseDown = 31,
    DungeonTileID_ExitDungeon = 32,
    
    DungeonTileID_EntityStatus = 33,
    DungeonTileID_EntityAlerted = 34,
    DungeonTileID_EntityInvisible = 35,
    
    DungeonTileID_FootstepsUp = 36,
    DungeonTileID_FootstepsDown = 37,
    DungeonTileID_FootstepsLeft = 38,
    DungeonTileID_FootstepsRight = 39,
    
    DungeonTileID_FootstepsUpLeft = 40,
    DungeonTileID_FootstepsUpRight = 41,
    DungeonTileID_FootstepsDownLeft = 42,
    DungeonTileID_FootstepsDownRight = 43,
    
    DungeonTileID_SpikeTrap = 44,
    DungeonTileID_SwordTrap = 45,
    DungeonTileID_ArrowTrap = 46,
    DungeonTileID_MagicTrap = 47,
    DungeonTileID_BindTrap = 48,
    DungeonTileID_ShaftTrap = 49,
    DungeonTileID_SummonTrap = 50,
    DungeonTileID_TeleportTrap = 51,
    
    DungeonTileID_Water1 = 52,
    DungeonTileID_Water2 = 53,
    DungeonTileID_Water3 = 54,
    
    // Red Blood
    DungeonTileID_RedBloodGroundSmall1 = 55,
    DungeonTileID_RedBloodGroundSmall2 = 56,
    DungeonTileID_RedBloodGroundSmall3 = 57,
    
    DungeonTileID_RedBloodGroundMedium1 = 58,
    DungeonTileID_RedBloodGroundMedium2 = 59,
    
    DungeonTileID_RedBloodGroundLarge1 = 60,
    DungeonTileID_RedBloodGroundLarge2 = 61,
    
    DungeonTileID_RedBloodWallUp1 = 62,
    DungeonTileID_RedBloodWallUp2 = 63,
    DungeonTileID_RedBloodWallUp3 = 64,
    
    DungeonTileID_RedBloodWallDown1 = 65,
    DungeonTileID_RedBloodWallDown2 = 66,
    DungeonTileID_RedBloodWallDown3 = 67,
    
    DungeonTileID_RedBloodWallLeft1 = 68,
    DungeonTileID_RedBloodWallLeft2 = 69,
    DungeonTileID_RedBloodWallLeft3 = 70,
    
    DungeonTileID_RedBloodWallRight1 = 71,
    DungeonTileID_RedBloodWallRight2 = 72,
    DungeonTileID_RedBloodWallRight3 = 73,
    
    DungeonTileID_RedBloodWallUpLeft1 = 74,
    DungeonTileID_RedBloodWallUpLeft2 = 75,
    DungeonTileID_RedBloodWallUpLeft3 = 76,
    
    DungeonTileID_RedBloodWallUpRight1 = 77,
    DungeonTileID_RedBloodWallUpRight2 = 78,
    DungeonTileID_RedBloodWallUpRight3 = 79,
    
    DungeonTileID_RedBloodWallDownLeft1 = 80,
    DungeonTileID_RedBloodWallDownLeft2 = 81,
    DungeonTileID_RedBloodWallDownLeft3 = 82,
    
    DungeonTileID_RedBloodWallDownRight1 = 83,
    DungeonTileID_RedBloodWallDownRight2 = 84,
    DungeonTileID_RedBloodWallDownRight3 = 85,
    
    // Green Blood
    DungeonTileID_GreenBloodGroundSmall1 = 86,
    DungeonTileID_GreenBloodGroundSmall2 = 87,
    DungeonTileID_GreenBloodGroundSmall3 = 88,
    
    DungeonTileID_GreenBloodGroundMedium1 = 89,
    DungeonTileID_GreenBloodGroundMedium2 = 90,
    
    DungeonTileID_GreenBloodGroundLarge1 = 91,
    DungeonTileID_GreenBloodGroundLarge2 = 92,
    
    DungeonTileID_GreenBloodWallUp1 = 93,
    DungeonTileID_GreenBloodWallUp2 = 94,
    DungeonTileID_GreenBloodWallUp3 = 95,
    
    DungeonTileID_GreenBloodWallDown1 = 96,
    DungeonTileID_GreenBloodWallDown2 = 97,
    DungeonTileID_GreenBloodWallDown3 = 98,
    
    DungeonTileID_GreenBloodWallLeft1 = 99,
    DungeonTileID_GreenBloodWallLeft2 = 100,
    DungeonTileID_GreenBloodWallLeft3 = 101,
    
    DungeonTileID_GreenBloodWallRight1 = 102,
    DungeonTileID_GreenBloodWallRight2 = 103,
    DungeonTileID_GreenBloodWallRight3 = 104,
    
    DungeonTileID_GreenBloodWallUpLeft1 = 105,
    DungeonTileID_GreenBloodWallUpLeft2 = 106,
    DungeonTileID_GreenBloodWallUpLeft3 = 107,
    
    DungeonTileID_GreenBloodWallUpRight1 = 108,
    DungeonTileID_GreenBloodWallUpRight2 = 109,
    DungeonTileID_GreenBloodWallUpRight3 = 110,
    
    DungeonTileID_GreenBloodWallDownLeft1 = 111,
    DungeonTileID_GreenBloodWallDownLeft2 = 112,
    DungeonTileID_GreenBloodWallDownLeft3 = 113,
    
    DungeonTileID_GreenBloodWallDownRight1 = 114,
    DungeonTileID_GreenBloodWallDownRight2 = 115,
    DungeonTileID_GreenBloodWallDownRight3 = 116
    } DungeonTileID;

typedef enum
{
    DungeonTileType_None,
    
    DungeonTileType_Floor,
    DungeonTileType_Water,
    DungeonTileType_Torch,
    DungeonTileType_Door
} DungeonTileType;

typedef enum
{
    DungeonRandomPosType_None,
    
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
    DungeonRoomFeatureType_None,
    
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
    DungeonTrapType_Summon,
    DungeonTrapType_Teleport,
    DungeonTrapType_Shaft,
        
    DungeonTrapType_Count
} DungeonTrapType;

typedef enum
{
    DungeonTrapFlag_HasBeenSeen = (1 << 1)
} DungeonTrapFlag;

typedef struct
{
    b32 valid;
    u32 chance;
} ItemTypeInfo;

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
    b32 is_seen;
    b32 has_been_seen;
    b32 is_occupied;
    
    DungeonTileID tile;
    DungeonTileID remains;
    } DungeonTile;

typedef struct
{
    u32 width;
    DungeonTile *array; // Allocated separately because it's used by multiple things.
} DungeonTiles;

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
    
    // Flood Fill
    f32 flood_fill_area_min;
    
    // Water
    b32 has_water;
    f32 water_min_total_area;
    
    // Torch
    u32 torch_count;
    u32 torches_generated_count;
    u32 torch_retry_count;
    u32 torch_min_spacing;
    
    // Door
    u32 door_count;
    u32 doors_generated_count;
    u32 door_retry_count;
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
    u32 trap_minimum_distance;
    
    DamageInfo spike_trap_damage;
    DamageInfo sword_trap_damage;
    DamageInfo arrow_trap_damage;
    DamageInfo magic_trap_damage;
    
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

struct Dungeon
{
    b32 can_pathfind;
    
    u32 level;
    v2u size;
    u32 area;
    v4u rect;
    
    DungeonSpec spec;
    
    DungeonTiles tiles;
    DungeonRooms rooms;
    DungeonTraps traps;
    DungeonPassages passages;
    };

// TODO(rami): Test having more than 10 dungeon levels, make allocating more memory at start work.
typedef struct
{
    u32 current_level;
    Dungeon levels[MAX_DUNGEON_LEVEL];
} Dungeons;

internal void dungeon_automaton_room_step(Random *random, DungeonTiles src_tiles, DungeonTiles dest_tiles, v4u rect);
internal void set_dungeon_pos_wall(Random *random, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_floor(Random *random, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_water(Random *random, DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_level_valid(u32 dungeon_level);
internal u32 get_dungeon_pos_trap_count(DungeonTiles tiles, DungeonTraps *traps, v2u pos);
internal b32 is_dungeon_pos_trap(DungeonTraps *traps, v2u pos);
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
internal v2u get_random_dungeon_pos(Random *random, v2u dungeon_size);
internal v2u get_random_dungeon_rect_pos(Random *random, v4u rect);
internal v4u get_dungeon_tileset_rect(DungeonTileID tile);
internal Dungeon *get_dungeon_from_level(Dungeons *dungeons, u32 level);