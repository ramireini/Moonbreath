#ifndef LEVEL_GENERATION_H
#define LEVEL_GENERATION_H

#define LEVEL_WIDTH 2048
#define LEVEL_HEIGHT 2048
#define LEVEL_SIZE LEVEL_WIDTH / TILE_SIZE

#include <game.h>
#include <util_io.h>

// NOTE(Rami): redo the level gen, first generate corridors, then place rooms, then place doors
// currently we generate rooms, place corridors and then place doors which creates really bad
// door placement

void place_spawns(char *level_level, int level_pitch, int room_count, room_t *rooms);
void generate_level(char *level, int level_width, int level_height, int level_pitch, int room_count);
void init_level(char *level, int level_width, int level_height, int level_pitch);
void init_and_place_rooms(char *level, int level_width, int level_height, int level_pitch, int room_count, room_t *rooms);
void connect_rooms(char *level, int level_pitch, int room_count, room_t *rooms);
int is_room_valid(char *level, int level_pitch, room_t room);
void place_tile(char *level, int level_pitch, int x, int y, int tile);
pos_t rand_pos_in_rect(room_t room);

#endif // LEVEL_GENERATION_H