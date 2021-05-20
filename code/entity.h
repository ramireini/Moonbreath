#define MAX_ENTITY_COUNT 256
#define MAX_ENTITY_SPELL_COUNT 4
#define MAX_ENTITY_STATUS_COUNT 8
#define MAX_PATHFIND_TRAIL_COUNT MAX_DUNGEON_SIZE

#define ENEMY_START_ID (EntityID_EnemyStart + 1)
#define ENEMY_END_ID (EntityID_EnemyEnd - ENEMY_START_ID)

typedef enum
{
    EntityID_None,
    EntityID_Player,
    
    EntityID_EnemyStart,
    EntityID_Dummy,
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
    
    EntityID_DwarwenWarrior,
    EntityID_DwarwenSorcerer,
    EntityID_DwarwenPriest,
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
    EntityID_Mahjarrat,
    EntityID_EnemyEnd,
    
    EntityID_Count
} EntityID;

typedef enum
{
    EntityType_None,
    
    EntityType_Player,
    EntityType_Enemy
} EntityType;

typedef enum
{
    EntityBlood_Splatter,
    EntityBlood_Puddle
} EntityBlood;

typedef enum
{
    EntityRemains_None,
    
    EntityRemains_RedBlood,
    EntityRemains_GreenBlood
} EntityRemains;

typedef enum
{
    EntityFlag_NotifyAboutMultipleItems = (1 << 1),
    EntityFlag_UsesPhysicalAttacks = (1 << 2),
    EntityFlag_UsesRangedAttacks = (1 << 3),
    EntityFlag_HasBeenSeen = (1 << 4),
    EntityFlag_IsFlipped = (1 << 5),
    EntityFlag_InCombat = (1 << 6),
    EntityFlag_IsPathfinding = (1 << 7),
    EntityFlag_IsGhostEnabled = (1 << 8),
    EntityFlag_IsGhostFlipped = (1 << 9),
    EntityFlag_IsInvisible = (1 << 10),
    EntityFlag_MovesNormallyOnWater = (1 << 11)
} EntityFlag;

typedef struct
{
    Direction direction;
    v2u pos;
} PathfindTrail;

typedef struct
{
    b32 render_pathfind_trail;
    PathfindTrail pathfind_trail[MAX_PATHFIND_TRAIL_COUNT];
    
    f32 turn_action_time;
    u32 weight;
    u32 weight_to_evasion_ratio;
    
    Item *weapon;
} EntityPlayer;

typedef struct
{
    b32 action_in_water;
    
    u32 level;
    u32 turns_in_player_view;
    f32 action_timer;
    
    b32 saved_flipped_for_ghost;
    v2u saved_pos_for_ghost;
    v2u ghost_pos;
    
    Spell *spell;
    u32 spell_count;
    Spell spells[MAX_ENTITY_SPELL_COUNT];
    
    u32 damage;
    DamageType damage_type;
    Status poison;
    } EntityEnemy;

struct Entity
{
    u32 flags;
    char select_letter;
    
    EntityID id;
    String32 name;
    u32 hp;
    u32 max_hp;
    
    u32 dungeon_level;
    v2u new_pos;
    v2u pos;
    Direction new_direction;
    
    u32 width;
    u32 height;
    v4u tile_src;
    EntityRemains remains;
    v2u pathfind_target_pos;
    
    u32 fov;
    f32 action_time;
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    u32 defence;
    u32 evasion;
    u32 hit_chance;
    
    // Levels of resistance go from -5 to 5
    // Having 5 points of resistance grants you immunity
    s32 resistances[DamageType_Count];
     Status statuses[MAX_ENTITY_STATUS_COUNT];
    
    EntityType type;
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
};

typedef struct
{
    PathfindMap player_pathfind_map; // For player auto exploring
    PathfindMap enemy_pathfind_map; // For enemies to reach the player
    
    Entity array[MAX_ENTITY_COUNT];
} EntityState;

internal void remove_entity(Entity *entity);
internal void kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui);
internal void add_entity_status(Status *statuses, Status *new_status);
internal void teleport_entity(Random *random, Entity *player, Dungeon *dungeon, UI *ui);
internal void set_flag_if_player_is_not_pathfinding(u32 player_flags, u32 *flags, u32 flag);
internal void attack_entity(Random *random, Entity *attacker, Entity *defender, Dungeon *dungeon, UI *ui, u32 damage, DamageType damage_type);
internal u32 get_dungeon_pos_entity_count(EntityState *entities, u32 dungeon_level, v2u pos, b32 enemy_only);
internal u32 get_enemy_level_from_entity_id(EntityID id);
internal b32 move_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, v2u pos);
internal b32 is_entity_valid(Entity *entity);
internal b32 is_entity_valid_and_player(Entity *entity);
internal b32 heal_entity(Entity *entity, u32 value);
internal b32 was_pressed_core(InputState *state);
internal Entity *add_enemy_entity(EntityState *entities, Dungeon *dungeon, EntityID id, u32 x, u32 y);