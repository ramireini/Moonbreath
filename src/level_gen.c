#include <level_gen.h>

uint8 *level;

void place_tile(int32 x, int32 y, int32 tile)
{
	level[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
}

void fill_rect_with_tile(int32 level_pitch, SDL_Rect rect, int32 fill_tile)
{
	for(int32 x = rect.x; x < rect.x + rect.w; x++)
	{
		for(int32 y = rect.y; y < rect.y + rect.h; y++)
		{
			level[(y * level_pitch) + x] = fill_tile;
		}
	}
}

int32 check_rect_for_tile(int32 level_pitch, SDL_Rect rect, int32 check_tile)
{
	for(int32 x = rect.x; x < rect.x + rect.w; x++)
	{
		for(int32 y = rect.y; y < rect.y + rect.h; y++)
		{
			if(level[(y * level_pitch) + x] == check_tile)
			{
				return 1;
			}
		}
	}

	return 0;
}

int32 check_rect_is_only_tile(int32 level_pitch, SDL_Rect rect, int32 check_tile)
{
	for(int32 x = rect.x; x < rect.x + rect.w; x++)
	{
		for(int32 y = rect.y; y < rect.y + rect.h; y++)
		{
			if(level[(y * level_pitch) + x] != check_tile)
			{
				return 0;
			}
		}
	}

	return 1;
}

// NOTE(Rami): Seems to work on as low as 2 w/h and anything above
#define CHANCE_TO_START_ALIVE 65
// NOTE(Rami): Standard
// #define CHANCE_TO_START_ALIVE 55
#define SIMULATION_STEPS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3

#define ALIVE TILE_FLOOR_STONE
#define DEAD TILE_NONE

int32 count_alive_neighbours(int32 cell_x, int32 cell_y)
{
	int32 count = 0;

	for(int32 x = cell_x - 1; x < cell_x + 2; x++)
	{
		for(int32 y = cell_y - 1; y < cell_y + 2; y++)
		{
			if(x == cell_x && y == cell_y)
			{
			}
			else if(x < 0 || y < 0 || x >= LEVEL_WIDTH_IN_TILES || y >= LEVEL_HEIGHT_IN_TILES ||
				level[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				count++;
			}
		}
	}

	return count;
}

void simulation_step(int32 area_x, int32 area_y, int32 area_w, int32 area_h)
{
	uint8 new_level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

	for(int32 x = area_x; x < area_x + area_w; x++)
	{
		for(int32 y = area_y; y < area_y + area_h; y++)
		{
			int32 count = count_alive_neighbours(x, y);
			if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
				else
				{
					new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
				else
				{
					new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
			}
		}
	}

	for(int32 x = area_x; x < area_x + area_w; x++)
	{
		for(int32 y = area_y; y < area_y + area_h; y++)
		{
			level[(y * LEVEL_WIDTH_IN_TILES) + x] = new_level[(y * LEVEL_WIDTH_IN_TILES) + x];
		}
	}
}

void generate_room(int32 room_x, int32 room_y, int32 room_w, int32 room_h)
{	
	for(int32 x = room_x; x < room_x + room_w; x++)
	{
		for(int32 y = room_y; y < room_y + room_h; y++)
		{
			if(int_between(1, 100) <= CHANCE_TO_START_ALIVE)
			{
				level[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
			}
		}
	}

	for(int32 i = 0; i < SIMULATION_STEPS; i++)
	{
		simulation_step(room_x, room_y, room_w, room_h);
	}
}

void example(int32 x, int32 y, int32 w, int32 h)
{
	for(int ly = y; ly < y + h; ly++)
	{
		for(int lx = x; lx < x + w; lx++)
		{
			level[(ly * LEVEL_WIDTH_IN_TILES) + lx] = TILE_FLOOR_STONE;
		}
	}
}

void generate_level()
{
	level = calloc(1, LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES);

	place_player(0, 0);

	for(int32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
	{
		level[i] = TILE_NONE;
	}

	example(2, 2, 5, 5);
	place_tile(4, 7, TILE_DOOR_CLOSED);
	example(4, 8, 5, 5);

	// int32 fuck = int_between(4, 4);
	// int32 shit = int_between(4, 4);
	// generate_room(3, 3, fuck, shit);

	for(int32 y = 0; y < LEVEL_HEIGHT_IN_TILES; y++)
	{
		for(int32 x = 0; x < LEVEL_WIDTH_IN_TILES; x++)
		{
			if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == TILE_FLOOR_STONE)
			{
				if(level[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_NONE)
				{
					level[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] = TILE_WALL_STONE;
				}

				if(level[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_NONE)
				{
					level[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] = TILE_WALL_STONE;
				}

				if(level[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == TILE_NONE)
				{
					level[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] = TILE_WALL_STONE;
				}

				if(level[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == TILE_NONE)
				{
					level[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] = TILE_WALL_STONE;
				}
			}
		}
	}
}