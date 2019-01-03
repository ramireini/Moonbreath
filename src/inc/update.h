#ifndef UPDATE_H
#define UPDATE_H

#include <game.h>

int32 attack_entity(entity_t *attacker, entity_t *target);
void update_input(char *level);
void update_events();
void update_camera();

// NOTE(Rami): add later
//void update_lighting(char *level, char *fov);

#endif // UPDATE_H