internal i32
count_alive_neighbours(level_gen_buffers_t *buffers, iv2 p)
{
	i32 count = 0;
    
	for(i32 y = p.y - 1; y < p.y + 2; ++y)
	{
		for(i32 x = p.x - 1; x < p.x + 2; ++x)
		{
            if(v2_equal(v2(x, y), p))
            {
            }
			else if(x >= 0 && y >= 0 && buffers->buff_two[(y * LEVEL_TILE_WIDTH) + x] == ALIVE)
			{
				++count;
			}
		}
	}
    
	return(count);
}

internal void
copy_src_to_dest(u32 *src, u32 *dest, iv4 src_r, iv2 dest_c)
{
	for(i32 y = 0; y < src_r.h; ++y)
	{
		for(i32 x = 0; x < src_r.w; ++x)
		{
			dest[((y + dest_c.y) * LEVEL_TILE_WIDTH) + (x + dest_c.x)] = src[((y + src_r.y) * LEVEL_TILE_WIDTH) + (x + src_r.x)];
		}
	}
}

internal void
set_rect_to_dest(u32 *dest, iv4 r, i32 tile)
{
	for(i32 y = r.y; y < r.y + r.h; ++y)
	{
		for(i32 x = r.x; x < r.x + r.w; ++x)
		{
			dest[(y * LEVEL_TILE_WIDTH) + x] = tile;
		}
	}
}

internal i32
is_rect_in_dest_unused(u32 *dest, iv4 r)
{
    i32 result = 1;
    
	for(i32 y = r.y; y < r.y + r.h; ++y)
	{
		for(i32 x = r.x; x < r.x + r.w; ++x)
		{
			if(dest[(y * LEVEL_TILE_WIDTH) + x] != tile_none)
			{
                result = 0;
            }
		}
	}
    
    return(result);
}

internal i32
find_door_position(iv2 c, iv2 *door)
{
    i32 result = 0;
    
	for(i32 y = c.y - 1; y < c.y + 2; ++y)
	{
		for(i32 x = c.x - 1; x < c.x + 2; ++x)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level.tiles[((y - 1) * LEVEL_TILE_WIDTH) + x] == tile_floor_stone ||
				   level.tiles[(y * LEVEL_TILE_WIDTH) + (x - 1)] == tile_floor_stone ||
				   level.tiles[(y * LEVEL_TILE_WIDTH) + (x + 1)] == tile_floor_stone ||
				   level.tiles[((y + 1) * LEVEL_TILE_WIDTH) + x] == tile_floor_stone)
				{
					door->x = x;
					door->y = y;
                    result = 1;
                    break;
				}
			}
		}
	}
    
    return(result);
}

internal void
add_walls_to_rect_in_dest(u32 *dest, iv4 r)
{
	for(i32 y = r.y; y < r.y + r.h; ++y)
	{
		for(i32 x = r.x; x < r.x + r.w; ++x)
		{
			if(dest[(y * LEVEL_TILE_WIDTH) + x] == tile_floor_stone)
			{
                if(dest[((y - 1) * LEVEL_TILE_WIDTH) + (x - 1)] == tile_none) dest[((y - 1) * LEVEL_TILE_WIDTH) + (x - 1)] = tile_wall_stone;
                if(dest[((y - 1) * LEVEL_TILE_WIDTH) + (x + 1)] == tile_none) dest[((y - 1) * LEVEL_TILE_WIDTH) + (x + 1)] = tile_wall_stone;
                if(dest[((y + 1) * LEVEL_TILE_WIDTH) + (x - 1)] == tile_none) dest[((y + 1) * LEVEL_TILE_WIDTH) + (x - 1)] = tile_wall_stone;
                if(dest[((y + 1) * LEVEL_TILE_WIDTH) + (x + 1)] == tile_none) dest[((y + 1) * LEVEL_TILE_WIDTH) + (x + 1)] = tile_wall_stone;
				if(dest[((y - 1) * LEVEL_TILE_WIDTH) + x] == tile_none) dest[((y - 1) * LEVEL_TILE_WIDTH) + x] = tile_wall_stone;
				if(dest[((y + 1) * LEVEL_TILE_WIDTH) + x] == tile_none) dest[((y + 1) * LEVEL_TILE_WIDTH) + x] = tile_wall_stone;
				if(dest[(y * LEVEL_TILE_WIDTH) + (x - 1)] == tile_none) dest[(y * LEVEL_TILE_WIDTH) + (x - 1)] = tile_wall_stone;
				if(dest[(y * LEVEL_TILE_WIDTH) + (x + 1)] == tile_none) dest[(y * LEVEL_TILE_WIDTH) + (x + 1)] = tile_wall_stone;
			}
		}
	}
}

