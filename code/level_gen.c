internal u32
count_alive_neighbours(level_gen_buffers_t *buffers, v2u p)
{
	u32 count = 0;
    
	for(u32 y = p.y - 1; y < p.y + 2; ++y)
	{
		for(u32 x = p.x - 1; x < p.x + 2; ++x)
		{
            if(V2u_equal(V2u(x, y), p))
            {
                continue;
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
copy_src_to_dest(u32 *src, u32 *dest, v4u src_r, v2u dest_c)
{
	for(u32 y = 0; y < src_r.h; ++y)
	{
		for(u32 x = 0; x < src_r.w; ++x)
		{
			dest[((y + dest_c.y) * LEVEL_TILE_WIDTH) + (x + dest_c.x)] = src[((y + src_r.y) * LEVEL_TILE_WIDTH) + (x + src_r.x)];
		}
	}
}

internal void
set_rect_to_dest(u32 *dest, v4u r, u32 tile)
{
	for(u32 y = r.y; y < r.y + r.h; ++y)
	{
		for(u32 x = r.x; x < r.x + r.w; ++x)
		{
			dest[(y * LEVEL_TILE_WIDTH) + x] = tile;
		}
	}
}

internal u32
is_rect_in_dest_unused(u32 *dest, v4u r)
{
    u32 result = 1;
    
	for(u32 y = r.y; y < r.y + r.h; ++y)
	{
		for(u32 x = r.x; x < r.x + r.w; ++x)
		{
			if(dest[(y * LEVEL_TILE_WIDTH) + x] != tile_none)
			{
                result = 0;
            }
		}
	}
    
    return(result);
}

internal u32
find_door_position(v2u c, v2u *door)
{
    u32 result = 0;
    
	for(u32 y = c.y - 1; y < c.y + 2; ++y)
	{
		for(u32 x = c.x - 1; x < c.x + 2; ++x)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level.map[((y - 1) * LEVEL_TILE_WIDTH) + x] == tile_floor_stone ||
				   level.map[(y * LEVEL_TILE_WIDTH) + (x - 1)] == tile_floor_stone ||
				   level.map[(y * LEVEL_TILE_WIDTH) + (x + 1)] == tile_floor_stone ||
				   level.map[((y + 1) * LEVEL_TILE_WIDTH) + x] == tile_floor_stone)
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
add_walls_to_rect_in_dest(u32 *dest, v4u r)
{
	for(u32 y = r.y; y < r.y + r.h; ++y)
	{
		for(u32 x = r.x; x < r.x + r.w; ++x)
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

internal u32
can_room_be_placed(level_gen_buffers_t *buffers, v4u r)
{
    u32 result = 0;
    
    if(is_rect_in_dest_unused(level.map, r))
    {
        for(u32 y = 0; y < r.h; ++y)
        {
            for(u32 x = 0; x < r.w; ++x)
            {
                if(buffers->buff_one[(y * LEVEL_TILE_WIDTH) + x] == tile_floor_stone)
                {
                    if((y != 0 || (x != 0 && x != r.w - 1)) &&
                       (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
                       (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
                    {
                        v2u door = {0};
                        if(find_door_position(V2u(x + r.x, y + r.y), &door))
                        {
                            level.map[(door.y * LEVEL_TILE_WIDTH) + door.x] = tile_door_closed;
                            copy_src_to_dest(buffers->buff_one, level.map, V4u(0, 0, r.w, r.h), V2u(r.x, r.y));
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
smoothing(level_gen_buffers_t *buffers, v2u r)
{
	for(u32 y = 0; y < r.h; ++y)
	{
		for(u32 x = 0; x < r.w; ++x)
		{
			u32 count = count_alive_neighbours(buffers, V2u(x, y));
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
    v4u r = {0};
    
    u32 type_chance = rand_num(0, 100);
	if(type_chance <= 20)
	{	
        r.w = rand_num(4, 8);
        r.h = rand_num(4, 8);
        r.x = rand_num(2, LEVEL_TILE_WIDTH - r.w - 2);
        r.y = rand_num(2, LEVEL_TILE_HEIGHT - r.h - 2);
        
		set_rect_to_dest(buffers->buff_one, V4u(0, 0, r.w, r.h), tile_floor_stone);
	}
    else if(type_chance <= 40)
    {
        u32 orientation = rand_num(type_horizontal, type_vertical);
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
        
        set_rect_to_dest(buffers->buff_one, V4u(0, 0, r.w, r.h), tile_floor_stone);
    }
    else if(type_chance <= 100)
    {
        r.w = rand_num(4, 10);
        r.h = rand_num(4, 10);
        r.x = rand_num(2, LEVEL_TILE_WIDTH - r.w - 2);
        r.y = rand_num(2, LEVEL_TILE_HEIGHT - r.h - 2);
        
        for(u32 y = 0; y < r.h; ++y)
        {
            for(u32 x = 0; x < r.w; ++x)
            {
                if(rand_num(1, 100) <= START_ALIVE_CHANCE)
                {
                    buffers->buff_two[(y * LEVEL_TILE_WIDTH) + x] = ALIVE;
                }
            }
        }
        
        for(u32 i = 0; i < SMOOTHING_ITERATIONS; ++i)
        {
            smoothing(buffers, V2u(r.w, r.h));
            copy_src_to_dest(buffers->buff_one, buffers->buff_two, V4u(0, 0, r.w, r.h), V2u(0, 0));
        }
    }
    
	if(can_room_be_placed(buffers, r))
	{
        add_walls_to_rect_in_dest(level.map, r);
		result.success = 1;
        result.room = r;
	}
    
	return(result);
}

internal void
generate_level()
{
    memset(&level.fov_map, 0, sizeof(level.fov_map));
    memset(&level.map, 0, sizeof(level.map));
    memset(&level.rooms, 0, sizeof(level.rooms));
    level_gen_buffers_t *buffers = malloc(sizeof(level_gen_buffers_t));
    
    // TODO(rami): Debug
    /*for(u32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
    {
        u32 rand = rand_num(0, 2);
        if(rand == 0)
        {
            level.map[i] = tile_wall_stone;
        }
        else
        {
            level.map[i] = tile_floor_stone;
        }
    }
    
    free(buffers);
    
    return;*/
    
    v4u first_room = {0};
    first_room.w = rand_num(4, 8);
    first_room.h = rand_num(4, 8);
    first_room.x = rand_num(2, LEVEL_TILE_WIDTH - first_room.w - 2);
    first_room.y = rand_num(2, LEVEL_TILE_HEIGHT - first_room.w - 2);
    
    set_rect_to_dest(level.map, first_room, tile_floor_stone);
    add_walls_to_rect_in_dest(level.map, first_room);
    
    for(int i = 0; i < ROOM_COUNT; ++i)
	{
		for(;;)
		{
            room_data_t room_data = generate_room(buffers);
			if(room_data.success)
			{
				printf("Room %u complete\n", i);
                level.rooms[i] = room_data.room;
				break;
			}
		}
	}
    
    printf("\n");
    
    free(buffers);
    
    // Get rid of lone empty map
    for(u32 y = 1; y < LEVEL_TILE_HEIGHT - 1; ++y)
    {
        for(u32 x = 1; x < LEVEL_TILE_WIDTH - 1; ++x)
        {
            if(level.map[(y * LEVEL_TILE_WIDTH) + x] == tile_none &&
               level.map[((y - 1) * LEVEL_TILE_WIDTH) + x] != tile_none &&
               level.map[((y + 1) * LEVEL_TILE_WIDTH) + x] != tile_none &&
               level.map[(y * LEVEL_TILE_WIDTH) + (x - 1)] != tile_none &&
               level.map[(y * LEVEL_TILE_WIDTH) + (x + 1)] != tile_none)
            {
                level.map[(y * LEVEL_TILE_WIDTH) + x] = tile_wall_stone;
            }
        }
    }
    
    // Place start of level
    u32 start_room = 0;
    v2u up_path = {0};
    
    for(;;)
    {
        start_room = rand_num(0, ROOM_COUNT - 1);
        up_path.x = rand_num(level.rooms[start_room].x + 1, level.rooms[start_room].x + level.rooms[start_room].w - 2);
        up_path.y = rand_num(level.rooms[start_room].y + 1, level.rooms[start_room].y + level.rooms[start_room].h - 2);
        
        if(is_traversable(up_path))
        {
            level.map[(up_path.y * LEVEL_TILE_WIDTH) + up_path.x] = tile_path_up;
            break;
        }
    }
    
    player.pos = up_path;
    player.new_pos = player.pos;
    
    // Find the furthest room from the level start room
    u32 end_room = 0;
    u32 best_dist = 0;
    
    for(u32 i = 0; i < ROOM_COUNT; ++i)
    {
        u32 dist = tile_dist(V2u(level.rooms[start_room].x, level.rooms[start_room].y),
                             V2u(level.rooms[i].x, level.rooms[i].y));
        if(dist > best_dist)
        {
            end_room = i;
            best_dist = dist;
        }
    }
    
    for(;;)
    {
        // Place end of level
        u32 end_x = rand_num(level.rooms[end_room].x + 1,
                             level.rooms[end_room].x + level.rooms[end_room].w - 2);
        u32 end_y = rand_num(level.rooms[end_room].y + 1,
                             level.rooms[end_room].y + level.rooms[end_room].h - 2);
        v2u down_path = V2u(end_x, end_y);
        
        if(is_traversable(down_path))
        {
            level.map[(down_path.y * LEVEL_TILE_WIDTH) + down_path.x] = tile_path_down;
            break;
        }
        
        break;
    }
}