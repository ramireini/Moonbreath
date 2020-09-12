#define MAX_ENTITY_COUNT 128

typedef enum
{
    EntityID_None,
    
    EntityID_Dummy = EntityID_None,
    
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
    EntityBlood_Puddle,
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
    SpellID_Heal
} SpellID;

typedef struct
{
    SpellID id;
    u32 value;
} EntitySpell;

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
    
    u32 fov;
    u32 weight;
    u32 weight_to_evasion_ratio;
    
    StatusEffect statuses[StatusEffectType_Count];
} EntityPlayer;

typedef struct
{
    u32 damage;
    f32 time_waited;
    u32 level;
    
    u32 poison_chance;
    u32 poison_damage;
    u32 poison_duration;
    
    b32 is_ranger;
    b32 is_spellcaster;
    b32 in_combat;
    b32 is_flipped;
    b32 has_been_seen;
    
    b32 is_ghost_enabled;
    b32 is_ghost_flipped;
    v2u pos_save_for_ghost;
    v2u ghost_pos;
    
    b32 is_pathfind_set;
    Pathfind old_pathfind;
    v2u old_pathfind_pos;
    
    u32 spell_index;
    EntitySpell spells[2];
} EntityEnemy;

typedef struct
{
    EntityID id;
    
    char name[32];
    u32 max_hp;
    u32 hp;
    v2u pos;
    v2u new_pos;
    u32 w, h;
    v2u tile;
    EntityRemains remains;
    
    u32 defence;
    u32 evasion;
    f32 action_time;
    
    EntityType type;
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
} Entity;

internal void move_entity(DungeonTiles tiles, Entity *entity, v2u new_pos);
internal void add_enemy_entity(Entity *entities, DungeonTiles tiles, u32 *enemy_levels, EntityID id, u32 x, u32 y);
internal void remove_entity(Entity *entity);