#define MAX_ENTITY_COUNT 256
#define MAX_ENTITY_SPELL_COUNT 4
#define MAX_ENTITY_STATUS_COUNT 64
#define MAX_PATHFIND_TRAIL_COUNT MAX_DUNGEON_SIZE

#define MAX_ENTITY_VIEW_RANGE 8
#define MAX_ENTITY_VIEW_TILES (MAX_ENTITY_VIEW_RANGE * MAX_ENTITY_VIEW_RANGE)

#define ENEMY_ID_START (EntityID_EnemyStart + 1)
#define ENEMY_ID_END (EntityID_EnemyEnd - ENEMY_ID_START)

#define get_player_entity() &entities->array[0];

typedef enum
{
    EntityID_None,
    
    EntityID_Dummy,
    EntityID_Player,
    
    EntityID_EnemyStart,
    EntityID_SkeletonWarrior,
    EntityID_SkeletonArcher,
    EntityID_SkeletonMage,
    EntityID_Bat,
    EntityID_Rat,
    
    EntityID_KoboldWarrior,
    EntityID_KoboldShaman,
    EntityID_Snail,
    EntityID_Slime,
    EntityID_Dog,
    
    EntityID_OrcWarrior,
    EntityID_OrcArcher,
    EntityID_OrcShaman,
    EntityID_Python,
    EntityID_Shade,
    
    EntityID_ElfKnight,
    EntityID_ElfArbalest,
    EntityID_ElfMage,
    EntityID_GiantSlime,
    EntityID_Spectre,
    
    EntityID_OrcAssassin,
    EntityID_OrcSorcerer,
    EntityID_Minotaur,
    EntityID_Treant,
    EntityID_Viper,
    
    EntityID_CentaurWarrior,
    EntityID_CentaurSpearman,
    EntityID_CentaurArcher,
    EntityID_CursedSkull,
    EntityID_Wolf,
    
    EntityID_OgreWarrior,
    EntityID_OgreArcher,
    EntityID_OgreMage,
    EntityID_Cyclops,
    EntityID_ShadowWalker,
    
    EntityID_DwarfKnight,
    EntityID_DwarfSoldier,
    EntityID_DwarfTinkerer,
    EntityID_ScarletSnake,
    EntityID_Lich,
    
    EntityID_AbyssalFiend,
    EntityID_BloodTroll,
    EntityID_IronGolem,
    EntityID_Griffin,
    EntityID_Imp,
    
    EntityID_BlackKnight,
    EntityID_GiantDemon,
    EntityID_Hellhound,
    EntityID_AbyssalHexmaster,
    EntityID_Zarimahar,
    EntityID_EnemyEnd,
    
    EntityID_Count
} EntityID;

typedef enum
{
    EntityFlag_ShowViewRange = (1 << 1),
    EntityFlag_NotifyAboutMultipleItems = (1 << 2),
    EntityFlag_UsesPhysicalAttacks = (1 << 3),
    EntityFlag_UsesRangedAttacks = (1 << 4),
    EntityFlag_HasBeenSeen = (1 << 5),
    EntityFlag_Flipped = (1 << 6),
    EntityFlag_InCombat = (1 << 7),
    EntityFlag_Pathfinding = (1 << 8),
    EntityFlag_GhostEnabled = (1 << 9),
    EntityFlag_GhostFlipped = (1 << 10),
    EntityFlag_Invisible = (1 << 11),
    EntityFlag_NormalWaterMovement = (1 << 12),
    EntityFlag_InvulnerableToTraps = (1 << 13),
    EntityFlag_Undead = (1 << 14)
} EntityFlag;

typedef enum
{
    EntityBlood_Splatter,
    EntityBlood_Puddle
} EntityBlood;

typedef enum
{
    EntityRemainsType_None,
    
    EntityRemainsType_Wound,
    EntityRemainsType_Kill
} EntityRemainsType;

typedef enum
{
    EntityRemains_None,
    
    EntityRemains_RedBlood,
    EntityRemains_GreenBlood
} EntityRemains;

typedef enum
{
    SummonType_None,
    
    SummonType_Undead
} SummonType;

typedef enum
{
    EntityType_None,
    
    EntityType_Player,
    EntityType_Enemy
} EntityType;

typedef enum
{
    ResistAmount_None,
    
    ResistAmount_Partial,
    ResistAmount_Full
} ResistAmount;

typedef struct
{
    ResistAmount amount;
    u32 damage_after;
} ResistInfo;

struct Spell
{
    String32 name;
    String128 description;
    char select_letter;
    
    StatusType type;
    StatType stat_type;
    EntityType target_type;
    
    u32 summon_flags;
    SummonType summon_type;
    
    union
    {
        DamageInfo value;
        DamageInfo damage;
    };
    
    u32 duration;
    u32 chance;
    u32 range;
};

typedef struct
{
    Direction direction;
    v2u pos;
} PathfindTrail;

typedef struct
{
    b32 next_turn;
    
    u32 hp_increase;
    u32 max;
    u32 current;
    u32 advance;
    } EntityRegen;

