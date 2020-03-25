internal u32_t
get_room_index_for_pos(v2u pos, room_data_t *data)
{
    u32_t result = {0};
    
    for(u32 room_index = 0;
        room_index < data->room_count;
        ++room_index)
    {
        if(is_in_rectangle(pos, data->rooms[room_index]))
        {
            result.success = true;
            result.value = room_index;
            break;
        }
    }
    
    return(result);
}

internal b32
is_dungeon_tile(v2u pos, u32 tile)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].value == tile);
    return(result);
}

internal void
set_dungeon_tile(v2u pos, u32 tile)
{
    dungeon.tiles[(pos.y * dungeon.w) + pos.x].value = tile;
}

internal b32
is_dungeon_occupied(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].occupied);
    return(result);
}

internal void
set_dungeon_occupied(v2u pos, b32 value)
{
    dungeon.tiles[(pos.y * dungeon.w) + pos.x].occupied = value;
}

internal b32
is_dungeon_wall(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].value >= tile_wall_start &&
                  dungeon.tiles[(pos.y * dungeon.w) + pos.x].value <= tile_wall_end);
    
    return(result);
}

internal void
set_dungeon_wall(v2u pos)
{
    u32 wall = random_number(tile_stone_wall_1, tile_stone_wall_8);
    set_dungeon_tile(pos, wall);
}

internal b32
is_dungeon_floor(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].value >= tile_floor_start &&
                  dungeon.tiles[(pos.y * dungeon.w) + pos.x].value <= tile_floor_end);
    
    return(result);
}

internal void
set_dungeon_floor(v2u pos)
{
    u32 floor = random_number(tile_stone_floor_1, tile_stone_floor_4);
    set_dungeon_tile(pos, floor);
}

internal b32
is_dungeon_traversable(v2u pos)
{
    b32 result = (is_dungeon_floor(pos) ||
                  is_dungeon_tile(pos, tile_stone_door_open) ||
                  is_dungeon_tile(pos, tile_stone_path_up) ||
                  is_dungeon_tile(pos, tile_stone_path_down));
    
    return(result);
}

internal b32
is_inside_dungeon(v2u pos)
{
    b32 result = (pos.x < dungeon.w &&
                  pos.y < dungeon.h);
    
    return(result);
}

internal u32
get_neighbour_wall_count(v2u pos)
{
    u32 result = 0;
    
    for(u32 y = pos.y - 1; y < pos.y + 2; ++y)
    {
        for(u32 x = pos.x - 1; x < pos.x + 2; ++x)
        {
            if(is_dungeon_wall(V2u(x, y)))
            {
                ++result;
            }
        }
    }
    
    return(result);
}

internal u32
rect_wall_count_with_padding(v4u rect)
{
    u32 wall_count = 0;
    
    rect.x -= 1;
    rect.y -= 1;
    
    for(u32 y = rect.y; y < (rect.y + rect.h + 2); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w + 2); ++x)
        {
            v2u pos = {x, y};
            if(is_dungeon_wall(pos))
            {
                ++wall_count;
            }
            else if(is_dungeon_tile(pos, tile_stone_door_closed))
            {
                // NOTE(rami): I don't want grates near a door.
                return(0);
            }
        }
    }
    
    return(wall_count);
}

internal b32
is_rect_wall(v4u room, u32 padding)
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
            if(!is_inside_dungeon(pos) || !is_dungeon_wall(pos))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal v2u
rand_rect_pos(v4u rect)
{
    v2u result =
    {
        result.x = random_number(rect.x, rect.x + rect.w - 1),
        result.y = random_number(rect.y, rect.y + rect.h - 1)
    };
    
    return(result);
}

internal v2u
random_dungeon_pos()
{
    // NOTE(rami): Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        random_number(1, dungeon.w - 2),
        random_number(1, dungeon.h - 2)
    };
    
    return(result);
}

internal void
set_player(v2u pos)
{
    player.pos = pos;
    player.new_pos = pos;
    set_dungeon_occupied(player.pos, true);
}

