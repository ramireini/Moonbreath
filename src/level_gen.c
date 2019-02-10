#include <level_gen.h>

uint8 *level;
uint8 *room_gen;
uint8 *temp;


// NOTE(Rami): Seems to work on 2x2 rooms and anything
// #define CHANCE_TO_START_ALIVE 65
// NOTE(Rami): Does not work on 3x3 rooms or below, seems to work on above
#define CHANCE_TO_START_ALIVE 55
#define SIMULATION_STEPS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3
#define ALIVE TILE_FLOOR_STONE
#define DEAD TILE_NONE

int32 count_alive_neighbours(int32 x, int32 y)
{
	int32 count = 0;

	for(int32 ax = x - 1; ax < x + 2; ax++)
	{
		for(int32 ay = y - 1; ay < y + 2; ay++)
		{
			if(ax == x && ay == y)
			{
			}
			else if(ax < 0 || ay < 0 || ax >= LEVEL_WIDTH_IN_TILES || ay >= LEVEL_HEIGHT_IN_TILES ||
				level[(ay * LEVEL_WIDTH_IN_TILES) + ax] == ALIVE)
			{
				count++;
			}
		}
	}

	return count;
}

// void simulation_step(int32 w, int32 h)
// {
// 	for(int32 ax = x; ax < x + w; ax++)
// 	{
// 		for(int32 ay = y; ay < y + h; ay++)
// 		{
// 			int32 count = count_alive_neighbours(ax, ay);
// 			if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
// 			{
// 				if(count < DEATH_LIMIT)
// 				{
// 					temp[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
// 				}
// 				else
// 				{
// 					temp[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
// 				}
// 			}
// 			else
// 			{
// 				if(count > BIRTH_LIMIT)
// 				{
// 					temp[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
// 				}
// 				else
// 				{
// 					temp[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
// 				}
// 			}
// 		}
// 	}
// }

void generate_room_to_room_gen(int32 w, int32 h)
{
	for(int32 ax = 0; ax < w; ax++)
	{
		for(int32 ay = 0; ay < h; ay++)
		{
			if(int_between(1, 100) <= CHANCE_TO_START_ALIVE)
			{
				room_gen[(ay * LEVEL_WIDTH_IN_TILES) + ax] = ALIVE;
				// temp[(ay * LEVEL_WIDTH_IN_TILES) + ax] = ALIVE;
			}
		}
	}

	// for(int32 i = 0; i < SIMULATION_STEPS; i++)
	for(int32 i = 0; i < 1; i++)
	{
		// simulation_step(w, h);
	}
}

bool32 set_tile_to_dst(uint8 *dst, cell_t c, int32 dst_tile)
{
	if(!dst)
	{
		return false;
	}

	dst[(c.y * LEVEL_WIDTH_IN_TILES) + c.x] = dst_tile;
	return true;
}

