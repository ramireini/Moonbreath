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
    v2i pos;
    v2i size;
    b32 in_combat;
    i32 max_hp;
    i32 hp;
    i32 damage;
    i32 armor;
    i32 speed;
    i32 level;
} monster_t;

#endif // MONSTER_H