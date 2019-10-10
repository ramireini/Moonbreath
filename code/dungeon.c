internal b32
is_tile(v2u pos, u32 tile)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].value == tile);
    return(result);
}

internal void
set_tile(v2u pos, u32 tile)
{
    dungeon.tiles[pos.y][pos.x].value = tile;
}

internal b32
is_occupied(v2u pos)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].occupied);
    return(result);
}

internal void
set_occupied(v2u pos, b32 value)
{
    dungeon.tiles[pos.y][pos.x].occupied = value;
}

internal b32
is_wall(v2u pos)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].value >= tile_wall_start &&
                  dungeon.tiles[pos.y][pos.x].value <= tile_wall_end);
    
    return(result);
}

internal void
set_wall(v2u pos)
{
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_four);
    set_tile(pos, wall);
}

internal b32
is_floor(v2u pos)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].value >= tile_floor_start &&
                  dungeon.tiles[pos.y][pos.x].value <= tile_floor_end);
    
    return(result);
}

internal void
set_floor(v2u pos)
{
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_five);
    set_tile(pos, floor);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = (is_floor(pos) || is_tile(pos, tile_stone_door_open));
    return(result);
}

internal b32
is_inside_dungeon(v2u pos)
{
    b32 result = (pos.x < MAX_DUNGEON_WIDTH &&
                  pos.y < MAX_DUNGEON_HEIGHT);
    
    return(result);
}

internal v2u
get_rand_rect_pos(v4u rect)
{
    v2u result = {0};
    result.x = rand_num(rect.x, rect.x + rect.w - 1);
    result.y = rand_num(rect.y, rect.y + rect.h - 1);
    return(result);
}

internal v2u
get_rand_dungeon_pos()
{
    // NOTE(rami): Cannot return a position on the edge of the dungeon.
    v2u result = {0};
    result.x = rand_num(1, dungeon.w - 2);
    result.y = rand_num(1, dungeon.h - 2);
    return(result);
}

internal monster_type
get_dungeon_monster()
{
    monster_type result = monster_none;
    
    u32 rand = rand_num(0, 100);
    u32 count = 0;
    
    for(u32 i = 0; i < monster_total; ++i)
    {
        count += monster_spawn_chance[i][dungeon.level - 1];
        if(count >= rand)
        {
            result = i + 1;
            break;
        }
    }
    
    return(result);
}

internal void
set_dungeon_player_start(v2u start_pos)
{
    player.pos = start_pos;
    player.new_pos = start_pos;
    set_occupied(player.pos, true);
}

internal void
set_monsters()
{
    memset(monsters, 0, sizeof(monsters));
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_type type = get_dungeon_monster();
        
        for(;;)
        {
            v2u pos = get_rand_dungeon_pos();
            if(is_traversable(pos) && !is_occupied(pos))
            {
                add_monster(type, pos);
                break;
            }
        }
    }
}

internal b32
is_automaton_tile(automaton_t *automaton, v2u pos, u32 tile)
{
    b32 result = (automaton->ptr[(pos.y * automaton->width) + pos.x].value == tile);
    return(result);
}

internal void
set_automaton_tile(automaton_t *automaton, v2u pos, u32 tile)
{
    automaton->ptr[(pos.y * automaton->width) + pos.x].value = tile;
}

internal b32
is_automaton_wall(automaton_t *automaton, v2u pos)
{
    b32 result = (automaton->ptr[(pos.y * automaton->width) + pos.x].value >= tile_wall_start &&
                  automaton->ptr[(pos.y * automaton->width) + pos.x].value <= tile_wall_end);
    
    return(result);
}

internal void
set_automaton_wall(automaton_t *automaton, v2u pos)
{
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_four);
    set_automaton_tile(automaton, pos, wall);
}

internal b32
is_automaton_floor(automaton_t *automaton, v2u pos)
{
    b32 result = (automaton->ptr[(pos.y * automaton->width) + pos.x].value >= tile_floor_start &&
                  automaton->ptr[(pos.y * automaton->width) + pos.x].value <= tile_floor_end);
    
    return(result);
}

internal void
set_automaton_floor(automaton_t *automaton, v2u pos)
{
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_five);
    automaton->ptr[(pos.y * automaton->width) + pos.x].value = floor;
}

internal void
place_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = {room.x + x, room.y + y};
            u32 tile = src->ptr[(y * src->width) + x].value;
            set_automaton_tile(dest, tile_pos, tile);
        }
    }
}

