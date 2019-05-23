internal i32
count_alive_neighbours(level_gen_buffers_t *buffers, iv2_t p)
{
	i32 count = 0;

	for(i32 y = p.y - 1; y < p.y + 2; y++)
	{
		for(i32 x = p.x - 1; x < p.x + 2; x++)
		{
      if(iv2_equal((iv2_t){x, y}, p))
      {
      }
			else if(x >= 0 && y >= 0 && buffers->buff_two[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				count++;
			}
		}
	}

	return count;
}

internal b32
copy_src_to_dst(u8 *src, u8 *dst, SDL_Rect src_r, iv2_t dst_c)
{
	for(i32 y = 0; y < src_r.h; y++)
	{
		for(i32 x = 0; x < src_r.w; x++)
		{
			dst[((y + dst_c.y) * LEVEL_WIDTH_IN_TILES) + (x + dst_c.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}

	return true;
}

internal b32
set_rect_to_dst(u8 *dst, SDL_Rect r, i32 tile)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			dst[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
		}
	}

	return true;
}

internal b32
is_rect_in_dst_unused(u8 *dst, SDL_Rect r)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] != tile_none)
			{
				return false;
			}
		}
	}

	return true;
}

internal b32
find_door_position(iv2_t c, iv2_t *door)
{
	for(i32 y = c.y - 1; y < c.y + 2; y++)
	{
		for(i32 x = c.x - 1; x < c.x + 2; x++)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level.tiles[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone ||
				   level.tiles[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_floor_stone ||
				   level.tiles[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_floor_stone ||
				   level.tiles[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
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

internal void
add_walls_to_rect_in_dst(u8 *dst, SDL_Rect r)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			if(dst[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
			{
        if(dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
        if(dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
        if(dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
        if(dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
				if(dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none) dst[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				if(dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none) dst[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dst[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
				if(dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dst[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
			}
		}
	}
}

internal b32
can_room_be_placed(level_gen_buffers_t *buffers, SDL_Rect r)
{
	if(!is_rect_in_dst_unused(level.tiles, (SDL_Rect){r.x, r.y, r.w, r.h}))
	{
		return 0;
	}

	for(i32 y = 0; y < r.h; y++)
	{
		for(i32 x = 0; x < r.w; x++)
		{
			if(buffers->buff_one[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
			{
				if((y != 0 || (x != 0 && x != r.w - 1)) &&
					 (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
					 (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
				{
					iv2_t door;
					if(find_door_position((iv2_t){x + r.x, y + r.y}, &door))
					{
            level.tiles[(door.y * LEVEL_WIDTH_IN_TILES) + door.x] = tile_door_closed;
						copy_src_to_dst(buffers->buff_one, level.tiles, (SDL_Rect){0, 0, r.w, r.h}, (iv2_t){r.x, r.y});
						return true;
					}
				}
			}
		}
	}

	return false;
}

internal void
smoothing(level_gen_buffers_t *buffers, aspect_t r)
{
	for(i32 y = 0; y < r.h; y++)
	{
		for(i32 x = 0; x < r.w; x++)
		{
			i32 count = count_alive_neighbours(buffers, (iv2_t){x, y});
			if(buffers->buff_two[(y * LEVEL_WIDTH_IN_TILES) + x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					buffers->buff_one[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
				else
				{
					buffers->buff_one[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					buffers->buff_one[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
				}
				else
				{
					buffers->buff_one[(y * LEVEL_WIDTH_IN_TILES) + x] = DEAD;
				}
			}
		}
	}
}

internal b32
generate_room(level_gen_buffers_t *buffers, SDL_Rect *complete_room)
{
  memset(buffers, 0, sizeof(level_gen_buffers_t));

  SDL_Rect r;

  i32 type_chance = rand_num(0, 100);
	if(type_chance <= 20)
	{	
    r.w = rand_num(4, 8);
    r.h = rand_num(4, 8);
    r.x = rand_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rand_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

		set_rect_to_dst(buffers->buff_one, (SDL_Rect){0, 0, r.w, r.h}, tile_floor_stone);
	}
  else if(type_chance <= 40)
  {
    i32 orientation = rand_num(type_horizontal, type_vertical);
    if(orientation == type_horizontal)
    {
      r.w = rand_num(8, 15);
      r.h = rand_num(2, 3);
    }
    else if(orientation == type_vertical)
    {
      r.w = rand_num(2, 3);
      r.h = rand_num(8, 15);
    }

    r.x = rand_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rand_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    set_rect_to_dst(buffers->buff_one, (SDL_Rect){0, 0, r.w, r.h}, tile_floor_stone);
  }
  else if(type_chance <= 100)
  {
    r.w = rand_num(4, 10);
    r.h = rand_num(4, 10);
    r.x = rand_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = rand_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    for(i32 y = 0; y < r.h; y++)
    {
      for(i32 x = 0; x < r.w; x++)
      {
        if(rand_num(1, 100) <= START_ALIVE_CHANCE)
        {
          buffers->buff_two[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
        }
      }
    }

    for(i32 i = 0; i < SMOOTHING_ITERATIONS; i++)
    {
      smoothing(buffers, (aspect_t){r.w, r.h});
      copy_src_to_dst(buffers->buff_one, buffers->buff_two, (SDL_Rect){0, 0, r.w, r.h}, (iv2_t){0, 0});
    }
  }

	if(can_room_be_placed(buffers, r))
	{
    add_walls_to_rect_in_dst(level.tiles, r);
    *complete_room = r;
		return true;
	}

	return false;
}

internal void
generate_level()
{
  memset(&level, 0, sizeof(level_t));
  level_gen_buffers_t *buffers = malloc(sizeof(level_gen_buffers_t));

  // NOTE(Rami):
  // for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  // {
  //   level.tiles[i] = tile_floor_stone;
  // }

  // free(buffers);

  // return;

  SDL_Rect first_room;
  first_room.w = rand_num(4, 8);
  first_room.h = rand_num(4, 8);
  first_room.x = rand_num(2, LEVEL_WIDTH_IN_TILES - first_room.w - 2);
  first_room.y = rand_num(2, LEVEL_HEIGHT_IN_TILES - first_room.w - 2);

  set_rect_to_dst(level.tiles, first_room, tile_floor_stone);
  add_walls_to_rect_in_dst(level.tiles, first_room);

  for(int i = 0; i < ROOM_COUNT; i++)
	{
    SDL_Rect room;

		for(;;)
		{
			if(generate_room(buffers, &room))
			{
				debug("Room %d complete", i + 1);
        level.rooms[i] = room;
				break;
			}
		}
	}

  free(buffers);

  // Get rid of lone empty tiles
  for(i32 y = 0; y < LEVEL_HEIGHT_IN_TILES; y++)
  {
    for(i32 x = 0; x < LEVEL_WIDTH_IN_TILES; x++)
    {
      if(level.tiles[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_none &&
         level.tiles[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] != tile_none &&
         level.tiles[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] != tile_none &&
         level.tiles[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] != tile_none &&
         level.tiles[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] != tile_none)
      {
        level.tiles[(y * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
      }
    }
  }

  // NOTE(Rami): Make the start and end paths be in a random spot of the room,
  // not all rooms are squares so check if the spot you're putting it in is traversable,
  // if not, keep rolling another room.

  // Place start of level
  i32 start_room = 0;
  iv2_t up_path = {0};

  for(;;)
  {
    start_room = rand_num(0, ROOM_COUNT - 1);
    up_path.x = rand_num(level.rooms[start_room].x + 1, level.rooms[start_room].x + level.rooms[start_room].w - 2);
    up_path.y = rand_num(level.rooms[start_room].y + 1, level.rooms[start_room].y + level.rooms[start_room].h - 2);

    if(is_traversable(up_path))
    {
      level.tiles[(up_path.y * LEVEL_WIDTH_IN_TILES) + up_path.x] = tile_path_up;
      break;
    }
  }

  up_path.x++;
  player.entity.new_pos = up_path;

  // Find the furthest room from the level start room
  i32 end_room = 0;
  i32 best_dist = 0;

  for(i32 i = 0; i < ROOM_COUNT; i++)
  {
    i32 dist = tile_dist((iv2_t){level.rooms[start_room].x, level.rooms[start_room].y}, (iv2_t){level.rooms[i].x, level.rooms[i].y});
    if(dist > best_dist)
    {
      end_room = i;
      best_dist = dist;
    }
  }

  for(;;)
  {
    // Place end of level
    iv2_t down_path =
    {rand_num(level.rooms[end_room].x + 1, level.rooms[end_room].x + level.rooms[end_room].w - 2),
     rand_num(level.rooms[end_room].y + 1, level.rooms[end_room].y + level.rooms[end_room].h - 2)};

    if(is_traversable(down_path))
    {
      level.tiles[(down_path.y * LEVEL_WIDTH_IN_TILES) + down_path.x] = tile_path_down;
      break;
    }

    break;
  }
}