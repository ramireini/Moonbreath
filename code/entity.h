#define MAX_ENTITY_COUNT 128
#define MAX_ENTITY_SPELL_COUNT 4

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
    StatusEffectType_Might,
    StatusEffectType_Wisdom,
    StatusEffectType_Agility,
    StatusEffectType_Elusion,
    StatusEffectType_Healing,
    StatusEffectType_Decay,
    StatusEffectType_Confusion,
    StatusEffectType_Poison,
    
    StatusEffectType_Count
} StatusEffectType;

typedef enum
{
    SpellID_None,
    
    SpellID_DarkBolt,
    SpellID_LesserHeal
} SpellID;

typedef enum
{
    SpellType_None,
    
    SpellType_Offensive,
    SpellType_Defensive
} SpellType;

typedef enum
{
    EntityFlags_PhysicalAttacks = (1 << 1),
    EntityFlags_RangedAttacks = (1 << 2),
    EntityFlags_MagicAttacks = (1 << 3),
    
    EntityFlags_Flipped = (1 << 4),
    EntityFlags_InCombat = (1 << 5),
    EntityFlags_HasBeenSeen = (1 << 6),
    EntityFlags_Pathfinding = (1 << 7),
    
    EntityFlags_GhostEnabled = (1 << 8),
    EntityFlags_GhostFlipped = (1 << 9)
} EntityFlags;

typedef struct
{
    b32 should_update;
    f32 action_count;
} player_input_result;

typedef struct
{
    SpellID id;
    SpellType type;
    DamageType damage_type;
    
    u32 value;
    u32 range;
    u32 chance;
} Spell;

typedef struct
{
    b32 is_enabled;
    u32 duration;
    u32 value;
} StatusEffect;

typedef struct
{
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    u32 weight;
    u32 weight_to_evasion_ratio;
    
    Pathfind pathfind_map;
    v2u pathfind_target;
    
    StatusEffect statuses[StatusEffectType_Count];
} EntityPlayer;

typedef struct
{
    f32 action_count_timer;
    u32 turns_in_player_view;
    u32 damage;
    u32 level;
    u32 poison_chance;
    u32 poison_damage;
    u32 poison_duration;
    
    Direction new_direction;
    b32 saved_flipped_for_ghost;
    v2u saved_pos_for_ghost;
    v2u ghost_pos;
    
    Pathfind pathfind_map;
    v2u pathfind_target;
    
    u32 spell_index;
    u32 spell_count;
    Spell spells[MAX_ENTITY_SPELL_COUNT];
} EntityEnemy;

typedef struct
{
    u32 flags;
    EntityID id;
    
    char name[32];
    u32 max_hp;
    u32 hp;
    v2u pos;
    v2u new_pos;
    u32 w, h;
    v2u tile_pos;
    EntityRemains remains;
    
    f32 action_count;
    u32 defence;
    u32 evasion;
    u32 fov;
    
    EntityType type;
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
} Entity;

internal b32 is_entity_valid_and_not_player(EntityType type);
internal void move_entity(Tiles tiles, Entity *entity, v2u new_pos);
internal void add_enemy_entity(Entity *entities, Tiles tiles, u32 *entity_levels, EntityID id, u32 x, u32 y);
internal void remove_entity(Entity *entity);
internal void kill_entity(Random *random, Tiles tiles, UI *ui, Entity *entity);