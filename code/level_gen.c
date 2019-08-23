// TODO(rami): Debug
#if 0
printf("\nRandomised Base\n");
for(u32 y = room.y; y < room.y + room.h; ++y)
{
    for(u32 x = room.x; x < room.x + room.w; ++x)
    {
        if(level.tiles[y][x] == tile_none)
        {
            printf("-");
        }
        else if(level.tiles[y][x] == tile_wall_stone)
        {
            printf("#");
        }
        else if(level.tiles[y][x] == tile_floor_stone)
        {
            printf(".");
        }
    }
    
    printf("\n");
}

printf("\nFirst Pass After:\n");
print_array_state(&first_pass_data);
printf("\nSecond Pass After:\n");
print_array_state(&second_pass_data);
printf("\nThird Pass After:\n");
print_array_state(&third_pass_data);
printf("\nFourth Pass After:\n");
print_array_state(&fourth_pass_data);
printf("\nFifth Pass After:\n");
print_array_state(&fifth_pass_data);

internal void
print_array_state(automata_t *src)
{
    for(u32 y = 0; y < src->pitch; ++y)
    {
        for(u32 x = 0; x < src->pitch; ++x)
        {
            if(src->ptr[(y * src->pitch) + x] == tile_none)
            {
                printf("-");
            }
            else if(src->ptr[(y * src->pitch) + x] == tile_wall_stone)
            {
                printf("#");
            }
            else if(src->ptr[(y * src->pitch) + x] == tile_floor_stone)
            {
                printf(".");
            }
        }
        
        printf("\n");
    }
}
#endif

internal void
render_result(automata_t *src, automata_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = V2u(room.x + x, room.y + y);
            dest->ptr[(tile_pos.y * dest->width) + tile_pos.x] = src->ptr[(y * src->width) + x];
        }
    }
}

internal u32
get_neighbour_wall_count(automata_t *src, v2u pos, v4u room)
{
    u32 count = 0;
    
    for(i32 y = pos.y - 1; y < (i32)pos.y + 2; ++y)
    {
        for(i32 x = pos.x - 1; x < (i32)pos.x + 2; ++x)
        {
            if(x < room.x || y < room.y || x >= room.x + room.w || y >= room.y + room.h)
            {
                ++count;
            }
            else if(x != pos.x || y != pos.y)
            {
                if(src->ptr[(y * src->width) + x] == tile_wall_stone)
                {
                    ++count;
                }
            }
        }
    }
    
    return(count);
}

internal void
scan_room(automata_t *src, automata_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = V2u(room.x + x, room.y + y);
            u32 wall_count = get_neighbour_wall_count(src, tile_pos, room);
            
            if(src->ptr[(tile_pos.y * src->width) + tile_pos.x] == tile_floor_stone)
            {
                if(wall_count >= 5)
                {
                    dest->ptr[(y * dest->width) + x] = tile_wall_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x] = tile_floor_stone;
                }
            }
            else if(src->ptr[(tile_pos.y * src->width) + tile_pos.x] == tile_wall_stone)
            {
                if(wall_count <= 4)
                {
                    dest->ptr[(y * dest->width) + x] = tile_floor_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x] = tile_wall_stone;
                }
            }
        }
    }
}

internal v2u
get_open_rect_pos(v4u room)
{
    v2u pos = {0};
    pos.x = rand_num(room.x, room.x + room.w);
    pos.y = rand_num(room.y, room.y + room.h);
    return(pos);
}

