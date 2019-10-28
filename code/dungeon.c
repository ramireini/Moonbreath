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
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_three);
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
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_seven);
    set_tile(pos, floor);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = (is_floor(pos) ||
                  is_tile(pos, tile_stone_door_open) ||
                  is_tile(pos, tile_stone_path_up) ||
                  is_tile(pos, tile_stone_path_down));
    
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
set_player(v2u pos)
{
    player.pos = pos;
    player.new_pos = pos;
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
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_three);
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
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_seven);
    automaton->ptr[(pos.y * automaton->width) + pos.x].value = floor;
}

internal void
set_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
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
        
        invalid_default_case;
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
generate_and_set_automaton_room(v4u room)
{
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
    
    v4u buff_room = {0, 0, room.w, room.h};
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    
    set_automaton_room(&buff_two_data, &dungeon_data, room);
}

internal v4u_t
generate_room()
{
    v4u_t result = {0};
    
    room_type type = room_none;
    u32 chance = rand_num(0, 100);
    if(chance <= 15)
    {
        type = room_rectangle;
    }
    else if(chance <= 30)
    {
        type = room_double_rectangle;
    }
    else
    {
        type = room_automaton;
    }
    
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
                generate_and_set_automaton_room(room);
            }
            
            result.success = true;
            result.rect = room;
        }
    }
    
    return(result);
}

internal u32
set_start(v4u *rooms, u32 room_count)
{
    u32 start_room_index = rand_num(0, room_count - 1);
    v2u start_pos = {0};
    
    for(;;)
    {
        start_pos = get_rand_rect_pos(rooms[start_room_index]);
        if(is_traversable(start_pos))
        {
            set_tile(start_pos, tile_stone_path_up);
            set_player(start_pos);
            break;
        }
    }
    
    return(start_room_index);
}

internal void
set_end(v4u *rooms, u32 room_count, u32 start_room_index)
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
get_closest_room_index(v4u *rooms, u32 room_count, b32 *is_connected, u32 a_room_index)
{
    u32_t result = {0};
    u32 best_distance = 512;
    
    for(u32 b_room_index = 0;
        b_room_index < room_count;
        ++b_room_index)
    {
        if((a_room_index != b_room_index) && (!is_connected[b_room_index]))
        {
            v2u a_pos = center(rooms[a_room_index]);
            v2u b_pos = center(rooms[b_room_index]);
            
            u32 distance = tile_dist(a_pos, b_pos);
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
}

internal void
connect_rooms(v4u *rooms, u32 room_count)
{
    b32 is_connected[MAX_ROOM_COUNT] = {0};
    
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
set_wall_details()
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
                    u32 tile;
                    
                    u32 chance = rand_num(0, 100);
                    if(chance <= 20)
                    {
                        tile = rand_num(tile_stone_wall_grate_one, tile_stone_wall_grate_three);
                    }
                    else
                    {
                        tile = tile_stone_wall_unlit_torch_one;
                    }
                    
                    set_tile(current, tile);
                    break;
                }
            }
        }
    }
}

internal void
set_doors()
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

internal u32
flood_fill(v2u pos, u32 fill_count, b32 *fill_tiles)
{
    if(!fill_tiles[(pos.y * MAX_DUNGEON_WIDTH) + pos.x] && is_floor(pos))
    {
        fill_tiles[(pos.y * MAX_DUNGEON_WIDTH) + pos.x] = true;
        ++fill_count;
        
        v2u up = {pos.x, pos.y - 1};
        v2u down = {pos.x, pos.y + 1};
        v2u left = {pos.x - 1, pos.y};
        v2u right = {pos.x + 1, pos.y};
        
        fill_count = flood_fill(up, fill_count, fill_tiles);
        fill_count = flood_fill(down, fill_count, fill_tiles);
        fill_count = flood_fill(left, fill_count, fill_tiles);
        fill_count = flood_fill(right, fill_count, fill_tiles);
    }
    
    return(fill_count);
}

internal void
fill_unreachable_tiles(v4u *rooms, u32 room_count)
{
    b32 fill_tiles[MAX_DUNGEON_HEIGHT][MAX_DUNGEON_WIDTH] = {0};
    
    for(;;)
    {
        // NOTE(rami): If the fill fails, there's data already in the fill array,
        // so we clear it before starting on every needed iteration.
        memset(&fill_tiles, 0, sizeof(fill_tiles));
        
        u32 room_index = rand_num(0, room_count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = get_rand_rect_pos(rooms[room_index]);
            if(is_traversable(room_pos))
            {
                break;
            }
        }
        
        u32 tiles_flood_filled = flood_fill(room_pos, 0, (b32 *)fill_tiles);
        u32 flood_fill_start_room_area = rooms[room_index].w * rooms[room_index].h;
        
#if MOONBREATH_SLOW
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
    
    for(u32 y = 0; y < MAX_DUNGEON_HEIGHT; ++y)
    {
        for(u32 x = 0; x < MAX_DUNGEON_WIDTH; ++x)
        {
            if(!fill_tiles[y][x])
            {
                v2u pos = {x, y};
                set_wall(pos);
            }
        }
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
    
    v4u rooms[MAX_ROOM_COUNT] = {0};
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
    fill_unreachable_tiles(rooms, room_count);
    
    u32 start_room_index = set_start(rooms, room_count);
    set_end(rooms, room_count, start_room_index);
    
    set_wall_details();
    set_doors();
    
    //set_monsters();
    
    // NOTE(rami): Generation Info
#if MOONBREATH_SLOW
    u32 total_tile_count = MAX_DUNGEON_WIDTH * MAX_DUNGEON_HEIGHT;
    u32 floor_tile_count = 0;
    
    for(u32 y = 0; y < MAX_DUNGEON_HEIGHT; ++y)
    {
        for(u32 x = 0; x < MAX_DUNGEON_WIDTH; ++x)
        {
            v2u pos = {x, y};
            if(is_floor(pos))
            {
                ++floor_tile_count;
            }
        }
    }
    
    printf("total_tile_count: %u\n", total_tile_count);
    printf("floor_tile_count: %u\n", floor_tile_count);
    printf("level is %.02f%% floor\n\n", ((f32)floor_tile_count / (f32)total_tile_count) * 100.0f);
#endif
    
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
    
#if MOONBREATH_SLOW
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
                
                invalid_default_case;
            }
        }
    }
    
    printf("Monsters Set\n");
    printf("Slimes: %u\n", slime_count);
    printf("Skeletons: %u\n\n", skeleton_count);
#endif
}