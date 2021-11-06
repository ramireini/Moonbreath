#define MAX_ENTITY_COUNT 256
#define MAX_ENTITY_SPELL_COUNT 4
#define MAX_ENTITY_STATUS_COUNT 64
#define MAX_PATHFIND_TRAIL_COUNT MAX_DUNGEON_SIZE

#define MAX_ENTITY_GHOST_COUNT MAX_ENTITY_COUNT
#define MAX_ENTITY_VIEW_RANGE 8
#define MAX_ENTITY_VIEW_TILES (MAX_ENTITY_VIEW_RANGE * MAX_ENTITY_VIEW_RANGE)

#define ENEMY_ID_START (EntityID_EnemyStart + 1)
#define ENEMY_ID_END (EntityID_EnemyEnd - ENEMY_ID_START)

#define MAX_ENTITY_RESIST_VALUE 5
#define MIN_ENTITY_RESIST_VALUE (-MAX_ENTITY_RESIST_VALUE)
#define ENTITY_RESISTANCE_PER_POINT ((100 / MAX_ENTITY_RESIST_VALUE) / 10)
#define get_player_entity() &entity_state->entities[0];

enum EntityID
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
};

typedef enum
{
    EntityGhostFlag_Active = (1 << 1),
    EntityGhostFlag_Flipped = (1 << 2),
} EntityGhostFlag;

typedef enum
{
    EntityFlag_ShowViewRange = (1 << 1),
    EntityFlag_NotifyAboutMultipleItems = (1 << 2),
    EntityFlag_UsesPhysicalAttacks = (1 << 3),
    EntityFlag_UsesRangedAttacks = (1 << 4),
    EntityFlag_HasBeenSeen = (1 << 5),
    EntityFlag_Flipped = (1 << 6),
    EntityFlag_FightingWithInvisible = (1 << 7),
    EntityFlag_Pathfinding = (1 << 8),
    EntityFlag_NormalWaterMovement = (1 << 9),
    EntityFlag_InvulnerableToTraps = (1 << 10),
    EntityFlag_Undead = (1 << 11),
    EntityFlag_CanOpenDoors = (1 << 12),
    EntityFlag_FleeOnLowHP = (1 << 13),
    EntityFlag_FightingFromCornered = (1 << 14),
    EntityFlag_HealthRegeneration = (1 << 15),
} EntityFlag;

typedef enum
{
    EnemyEntityState_None,
    
    EnemyEntityState_Wandering,
    EnemyEntityState_Fighting,
    EnemyEntityState_Chasing,
    EnemyEntityState_Fleeing,
    EnemyEntityState_Cornered,
} EnemyEntityState;

typedef enum
{
    EntityRemainsPlaceType_None,
    
    EntityRemainsPlaceType_Wound,
    EntityRemainsPlaceType_Kill
} EntityRemainsPlaceType;

typedef enum
{
    EntityRemainsType_None,
    
    EntityRemainsType_RedBlood,
    EntityRemainsType_GreenBlood
} EntityRemainsType;

typedef enum
{
    EntitySummonType_None,
    
    EntitySummonType_Undead
} EntitySummonType;

typedef enum
{
    EntityType_None,
    
    EntityType_Player,
    EntityType_Enemy
} EntityType;

typedef enum
{
    EntityResistInfoType_None,
    
    EntityResistInfoType_Partial,
    EntityResistInfoType_Full
} EntityResistInfoType;

typedef enum
{
    EntityInteractInfoType_None,
    
    EntityInteractInfoType_AttackBlocked,
    EntityInteractInfoType_AttackMissed,
} EntityInteractInfoType;

typedef enum
{
    EnemyWanderingType_None,
    
    EnemyWanderingType_Random,
    EnemyWanderingType_Travel,
} EnemyWanderingType;

typedef enum
{
    UpdateEnemyPathfindType_None,
    
    UpdateEnemyPathfindType_ReachedTarget,
    UpdateEnemyPathfindType_CantMove,
} UpdateEnemyPathfindType;

typedef struct
{
    b32 target_not_in_spell_range;
} EntitySpellCastResult;

typedef struct
{
    EntityResistInfoType type;
    u32 damage_after;
} EntityResistInfo;

struct Spell
{
    String32 name;
    String128 description;
    char select_letter;
    
    EntityStatusType status_type;
    EntityStatusStatType stat_type;
    EntityType target_type;
    
    u32 summon_flags;
    EntitySummonType summon_type;
    
    union
    {
        EntityDamage value;
        EntityDamage damage;
    };
    
    u32 duration;
    u32 chance;
    u32 range;
};

typedef struct
{
    v2u pos;
    Direction direction;
} PathfindTrail;

typedef struct
{
    b32 next_turn;
    u32 value;
    u32 max;
    u32 advance;
    u32 current;
} EntityRegen;

typedef struct
{
    u32 str;
    u32 intel;
    u32 dex;
    u32 def;
    u32 ev;
    u32 fov;
} EntityStats;

typedef struct
{
    EntityStats base_stats;
    u32 weight;
    u32 weight_evasion_ratio;
    f32 action_time;
    
    b32 render_pathfind;
    b32 wait_before_pathfind_reset;
    PathfindTrail pathfind_trail[MAX_PATHFIND_TRAIL_COUNT];
    
    Item *equipment[ItemSlot_Count];
    
    // Store a pointer because otherwise we'd have to pass it many times.
    PathfindMap *pathfind_map_to_player;
} PlayerEntity;

