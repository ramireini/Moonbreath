#ifndef PLAYER_H
#define PLAYER_H

#include <game.h>

extern player_t *player;

void create_player(char *name, int32 tile, int32 level, int32 money, int32 hp, int32 max_hp, int32 xp, int32 x, int32 y, int32 w, int32 h, int32 speed, int32 fov, int32 damage, int32 armor);
void update_player(char *level);
void render_player();
void place_player(int32 tile_x, int32 tile_y);
void free_player();

#endif // PLAYER_H