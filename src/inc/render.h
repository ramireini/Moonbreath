#ifndef RENDER_H
#define RENDER_H

#include <game.h>

void render_inventory(player_t *player);
void render_items(SDL_Rect *camera);
void render_interface(player_t *player);
void render_player(SDL_Rect *camera, player_t *player);
void render_level(char *level, char *fov, SDL_Rect *camera);

#endif // RENDER_H