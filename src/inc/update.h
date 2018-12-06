#ifndef UPDATE_H
#define UPDATE_H

#include <game.h>

int create_player(char *name, int tile, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov, int attack, int armor);
void add_console_msg(char *msg, int msg_color, ...);
void update_input(char *level);
void update_events();
void update_camera(SDL_Rect *camera);
void update_entities(char *level);

// NOTE(Rami): add later
//void update_lighting(char *level, char *fov, player_t *player);

#endif // UPDATE_H