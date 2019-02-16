#include <level_gen.h>

uint8 *level;
uint8 *room_gen;
uint8 *temp;

#define START_ALIVE_CHANCE 55
#define SMOOTHING_ITERATIONS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3
#define ALIVE TILE_FLOOR_STONE
#define DEAD TILE_NONE

int32 count_alive_neighbours(pos_t p)
{
	int32 count = 0;

	for(int32 y = p.y - 1; y < p.y + 2; y++)
	{
		for(int32 x = p.x - 1; x < p.x + 2; x++)
		{
			if(x == p.x && y == p.y)
			{
			}
			else if(x >= 0 && y >= 0 && temp[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				count++;
			}
		}
	}

	return count;
}

bool32 copy_src_to_dst(uint8 *src, uint8 *dst, SDL_Rect src_r, pos_t dst_c)
{
	if(!src || !dst)
	{
		return false;
	}

	for(int32 y = 0; y < src_r.h; y++)
	{
		for(int32 x = 0; x < src_r.w; x++)
		{
			dst[((y + dst_c.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_c.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}

	return true;
}

bool32 set_rect_to_dst(uint8 *dst, SDL_Rect r, int32 tile)
{
	if(!dst)
	{
		 return false;
	}

	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			dst[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
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

bool32 search_for_door_position(pos_t c, pos_t *door)
{
	for(int32 y = c.y - 1; y < c.y + 2; y++)
	{
		for(int32 x = c.x - 1; x < c.x + 2; x++)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_FLOOR_STONE ||
				 level[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == TILE_FLOOR_STONE ||
				 level[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == TILE_FLOOR_STONE ||
				 level[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_FLOOR_STONE)
				{
					door->x = x;
					door->y = y;
					return true;
				}
			}
		}
	}

	return false;
}

void add_walls_to_rect_in_dst(uint8 *dst, SDL_Rect r)
{
	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] == TILE_FLOOR_STONE)
			{
				if(dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_NONE)
				{
					dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] = TILE_WALL_STONE;
				}
				else if(dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == TILE_NONE)
				{
					dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] = TILE_WALL_STONE;
				}

				if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == TILE_NONE)
				{
					dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] = TILE_WALL_STONE;
				}
				else if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == TILE_NONE)
				{
					dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] = TILE_WALL_STONE;
				}
			}
		}
	}
}

bool32 can_room_be_placed(SDL_Rect r)
{
	if(!is_rect_in_dst_unused(level, (SDL_Rect){r.x, r.y, r.w, r.h}))
	{
		printf("Space was not unused\n");
		return false;
	}

	for(int32 y = 0; y < r.h; y++)
	{
		for(int32 x = 0; x < r.w; x++)
		{
			if(room_gen[(y * LEVEL_WIDTH_IN_TILES) + x] == TILE_FLOOR_STONE)
			{
				if((y != 0 || (x != 0 && x != r.w - 1)) &&
					 (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
					 (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
				{
					pos_t door = {0};
					if(search_for_door_position((pos_t){x + r.x, y + r.y}, &door))
					{
						level[(door.y * LEVEL_WIDTH_IN_TILES) + door.x] = TILE_DOOR_CLOSED;
						copy_src_to_dst(room_gen, level, (SDL_Rect){0, 0, r.w, r.h}, (pos_t){r.x, r.y});
						return true;
					}
				}
			}
		}
	}

	return false;
}

void smoothing(dimensions_t r)
{
	for(int32 y = 0; y < r.h; y++)
	{
		for(int32 x = 0; x < r.w; x++)
		{
			int32 count = count_alive_neighbours((pos_t){x, y});
			if(temp[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					room_gen[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
				else
				{
					room_gen[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					room_gen[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
				else
				{
					room_gen[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
			}
		}
	}
}

void generate_room(dimensions_t r)
{
	clear_dst(temp);
	clear_dst(room_gen);

	int32 room_type = num_between(0, 1);
	if(room_type == 0)
	{	
		set_rect_to_dst(room_gen, (SDL_Rect){0, 0, r.w, r.h}, TILE_FLOOR_STONE);
	}
	else if(room_type == 1)
	{
		for(int32 y = 0; y < r.h; y++)
		{
			for(int32 x = 0; x < r.w; x++)
			{
				if(num_between(1, 100) <= START_ALIVE_CHANCE)
				{
					temp[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
		}

		for(int32 i = 0; i < SMOOTHING_ITERATIONS; i++)
		{
			smoothing(r);
			copy_src_to_dst(room_gen, temp, (SDL_Rect){0, 0, r.w, r.h}, (pos_t){0, 0});
		}
	}
}

// NOTE(Rami): Delete later?
void place_player(int32 x, int32 y)
{
	player->new_x = x;
	player->new_y = y;
}

// NOTE(Rami): Have a 50/50 or another kind of chance to place a door/no door?
void generate_level()
{
	level = calloc(1, LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES);
	room_gen = calloc(1,  LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES);
	temp = calloc(1,  LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES);

	int32 x = LEVEL_WIDTH_IN_TILES / 2;
	int32 y = LEVEL_HEIGHT_IN_TILES / 2;
	int32 w = num_between(3, 10);
	int32 h = num_between(3, 10);

	place_player(x, y);

	set_rect_to_dst(level, (SDL_Rect){x, y, w, h}, TILE_FLOOR_STONE);
	add_walls_to_rect_in_dst(level, (SDL_Rect){x, y, w, h});

	for(int i = 0; i < 30; i++)
	{
		for(;;)
		{
			w = num_between(3, 10);
			h = num_between(3, 10);
			x = num_between(2, LEVEL_WIDTH_IN_TILES - w - 2);
			y = num_between(2, LEVEL_HEIGHT_IN_TILES - h - 2);

			generate_room((dimensions_t){w, h});
			if(can_room_be_placed((SDL_Rect){x, y, w, h}))
			{
				add_walls_to_rect_in_dst(level, (SDL_Rect){x, y, w, h});
				printf("\nRoom %d complete\n", i);
				break;
			}
		}
	}

	free(room_gen);
	free(temp);
}