internal i32
can_room_be_placed(level_gen_buffers_t *buffers, iv4 r)
{
    i32 result = 0;
    
    if(is_rect_in_dest_unused(level.tiles, v4(r.x, r.y, r.w, r.h)))
    {
        for(i32 y = 0; y < r.h; ++y)
        {
            for(i32 x = 0; x < r.w; ++x)
            {
                if(buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] == tile_floor_stone)
                {
                    if((y != 0 || (x != 0 && x != r.w - 1)) &&
                       (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
                       (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
                    {
                        iv2 door = {0};
                        if(find_door_position(v2(x + r.x, y + r.y), &door))
                        {
                            level.tiles[(door.y * LEVEL_TILE_WIDTH) + door.x] = tile_door_closed;
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
    return(result);
}

internal void
smoothing(level_gen_buffers_t *buffers, iv2 r)
{
	for(i32 y = 0; y < r.h; ++y)
	{
		for(i32 x = 0; x < r.w; ++x)
		{
			i32 count = count_alive_neighbours(buffers, v2(x, y));
			if(buffers->buff_two[(y * LEVEL_TILE_WIDTH) + x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] = DEAD;
				}
				else
				{
					buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] = ALIVE;
				}
				else
				{
					buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] = DEAD;
				}
			}
		}
	}
}

internal room_data_t
generate_room(level_gen_buffers_t *buffers)
{
    room_data_t result = {0};
    
    memset(buffers, 0, sizeof(level_gen_buffers_t));
    iv4 r = {0};
    
    i32 type_chance = rand_num(0, 100);
	if(type_chance <= 20)
	{	
        r.w = rand_num(4, 8);
        r.h = rand_num(4, 8);
        r.x = rand_num(2, LEVEL_TILE_WIDTH - r.w - 2);
        r.y = rand_num(2, LEVEL_TILE_HEIGHT - r.h - 2);
        
		set_rect_to_dest(buffers->buff_one, v4(0, 0, r.w, r.h), tile_floor_stone);
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
        
        r.x = rand_num(2, LEVEL_TILE_WIDTH - r.w - 2);
        r.y = rand_num(2, LEVEL_TILE_HEIGHT - r.h - 2);
        
        set_rect_to_dest(buffers->buff_one, v4(0, 0, r.w, r.h), tile_floor_stone);
    }
    else if(type_chance <= 100)
    {
        r.w = rand_num(4, 10);
        r.h = rand_num(4, 10);
        r.x = rand_num(2, LEVEL_TILE_WIDTH - r.w - 2);
        r.y = rand_num(2, LEVEL_TILE_HEIGHT - r.h - 2);
        
        for(i32 y = 0; y < r.h; ++y)
        {
            for(i32 x = 0; x < r.w; ++x)
            {
                if(rand_num(1, 100) <= START_ALIVE_CHANCE)
                {
                    buffers->buff_two[(y * LEVEL_TILE_WIDTH) + x] = ALIVE;
                }
            }
        }
        
        for(i32 i = 0; i < SMOOTHING_ITERATIONS; ++i)
        {
            smoothing(buffers, v2(r.w, r.h));
            copy_src_to_dest(buffers->buff_one, buffers->buff_two, v4(0, 0, r.w, r.h), v2(0, 0));
        }
    }
    
	if(can_room_be_placed(buffers, r))
	{
        add_walls_to_rect_in_dest(level.tiles, r);
		result.success = 1;
        result.room = r;
	}
    
	return(result);
}

internal void
generate_level()
{
    memset(&level, 0, sizeof(level_t));
    level_gen_buffers_t *buffers = malloc(sizeof(level_gen_buffers_t));
    
    // NOTE(rami):
    // for(i32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
    // {
    //   level.tiles[i] = tile_floor_stone;
    // }
    
    // free(buffers);
    
    // return;
    
    iv4 first_room = {0};
    first_room.w = rand_num(4, 8);
    first_room.h = rand_num(4, 8);
    first_room.x = rand_num(2, LEVEL_TILE_WIDTH - first_room.w - 2);
    first_room.y = rand_num(2, LEVEL_TILE_HEIGHT - first_room.w - 2);
    
    set_rect_to_dest(level.tiles, first_room, tile_floor_stone);
    add_walls_to_rect_in_dest(level.tiles, first_room);
    
    for(int i = 0; i < ROOM_COUNT; ++i)
	{
		for(;;)
		{
            room_data_t room_data = generate_room(buffers);
			if(room_data.success)
			{
				printf("Room %d complete\n", i);
                level.rooms[i] = room_data.room;
				break;
			}
		}
	}
    
    printf("\n");
    
    free(buffers);
    
    // Get rid of lone empty tiles
    for(i32 y = 0; y < LEVEL_TILE_HEIGHT; ++y)
    {
        for(i32 x = 0; x < LEVEL_TILE_WIDTH; ++x)
        {
            if(level.tiles[(y * LEVEL_TILE_WIDTH) + x] == tile_none &&
               level.tiles[((y - 1) * LEVEL_TILE_WIDTH) + x] != tile_none &&
               level.tiles[((y + 1) * LEVEL_TILE_WIDTH) + x] != tile_none &&
               level.tiles[(y * LEVEL_TILE_WIDTH) + (x - 1)] != tile_none &&
               level.tiles[(y * LEVEL_TILE_WIDTH) + (x + 1)] != tile_none)
            {
                level.tiles[(y * LEVEL_TILE_WIDTH) + x] = tile_wall_stone;
            }
        }
    }
    
    // Place start of level
    i32 start_room = 0;
    iv2 up_path = {0};
    
    for(;;)
    {
        start_room = rand_num(0, ROOM_COUNT - 1);
        up_path.x = rand_num(level.rooms[start_room].x + 1, level.rooms[start_room].x + level.rooms[start_room].w - 2);
        up_path.y = rand_num(level.rooms[start_room].y + 1, level.rooms[start_room].y + level.rooms[start_room].h - 2);
        
        if(is_traversable(up_path))
        {
            level.tiles[(up_path.y * LEVEL_TILE_WIDTH) + up_path.x] = tile_path_up;
            break;
        }
    }
    
    player.x = up_path.x;
    player.y = up_path.y;
    player.new_x = up_path.x;
    player.new_y = up_path.y;
    
    // Find the furthest room from the level start room
    i32 end_room = 0;
    i32 best_dist = 0;
    
    for(i32 i = 0; i < ROOM_COUNT; ++i)
    {
        i32 dist = tile_dist(v2(level.rooms[start_room].x, level.rooms[start_room].y),
                             v2(level.rooms[i].x, level.rooms[i].y));
        if(dist > best_dist)
        {
            end_room = i;
            best_dist = dist;
        }
    }
    
    for(;;)
    {
        // Place end of level
        i32 end_x = rand_num(level.rooms[end_room].x + 1,
                             level.rooms[end_room].x + level.rooms[end_room].w - 2);
        i32 end_y = rand_num(level.rooms[end_room].y + 1,
                             level.rooms[end_room].y + level.rooms[end_room].h - 2);
        iv2 down_path = v2(end_x, end_y);
        
        if(is_traversable(down_path))
        {
            level.tiles[(down_path.y * LEVEL_TILE_WIDTH) + down_path.x] = tile_path_down;
            break;
        }
        
        break;
    }
}