internal void
set_dungeon_monsters(room_data_t *data)
{
    memset(monsters, 0, sizeof(monsters));
    
    s32 range_min = dungeon.level - 2;
    if(range_min < 1)
    {
        range_min = 1;
    }
    
    s32 range_max = dungeon.level + 2;
    if(range_max > MAX_DUNGEON_LEVEL)
    {
        range_max = MAX_DUNGEON_LEVEL;
    }
    
    u32_t player_room_index = get_room_index_for_pos(player.pos, data);
    assert(player_room_index.success);
    
    // TODO(rami): Figure out how many monsters we want to spawn for each level.
    for(u32 monster_count = 0;
        monster_count < 1;
        ++monster_count)
    {
        for(;;)
        {
            u32 monster_id = random_number(1, monster_total - 1);
            u32 monster_info_index = monster_info_index_from_monster_id(monster_id);
            monster_info_t *monster_info = &monster_information[monster_info_index];
            
            if(monster_info->level >= range_min &&
               monster_info->level <= range_max)
            {
                v2u random_pos = random_dungeon_pos();
                if(is_dungeon_traversable(random_pos))
                {
                    if(!is_in_rectangle(random_pos, data->rooms[player_room_index.value]))
                    {
                        add_monster(monster_id, random_pos.x, random_pos.y);
                        break;
                    }
                }
            }
        }
    }
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
    u32 wall = random_number(tile_stone_wall_1, tile_stone_wall_8);
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
    u32 floor = random_number(tile_stone_floor_1, tile_stone_floor_4);
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
                if(wall_count >= 4)
                {
                    set_automaton_wall(dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_floor(dest, dest_tile_pos);
                }
            }
        }
    }
}

internal b32
is_rect_traversable(v4u rect)
{
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            v2u pos = {x, y};
            if(!is_dungeon_traversable(pos))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
place_rectangle_room(v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            v2u pos = {x, y};
            set_dungeon_floor(pos);
        }
    }
}

internal v4u_t
place_double_rectangle_room(v4u room_one)
{
    v4u_t result = {0};
    
    v4u room_two = {0};
    room_two.w = random_number(dungeon.double_rect_min_size, dungeon.double_rect_max_size);
    room_two.h = random_number(dungeon.double_rect_min_size, dungeon.double_rect_max_size);
    room_two.x = room_one.x + random_number(2, room_one.w - 2);
    room_two.y = room_one.y + random_number(2, room_one.h - 2);
    
    result.rect.x = room_one.x;
    result.rect.y = room_one.y;
    
    // NOTE(rami): Set the correct final room width.
    if(room_one.x + room_one.w >= room_two.x + room_two.w)
    {
        result.rect.w = (room_one.x + room_one.w) - room_one.x;
    }
    else
    {
        result.rect.w = (room_two.x + room_two.w) - room_one.x;
    }
    
    // NOTE(rami): Set the correct final room height.
    if(room_one.y + room_one.h >= room_two.y + room_two.h)
    {
        result.rect.h = (room_one.y + room_one.h) - room_one.y;
    }
    else
    {
        result.rect.h = (room_two.y + room_two.h) - room_one.y;
    }
    
    // NOTE(rami): Top left pos of result room is inside the dungeon,
    // make sure the bottom right pos of result room is also inside the dungeon.
    v2u result_bottom_right =
    {
        result.rect.x + result.rect.w,
        result.rect.y + result.rect.h
    };
    
    if(is_inside_dungeon(result_bottom_right))
    {
        if(is_rect_wall(result.rect, 2))
        {
            place_rectangle_room(room_one);
            place_rectangle_room(room_two);
            
            result.success = true;
        }
    }
    
    return(result);
}

internal void
generate_and_place_automaton_room(v4u room)
{
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            u32 random = random_number(0, 100);
            if(random <= 55)
            {
                v2u pos = {x, y};
                set_dungeon_floor(pos);
            }
        }
    }
    
    tile_t buff_one[dungeon.automaton_max_size * dungeon.automaton_max_size];
    tile_t buff_two[dungeon.automaton_max_size * dungeon.automaton_max_size];
    memset(buff_one, 0, sizeof(buff_one));
    memset(buff_two, 0, sizeof(buff_two));
    
    automaton_t dungeon_data = {dungeon.tiles, dungeon.w};
    automaton_t buff_one_data = {buff_one, dungeon.automaton_max_size};
    automaton_t buff_two_data = {buff_two, dungeon.automaton_max_size};
    
    apply_automaton(&dungeon_data, &buff_one_data, room);
    
    v4u buff_room = {0, 0, room.w, room.h};
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    
    place_automaton_room(&buff_two_data, &dungeon_data, room);
}