bool32 copy_src_to_dst(uint8 *src, uint8 *dst, SDL_Rect src_r, SDL_Rect dst_r)
{
	if(!src || !dst)
	{
		return false;
	}

	for(int32 y = 0; y < src_r.h; y++)
	{
		for(int32 x = 0; x < src_r.w; x++)
		{
			dst[((y + dst_r.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_r.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}

	return true;
}

bool32 set_rect_to_dst(uint8 *dst, SDL_Rect dst_r, int32 dst_tile)
{
	if(!dst)
	{
		 return false;
	}

	for(int32 y = 0; y < dst_r.h; y++)
	{
		for(int32 x = 0; x < dst_r.w; x++)
		{
			dst[((y + dst_r.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_r.x)] = dst_tile;
		}
	}

	return true;
}

bool32 is_rect_in_dst_unused(uint8 *dst, SDL_Rect r)
{
	if(!dst)
	{
		return false;
	}

	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] != TILE_NONE)
			{
				return false;
			}
		}
	}

	return true;
}

bool32 clear_dst(uint8 *dst)
{
	if(!dst)
	{
		return false;
	}

	for(int32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES; i++)
	{
		dst[i] = TILE_NONE;
	}

	return true;
}

cell_t search_door_placement(int32 x, int32 y)
{
	cell_t door = {0};

	// printf("Trying to find a place for a door from\n");
	// printf("x: %d\n", x);
	// printf("y: %d\n", y);

	for(int32 ay = y - 1; ay < y + 2; ay++)
	{
		for(int32 ax = x - 1; ax < x + 2; ax++)
		{
			if((ay == y || ax == x) && (ay != y || ax != x))
			{
				if(level[(ay * LEVEL_WIDTH_IN_TILES) + ax] == TILE_WALL_STONE)
				{
					if(level[((ay - 1) * LEVEL_WIDTH_IN_TILES) + ax] == TILE_FLOOR_STONE ||
						level[(ay * LEVEL_WIDTH_IN_TILES) + (ax - 1)] == TILE_FLOOR_STONE ||
						level[(ay * LEVEL_WIDTH_IN_TILES) + (ax + 1)] == TILE_FLOOR_STONE ||
						level[((ay + 1) * LEVEL_WIDTH_IN_TILES) + ax] == TILE_FLOOR_STONE)
					{
						// printf("Found a placement for a door at exacly\n");
						// printf("door x: %d\n", ax);
						// printf("door y: %d\n", ay);

						door.x = ax;
						door.y = ay;

						return door;	
					}
				}
			}
		}
	}

	return door;
}

bool32 set_room_gen_to_level(int32 x, int32 y, int32 w, int32 h)
{
	bool32 door_placed = false;

	// if(is_area_free(x, y, w, h))
	// {	
	// 	for(int32 ay = 0; ay < h; ay++)
	// 	{ 
	// 		for(int32 ax = 0; ax < w; ax++)
	// 		{
	// 			if(room_gen[(ay * LEVEL_WIDTH_IN_TILES) + ax] == TILE_FLOOR_STONE)
	// 			{
	// 				if(ay == 0 || ay == h - 1 || ax == 0 || ax == w - 1)
	// 				{
	// 					pos_t door = search_door_placement(ax + x, ay + y);
	// 					if(door.y && door.x)
	// 					{
	// 						level[(door.y * LEVEL_WIDTH_IN_TILES) + door.x] = TILE_DOOR_CLOSED;
	// 						door_placed = true;
	// 						goto is_door_placed;
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// is_door_placed:
	if(door_placed || !door_placed) // NOTE(Rami): 
	// if(door_placed)
	{
		for(int32 ay = 0; ay < h; ay++)
		{
			for(int32 ax = 0; ax < w; ax++)
			{
				level[((ay + y) * LEVEL_WIDTH_IN_TILES) + (ax + x)] = room_gen[(ay * LEVEL_WIDTH_IN_TILES) + ax];
			}
		}

		// for(int32 ay = y - 1; ay < y + h + 1; ay++)
		// {
		// 	for(int32 ax = x - 1; ax < x + w + 1; ax++)
		// 	{
		// 		if(level[(ay * LEVEL_WIDTH_IN_TILES) + ax] == TILE_NONE)
		// 		{
		// 			level[(ay * LEVEL_WIDTH_IN_TILES) + ax] = TILE_WALL_STONE;
		// 		}
		// 	}
		// }
	}

	return door_placed;
}

void generate_level()
{
	level = calloc(1, LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES);
	room_gen = calloc(1,  LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES);
	temp = calloc(1,  LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES);

	place_player(0, 0);

	// set_room_to_level(10, 8, 5, 5);

	for(int32 ay = 0; ay < 5; ay++)
	{
		for(int32 ax = 0; ax < 5; ax++)
		{
			room_gen[(ay * LEVEL_WIDTH_IN_TILES) + ax] = TILE_FLOOR_STONE;
		}
	}

	// copy_src_to_dst(room_gen, level, (SDL_Rect){0, 0, 5, 5}, (SDL_Rect){2, 2, 5, 5});
	set_rect_to_dst(level, (SDL_Rect){4, 4, 5, 5}, TILE_FLOOR_STONE);

	// for(int32 i = 0; i < 1; i++)
	// {
		// int32 room_w = 5;
		// int32 room_h = 5;
		// int32 room_x = 20;
		// int32 room_y = 20;

	// 	for(;;)
	// 	{
	// 		int32 room_w = int_between(2, 10);
	// 		int32 room_h = int_between(2, 10);
	// 		int32 room_x = int_between(1, LEVEL_WIDTH_IN_TILES - room_w - 2);
	// 		int32 room_y = int_between(1, LEVEL_HEIGHT_IN_TILES - room_h - 2);

	// 		printf("\nRoom %d\n", i);
		// generate_room_to_room_gen(room_w, room_h); // NOTE(Rami): 
			// set_room_to_room_gen(room_w, room_h);
		// set_room_gen_to_level(room_x, room_y, room_w, room_h);
	// 		if(set_room_gen_to_level(room_x, room_y, room_w, room_h))
	// 		{
	// 			printf("Was able to put a door\n");
	// 			clear_room_gen();
	// 			break;
	// 		}
	// 		else
	// 		{
	// 			printf("Was NOT able to put a door\n");
	// 		}
	// 	}
	// }

	free(room_gen);
	free(temp);
}