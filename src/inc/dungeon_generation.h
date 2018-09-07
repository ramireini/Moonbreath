#ifndef DUNGEON_GENERATION_H
#define DUNGEON_GENERATION_H

#include "tiles.h"
#include "structs.h"

// NOTE(Rami): We could remove doors but I prefer having them.

// NOTE(Rami): We made the new pixel art for the door, now make one for the open door as well.

void generate_dungeon(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t* player);
void initialize_map(unsigned char *map, int map_pitch, int map_width, int map_height);
void initialize_and_place_rooms(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms);
void connect_rooms(unsigned char *map, int map_pitch, int room_count, room_t *rooms);
void place_corridors(unsigned char *map, int map_pitch, room_t room_a, room_t room_b, int direction);
int is_room_valid(unsigned char *map, int map_pitch, room_t room);
int random_int(int from, int to);
cell_t random_cell_in_rect(room_t room);

void place_spawns(entity_t *player, unsigned char *map, int map_pitch, int room_count, room_t *rooms);

#endif // DUNGEON_GENERATION_H