typedef struct
{
    b32 set;
    b32 flipped;
    
    u32 dungeon_level;
    v2u pos;
    v4u tile_src;
    
    Entity *parent;
} EntityGhost;

typedef struct
{
    EnemyEntityState state;
    
    u32 level;
    u32 player_seen_count;
    f32 action_timer;
    v4u view_rect;
    EnemyWanderingType wandering_type;
    
    b32 saved_ghost_flipped;
    v2u saved_ghost_pos;
    EntityGhost *ghost;
    
    Spell *selected_spell;
    u32 spell_count;
    Spell spells[MAX_ENTITY_SPELL_COUNT];
    
    EntityDamage damage;
    EntityStatus on_hit_status;
    u32 get_hit_teleport_chance;
    f32 hp_flee_percent;
    f32 hp_resume_wandering_percent;
    
    PathfindMap pathfind_map_to_pos;
    PathfindMap *pathfind_map_to_player;
} EnemyEntity;

typedef struct
{
    EntityID id;
    u32 level;
    u32 flags;
} EntityInfo;

typedef struct
{
    s32 value; // MIN_ENTITY_RESIST_VALUE - MAX_ENTITY_RESIST_VALUE
} EntityResist;

// Entity evasion value ranges
// 0, None
// 1 - 3, Very Low
// 4 - 7, Low
// 8 - 13, Average
// 14 - 17, High
// 18 - 21, Very High
// 22, Unhittable

struct Entity
{
    
#if MOONBREATH_SLOW
    b32 is_tracked;
    DebugTree *debug_tree;
#endif
    
    u32 index;
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
    EntityRemainsType remains_type;
    
    EntityStats stats;
    u32 hit_chance;
    f32 action_time;
    EntityRegen regen;
    
    EntityResist resists[EntityDamageType_Count];
    EntityStatus statuses[MAX_ENTITY_STATUS_COUNT];
    
    v2u pathfind_target_pos;
    PathfindMap pathfind_map;
    
    EntityType type;
    union
    {
        PlayerEntity p;
        EnemyEntity e;
    };
};

struct EntityState
{
    Entity entities[MAX_ENTITY_COUNT];
    EntityGhost ghosts[MAX_ENTITY_GHOST_COUNT];
    PathfindMap pathfind_map_to_player;
};

#if MOONBREATH_SLOW
internal char *get_entity_type_string(EntityType type);
internal char *get_enemy_entity_state_string(EnemyEntityState type);
#endif

internal void log_add_new_seen_target(char *string, UI *ui);
internal void change_entity_resist(EntityResist *resist, s32 change, b32 is_add);
internal void entity_move_force(Entity *entity, Dungeon *dungeon, v2u new_pos, u32 dungeon_level);
internal void remove_entity(Entity *entity, DungeonTiles tiles);
internal void log_add_entity_interact_string(Random *random, Entity *attacker, DungeonTrap *trap, Entity *defender, UI *ui, u32 value, EntityResistInfoType resist_type, EntityInteractInfoType info_type);
internal void change_entity_stat(u32 *value, s32 change, b32 is_add);
internal void update_entity_statuses(Random *random, Entity *entity, Dungeon *dungeon, Inventory *inventory, UI *ui);
internal void cast_entity_spell(Random *random, Entity *caster, Entity *target, Dungeon *dungeon, UI *ui);
internal void kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui);
internal void add_entity_status(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, EntityStatus *new_status);
internal void teleport_entity(Random *random, Entity *player, Dungeon *dungeon, UI *ui, DungeonRandomPosType type);
internal s32 get_total_stat_status_value(EntityStatus *statuses, EntityStatusStatType type);
internal u32 attack_entity(Random *random, Entity *attacker, DungeonTrap *trap, Entity *defender, Dungeon *dungeon, UI *ui, EntityDamage damage, b32 log_interact);
internal u32 get_seen_enemy_entity_count(Entity *entities, Dungeon *dungeon);
internal u32 get_enemy_entity_level(EntityID id);
internal b32 is_entity_in_view_and_spell_range(Entity *attacker, v2u defender_pos, u32 spell_range);
internal b32 entity_move(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, v2u pos);
internal b32 is_enemy_entity_valid_in_level(Entity *enemy, u32 dungeon_level);
internal b32 is_enemy_entity_valid(Entity *entity);
internal b32 is_entity_valid(Entity *entity);
internal b32 is_player_entity_valid(Entity *entity);
internal u32 heal_entity(Entity *entity, u32 value);
internal b32 was_pressed_core(InputState *state, b32 is_repeat_valid);
internal b32 update_entity_health_regen(Entity *entity);
internal b32 is_pos_in_entity_view_rect(Entity *entity, v2u dungeon_size, v2u pos);
internal v2u get_entity_tile_pos(EntityID id);
internal EntityID get_random_enemy_entity_id(Random *random);
internal EntityInfo get_enemy_entity_info(EntityID id);
internal EntityInfo get_random_enemy_entity_info(Random *random);
internal Entity *add_entity(EntityState *entity_state, EntityID id, u32 x, u32 y, Dungeon *dungeon);