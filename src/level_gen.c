uint8 level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

// NOTE(Rami): Do we need rooms to persist?
room_t *rooms;

int32 count_alive_neighbours(level_gen_buffers_t *buffers, pos_t p)
{
	int32 count = 0;

	for(int32 y = p.y - 1; y < p.y + 2; y++)
	{
		for(int32 x = p.x - 1; x < p.x + 2; x++)
		{
			if(x == p.x && y == p.y)
			{
			}
			else if(x >= 0 && y >= 0 && buffers->temp_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				count++;
			}
		}
	}

	return count;
}

int32 copy_src_to_dst(uint8 *src, uint8 *dst, SDL_Rect src_r, pos_t dst_c)
{
	if(!src || !dst)
	{
		return 0;
	}

	for(int32 y = 0; y < src_r.h; y++)
	{
		for(int32 x = 0; x < src_r.w; x++)
		{
			dst[((y + dst_c.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_c.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}

	return 1;
}

int32 set_rect_to_dst(uint8 *dst, SDL_Rect r, int32 tile)
{
	if(!dst)
	{
		 return 0;
	}

	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			dst[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
		}
	}

	return 1;
}

int32 is_rect_in_dst_unused(uint8 *dst, SDL_Rect r)
{
	if(!dst)
	{
		return 0;
	}

	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] != tile_none)
			{
				return 0;
			}
		}
	}

	return 1;
}

int32 clear_dst(uint8 *dst)
{
	if(!dst)
	{
		return 0;
	}

	for(int32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES; i++)
	{
		dst[i] = tile_none;
	}

	return 1;
}

int32 search_for_door_position(pos_t c, pos_t *door)
{
	for(int32 y = c.y - 1; y < c.y + 2; y++)
	{
		for(int32 x = c.x - 1; x < c.x + 2; x++)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone ||
				 level[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_floor_stone ||
				 level[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_floor_stone ||
				 level[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
				{
					door->x = x;
					door->y = y;
					return 1;
				}
			}
		}
	}

	return 0;
}

void add_walls_to_rect_in_dst(uint8 *dst, SDL_Rect r)
{
	for(int32 y = r.y; y < r.y + r.h; y++)
	{
		for(int32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
			{
				if(dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none)
				{
					dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				}

				if(dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none)
				{
					dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				}

				if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none)
				{
					dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
				}

				if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none)
				{
					dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
				}
			}
		}
	}
}

int32 can_room_be_placed(level_gen_buffers_t *buffers, SDL_Rect r)
{
	if(!is_rect_in_dst_unused(level, (SDL_Rect){r.x, r.y, r.w, r.h}))
	{
		return 0;
	}

	for(int32 y = 0; y < r.h; y++)
	{
		for(int32 x = 0; x < r.w; x++)
		{
			if(buffers->room_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
			{
				if((y != 0 || (x != 0 && x != r.w - 1)) &&
					 (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
					 (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
				{
					pos_t door = {0};
					if(search_for_door_position((pos_t){x + r.x, y + r.y}, &door))
					{
						level[(door.y * LEVEL_WIDTH_IN_TILES) + door.x] = tile_door_closed;
						copy_src_to_dst(buffers->room_buffer, level, (SDL_Rect){0, 0, r.w, r.h}, (pos_t){r.x, r.y});
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

void smoothing(level_gen_buffers_t *buffers, dimensions_t r)
{
	for(int32 y = 0; y < r.h; y++)
	{
		for(int32 x = 0; x < r.w; x++)
		{
			int32 count = count_alive_neighbours(buffers, (pos_t){x, y});
			if(buffers->temp_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					buffers->room_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
				else
				{
					buffers->room_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					buffers->room_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
				else
				{
					buffers->room_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
			}
		}
	}
}

int32 generate_room(level_gen_buffers_t *buffers, SDL_Rect r)
{
	clear_dst(buffers->room_buffer);
	clear_dst(buffers->temp_buffer);

	int32 room_type = num_between(type_rectangle, type_cellular_automata);
	if(room_type == type_rectangle)
	{	
		set_rect_to_dst(buffers->room_buffer, (SDL_Rect){0, 0, r.w, r.h}, tile_floor_stone);
	}
  else if (room_type == type_overlaid_rectangle)
  {

  }
	else if (room_type == type_cellular_automata)
	{
		for(int32 y = 0; y < r.h; y++)
		{
			for(int32 x = 0; x < r.w; x++)
			{
				if(num_between(1, 100) <= START_ALIVE_CHANCE)
				{
					buffers->temp_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
		}

		for(int32 i = 0; i < SMOOTHING_ITERATIONS; i++)
		{
			smoothing(buffers, (dimensions_t){r.w, r.h});
			copy_src_to_dst(buffers->room_buffer, buffers->temp_buffer, (SDL_Rect){0, 0, r.w, r.h}, (pos_t){0, 0});
		}
	}

	if(can_room_be_placed(buffers, (SDL_Rect){r.x, r.y, r.w, r.h}))
	{
		add_walls_to_rect_in_dst(level, (SDL_Rect){r.x, r.y, r.w, r.h});
		return 1;
	}

	return 0;
}

void generate_level()
{
  level_gen_buffers_t buffers = {0};
	rooms = calloc(1, ROOM_COUNT * sizeof(room_t));

	int32 x = LEVEL_WIDTH_IN_TILES / 2;
	int32 y = LEVEL_HEIGHT_IN_TILES / 2;
	int32 w = num_between(4, 10);
	int32 h = num_between(4, 10);

	set_rect_to_dst(level, (SDL_Rect){x, y, w, h}, tile_floor_stone);
	add_walls_to_rect_in_dst(level, (SDL_Rect){x, y, w, h});

	for(int i = 0; i < ROOM_COUNT; i++)
	{
		for(;;)
		{
			w = num_between(4, 10);
			h = num_between(4, 10);
			x = num_between(2, LEVEL_WIDTH_IN_TILES - w - 2);
			y = num_between(2, LEVEL_HEIGHT_IN_TILES - h - 2);

			if(generate_room(&buffers, (SDL_Rect){x, y, w, h}))
			{
				rooms[i] = (room_t){x, y, w, h};
				debug("Room %d complete\n", i);
				break;
			}
		}
	}

	int32 rand_room = num_between(0, ROOM_COUNT - 1);
	player->new_x = rooms[rand_room].x + (rooms[rand_room].w / 2);
	player->new_y = rooms[rand_room].y + (rooms[rand_room].h / 2);

	free(rooms);
}