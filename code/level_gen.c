internal v2u
get_open_level_pos()
{
    v2u result = {0};
    
    for(;;)
    {
        b32 pos_is_vacant = true;
        v2u pos = V2u(rand_num(0, level.width), rand_num(0, level.height));
        if(is_traversable(pos))
        {
            // TODO(rami): We're going to have to think about
            // whether we want all tiles to have an occupied flag
            // or do we check all of the monsters/npcs etc. to be
            // iterated over everytime something like this is done.
            
            for(u32 i = 0; i < MONSTER_COUNT; ++i)
            {
                if(monster[i].type)
                {
                    if(V2u_equal(pos, monster[i].pos))
                    {
                        pos_is_vacant = false;
                        break;
                    }
                }
            }
            
            if(pos_is_vacant)
            {
                result = pos;
                break;
            }
        }
    }
    
    return(result);
}

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
			else if(x >= 0 && y >= 0 && buffers->buff_two[y][x] == ALIVE)
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
			dest[((y + dest_c.y) * level.width) + (x + dest_c.x)] = src[((y + src_r.y) * level.width) + (x + src_r.x)];
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
			dest[(y * level.width) + x] = tile;
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
			if(dest[(y * level.width) + x] != tile_none)
			{
                result = 0;
            }
		}
	}
    
    return(result);
}

internal b32
find_door_pos(v2u c, v2u *door)
{
    u32 found_door = false;
    
	for(u32 y = c.y - 1; y < c.y + 2; ++y)
	{
		for(u32 x = c.x - 1; x < c.x + 2; ++x)
		{
			if((y == c.y || x == c.x) && (y != c.y || x != c.x))
			{
				if(level.tiles[y - 1][x] == tile_floor_stone ||
				   level.tiles[y][x - 1] == tile_floor_stone ||
				   level.tiles[y][x + 1] == tile_floor_stone ||
				   level.tiles[y + 1][x] == tile_floor_stone)
				{
                    *door = V2u(x, y);
                    found_door = true;
                    goto end;
				}
			}
		}
	}
    
    end:
    return(found_door);
}

internal void
add_walls_to_rect_in_dest(u32 *dest, v4u r)
{
	for(u32 y = r.y; y < r.y + r.h; ++y)
	{
		for(u32 x = r.x; x < r.x + r.w; ++x)
		{
			if(dest[(y * level.width) + x] == tile_floor_stone)
			{
                if(dest[((y - 1) * level.width) + (x - 1)] == tile_none) dest[((y - 1) * level.width) + (x - 1)] = tile_wall_stone;
                if(dest[((y - 1) * level.width) + (x + 1)] == tile_none) dest[((y - 1) * level.width) + (x + 1)] = tile_wall_stone;
                if(dest[((y + 1) * level.width) + (x - 1)] == tile_none) dest[((y + 1) * level.width) + (x - 1)] = tile_wall_stone;
                if(dest[((y + 1) * level.width) + (x + 1)] == tile_none) dest[((y + 1) * level.width) + (x + 1)] = tile_wall_stone;
				if(dest[((y - 1) * level.width) + x] == tile_none) dest[((y - 1) * level.width) + x] = tile_wall_stone;
				if(dest[((y + 1) * level.width) + x] == tile_none) dest[((y + 1) * level.width) + x] = tile_wall_stone;
				if(dest[(y * level.width) + (x - 1)] == tile_none) dest[(y * level.width) + (x - 1)] = tile_wall_stone;
				if(dest[(y * level.width) + (x + 1)] == tile_none) dest[(y * level.width) + (x + 1)] = tile_wall_stone;
			}
		}
	}
}

