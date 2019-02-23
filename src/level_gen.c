u8 level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

// NOTE(Rami): Do we need rooms to persist?
SDL_Rect *rooms;

i32 count_alive_neighbours(level_gen_buffers_t *buffers, pos_t p)
{
	i32 count = 0;

	for(i32 y = p.y - 1; y < p.y + 2; y++)
	{
		for(i32 x = p.x - 1; x < p.x + 2; x++)
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

i32 copy_src_to_dst(u8 *src, u8 *dst, SDL_Rect src_r, pos_t dst_c)
{
	if(!src || !dst)
	{
		return 0;
	}

	for(i32 y = 0; y < src_r.h; y++)
	{
		for(i32 x = 0; x < src_r.w; x++)
		{
			dst[((y + dst_c.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_c.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}

	return 1;
}

i32 set_rect_to_dst(u8 *dst, SDL_Rect r, i32 tile)
{
	if(!dst)
	{
		 return 0;
	}

	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			dst[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
		}
	}

	return 1;
}

i32 is_rect_in_dst_unused(u8 *dst, SDL_Rect r)
{
	if(!dst)
	{
		return 0;
	}

	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] != tile_none)
			{
				return 0;
			}
		}
	}

	return 1;
}

i32 clear_dst(u8 *dst)
{
	if(!dst)
	{
		return 0;
	}

	for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_WIDTH_IN_TILES; i++)
	{
		dst[i] = tile_none;
	}

	return 1;
}

i32 search_for_door_position(pos_t c, pos_t *door)
{
	for(i32 y = c.y - 1; y < c.y + 2; y++)
	{
		for(i32 x = c.x - 1; x < c.x + 2; x++)
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

void add_walls_to_rect_in_dst(u8 *dst, SDL_Rect r)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
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

i32 can_room_be_placed(level_gen_buffers_t *buffers, SDL_Rect r)
{
	if(!is_rect_in_dst_unused(level, (SDL_Rect){r.x, r.y, r.w, r.h}))
	{
		return 0;
	}

	for(i32 y = 0; y < r.h; y++)
	{
		for(i32 x = 0; x < r.w; x++)
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
	for(i32 y = 0; y < r.h; y++)
	{
		for(i32 x = 0; x < r.w; x++)
		{
			i32 count = count_alive_neighbours(buffers, (pos_t){x, y});
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

i32 gen_room(level_gen_buffers_t *buffers, SDL_Rect *complete_room)
{
	clear_dst(buffers->room_buffer);
	clear_dst(buffers->temp_buffer);

  SDL_Rect r = {0};

  i32 type_chance = rnum(0, 100);
	if(type_chance <= 25)
	{	
    r.w = rnum(3, 6);
    r.h = rnum(3, 6);
    r.x = rnum(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rnum(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

		set_rect_to_dst(buffers->room_buffer, (SDL_Rect){0, 0, r.w, r.h}, tile_floor_stone);
	}
  else if(type_chance <= 50)
  {
    i32 orientation = rnum(horizontal, vertical);
    if(orientation == horizontal)
    {
      r.w = rnum(8, 15);
      r.h = rnum(2, 3);
    }
    else if(orientation == vertical)
    {
      r.w = rnum(2, 3);
      r.h = rnum(8, 15);
    }

    r.x = rnum(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rnum(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    set_rect_to_dst(buffers->room_buffer, (SDL_Rect){0, 0, r.w, r.h}, tile_floor_stone);
  }
  else if (type_chance <= 100)
  {
    r.w = rnum(5, 10);
    r.h = rnum(5, 10);
    r.x = rnum(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rnum(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    for(i32 y = 0; y < r.h; y++)
    {
      for(i32 x = 0; x < r.w; x++)
      {
        if(rnum(1, 100) <= START_ALIVE_CHANCE)
        {
          buffers->temp_buffer[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
        }
      }
    }

    for(i32 i = 0; i < SMOOTHING_ITERATIONS; i++)
    {
      smoothing(buffers, (dimensions_t){r.w, r.h});
      copy_src_to_dst(buffers->room_buffer, buffers->temp_buffer, (SDL_Rect){0, 0, r.w, r.h}, (pos_t){0, 0});
    }
  }

	if(can_room_be_placed(buffers, r))
	{
    add_walls_to_rect_in_dst(level, r);
    *complete_room = r;
		return 1;
	}

	return 0;
}

// NOTE(Rami): Consider adding the length params to str_cmp, if either of the pointer
// are not null terminated we will segfault.
// Having length params would mean we have a max length we can iterate over and if any
// of the characters don't match during that iteration cycle then we can return false.
void gen_extra_corridors(i32 x, i32 y, char *dir)
{
  i32 x_scan_dir = 0;
  i32 y_scan_dir = 0;
  i32 scan_range = 10;

  if(str_cmp(dir, "up"))
  {
    x_scan_dir = 0;
    y_scan_dir = -1;
  }
  else if(str_cmp(dir, "left"))
  {
    x_scan_dir = -1;
    y_scan_dir = 0;
  }
  else if(str_cmp(dir, "down"))
  {
    x_scan_dir = 0;
    y_scan_dir = 1;
  }
  else if(str_cmp(dir, "right"))
  {
    x_scan_dir = 1;
    y_scan_dir = 0;
  }
}

void gen_level()
{
  level_gen_buffers_t buffers = {0};
	rooms = calloc(1, ROOM_COUNT * sizeof(SDL_Rect));

  SDL_Rect first_room = {LEVEL_WIDTH_IN_TILES / 2, LEVEL_HEIGHT_IN_TILES / 2, rnum(3, 6), rnum(4, 10)};
	set_rect_to_dst(level, first_room, tile_floor_stone);
	add_walls_to_rect_in_dst(level, first_room);

  player->new_x = first_room.x;
  player->new_y = first_room.y;

	for(int i = 0; i < ROOM_COUNT; i++)
	{
    SDL_Rect room = {0};

		for(;;)
		{
			if(gen_room(&buffers, &room))
			{
				debug("Room %d complete", i);
        rooms[i] = room;
				break;
			}
		}
	}

  gen_extra_corridors(5, 5, "up");

  // for(i32 i = 0; i < 1; i++)
  // {
    // i32 scan_range = 6;
    // i32 x = 26;
    // i32 y = 34;
    // b32 found_endpoint = false;

    // player->new_x = 26;
    // player->new_y = 34;

    // if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_wall_stone)
    // {
      // for(i32 scan = x; scan < x + scan_range; scan++)
      // {
        // level[(y * LEVEL_WIDTH_IN_TILES) + scan] = tile_floor_grass;
        // if(level[(y * LEVEL_WIDTH_IN_TILES) + scan] == tile_wall_stone)
        // {
        //   if(level[(y * LEVEL_WIDTH_IN_TILES) + (scan - 1)] == tile_floor_stone)
        //   {
        //     found_endpoint = true;
        //   }
        // }
      // }

      // if(found_endpoint)
      // {
      //   for(i32 scan = x; scan > x - scan_range; scan--)
      //   {
      //     level[(y * LEVEL_WIDTH_IN_TILES) + scan] = tile_floor_stone;
      //   }
      // }
    // }
  // }

	free(rooms);
}