internal v2u
get_open_level_pos()
{
    v2u result = {0};
    
    for(;;)
    {
        b32 pos_is_vacant = true;
        v2u pos = V2u(rand_num(0, level.w), rand_num(0, level.h));
        if(is_traversable(pos))
        {
            // TODO(rami): We're going to have to think about
            // whether we want all tiles to have an occupied flag
            // or do we check all of the monsters/npcs etc. to be
            // iterated over everytime something like this is done.
            
            for(u32 i = 0; i < MONSTER_COUNT; ++i)
            {
                if(monsters[i].type)
                {
                    if(V2u_equal(pos, monsters[i].pos))
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

internal b32
is_rect_free(v4u room)
{
    // NOTE(rami): Offset by one so there will be at least
    // one tile of space between rooms.
    for(u32 y = room.y - 1; y < room.y + room.h + 1; ++y)
    {
        for(u32 x = room.x - 1; x < room.x + room.w + 1; ++x)
        {
            if(level.tiles[y][x] != tile_wall_stone)
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal b32
place_room(v4u room, room_type type)
{
    b32 result = false;
    
    if(is_rect_free(room))
    {
        if(type == room_rectangle)
        {
            for(u32 y = room.y; y < room.y + room.h; ++y)
            {
                for(u32 x = room.x; x < room.x + room.w; ++x)
                {
                    level.tiles[y][x] = tile_floor_stone;
                }
            }
            
            result = true;
        }
        else if(type == room_automata)
        {
            for(u32 y = room.y; y < room.y + room.h; ++y)
            {
                for(u32 x = room.x; x < room.x + room.w; ++x)
                {
                    u32 rand = rand_num(0, 100);
                    if(rand <= 55)
                    {
                        level.tiles[y][x] = tile_floor_stone;
                    }
                }
            }
            
            u32 buff_one[MAX_ROOM_SIZE * MAX_ROOM_SIZE] = {tile_none};
            u32 buff_two[MAX_ROOM_SIZE * MAX_ROOM_SIZE] = {tile_none};
            
            automata_t level_data = {(u32 *)level.tiles, MAX_LEVEL_WIDTH};
            automata_t buff_one_data = {buff_one, MAX_ROOM_SIZE};
            automata_t buff_two_data = {buff_two, MAX_ROOM_SIZE};
            
            scan_room(&level_data, &buff_one_data, room);
            scan_room(&buff_one_data, &buff_two_data, V4u(0, 0, room.w, room.h));
            scan_room(&buff_two_data, &buff_one_data, V4u(0, 0, room.w, room.h));
            scan_room(&buff_one_data, &buff_two_data, V4u(0, 0, room.w, room.h));
            scan_room(&buff_two_data, &buff_one_data, V4u(0, 0, room.w, room.h));
            render_result(&buff_one_data, &level_data, room);
            
            result = true;
        }
    }
    
    return(result);
}

internal void
generate_level()
{
    memset(&level.tiles, 0, sizeof(level.tiles));
    
    for(u32 y = 0; y < level.h; ++y)
    {
        for(u32 x = 0; x < level.w; ++x)
        {
            level.tiles[y][x] = tile_wall_stone;
        }
    }
    
    b32 rooms_done = false;
    v4u rooms[128] = {0};
    u32 room_count = 0;
    u32 tiles_occupied = 0;
    
    while(!rooms_done)
    {
        v4u room = {0};
        
        room_type type = rand_num(room_rectangle, room_automata);
        // TODO(rami):
        //room_type type = room_rectangle;
        //room_type type = room_automata;
        if(type == room_rectangle)
        {
            room.w = rand_num(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
            room.h = rand_num(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
        }
        else if(type == room_automata)
        {
            // NOTE(rami): room.w and room.h can't be more than MAX_ROOM_SIZE
            // or you will get stack smashing.
            room.w = rand_num(8, MAX_ROOM_SIZE);
            room.h = rand_num(8, MAX_ROOM_SIZE);
        }
        
        room.x = rand_num(1, (level.w - 1) - room.w);
        room.y = rand_num(1, (level.h - 1) - room.h);
        
        if(place_room(room, type))
        {
            rooms[room_count++] = room;
            tiles_occupied += room.w * room.h;
            
            // TODO(rami): Debug
            //printf("occupied: %.2f\n", (f32)tiles_occupied / (f32)(level.w * level.h));
            
            if((f32)tiles_occupied / (f32)(level.w * level.h) >= 0.45f)
            {
                rooms_done = true;
            }
        }
    }
    
    // TODO(rami): Debug
    printf("\nRoom Count: %u\n", room_count);
#if 0
    
    for(u32 i = 0; i < room_count; ++i)
    {
        printf("rooms[%u].x: %u\n", i, rooms[i].x);
        printf("rooms[%u].y: %u\n", i, rooms[i].y);
        printf("rooms[%u].w: %u\n", i, rooms[i].w);
        printf("rooms[%u].h: %u\n\n", i, rooms[i].h);
    }
#endif
    
    // Place start of level
    u32 start_room = 0;
    v2u start_pos = {0};
    
    for(;;)
    {
        start_room = rand_num(0, room_count - 1);
        start_pos = get_open_rect_pos(rooms[start_room]);
        
        if(is_traversable(start_pos))
        {
            level.tiles[start_pos.y][start_pos.x] = tile_path_up;
            break;
        }
    }
    
    player.pos = start_pos;
    player.new_pos = start_pos;
    
    // Find the furthest room from the start room
    v2u start_room_pos = V2u(rooms[start_room].x, rooms[start_room].y);
    u32 end_room = 0;
    u32 best_dist = 0;
    
    for(u32 i = 0; i < room_count; ++i)
    {
        v2u current_room_pos = V2u(rooms[i].x, rooms[i].y);
        
        u32 dist = tile_dist(start_room_pos, current_room_pos);
        if(dist > best_dist)
        {
            end_room = i;
            best_dist = dist;
        }
    }
    
    // Place end of level
    for(;;)
    {
        v2u end_pos = get_open_rect_pos(rooms[end_room]);
        if(is_traversable(end_pos))
        {
            level.tiles[end_pos.y][end_pos.x] = tile_path_down;
            break;
        }
    }
    
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    // Add monsters
    //for(u32 i = 0; i < MONSTER_COUNT; ++i)
    for(u32 i = 0; i < 0; ++i)
    {
        // TODO(rami): Debug
        monster_type type = get_monster_for_level();
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
    
    printf("slimes: %u\n", slime_count);
    printf("skeletons: %u\n", skeleton_count);
}