internal u32
get_automaton_neighbour_wall_count(automaton_t *src, v2u pos, v4u room)
{
    u32 count = 0;
    
    for(s32 y = pos.y - 1; y < (s32)pos.y + 2; ++y)
    {
        for(s32 x = pos.x - 1; x < (s32)pos.x + 2; ++x)
        {
            if(x < (s32)room.x || y < (s32)room.y || x >= (s32)room.x + (s32)room.w || y >= (s32)room.y + (s32)room.h)
            {
                ++count;
            }
            else if(x != (s32)pos.x || y != (s32)pos.y)
            {
                v2u current = {x, y};
                if(is_automaton_wall(src, current))
                {
                    ++count;
                }
            }
        }
    }
    
    return(count);
}

internal void
apply_automaton(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u src_tile_pos = {room.x + x, room.y + y};
            v2u dest_tile_pos = {x, y};
            u32 wall_count = get_automaton_neighbour_wall_count(src, src_tile_pos, room);
            
            if(is_automaton_floor(src, src_tile_pos))
            {
                if(wall_count >= 5)
                {
                    set_automaton_wall(dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_floor(dest, dest_tile_pos);
                }
            }
            else if(is_automaton_wall(src, src_tile_pos))
            {
                if(wall_count <= 4)
                {
                    set_automaton_floor(dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_wall(dest, dest_tile_pos);
                }
            }
        }
    }
}

internal v4u
get_room_size(room_type type)
{
    v4u result = {0};
    
    switch(type)
    {
        case room_rectangle:
        {
            result.w = rand_num(rectangle_min_size, rectangle_max_size);
            result.h = rand_num(rectangle_min_size, rectangle_max_size);
        } break;
        
        case room_double_rectangle:
        {
            result.w = rand_num(double_rectangle_min_size, double_rectangle_max_size);
            result.h = rand_num(double_rectangle_min_size, double_rectangle_max_size);
        } break;
        
        case room_automaton:
        {
            result.w = rand_num(automaton_min_size, automaton_max_size);
            result.h = rand_num(automaton_min_size, automaton_max_size);
        } break;
    }
    
    result.x = rand_num(1, (dungeon.w - 1) - result.w);
    result.y = rand_num(1, (dungeon.h - 1) - result.h);
    
    return(result);
}

internal b32
is_area_wall(v4u room, u32 padding)
{
    for(s32 y = ((s32)room.y - (s32)padding);
        y < ((s32)room.y + (s32)room.h + (s32)padding);
        ++y)
    {
        for(s32 x = ((s32)room.x - (s32)padding);
            x < ((s32)room.x + (s32)room.w + (s32)padding);
            ++x)
        {
            v2u pos = {(u32)x, (u32)y};
            if(!is_inside_dungeon(pos) || !is_wall(pos))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
set_rectangle_room(v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            v2u pos = {x, y};
            set_floor(pos);
        }
    }
}

internal v4u_t
set_double_rectangle_room(v4u room_one)
{
    v4u_t result = {0};
    
    v4u room_two = {0};
    room_two.w = rand_num(3, 6);
    room_two.h = rand_num(3, 6);
    room_two.x = room_one.x + rand_num(2, room_one.w - 2);
    room_two.y = room_one.y + rand_num(2, room_one.h - 2);
    
    v4u new_room = {room_one.x, room_one.y, 0, 0};
    
    // NOTE(rami): Set the correct final room width.
    if(room_one.x + room_one.w >= room_two.x + room_two.w)
    {
        new_room.w = (room_one.x + room_one.w) - room_one.x;
    }
    else
    {
        new_room.w = (room_two.x + room_two.w) - room_one.x;
    }
    
    // NOTE(rami): Set the correct final room height.
    if(room_one.y + room_one.h >= room_two.y + room_two.h)
    {
        new_room.h = (room_one.y + room_one.h) - room_one.y;
    }
    else
    {
        new_room.h = (room_two.y + room_two.h) - room_one.y;
    }
    
    // NOTE(rami): new_room top left point is inside the dungeon,
    // check if new_room bottom right point is inside the dungeon as well.
    v2u new_room_bottom_right = {new_room.x + new_room.w, new_room.y + new_room.h};
    if(is_inside_dungeon(new_room_bottom_right))
    {
        if(is_area_wall(new_room, 2))
        {
            set_rectangle_room(room_one);
            set_rectangle_room(room_two);
            
            result.success = true;
            result.rect = new_room;
        }
    }
    
    return(result);
}

internal void
set_automaton_room(v4u room)
{
    // http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels
    
    // TODO(rami): Automaton rooms can split in two or even more pieces by chance,
    // we need to detect this kind of case with something like a flood fill etc.
    // so that we can just fill in the parts that don't connect to the rest of the dungeon.
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            u32 rand = rand_num(0, 100);
            if(rand <= 55)
            {
                v2u pos = {x, y};
                set_floor(pos);
            }
        }
    }
    
    tile_t buff_one[automaton_max_size * automaton_max_size] = {0};
    tile_t buff_two[automaton_max_size * automaton_max_size] = {0};
    
    automaton_t dungeon_data = {(tile_t *)dungeon.tiles, MAX_DUNGEON_WIDTH};
    automaton_t buff_one_data = {buff_one, automaton_max_size};
    automaton_t buff_two_data = {buff_two, automaton_max_size};
    
    apply_automaton(&dungeon_data, &buff_one_data, room);
    
    v4u buff_room = V4u(0, 0, room.w, room.h);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    
    place_automaton_room(&buff_one_data, &dungeon_data, room);
}

