#define MAX_ENTITIES 128

typedef enum
{
    EntityID_None,
    
    EntityID_Rat,
    EntityID_Snail,
    EntityID_Slime,
    EntityID_GiantSlime,
    EntityID_Skeleton,
    EntityID_SkeletonWarrior,
    EntityID_OrcWarrior,
    EntityID_CaveBat,
    EntityID_Python,
    EntityID_Kobold,
    EntityID_Ogre,
    EntityID_Tormentor,
    EntityID_Imp,
    EntityID_GiantDemon,
    EntityID_Hellhound,
    EntityID_UndeadElfWarrior,
    EntityID_Assassin,
    EntityID_Goblin,
    EntityID_GoblinWarrior,
    EntityID_Viper,
    EntityID_ScarletKingsnake,
    EntityID_Dog,
    EntityID_GreenMamba,
    EntityID_Wolf,
    EntityID_FloatingEye,
    EntityID_Devourer,
    EntityID_Ghoul,
    EntityID_Cyclops,
    EntityID_DwarwenWarrior,
    EntityID_BlackKnight,
    EntityID_CursedBlackKnight,
    EntityID_Treant,
    EntityID_Minotaur,
    EntityID_CentaurWarrior,
    EntityID_Centaur,
    EntityID_FrostShards,
    EntityID_FrostWalker,
    EntityID_Griffin,
    EntityID_Spectre,
    EntityID_FlyingSkull,
    EntityID_BrimstoneImp,
    
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
} Effect;

typedef struct
{
    u32 accuracy;
    f32 attack_speed;
    u32 defence;
    u32 fov;
    
    // TODO(rami): Should enemies drop gold sometimes?
    u32 gold;
    
    Effect effects[EffectType_Count];
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
    // TODO(rami): Do we want enemies to sometimes open door? (boolean + implementation)
    b32 is_red_blooded;
    b32 is_green_blooded;
    b32 is_made_of_bone;
} EntityEnemy;

typedef struct
{
    char name[32];
    u32 max_hp;
    u32 hp;
    v2u pos;
    v2u new_pos;
    u32 w, h;
    v2u tile;
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    
    u32 damage;
    u32 evasion;
    
    f32 action_speed;
    
    EntityID id; // Which specific creature the entity is.
    EntityType type; // The type of creature the entity is.
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
} Entity;

internal void move_entity(Dungeon *dungeon, v2u new_pos, Entity *entity);
internal void add_enemy_entity(Entity *entities, Dungeon *dungeon, u32 *enemy_levels, EntityID id, u32 x, u32 y);
internal void kill_entity(GameState *game, Dungeon *dungeon, String128 *log, Entity *entity);
internal void remove_entity(Entity *entity);