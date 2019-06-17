#ifndef MONSTER_H
#define MONSTER_H

#define MONSTER_COUNT 10

typedef enum
{
  ai_none,
  ai_wander
} monster_ai;

typedef enum
{
  monster_none,
  monster_player,
  monster_slime,
  monster_skeleton
} monster_type;

typedef struct
{
  monster_type type;
  monster_ai ai;
  sprite_t sprite;
  i32 x, y;
  i32 w, h;
  b32 in_combat;
  i32 max_hp;
  i32 hp;
  i32 damage;
  i32 armor;
  i32 speed;
  i32 level;
} monster_t;

#endif // MONSTER_H