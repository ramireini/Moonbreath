// TODO(rami): Debug
#if 0
printf("\nRandomised Base\n");
for(u32 y = room.y; y < room.y + room.h; ++y)
{
    for(u32 x = room.x; x < room.x + room.w; ++x)
    {
        v2u pos = V2u(x, y);
        if(is_tile(pos, tile_none))
        {
            printf("-");
        }
        else if(is_tile(pos, tile_wall_stone))
        {
            printf("#");
        }
        else if(is_tile(pos, tile_floor_stone))
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
    for(u32 y = 0; y < src->width; ++y)
    {
        for(u32 x = 0; x < src->width; ++x)
        {
            if(src->ptr[(y * src->width) + x].tile == tile_none)
            {
                printf("-");
            }
            else if(src->ptr[(y * src->width) + x].tile == tile_wall_stone)
            {
                printf("#");
            }
            else if(src->ptr[(y * src->width) + x].tile == tile_floor_stone)
            {
                printf(".");
            }
        }
        
        printf("\n");
    }
}
#endif

internal void
set_tile(v2u pos, tile_type tile)
{
    level.tiles[pos.y][pos.x].tile = tile;
}

internal b32
is_tile(v2u pos, u32 tile)
{
    b32 result = (level.tiles[pos.y][pos.x].tile == tile);
    return(result);
}

internal void
set_occupied(v2u pos, b32 value)
{
    level.tiles[pos.y][pos.x].occupied = value;
}

internal b32
is_occupied(v2u pos)
{
    b32 result = level.tiles[pos.y][pos.x].occupied;
    return(result);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = false;
    
    if(is_tile(pos, tile_floor_stone) ||
       is_tile(pos, tile_floor_grass) ||
       is_tile(pos, tile_door_open) ||
       is_tile(pos, tile_path_up) ||
       is_tile(pos, tile_path_down))
    {
        result = true;
    }
    
    return(result);
}

internal b32
is_inside_level(v2u pos)
{
    b32 result = false;
    
    if(pos.x >= 0 && pos.x < MAX_LEVEL_WIDTH &&
       pos.y >= 0 && pos.y < MAX_LEVEL_HEIGHT)
    {
        result = true;
    }
    
    return(result);
}

internal void
render_result(automata_t *src, automata_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = V2u(room.x + x, room.y + y);
            dest->ptr[(tile_pos.y * dest->width) + tile_pos.x].tile = src->ptr[(y * src->width) + x].tile;
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
                if(src->ptr[(y * src->width) + x].tile == tile_wall_stone)
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
            
            if(src->ptr[(tile_pos.y * src->width) + tile_pos.x].tile == tile_floor_stone)
            {
                if(wall_count >= 5)
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_wall_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_floor_stone;
                }
            }
            else if(src->ptr[(tile_pos.y * src->width) + tile_pos.x].tile == tile_wall_stone)
            {
                if(wall_count <= 4)
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_floor_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_wall_stone;
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
            
            for(u32 i = 0; i < array_count(monsters); ++i)
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
            if(!is_tile(V2u(x, y), tile_wall_stone))
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
                    set_tile(V2u(x, y), tile_floor_stone);
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
                        set_tile(V2u(x, y), tile_floor_stone);
                    }
                }
            }
            
            tile_t buff_one[MAX_ROOM_SIZE * MAX_ROOM_SIZE] = {tile_none};
            tile_t buff_two[MAX_ROOM_SIZE * MAX_ROOM_SIZE] = {tile_none};
            
            automata_t level_data = {(tile_t *)level.tiles, MAX_LEVEL_WIDTH};
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
place_monsters()
{
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
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
        add_monster(type, pos.x, pos.y);
    }
    
    printf("slimes: %u\n", slime_count);
    printf("skeletons: %u\n", skeleton_count);
}

internal u32
place_level_start(v4u *rooms, u32 room_count)
{
    u32 start_room_index = 0;
    v2u start_pos = {0};
    
    for(;;)
    {
        start_room_index = rand_num(0, room_count - 1);
        start_pos = get_open_rect_pos(rooms[start_room_index]);
        
        if(is_traversable(start_pos))
        {
            set_tile(start_pos, tile_path_up);
            break;
        }
    }
    
    player.pos = start_pos;
    player.new_pos = start_pos;
    
    return(start_room_index);
}

internal void
place_level_end(v4u *rooms, u32 room_count, u32 start_room_index)
{
    v2u start_room_pos = V2u(rooms[start_room_index].x,
                             rooms[start_room_index].y);
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
    
    for(;;)
    {
        v2u end_pos = get_open_rect_pos(rooms[end_room]);
        if(is_traversable(end_pos))
        {
            set_tile(end_pos, tile_path_down);
            break;
        }
    }
    
}

internal void
generate_level()
{
    for(u32 y = 0; y < level.h; ++y)
    {
        for(u32 x = 0; x < level.w; ++x)
        {
            v2u pos = V2u(x, y);
            set_occupied(pos, false);
            set_tile(pos, tile_wall_stone);
        }
    }
    
    b32 rooms_done = false;
    v4u rooms[128] = {0};
    u32 room_count = 0;
    u32 tiles_occupied = 0;
    
    while(!rooms_done)
    {
        v4u room = {0};
        
        room_type rand_type = rand_num(room_rectangle, room_automata);
        // TODO(rami):
        //room_type type = room_rectangle;
        //room_type type = room_automata;
        if(rand_type == room_rectangle)
        {
            room.w = rand_num(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
            room.h = rand_num(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
        }
        else if(rand_type == room_automata)
        {
            // NOTE(rami): room.w and room.h can't be more than MAX_ROOM_SIZE
            // or you will get stack smashing.
            room.w = rand_num(8, MAX_ROOM_SIZE);
            room.h = rand_num(8, MAX_ROOM_SIZE);
        }
        
        room.x = rand_num(1, (level.w - 1) - room.w);
        room.y = rand_num(1, (level.h - 1) - room.h);
        
        if(place_room(room, rand_type))
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
    
    u32 start_room_index = place_level_start(rooms, room_count);
    place_level_end(rooms, room_count, start_room_index);
    
    // TODO(rami): !
    //place_monsters();
}