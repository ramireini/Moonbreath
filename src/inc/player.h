#ifndef PLAYER_H
#define PLAYER_H

#include <game.h>

extern player_t *player;

void create_player(char *name, int tile, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov, int attack, int armor);
void update_player(char *level);
void free_player();

#endif // PLAYER_H