internal v4u_t
generate_room()
{
    v4u_t result = {0};
    
    //room_type type = rand_num(room_rectangle, room_automaton);
    room_type type = rand_num(room_rectangle, room_double_rectangle);
    v4u room = get_room_size(type);
    
    if(type == room_double_rectangle)
    {
        v4u_t new_room = set_double_rectangle_room(room);
        if(new_room.success)
        {
            result.success = true;
            result.rect = new_room.rect;
        }
    }
    else
    {
        if(is_area_wall(room, 2))
        {
            if(type == room_rectangle)
            {
                set_rectangle_room(room);
            }
            else
            {
                set_automaton_room(room);
            }
            
            result.success = true;
            result.rect = room;
        }
    }
    
    return(result);
}

internal u32
set_dungeon_start(v4u *rooms, u32 room_count)
{
    u32 start_room_index = rand_num(0, room_count - 1);
    v2u start_pos = {0};
    
    for(;;)
    {
        start_pos = get_rand_rect_pos(rooms[start_room_index]);
        if(is_traversable(start_pos))
        {
            set_tile(start_pos, tile_stone_path_up);
            set_dungeon_player_start(start_pos);
            break;
        }
    }
    
    return(start_room_index);
}

internal void
set_dungeon_end(v4u *rooms, u32 room_count, u32 start_room_index)
{
    v2u start_room_pos = {rooms[start_room_index].x, rooms[start_room_index].y};
    u32 end_room = 0;
    u32 best_dist = 0;
    
    for(u32 i = 0; i < room_count; ++i)
    {
        v2u current_room_pos = {rooms[i].x, rooms[i].y};
        
        u32 dist = tile_dist(start_room_pos, current_room_pos);
        if(dist > best_dist)
        {
            end_room = i;
            best_dist = dist;
        }
    }
    
    for(;;)
    {
        v2u end_pos = get_rand_rect_pos(rooms[end_room]);
        if(is_traversable(end_pos))
        {
            set_tile(end_pos, tile_stone_path_down);
            break;
        }
    }
}

internal u32_t
get_closest_room_index(v4u *rooms, u32 room_count, b32 *is_connected, u32 room_index_a)
{
    u32_t result = {0};
    u32 best_distance = 512;
    
    for(u32 room_index_b = 0;
        room_index_b < room_count;
        ++room_index_b)
    {
        if((room_index_a != room_index_b) && (!is_connected[room_index_b]))
        {
            v2u pos_a = get_rect_center(rooms[room_index_a]);
            v2u pos_b = get_rect_center(rooms[room_index_b]);
            
            u32 distance = tile_dist(pos_a, pos_b);
            if(distance < best_distance)
            {
                best_distance = distance;
                
                result.success = true;
                result.value = room_index_b;
            }
        }
    }
    
    return(result);
}

internal void
set_corridor(v2u start, v2u end)
{
#if 0
    printf("\nstart_pos: %u, %u\n", start_pos.x, start_pos.y);
    printf("end_pos: %u, %u\n", end_pos.x, end_pos.y);
#endif
    
    s32 x_direction = 0;
    
    if(start.x <= end.x)
    {
        x_direction = 1;
    }
    else if(start.x > end.x)
    {
        x_direction = -1;
    }
    
    while(start.x != end.x)
    {
        set_floor(start);
        start.x += x_direction;
    }
    
    s32 y_direction = 0;
    
    if(start.y <= end.y)
    {
        y_direction = 1;
    }
    else if(start.y > end.y)
    {
        y_direction = -1;
    }
    
    while(start.y != end.y)
    {
        set_floor(start);
        start.y += y_direction;
    }
    
#if 0
    printf("x_direction: %d\n", x_direction);
    printf("y_direction: %d\n", y_direction);
#endif
}

