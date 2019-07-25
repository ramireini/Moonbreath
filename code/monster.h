#ifndef MONSTER_H
#define MONSTER_H

#define MONSTER_COUNT 10

typedef enum
{
    ai_none,
    ai_wandering
} monster_ai;

typedef enum
{
    monster_none,
    monster_slime,
    monster_skeleton
} monster_type;

typedef struct
{
    monster_type type;
    monster_ai ai;
    sprite_t sprite;
    v2u pos;
    v2u size;
    b32 in_combat;
    u32 max_hp;
    u32 hp;
    u32 damage;
    u32 armor;
    u32 speed;
    u32 level;
} monster_t;

#endif // MONSTER_H