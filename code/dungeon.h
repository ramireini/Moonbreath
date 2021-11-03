#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 128
#define MAX_DUNGEON_AREA (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE)

#define MAX_DUNGEON_ROOM_COUNT 256
#define MAX_DUNGEON_TRAP_COUNT 128
#define MAX_DUNGEON_PASSAGE_COUNT 8
#define TILE_SIZE 32

typedef enum
{
    DungeonTileID_None,
    
    DungeonTileID_StoneWall1,
    DungeonTileID_StoneWall2,
    DungeonTileID_StoneWall3,
    DungeonTileID_StoneWall4,
    DungeonTileID_StoneWall5,
    DungeonTileID_StoneWall6,
    DungeonTileID_StoneWall7,
    DungeonTileID_StoneWall8,
    DungeonTileID_StoneWall9,
    DungeonTileID_StoneWall10,
    
    DungeonTileID_StoneWallTorch1,
    DungeonTileID_StoneWallTorch2,
    DungeonTileID_StoneWallTorch3,
    DungeonTileID_StoneWallTorch4,
    
    DungeonTileID_StoneFloor1,
    DungeonTileID_StoneFloor2,
    DungeonTileID_StoneFloor3,
    DungeonTileID_StoneFloor4,
    
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
    DungeonTileID_SummonTrap,
    DungeonTileID_TeleportTrap,
    DungeonTileID_ShaftTrap,
    
    DungeonTileID_Water1,
    DungeonTileID_Water2,
    DungeonTileID_Water3,
    DungeonTileID_RedBloodWater1,
    DungeonTileID_RedBloodWater2,
    DungeonTileID_GreenBloodWater1,
    DungeonTileID_GreenBloodWater2,
    
    // Red Blood
    DungeonTileID_RedBloodGroundSmall1,
    DungeonTileID_RedBloodGroundSmall2,
    
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
    
    DungeonTileID_RedBloodWallUpLeft1,
    DungeonTileID_RedBloodWallUpLeft2,
    DungeonTileID_RedBloodWallUpLeft3,
    
    DungeonTileID_RedBloodWallUpRight1,
    DungeonTileID_RedBloodWallUpRight2,
    DungeonTileID_RedBloodWallUpRight3,
    
    DungeonTileID_RedBloodWallDownLeft1,
    DungeonTileID_RedBloodWallDownLeft2,
    DungeonTileID_RedBloodWallDownLeft3,
    
    DungeonTileID_RedBloodWallDownRight1,
    DungeonTileID_RedBloodWallDownRight2,
    DungeonTileID_RedBloodWallDownRight3,
    
    // Green Blood
    DungeonTileID_GreenBloodGroundSmall1,
    DungeonTileID_GreenBloodGroundSmall2,
    
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
    
    DungeonTileID_GreenBloodWallUpLeft1,
    DungeonTileID_GreenBloodWallUpLeft2,
    DungeonTileID_GreenBloodWallUpLeft3,
    
    DungeonTileID_GreenBloodWallUpRight1,
    DungeonTileID_GreenBloodWallUpRight2,
    DungeonTileID_GreenBloodWallUpRight3,
    
    DungeonTileID_GreenBloodWallDownLeft1,
    DungeonTileID_GreenBloodWallDownLeft2,
    DungeonTileID_GreenBloodWallDownLeft3,
    
    DungeonTileID_GreenBloodWallDownRight1,
    DungeonTileID_GreenBloodWallDownRight2,
    DungeonTileID_GreenBloodWallDownRight3,
    
    DungeonTileID_Count
} DungeonTileID;

typedef enum
{
    DungeonWallTileType_None,
    
    DungeonWallTileType_StoneWall1,
    DungeonWallTileType_StoneWall2,
    DungeonWallTileType_StoneWall3,
    DungeonWallTileType_StoneWall4,
    DungeonWallTileType_StoneWall5,
    DungeonWallTileType_StoneWall6,
    DungeonWallTileType_StoneWall7,
    DungeonWallTileType_StoneWall8,
    DungeonWallTileType_StoneWall9,
    DungeonWallTileType_StoneWall10,
    
    DungeonWallTileType_Count,
} DungeonWallTileType;

typedef enum
{
    DungeonTileType_None,
    
    DungeonTileType_Passage,
    DungeonTileType_Floor,
    DungeonTileType_Wall,
    DungeonTileType_Water,
    DungeonTileType_Torch,
    DungeonTileType_Door,
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
    RandomChanceType_Default,
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

struct PathfindMap
{
    u32 width;
    u32 array[MAX_DUNGEON_AREA];
};

typedef struct
{
    v4u src;
    DungeonTileID tile_id;
} DungeonRemainsInfo;

typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
    
    Entity *entity;
    
    DungeonTileID id;
    DungeonTileID seen_id;
    DungeonTileID remains_id;
} DungeonTile;

