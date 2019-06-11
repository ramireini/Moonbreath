internal i32
count_alive_neighbours(level_gen_buffers_t *buffers, v2_t p)
{
	i32 count = 0;

	for(i32 y = p.y - 1; y < p.y + 2; y++)
	{
		for(i32 x = p.x - 1; x < p.x + 2; x++)
		{
      if(v2_equal(v2(x, y), p))
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

internal void
copy_src_to_dest(u32 *src, u32 *dest, v4_t src_r, v2_t dest_c)
{
	for(i32 y = 0; y < src_r.h; y++)
	{
		for(i32 x = 0; x < src_r.w; x++)
		{
			dest[((y + dest_c.y) * LEVEL_WIDTH_IN_TILES) + (x + dest_c.x)] = src[((y + src_r.y) * LEVEL_WIDTH_IN_TILES) + (x + src_r.x)];
		}
	}
}

internal void
set_rect_to_dest(u32 *dest, v4_t r, i32 tile)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			dest[(y * LEVEL_WIDTH_IN_TILES) + x] = tile;
		}
	}
}

internal i32
is_rect_in_dest_unused(u32 *dest, v4_t r)
{
  i32 result = 1;

	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			if(dest[(y * LEVEL_WIDTH_IN_TILES) + x] != tile_none)
			{
        result = 0;
      }
		}
	}

  return result;
}

internal i32
find_door_position(v2_t c, v2_t *door)
{
  i32 result = 0;

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
          result = 1;
          break;
				}
			}
		}
	}

  return result;
}