internal v4u_t
generate_and_place_room()
{
    v4u_t result = {0};
    
    room_type type = random_number(1, 3);
    switch(type)
    {
        case room_rect:
        {
            result.rect.w = random_number(dungeon.rect_min_size, dungeon.rect_max_size);
            result.rect.h = random_number(dungeon.rect_min_size, dungeon.rect_max_size);
        } break;
        
        case room_double_rect:
        {
            result.rect.w = random_number(dungeon.double_rect_min_size, dungeon.double_rect_max_size);
            result.rect.h = random_number(dungeon.double_rect_min_size, dungeon.double_rect_max_size);
        } break;
        
        case room_automaton:
        {
            result.rect.w = random_number(dungeon.automaton_min_size, dungeon.automaton_max_size);
            result.rect.h = random_number(dungeon.automaton_min_size, dungeon.automaton_max_size);
        } break;
        
        invalid_default_case;
    }
    
    v2u random_pos = random_dungeon_pos();
    result.rect.x = random_pos.x - result.rect.w;
    result.rect.y = random_pos.y - result.rect.h;
    
    if(type == room_rect && dungeon.can_have_rect_rooms)
    {
        if(is_rect_wall(result.rect, 2))
        {
            result.success = true;
            place_rectangle_room(result.rect);
        }
    }
    else if(type == room_double_rect && dungeon.can_have_double_rect_rooms)
    {
        result = place_double_rectangle_room(result.rect);
    }
    else if(type == room_automaton && dungeon.can_have_automaton_rooms)
    {
        if(is_rect_wall(result.rect, 2))
        {
            result.success = true;
            generate_and_place_automaton_room(result.rect);
        }
    }
    
    return(result);
}

internal u32
set_dungeon_start(room_data_t *data)
{
    u32 start_room_index = random_number(0, data->room_count - 1);
    
    for(;;)
    {
        v2u start_pos = rand_rect_pos(data->rooms[start_room_index]);
        if(is_dungeon_traversable(start_pos))
        {
            set_dungeon_tile(start_pos, tile_stone_path_up);
            set_player(start_pos);
            break;
        }
    }
    
    return(start_room_index);
}

internal void
set_dungeon_end(room_data_t *data, u32 start_room_index)
{
    v2u start_room_pos =
    {
        data->rooms[start_room_index].x,
        data->rooms[start_room_index].y
    };
    
    u32 end_room_index = 0;
    u32 furthest_distance = 0;
    
    for(u32 room_index = 0;
        room_index < data->room_count;
        ++room_index)
    {
        v2u current_room_pos =
        {
            data->rooms[room_index].x,
            data->rooms[room_index].y
        };
        
        u32 distance = tile_dist_cardinal(start_room_pos, current_room_pos);
        if(distance > furthest_distance)
        {
            end_room_index = room_index;
            furthest_distance = distance;
        }
    }
    
    for(;;)
    {
        v2u end_pos = rand_rect_pos(data->rooms[end_room_index]);
        if(is_dungeon_traversable(end_pos))
        {
            set_dungeon_tile(end_pos, tile_stone_path_down);
            break;
        }
    }
}

internal u32_t
get_closest_room_index(room_data_t *data, b32 *is_connected, u32 a_room_index)
{
    u32_t result = {0};
    u32 best_distance = 512;
    
    for(u32 b_room_index = 0;
        b_room_index < data->room_count;
        ++b_room_index)
    {
        if((a_room_index != b_room_index) && (!is_connected[b_room_index]))
        {
            v2u a_pos = center(data->rooms[a_room_index]);
            v2u b_pos = center(data->rooms[b_room_index]);
            
            u32 distance = tile_dist_cardinal(a_pos, b_pos);
            if(distance < best_distance)
            {
                best_distance = distance;
                
                result.success = true;
                result.value = b_room_index;
            }
        }
    }
    
    return(result);
}