internal b32
can_room_be_placed(level_gen_buffers_t *buffers, v4u r)
{
    b32 result = false;
    
    if(is_rect_in_dest_unused((u32 *)level.tiles, r))
    {
        for(u32 y = 0; y < r.h; ++y)
        {
            for(u32 x = 0; x < r.w; ++x)
            {
                if(buffers->buff_one[y][x] == tile_floor_stone)
                {
                    if((y != 0 || (x != 0 && x != r.w - 1)) &&
                       (y != r.h - 1 || (x != 0 && x != r.w - 1)) &&
                       (y == 0 || y == r.h - 1 || x == 0 || x == r.w - 1))
                    {
                        v2u door = {0};
                        if(find_door_pos(V2u(x + r.x, y + r.y), &door))
                        {
                            level.tiles[door.y][door.x] = tile_door_closed;
                            copy_src_to_dest((u32 *)buffers->buff_one, (u32 *)level.tiles, V4u(0, 0, r.w, r.h), V2u(r.x, r.y));
                            result = true;
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
			if(buffers->buff_two[y][x] == ALIVE)
			{
				if(count < DEATH_LIMIT)
				{
					buffers->buff_one[y][x] = DEAD;
				}
				else
				{
					buffers->buff_one[y][x] = ALIVE;
				}
			}
			else
			{
				if(count > BIRTH_LIMIT)
				{
					buffers->buff_one[y][x] = ALIVE;
				}
				else
				{
					buffers->buff_one[y][x] = DEAD;
				}
			}
		}
	}
}

internal room_t
generate_room(level_gen_buffers_t *buffers)
{
    room_t result = {0};
    
    memset(buffers, 0, sizeof(level_gen_buffers_t));
    v4u r = {0};
    
    u32 type_chance = rand_num(0, 100);
	if(type_chance <= 20)
	{	
        r.w = rand_num(4, 8);
        r.h = rand_num(4, 8);
        r.x = rand_num(2, level.width - r.w - 2);
        r.y = rand_num(2, level.height - r.h - 2);
        
		set_rect_to_dest((u32 *)buffers->buff_one, V4u(0, 0, r.w, r.h), tile_floor_stone);
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
        
        r.x = rand_num(2, level.width - r.w - 2);
        r.y = rand_num(2, level.height - r.h - 2);
        
        set_rect_to_dest((u32 *)buffers->buff_one, V4u(0, 0, r.w, r.h), tile_floor_stone);
    }
    else if(type_chance <= 100)
    {
        r.w = rand_num(4, 10);
        r.h = rand_num(4, 10);
        r.x = rand_num(2, level.width - r.w - 2);
        r.y = rand_num(2, level.height - r.h - 2);
        
        for(u32 y = 0; y < r.h; ++y)
        {
            for(u32 x = 0; x < r.w; ++x)
            {
                if(rand_num(1, 100) <= START_ALIVE_CHANCE)
                {
                    buffers->buff_two[y][x] = ALIVE;
                }
            }
        }
        
        for(u32 i = 0; i < SMOOTHING_ITERATIONS; ++i)
        {
            smoothing(buffers, V2u(r.w, r.h));
            copy_src_to_dest((u32 *)buffers->buff_one, (u32 *)buffers->buff_two, V4u(0, 0, r.w, r.h), V2u(0, 0));
        }
    }
    
	if(can_room_be_placed(buffers, r))
	{
        add_walls_to_rect_in_dest((u32 *)level.tiles, r);
		result.valid = true;
        result.x = r.x;
        result.y = r.y;
        result.w = r.w;
        result.h = r.h;
	}
    
	return(result);
}

internal void
generate_level()
{
    memset(&level.fov_tiles, 0, sizeof(level.fov_tiles));
    memset(&level.tiles, 0, sizeof(level.tiles));
    memset(&level.rooms, 0, sizeof(level.rooms));
    level_gen_buffers_t *buffers = malloc(sizeof(level_gen_buffers_t));
    
    // TODO(rami): Debug
    /*for(u32 i = 0; i < level.width * level.height; ++i)
    {
        u32 rand = rand_num(0, 2);
        if(rand == 0)
        {
            level.tiles[i] = tile_wall_stone;
        }
        else
        {
            level.tiles[i] = tile_floor_stone;
        }
    }
    
    free(buffers);
    
    return;*/
    
    v4u first_room = {0};
    first_room.w = rand_num(4, 8);
    first_room.h = rand_num(4, 8);
    first_room.x = rand_num(2, level.width - first_room.w - 2);
    first_room.y = rand_num(2, level.height - first_room.w - 2);
    
    set_rect_to_dest((u32 *)level.tiles, first_room, tile_floor_stone);
    add_walls_to_rect_in_dest((u32 *)level.tiles, first_room);
    
    for(int i = 0; i < ROOM_COUNT; ++i)
	{
		for(;;)
		{
            room_t room_data = generate_room(buffers);
			if(room_data.valid)
			{
				printf("Room %u complete\n", i);
                level.rooms[i] = room_data;
				break;
			}
		}
	}
    
    printf("\n");
    
    printf("DEBUG 1\n");
    
    free(buffers);
    
    // Get rid of lone empty map
    for(u32 y = 1; y < level.height - 1; ++y)
    {
        for(u32 x = 1; x < level.width - 1; ++x)
        {
            if(level.tiles[(y * level.width) + x] == tile_none &&
               level.tiles[((y - 1) * level.width) + x] != tile_none &&
               level.tiles[((y + 1) * level.width) + x] != tile_none &&
               level.tiles[(y * level.width) + (x - 1)] != tile_none &&
               level.tiles[(y * level.width) + (x + 1)] != tile_none)
            {
                level.tiles[y][x] = tile_wall_stone;
            }
        }
    }
    
    printf("DEBUG 2\n");
    
    // Place start of level
    u32 start_room = 0;
    v2u up_path = {0};
    
    for(;;)
    {
        start_room = rand_num(0, ROOM_COUNT - 1);
        // TODO(rami): !!!
        //up_path.x = rand_num(level.rooms[start_room].x + 1, level.rooms[start_room].x + level.rooms[start_room].w - 2);
        //up_path.y = rand_num(level.rooms[start_room].y + 1, level.rooms[start_room].y + level.rooms[start_room].h - 2);
        
        if(level.rooms[start_room].valid)
        {
            up_path.x = (level.rooms[start_room].x + level.rooms[start_room].w) / 2;
            up_path.y = (level.rooms[start_room].y + level.rooms[start_room].h) / 2;
            
            if(is_traversable(up_path))
            {
                level.tiles[up_path.y][up_path.x] = tile_path_up;
                break;
            }
        }
    }
    
    printf("DEBUG 3\n");
    
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
    
    printf("DEBUG 4\n");
    
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
            level.tiles[down_path.y][down_path.x] = tile_path_down;
            break;
        }
    }
    
    printf("DEBUG 5\n");
    
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    // Add monsters
    for(u32 i = 0; i < 0; ++i)
    {
        monster_type type = get_monster_for_level();
        
        // TODO(rami): Logging
        if(type == monster_slime)
        {
            ++slime_count;
        }
        else if(type == monster_skeleton)
        {
            ++skeleton_count;
        }
        
        v2u pos = get_open_level_pos();
        add_monster(type, pos);
    }
    
    printf("DEBUG 6\n");
    
    printf("slimes: %u\n", slime_count);
    printf("skeletons: %u\n", skeleton_count);
}