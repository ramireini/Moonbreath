typedef enum
{
    ai_none,
    ai_wandering
} monster_ai;

typedef enum
{
    monster_none,
    monster_slime,
    monster_skeleton,
    
    monster_total
} monster_type;

typedef struct
{
    char name[16];
    entity_state state;
    monster_type type;
    monster_ai ai;
    v2u pos;
    v2u new_pos;
    v2u size;
    b32 in_combat;
    u32 max_hp;
    u32 hp;
    u32 damage;
    u32 armor;
    u32 speed;
    u32 level;
    
    sprite_t sprite;
    b32 sprite_flip;
} monster_t;

internal void add_monster(monster_type type, v2u pos);