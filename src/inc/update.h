#ifndef UPDATE_H
#define UPDATE_H

#include <game.h>

// NOTE(Rami): add later
//void update_lighting(char *level, char *fov, player_t *player);

void update_add_console_msg(char *msg, int msg_color, ...);
void update_input(char *level, player_t *player);
void update_events();
void update_camera(SDL_Rect *camera, player_t *player);
void update_entity_move(char *level, player_t *entity, int x, int y);

#endif // UPDATE_H