#define MAX_ENTITY_COUNT 256
#define MAX_ENTITY_SPELL_COUNT 4
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
    EntityFlags_MultipleItemNotify = (1 << 1),
    EntityFlags_PhysicalAttacks = (1 << 2),
    EntityFlags_RangedAttacks = (1 << 3),
    EntityFlags_MagicAttacks = (1 << 4),
    EntityFlags_Flipped = (1 << 5),
    EntityFlags_Combat = (1 << 6),
    EntityFlags_HasBeenSeen = (1 << 7),
    EntityFlags_Pathfinding = (1 << 8),
    EntityFlags_GhostEnabled = (1 << 9),
    EntityFlags_GhostFlipped = (1 << 10),
    EntityFlags_Invisible = (1 << 11)
} EntityFlags;

typedef enum
{
    SpellID_None,
    
    SpellID_DarkBolt,
    SpellID_LesserHeal,
    SpellID_Bolster,
    
    SpellID_Count
} SpellID;

typedef enum
{
    SpellType_None,
    
    SpellType_Offensive,
    SpellType_Healing,
    SpellType_Buff,
} SpellType;

typedef struct
{
    SpellID id;
    SpellType type;
    DamageType damage_type;
    
    union
    {
        StatusEffect status_effect;
        
        struct
        {
            StatusEffectType status_effect_type;
            u32 value;
            u32 chance;
            u32 duration;
        };
    };
    
    u32 range;
} Spell;

typedef struct
{
    Direction direction;
    v2u pos;
} PathfindTrail;

typedef struct
{
    b32 render_pathfind_trail;
    PathfindTrail pathfind_trail[MAX_PATHFIND_TRAIL_COUNT];
    
    u32 weight;
    u32 weight_to_evasion_ratio;
} EntityPlayer;

typedef struct
{
    f32 action_count_timer;
    u32 turns_in_player_view;
    
    StatusEffect poison;
    u32 damage;
    u32 level;
    
    b32 saved_flipped_for_ghost;
    v2u saved_pos_for_ghost;
    v2u ghost_pos;
    
    u32 spell_index;
    u32 spell_count;
    Spell spells[MAX_ENTITY_SPELL_COUNT];
    
    DamageType damage_type;
} EntityEnemy;

typedef struct
{
    EntityID id;
    u32 flags;
    
    char name[32];
    
    u32 max_hp;
    u32 hp;
    
    v2u pos;
    v2u new_pos;
    u32 dungeon_level;
    
    u32 w, h;
    v4u tile_src;
    
    EntityRemains remains;
    Direction new_direction;
    
    f32 action_count;
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    u32 defence;
    u32 evasion;
    u32 fov;
    u32 hit_chance;
    
    v2u pathfind_target_pos;
    
    // Levels of resistance go from -5 to 5
    // Having 5 points of resistance grants you immunity
    s32 resistances[DamageType_Count];
    StatusEffect statuses[StatusEffectType_Count];
    
    EntityType type;
    union
    {
        EntityPlayer p;
        EntityEnemy e;
    };
} Entity;

typedef struct
{
    PathfindMap player_pathfind_map; // For player auto exploring
    PathfindMap enemy_pathfind_map; // For enemies to reach the player
    
    u32 levels[EntityID_Count];
    u32 spell_chances[SpellID_Count];
    Entity array[MAX_ENTITY_COUNT];
} EntityState;

internal void remove_entity(Entity *entity);
internal void kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui);
internal void start_entity_status_effect(Entity *entity, StatusEffect status);
internal void teleport_entity(Random *random, Entity *player, Dungeon *dungeon, UI *ui);
internal void attack_entity(Random *random, Entity *attacker, Entity *defender, Dungeon *dungeon, Inventory *inventory, UI *ui, u32 damage, DamageType damage_type, b32 came_from_trap);
internal b32 move_entity(Random *random, Entity *entity, Tiles tiles, UI *ui, v2u new_pos);
internal b32 heal_entity(Entity *entity, u32 value);
internal Entity *add_enemy_entity(EntityState *entities, Dungeon *dungeon, EntityID id, u32 x, u32 y);