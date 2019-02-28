#ifndef SLIME_H
#define SLIME_H

#define SLIME_COUNT 1

typedef struct
{
  b32 in_combat;
  entity_t *entity;
} slime_t;

// NOTE(Rami): Maybe have a MAX_ENTITY_COUNT instead of a count for each type of enemy,
// would make deallocating a little easier too..

// NOTE(Rami): Remove the breaks from the loops since we will probably
// have a function that will reorganize the array of slimes.
// This should also be made for every enemy type.
// 
// OR keep the breaks.
// How we deal with this depends on if we switch to stack.

#endif // SLIME_H