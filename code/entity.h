#define MAX_ENTITY_COUNT 128

typedef enum
{
    EntityID_None,
    
    // TODO(rami): Unused
    EntityID_FrostShards = EntityID_None,
    EntityID_GreenMamba = EntityID_None,
    
    EntityID_Skeleton,
    EntityID_CaveBat,
    EntityID_Slime,
    EntityID_Rat,
    EntityID_Snail,
    EntityID_Dog,
    EntityID_GiantSlime,
    EntityID_SkeletonWarrior,
    EntityID_Goblin,
    EntityID_Python,
    EntityID_OrcWarrior,
    EntityID_Assassin,
    EntityID_Kobold,
    EntityID_Ghoul,
    EntityID_Centaur,
    EntityID_Imp,
    EntityID_FloatingEye,
    EntityID_UndeadElfWarrior,
    EntityID_Viper,
    EntityID_FrostWalker,
    EntityID_GoblinWarrior,
    EntityID_DwarwenWarrior,
    EntityID_Minotaur,
    EntityID_Tormentor,
    EntityID_Treant,
    EntityID_Devourer,
    EntityID_Wolf,
    EntityID_CentaurWarrior,
    EntityID_BrimstoneImp,
    EntityID_Spectre,
    EntityID_FlyingSkull,
    EntityID_Hellhound,
    EntityID_BlackKnight,
    EntityID_GiantDemon,
    EntityID_CursedBlackKnight,
    EntityID_ScarletKingsnake,
    EntityID_Griffin,
    EntityID_Ogre,
    EntityID_Cyclops,
    
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
    EffectType_Might,
    EffectType_Wisdom,
    EffectType_Agility,
    EffectType_Fortitude,
    EffectType_Resistance,
    EffectType_Focus,
    EffectType_Curing,
    EffectType_Flight,
    EffectType_Decay,
    EffectType_Weakness,
    // TODO(rami): Should enemies be poisoned sometimes?
    EffectType_Poison,
    EffectType_Confusion,
    
    EffectType_Count
} EffectType;

typedef struct
{
    b32 is_enabled;
    u32 duration;
    u32 value;
} StatusEffect;

typedef struct
{
    u32 accuracy;
    f32 attack_speed;
    u32 fov;
    u32 weight;
    u32 weight_evasion_ratio;
    
    // TODO(rami): Should enemies drop gold sometimes?
    u32 gold;
    
    StatusEffect effects[EffectType_Count];
} EntityPlayer;

typedef struct
{
    f32 wait_timer;
    
    u32 level;
    b32 is_flipped;
    b32 in_combat;
    b32 has_been_seen;
    
    b32 is_ghost_enabled;
    b32 is_ghost_flipped;
    v2u pos_save_for_ghost;
    v2u ghost_pos;
    
    // TODO(rami): Do we want an is_aggressive?
    // TODO(rami): Do we want enemies to sometimes open doors? (boolean + implementation)
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
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    
    u32 damage;
    u32 defence;
    u32 evasion;
    f32 action_speed;
    
    EntityType type;
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
} Entity;

internal void move_entity(Dungeon *dungeon, v2u new_pos, Entity *entity);
internal void add_enemy_entity(Entity *entities, Dungeon *dungeon, u32 *enemy_levels, EntityID id, u32 x, u32 y);
internal void kill_entity(RandomState *random, Dungeon *dungeon, String128 *log, Entity *entity);
internal void remove_entity(Entity *entity);