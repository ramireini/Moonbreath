#ifndef RENDER_H
#define RENDER_H

#include <game.h>

void render_inventory(player_t *player, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, font_t *font_one, font_t *font_two);
void render_items(SDL_Texture *item_tileset_tex, SDL_Rect *camera);
void render_interface(player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex, font_t *font_one);
void render_player(SDL_Texture *player_tileset_tex, SDL_Texture *item_tileset_tex, SDL_Rect *camera, player_t *player);
void render_level(SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, char *level, char *fov, SDL_Rect *camera);

#endif // RENDER_H