typedef struct
{
    u32 width;
    DungeonTile *array; // Allocated separately because used by multiple things.
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
    
    v2u value_range;
    EntityDamage damage;
    
    // Shafts cause you to fall a certain amount of dungeon levels.
    // When you fall down the same shaft the same data is used making you fall the same amount.
    b32 is_shaft_set;
    u32 shaft_depth;
    v2u shaft_dest;
};

typedef struct
{
    DungeonPassageType type;
    
    v2u pos;
    v2u dest_pos;
    u32 dungeon_level;
} DungeonPassage;

typedef struct
{
    u32 count;
    DungeonRoom array[MAX_DUNGEON_ROOM_COUNT];
} DungeonRooms;

typedef struct
{
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
    
    u32 wall_tile_chances[DungeonWallTileType_Count];
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
    
    EntityDamage spike_trap_damage;
    EntityDamage sword_trap_damage;
    EntityDamage arrow_trap_damage;
    EntityDamage magic_trap_damage;
    
    v2u bind_trap_value_range;
    v2u shaft_trap_value_range;
    
    // Passage
    u32 down_passage_count;
    u32 passage_min_spacing;
    
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
    b32 can_pathfind; // TODO(rami): This is a debug meme.
    
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

// TODO(rami): Have some entities that are not hostile to the player by default? But if we do this,
// we don't have an incentive to engage those entities. If we had XP and level ups, then the incentive
// would be there.

// TODO(rami): Major Polish: Entity remains on tiles like open doors. Has to be between the tile and door layers.
// TODO(rami): Art: Skeleton remains, a pile of bones maybe.

// TODO(rami): How to make dungeons feel alive
// You have a dungeon which is populated with things, having these things interact whether they be
// enemies, items or other is how the dungeon feels alive.

// This dungeon exists inside of the Moonbreath mountain because one of the Lords of Chaos instructed his servants
// to create it so they could create a Portal at the depths of it which would allow the Chaos Lord
// and his minions to invade the surface. The minions, the things on the other side waiting for the
// portal are creatures that defy imagination and would most likely raze everything on the surface.
// Player has heard of suspicious activity relating to the Moonbreath mountain and has decided to
// see what's going on over there.

// TODO(rami): Game guide
// TODO(rami): Mist tiles, limit FOV
// TODO(rami): Generic: Gold, treasure chests (mimic), swamps (traversable but slower than water?)
// TODO(rami): Plant/other entities that are placed near water? Some could be generated as damaged already.
// TODO(rami): Lava tiles, the ability to jump into the lava and die, if you really want to :)
// TODO(Rami): ARt for weapons and armor.
typedef struct
{
    u32 current_level;
    Dungeon levels[MAX_DUNGEON_LEVEL];
} DungeonState;

internal void remove_dungeon_trap(DungeonTrap *trap);
internal void add_dungeon_trap(DungeonSpec *spec, DungeonTraps *traps, DungeonTrapType type, v2u pos);
internal void dungeon_automaton_room_step(Random *random, DungeonSpec *spec, DungeonTiles src_tiles, DungeonTiles dest_tiles, v4u rect);
internal void set_dungeon_pos_wall(Random *random, DungeonSpec *spec, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_floor(Random *random, DungeonTiles tiles, v2u pos);
internal void set_dungeon_pos_water(Random *random, DungeonTiles tiles, v2u pos);
internal u32 get_dungeon_area_tile_type_count(DungeonTiles tiles, v2u pos, u32 area, DungeonTileType type);
internal b32 is_dungeon_level_valid(u32 dungeon_level);
internal b32 is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_traversable(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_floor(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_passage(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile_id);
internal b32 is_pos_inside_dungeon(v2u dungeon_size, v2u pos);
internal b32 is_dungeon_pos_water(DungeonTiles tiles, v2u pos);
internal b32 is_dungeon_pos_traversable_and_unoccupied(DungeonTiles tiles, v2u pos);
internal v2u get_dungeon_tileset_pos_from_tile_id(DungeonTileID id);
internal v2u get_random_dungeon_pos(Random *random, v2u dungeon_size);
internal v2u get_random_dungeon_feature_pos(Random *random, Dungeon *dungeon, ItemState *item_state, b32 accept_water);
internal v2u get_random_dungeon_rect_pos(Random *random, v4u rect);
internal v4u get_dungeon_tileset_rect(DungeonTileID tile_id);
internal v4u get_dungeon_trap_tile_src(DungeonTrapType type);
internal v4u get_dungeon_tile_rect_from_entity_id(EntityID id);
internal v4u get_dungeon_tile_rect(v2u tile);
internal DungeonTrap *get_dungeon_pos_trap(DungeonTraps *traps, v2u pos);
internal DungeonTileID get_dungeon_pos_remains_tile_id(DungeonTiles tiles, v2u pos);
internal DungeonRemainsInfo get_dungeon_pos_remains_info();
internal Dungeon *get_dungeon_from_level(DungeonState *dungeon_state, u32 level);
internal Entity *get_dungeon_pos_entity(DungeonTiles tiles, v2u pos);
internal DungeonTile *get_dungeon_pos_tile(DungeonTiles tiles, v2u pos);