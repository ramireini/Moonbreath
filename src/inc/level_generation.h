#ifndef LEVEL_GENERATION_H
#define LEVEL_GENERATION_H

#include <tiles.h>
#include <types.h>

// NOTE(Rami): redo the dungeon gen, first generate corridors, then place rooms, then place doors
// currently we generate rooms, place corridors and then place doors which creates really bad
// door placement

void place_spawns(player_t *player, char *level_level, int level_pitch, int room_count, room_t *rooms);
void generate_level(char *level, int level_width, int level_height, int level_pitch, int room_count, player_t *player);
void init_level(char *level, int level_width, int level_height, int level_pitch);
void init_and_place_rooms(char *level, int level_width, int level_height, int level_pitch, int room_count, room_t *rooms);
void connect_rooms(char *level, int level_pitch, int room_count, room_t *rooms);
int is_room_valid(char *level, int level_pitch, room_t room);
int rand_int(int from, int to);
pos_t rand_pos_in_rect(room_t room);

#endif // LEVEL_GENERATION_H