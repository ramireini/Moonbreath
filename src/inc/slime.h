#ifndef SLIME_H
#define SLIME_H

#include <game.h>

#define SLIME_COUNT 1

extern slime_t slimes[SLIME_COUNT];

void create_slimes(int tile, int hp, int damage, int armor, int fov, int x, int y, int w, int h);
void update_slimes(char *level);
void render_slimes();
void delete_slimes(int i);

#endif // SLIME_H