internal void
add_walls_to_rect_in_dest(u32 *dest, v4_t r)
{
	for(i32 y = r.y; y < r.y + r.h; y++)
	{
		for(i32 x = r.x; x < r.x + r.w; x++)
		{
			if(dest[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone)
			{
        if(dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
        if(dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
        if(dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
        if(dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
				if(dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none) dest[((y - 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				if(dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] == tile_none) dest[((y + 1) * LEVEL_WIDTH_IN_TILES) + x] = tile_wall_stone;
				if(dest[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] == tile_none) dest[(y * LEVEL_WIDTH_IN_TILES) + (x - 1)] = tile_wall_stone;
				if(dest[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] == tile_none) dest[(y * LEVEL_WIDTH_IN_TILES) + (x + 1)] = tile_wall_stone;
			}
		}
	}
}

internal i32
can_room_be_placed(level_gen_buffers_t *buffers, v4_t r)
{
  i32 result = 0;

  if(is_rect_in_dest_unused(level.tiles, v4(r.x, r.y, r.w, r.h)))
  {
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
            v2_t door = {0};
            if(find_door_position(v2(x + r.x, y + r.y), &door))
            {
              level.tiles[(door.y * LEVEL_WIDTH_IN_TILES) + door.x] = tile_door_closed;
              copy_src_to_dest(buffers->buff_one, level.tiles, v4(0, 0, r.w, r.h), v2(r.x, r.y));
              result = 1;
              goto end;
            }
          }
        }
      }
    }
  }

  end:
  return result;
}

internal void
smoothing(level_gen_buffers_t *buffers, v2_t r)
{
	for(i32 y = 0; y < r.h; y++)
	{
		for(i32 x = 0; x < r.w; x++)
		{
			i32 count = count_alive_neighbours(buffers, v2(x, y));
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

internal i32
generate_room(level_gen_buffers_t *buffers, v4_t *complete_room)
{
  i32 result = 0;

  memset(buffers, 0, sizeof(level_gen_buffers_t));
  v4_t r = {0};

  i32 type_chance = get_num(0, 100);
	if(type_chance <= 20)
	{	
    r.w = get_num(4, 8);
    r.h = get_num(4, 8);
    r.x = get_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = get_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

		set_rect_to_dest(buffers->buff_one, v4(0, 0, r.w, r.h), tile_floor_stone);
	}
  else if(type_chance <= 40)
  {
    i32 orientation = get_num(type_horizontal, type_vertical);
    if(orientation == type_horizontal)
    {
      r.w = get_num(8, 15);
      r.h = get_num(2, 3);
    }
    else if(orientation == type_vertical)
    {
      r.w = get_num(2, 3);
      r.h = get_num(8, 15);
    }

    r.x = get_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = get_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    set_rect_to_dest(buffers->buff_one, v4(0, 0, r.w, r.h), tile_floor_stone);
  }
  else if(type_chance <= 100)
  {
    r.w = get_num(4, 10);
    r.h = get_num(4, 10);
    r.x = get_num(2, LEVEL_WIDTH_IN_TILES - r.w - 2);
    r.y = get_num(2, LEVEL_HEIGHT_IN_TILES - r.h - 2);

    for(i32 y = 0; y < r.h; y++)
    {
      for(i32 x = 0; x < r.w; x++)
      {
        if(get_num(1, 100) <= START_ALIVE_CHANCE)
        {
          buffers->buff_two[(y * LEVEL_WIDTH_IN_TILES) + x] = ALIVE;
        }
      }
    }

    for(i32 i = 0; i < SMOOTHING_ITERATIONS; i++)
    {
      smoothing(buffers, v2(r.w, r.h));
      copy_src_to_dest(buffers->buff_one, buffers->buff_two, v4(0, 0, r.w, r.h), v2(0, 0));
    }
  }

	if(can_room_be_placed(buffers, r))
	{
    add_walls_to_rect_in_dest(level.tiles, r);
    *complete_room = r;
		result = 1;
	}

	return result;
}

internal void
generate_level()
{
  memset(&level, 0, sizeof(level_t));
  level_gen_buffers_t *buffers = malloc(sizeof(level_gen_buffers_t));

  // NOTE(rami):
  // for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  // {
  //   level.tiles[i] = tile_floor_stone;
  // }

  // free(buffers);

  // return;

  v4_t first_room = {0};
  first_room.w = get_num(4, 8);
  first_room.h = get_num(4, 8);
  first_room.x = get_num(2, LEVEL_WIDTH_IN_TILES - first_room.w - 2);
  first_room.y = get_num(2, LEVEL_HEIGHT_IN_TILES - first_room.w - 2);

  set_rect_to_dest(level.tiles, first_room, tile_floor_stone);
  add_walls_to_rect_in_dest(level.tiles, first_room);

  for(int i = 0; i < ROOM_COUNT; i++)
	{
    v4_t room = {0};

		for(;;)
		{
			if(generate_room(buffers, &room))
			{
				debug("Room %d complete\n", i + 1);
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

  // Place start of level
  i32 start_room = 0;
  v2_t up_path = {0};

  for(;;)
  {
    start_room = get_num(0, ROOM_COUNT - 1);
    up_path.x = get_num(level.rooms[start_room].x + 1, level.rooms[start_room].x + level.rooms[start_room].w - 2);
    up_path.y = get_num(level.rooms[start_room].y + 1, level.rooms[start_room].y + level.rooms[start_room].h - 2);

    if(traversable(up_path))
    {
      level.tiles[(up_path.y * LEVEL_WIDTH_IN_TILES) + up_path.x] = tile_path_up;
      break;
    }
  }

  up_path.x++;
  player.x = up_path.x;
  player.y = up_path.y;
  player.new_x = up_path.x;
  player.new_y = up_path.y;

  // Find the furthest room from the level start room
  i32 end_room = 0;
  i32 best_dist = 0;

  for(i32 i = 0; i < ROOM_COUNT; i++)
  {
    i32 dist = tile_dist(v2(level.rooms[start_room].x, level.rooms[start_room].y), v2(level.rooms[i].x, level.rooms[i].y));
    if(dist > best_dist)
    {
      end_room = i;
      best_dist = dist;
    }
  }

  for(;;)
  {
    // Place end of level
    i32 end_x = get_num(level.rooms[end_room].x + 1,
                        level.rooms[end_room].x + level.rooms[end_room].w - 2);
    i32 end_y = get_num(level.rooms[end_room].y + 1,
                        level.rooms[end_room].y + level.rooms[end_room].h - 2);
    v2_t down_path = v2(end_x, end_y);

    if(traversable(down_path))
    {
      level.tiles[(down_path.y * LEVEL_WIDTH_IN_TILES) + down_path.x] = tile_path_down;
      break;
    }

    break;
  }
}