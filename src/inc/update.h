#ifndef UPDATE_H
#define UPDATE_H

#include <game.h>

void add_console_msg(char *msg, uint32 msg_color, ...);
void update_input(char *level);
void update_events();
void update_camera();

// NOTE(Rami): add later
//void update_lighting(char *level, char *fov);

#endif // UPDATE_H