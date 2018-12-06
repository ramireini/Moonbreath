#ifndef RENDER_H
#define RENDER_H

#include <game.h>

void render_inventory();
void render_items(SDL_Rect *camera);
void render_interface();
void render_entities(SDL_Rect *camera);
void render_level(char *level, char *fov, SDL_Rect *camera);

#endif // RENDER_H