internal void
set_corridor(v2u start, v2u end)
{
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
        set_dungeon_floor(start);
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
        set_dungeon_floor(start);
        start.y += y_direction;
    }
}

internal void
connect_dungeon_rooms(room_data_t *data)
{
    b32 is_connected[MAX_DUNGEON_ROOMS] = {0};
    
    for(u32 start_room_index = 0;
        start_room_index < (data->room_count - 1);
        ++start_room_index)
    {
        u32_t end_room_index = get_closest_room_index(data, is_connected, start_room_index);
        if(end_room_index.success)
        {
            for(;;)
            {
                v2u start_pos = rand_rect_pos(data->rooms[start_room_index]);
                if(is_dungeon_traversable(start_pos))
                {
                    v2u end_pos = rand_rect_pos(data->rooms[end_room_index.value]);
                    if(is_dungeon_traversable(end_pos))
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
set_dungeon_details(room_data_t *data)
{
    // Set different walls
    for(u32 i = 0; i < (f32)(dungeon.w * dungeon.h) * 0.02f; ++i)
    {
        for(;;)
        {
            v2u current = random_dungeon_pos();
            if(is_dungeon_wall(current))
            {
                v2u up = {current.x, current.y - 1};
                v2u down = {current.x, current.y + 1};
                v2u left = {current.x - 1, current.y};
                v2u right = {current.x + 1, current.y};
                
                if(is_dungeon_floor(up) ||
                   is_dungeon_floor(down) ||
                   is_dungeon_floor(left) ||
                   is_dungeon_floor(right))
                {
                    u32 random_tile = random_number(1, 8);
                    switch(random_tile)
                    {
                        case 1: set_dungeon_tile(current, tile_stone_wall_torch_1); break;
                        case 2: set_dungeon_tile(current, tile_stone_wall_torch_2); break;
                        case 3: set_dungeon_tile(current, tile_stone_wall_torch_3); break;
                        
                        case 4: set_dungeon_tile(current, tile_stone_wall_grate_1); break;
                        case 5: set_dungeon_tile(current, tile_stone_wall_grate_2); break;
                        
                        case 6: set_dungeon_tile(current, tile_stone_wall_vines_1); break;
                        case 7: set_dungeon_tile(current, tile_stone_wall_vines_2); break;
                        case 8: set_dungeon_tile(current, tile_stone_wall_vines_3); break;
                        
                        // TODO(rami): Banner art needs to be worked on.
                        //case : set_tile(current, tile_stone_wall_banner_1); break;
                        //case : set_tile(current, tile_stone_wall_banner_2); break;
                        //case : set_tile(current, tile_stone_wall_banner_3); break;
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
    
    // Set doors
    for(u32 i = 0; i < (f32)(dungeon.w * dungeon.h) * 0.5f; ++i)
    {
        v2u current = random_dungeon_pos();
        if(is_dungeon_floor(current))
        {
            v2u up = {current.x, current.y - 1};
            v2u down = {current.x, current.y + 1};
            v2u left = {current.x - 1, current.y};
            v2u right = {current.x + 1, current.y};
            
            v2u up_up = {current.x, current.y - 2};
            v2u down_down = {current.x, current.y + 2};
            v2u left_left = {current.x - 2, current.y};
            v2u right_right = {current.x + 2, current.y};
            
            if(is_dungeon_floor(left) &&
               is_dungeon_floor(right) &&
               is_dungeon_floor(left_left) &&
               is_dungeon_floor(right_right) &&
               is_dungeon_wall(up) &&
               is_dungeon_wall(down))
            {
                v2u left_up = {left.x, left.y - 1};
                v2u left_down = {left.x, left.y + 1};
                v2u right_up = {right.x, right.y - 1};
                v2u right_down = {right.x, right.y + 1};
                
                if((is_dungeon_floor(left_up) && is_dungeon_floor(left_down)) ||
                   (is_dungeon_floor(right_up) && is_dungeon_floor(right_down)))
                {
                    set_dungeon_tile(current, tile_stone_door_closed);
                }
            }
            else if(is_dungeon_floor(up) &&
                    is_dungeon_floor(down) &&
                    is_dungeon_floor(up_up) &&
                    is_dungeon_floor(down_down) &&
                    is_dungeon_wall(left) &&
                    is_dungeon_wall(right))
            {
                v2u up_left = {up.x - 1, up.y};
                v2u up_right = {up.x + 1, up.y};
                v2u down_left = {down.x - 1, down.y};
                v2u down_right = {down.x + 1, down.y};
                
                if((is_dungeon_floor(up_left) && is_dungeon_floor(up_right)) ||
                   (is_dungeon_floor(down_left) && is_dungeon_floor(down_right)))
                {
                    set_dungeon_tile(current, tile_stone_door_closed);
                }
            }
        }
    }
    
    // Set ground grates
    for(u32 room_index = 0;
        room_index < data->room_count;
        ++room_index)
    {
        u32 random = random_number(1, 3);
        if(random == 1)
        {
            for(;;)
            {
                v2u pos = rand_rect_pos(data->rooms[room_index]);
                v4u rect = {0};
                
                u32 random_grate = random_number(1, 2);
                if(random_grate == 1)
                {
                    rect = V4u(pos.x, pos.y, 2, 2);
                }
                else
                {
                    rect = V4u(pos.x, pos.y, 4, 4);
                }
                
                if(is_rect_traversable(rect))
                {
                    u32 wall_count = rect_wall_count_with_padding(rect);
                    if(wall_count >= 4)
                    {
                        if(random_grate == 1)
                        {
                            set_dungeon_tile(V2u(pos.x, pos.y), tile_stone_floor_grate_1);
                            set_dungeon_tile(V2u(pos.x + 1, pos.y), tile_stone_floor_grate_2);
                        }
                        else
                        {
                            set_dungeon_tile(V2u(pos.x, pos.y), tile_stone_floor_grate_3);
                            set_dungeon_tile(V2u(pos.x + 1, pos.y), tile_stone_floor_grate_4);
                            set_dungeon_tile(V2u(pos.x, pos.y + 1), tile_stone_floor_grate_5);
                            set_dungeon_tile(V2u(pos.x + 1, pos.y + 1), tile_stone_floor_grate_6);
                        }
                        
                        break;
                    }
                }
            }
        }
    }
}

internal u32
flood_fill(u32 x, u32 y, u32 fill_count, b32 *fill_tiles)
{
    if(!fill_tiles[(y * dungeon.h) + x] && is_dungeon_floor(V2u(x, y)))
    {
        fill_tiles[(y * dungeon.w) + x] = true;
        ++fill_count;
        
        fill_count = flood_fill(x, y - 1, fill_count, fill_tiles);
        fill_count = flood_fill(x, y + 1, fill_count, fill_tiles);
        fill_count = flood_fill(x - 1, y, fill_count, fill_tiles);
        fill_count = flood_fill(x + 1, y, fill_count, fill_tiles);
    }
    
    return(fill_count);
}

internal void
fill_unreachable_dungeon_tiles(room_data_t *data)
{
    b32 fill_tiles[dungeon.h][dungeon.w];
    
    for(;;)
    {
        // NOTE(rami): If the fill fails, there's data already in the fill array,
        // so we clear it before starting on every iteration.
        memset(&fill_tiles, 0, sizeof(fill_tiles));
        
        u32 room_index = random_number(0, data->room_count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = rand_rect_pos(data->rooms[room_index]);
            if(is_dungeon_traversable(room_pos))
            {
                break;
            }
        }
        
        u32 tiles_flood_filled = flood_fill(room_pos.x, room_pos.y, 0, (b32 *)fill_tiles);
        u32 flood_fill_start_room_area = data->rooms[room_index].w * data->rooms[room_index].h;
        
#if 0
        printf("Flood fill start room index: %u\n", room_index);
        printf("Flood fill start room pos: %u, %u\n", room_pos.x, room_pos.y);
        printf("Tiles flood filled: %u\n", tiles_flood_filled);
        printf("Flood fill start room area: %u\n\n", flood_fill_start_room_area);
#endif
        
        if(tiles_flood_filled > flood_fill_start_room_area)
        {
            break;
        }
    }
    
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            if(!fill_tiles[y][x])
            {
                v2u pos = {x, y};
                set_dungeon_wall(pos);
            }
        }
    }
}

internal void
generate_dungeon()
{
    dungeon.type = dungeon_cavern;
    
    dungeon.w = 128;
    dungeon.h = 128;
    
    dungeon.can_have_rect_rooms = true;
    dungeon.rect_min_size = 4;
    dungeon.rect_max_size = 8;
    
    dungeon.can_have_double_rect_rooms = false;
    dungeon.double_rect_min_size = 4;
    dungeon.double_rect_max_size = 6;
    
    dungeon.can_have_automaton_rooms = false;
    dungeon.automaton_min_size = 8;
    dungeon.automaton_max_size = 16;
    
    assert(dungeon.w <= MAX_DUNGEON_SIZE && dungeon.h <= MAX_DUNGEON_SIZE);
    
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            v2u pos = {x, y};
            set_seen(pos, false);
            set_dungeon_occupied(pos, false);
            set_dungeon_wall(pos);
        }
    }
    
    // NOTE(rami): Leave dungeon blank
#if 0
    return;
#endif
    
    room_data_t data = {0};
    
    u32 dungeon_area = dungeon.w * dungeon.h;
    u32 total_room_area = 0;
    
    while((f32)total_room_area / (f32)dungeon_area < 0.2f)
    {
        v4u_t room = generate_and_place_room();
        if(room.success)
        {
            data.rooms[data.room_count++] = room.rect;
            assert(data.room_count < MAX_DUNGEON_ROOMS);
            
            total_room_area += room.rect.w * room.rect.h;
        }
    }
    
#if 0
    printf("dungeon_area: %u\n", dungeon_area);
    printf("total_room_area: %u\n", total_room_area);
    printf("total_room_area / dungeon_area: %.02f\n", (f32)total_room_area / (f32)dungeon_area);
#endif
    
    connect_dungeon_rooms(&data);
    fill_unreachable_dungeon_tiles(&data);
    set_dungeon_details(&data);
    
    u32 start_room_index = set_dungeon_start(&data);
    set_dungeon_end(&data, start_room_index);
    
    //set_dungeon_monsters(&data);
    
#if 0
    printf("\nRoom Count: %u\n\n", data.room_count);
    for(u32 room_index = 0;
        room_index < data.room_count;
        ++room_index)
    {
        printf("rooms[%u].x: %u\n", room_index, data.rooms[room_index].x);
        printf("rooms[%u].y: %u\n", room_index, data.rooms[room_index].y);
        printf("rooms[%u].w: %u\n", room_index, data.rooms[room_index].w);
        printf("rooms[%u].h: %u\n\n", room_index, data.rooms[room_index].h);
    }
#endif
    
#if 0
    u32 slime_count = 0;
    u32 cave_bat_count = 0;
    u32 python_count = 0;
    u32 skeleton_count = 0;
    u32 armored_skeleton_count = 0;
    u32 orc_warrior_count = 0;
    u32 kobold_count = 0;
    u32 ogre_count = 0;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            switch(monster->type)
            {
                case monster_baby_slime: ++slime_count; break;
                case monster_slime: ++slime_count; break;
                case monster_cave_bat: ++cave_bat_count; break;
                case monster_python: ++python_count; break;
                case monster_skeleton: ++skeleton_count; break;
                case monster_armored_skeleton: ++armored_skeleton_count; break;
                case monster_orc_warrior: ++orc_warrior_count; break;
                case monster_kobold: ++kobold_count; break;
                case monster_ogre: ++ogre_count; break;
                
                invalid_default_case;
            }
        }
    }
    
    printf("Monsters Set\n");
    printf("Slime count: %u\n", slime_count);
    printf("Cave Bat count : %u\n", cave_bat_count);
    printf("Python count: %u\n", python_count);
    printf("Skeleton count: %u\n", skeleton_count);
    printf("Armored Skeleton count: %u\n", armored_skeleton_count);
    printf("Orc Warrior count: %u\n", orc_warrior_count);
    printf("Kobold count: %u\n", kobold_count);
    printf("Ogre count: %u\n\n", ogre_count);
#endif
}