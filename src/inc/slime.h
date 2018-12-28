#ifndef SLIME_H
#define SLIME_H

#include <game.h>

#define SLIME_COUNT 2

extern slime_t *slimes[SLIME_COUNT];

// NOTE(Rami): Maybe have an ENTITY_COUNT instead of a count for each type of enemy,
// would make deallocating a little easier too..

// NOTE(Rami): Remove the breaks from the loops since we will probably
// have a function that will reorganize the array of slimes.
// This should also be made for every enemy type.
// 
// OR keep the breaks.

void create_slimes(int32 tile, int32 hp, int32 damage, int32 armor, int32 fov, int32 x, int32 y, int32 w, int32 h);
void update_slimes(char *level);
void render_slimes();
void free_slimes(int32 i);

#endif // SLIME_H