typedef struct
{
    // Default player stats
    u32 base_str;
    u32 base_intel;
    u32 base_dex;
    u32 base_def;
    u32 base_ev;
    u32 base_view_range;
    
    f32 action_time;
    u32 weight;
    u32 weight_evasion_ratio;
    
    b32 render_pathfind_trail;
    PathfindTrail pathfind_trail[MAX_PATHFIND_TRAIL_COUNT];
    
    Item *weapon;
} PlayerEntity;

// TODO(rami): If the enemy is not chasing a target, it should be in a roam / patrol mode meaning it will
// travel to randomly selected positions in the dungeon.
typedef struct
{
    u32 level;
    u32 turns_in_player_view;
    f32 action_timer;
    v4u view_rect;
    
    b32 saved_flipped_for_ghost;
    v2u saved_pos_for_ghost;
    v2u ghost_pos;
    
    Spell *spell;
    u32 spell_count;
    Spell spells[MAX_ENTITY_SPELL_COUNT];
    
    DamageInfo damage;
    Status attack_status;
    
    u32 hit_teleport_chance;
    } EnemyEntity;

typedef struct
{
    EntityID id;
    u32 level;
    u32 flags;
} EntityInfo;

// Entity evasion values
// 0, None
// 1 - 3, Very Low
// 4 - 7, Low
// 8 - 13, Average
// 14 - 17, High
// 18 - 21, Very High
// 22, Unhittable

// Entity resistance values range from -5 to 5.
// Each point equals to 20% resistance/susceptibility.

struct Entity
{
    u32 flags;
    char select_letter;
    
    EntityID id;
    String32 name;
    
    u32 hp;
    u32 max_hp;
    v2u pos;
    v2u new_pos;
    u32 dungeon_level;
    Direction direction;
    
    u32 width;
    u32 height;
    v4u tile_src;
    EntityRemains remains;
    v2u pathfind_target_pos;
    
    u32 str;
    u32 intel;
    u32 dex;
    u32 def;
    u32 ev;
    u32 view_range;
    
    u32 hit_chance;
    f32 action_time;
    EntityRegen regen;
    
    s32 resists[DamageType_Count];
    Status statuses[MAX_ENTITY_STATUS_COUNT];
    
    EntityType type;
    union
    {
         PlayerEntity p;
         EnemyEntity e;
    };
    
    DebugTree *debug_tree;
};

typedef struct
{
    PathfindMap player_pathfind_map; // For player auto exploring
    PathfindMap enemy_pathfind_map; // For enemies to reach the player
    
    Entity array[MAX_ENTITY_COUNT];
} EntityState;

internal void change_stat(u32 *value, s32 change, b32 is_add, b32 zero_clamp);
internal void update_entity_statuses(Game *game, PathfindMap *enemy_pathfind_map, Entity *entity, Dungeon *dungeon, Inventory *inventory, UI *ui);
internal void cast_entity_spell(Random *random, Entity *caster, Entity *target, Dungeon *dungeon, UI *ui);
internal void kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui);
internal void add_entity_status(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, Status *new_status);
internal void teleport_entity(Random *random, PathfindMap *enemy_pathfind_map, Entity *player, Dungeon *dungeon, UI *ui, DungeonRandomPosType type);
internal void set_flag_if_player_is_not_pathfinding(u32 player_flags, u32 *flags, u32 flag);
internal char *get_entity_type_string(EntityType type);
internal s32 get_total_stat_status_value(Status *statuses, StatType type);
internal u32 attack_entity(Random *random, PathfindMap *enemy_pathfind_map, Entity *attacker, Entity *defender, Dungeon *dungeon, UI *ui, DamageInfo damage);
internal u32 get_player_view_enemy_count(EntityState *entities, Dungeon *dungeon);
internal u32 get_dungeon_pos_entity_count(EntityState *entities, u32 dungeon_level, v2u pos, b32 enemy_only);
internal u32 get_entity_level(EntityID id);
internal EntityID get_random_enemy_entity_id(Random *random);
internal b32 entity_in_view_and_spell_range(v4u attacker_view, v2u attacker_pos, v2u defender_pos, u32 spell_range);
internal b32 move_entity(Random *random, PathfindMap *enemy_pathfind_map, Entity *entity, Dungeon *dungeon, UI *ui, v2u pos);
internal b32 is_pos_seen_and_flag_not_set(DungeonTiles tiles, v2u pos, u32 flags, u32 flag);
internal b32 is_enemy_entity_valid(Entity *entity);
internal b32 is_entity_valid(Entity *entity);
internal b32 is_player_entity_valid(Entity *entity);
internal b32 heal_entity(Entity *entity, u32 value);
internal b32 was_pressed_core(InputState *state);
internal Entity *add_enemy_entity(EntityState *entities, Dungeon *dungeon, EntityID id, u32 x, u32 y);
internal EntityInfo get_entity_info(EntityID id);
internal EntityInfo get_random_enemy_entity_info(Random *random);