internal void
connect_rooms(v4u *rooms, u32 room_count)
{
    b32 is_connected[64] = {0};
    
    for(u32 start_room_index = 0;
        start_room_index < (room_count - 1);
        ++start_room_index)
    {
        u32_t end_room_index = get_closest_room_index(rooms, room_count, is_connected, start_room_index);
        if(end_room_index.success)
        {
            for(;;)
            {
                v2u start_pos = get_rand_rect_pos(rooms[start_room_index]);
                if(is_traversable(start_pos))
                {
                    v2u end_pos = get_rand_rect_pos(rooms[end_room_index.value]);
                    if(is_traversable(end_pos))
                    {
                        set_corridor(start_pos, end_pos);
                        is_connected[start_room_index] = true;
                        break;
                    }
                }
            }
        }
    }
}

internal void
add_wall_details()
{
    for(u32 i = 0; i < 80; ++i)
    {
        for(;;)
        {
            v2u current = get_rand_dungeon_pos();
            if(is_wall(current))
            {
                v2u up = {current.x, current.y - 1};
                v2u down = {current.x, current.y + 1};
                v2u left = {current.x - 1, current.y};
                v2u right = {current.x + 1, current.y};
                
                if(is_floor(up) ||
                   is_floor(down) ||
                   is_floor(left) ||
                   is_floor(right))
                {
                    u32 tile = rand_num(tile_stone_wall_grate_one, tile_stone_wall_unlit_torch_four);
                    set_tile(current, tile);
                    break;
                }
            }
        }
    }
}

internal void
add_doors()
{
    u32 attempts = 0;
    while(attempts < 4000)
    {
        v2u current = get_rand_dungeon_pos();
        if(is_floor(current))
        {
            v2u up = {current.x, current.y - 1};
            v2u down = {current.x, current.y + 1};
            v2u left = {current.x - 1, current.y};
            v2u right = {current.x + 1, current.y};
            
            v2u up_up = {current.x, current.y - 2};
            v2u down_down = {current.x, current.y + 2};
            v2u left_left = {current.x - 2, current.y};
            v2u right_right = {current.x + 2, current.y};
            
            if(is_floor(left) &&
               is_floor(right) &&
               is_floor(left_left) &&
               is_floor(right_right) &&
               is_wall(up) &&
               is_wall(down))
            {
                v2u left_up = {left.x, left.y - 1};
                v2u left_down = {left.x, left.y + 1};
                v2u right_up = {right.x, right.y - 1};
                v2u right_down = {right.x, right.y + 1};
                
                if((is_floor(left_up) && is_floor(left_down)) ||
                   (is_floor(right_up) && is_floor(right_down)))
                {
                    attempts = 0;
                    set_tile(current, tile_stone_door_closed);
                }
            }
            else if(is_floor(up) &&
                    is_floor(down) &&
                    is_floor(up_up) &&
                    is_floor(down_down) &&
                    is_wall(left) &&
                    is_wall(right))
            {
                v2u up_left = {up.x - 1, up.y};
                v2u up_right = {up.x + 1, up.y};
                v2u down_left = {down.x - 1, down.y};
                v2u down_right = {down.x + 1, down.y};
                
                if((is_floor(up_left) && is_floor(up_right)) ||
                   (is_floor(down_left) && is_floor(down_right)))
                {
                    attempts = 0;
                    set_tile(current, tile_stone_door_closed);
                }
            }
        }
        
        ++attempts;
    }
}

internal void
generate_dungeon()
{
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            v2u pos = {x, y};
            set_seen(pos, false);
            set_occupied(pos, false);
            set_wall(pos);
        }
    }
    
    v4u rooms[64] = {0};
    u32 room_count = 0;
    
    while(room_count < 32)
    {
        v4u_t room = generate_room();
        if(room.success)
        {
            rooms[room_count++] = room.rect;
        }
    }
    
    connect_rooms(rooms, room_count);
    
    u32 start_room_index = set_dungeon_start(rooms, room_count);
    set_dungeon_end(rooms, room_count, start_room_index);
    
    //add_wall_details();
    add_doors();
    
    //set_monsters();
    
#if 0
    printf("\nRoom Count: %u\n\n", room_count);
    for(u32 i = 0; i < room_count; ++i)
    {
        printf("rooms[%u].x: %u\n", i, rooms[i].x);
        printf("rooms[%u].y: %u\n", i, rooms[i].y);
        printf("rooms[%u].w: %u\n", i, rooms[i].w);
        printf("rooms[%u].h: %u\n\n", i, rooms[i].h);
    }
#endif
    
#if 0
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            switch(monster->type)
            {
                case monster_slime: ++slime_count; break;
                case monster_skeleton: ++skeleton_count; break;
            }
        }
    }
    
    printf("Monsters Placed\n");
    printf("Slimes: %u\n", slime_count);
    printf("Skeletons: %u\n\n", skeleton_count);
#endif
}