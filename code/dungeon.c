internal u32_t
get_room_index_for_pos(v2u pos, rooms_t *rooms)
{
    u32_t result = {0};
    
    for(u32 room_index = 0;
        room_index < rooms->count;
        ++room_index)
    {
        if(is_in_rectangle(pos, rooms->array[room_index]))
        {
            result.success = true;
            result.value = room_index;
            break;
        }
    }
    
    return(result);
}


internal void
set_dungeon_tile_blood(game_state_t *game, v2u pos)
{
    u32 blood_tile = random_number(&game->random, tile_blood_puddle_1, tile_blood_splatter_4);
    dungeon.tiles[(pos.y * dungeon.w) + pos.x].blood_tile = blood_tile;
}

internal u32
get_dungeon_tile_blood(v2u pos)
{
    u32 result = dungeon.tiles[(pos.y * dungeon.w) + pos.x].blood_tile;
    return(result);
}

internal b32
is_dungeon_tile(v2u pos, u32 tile)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile == tile);
    return(result);
}

internal void
set_dungeon_tile(v2u pos, u32 tile)
{
    dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile = tile;
}

internal u32
get_dungeon_tile(v2u pos)
{
    u32 result = dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile;
    return(result);
}

internal b32
is_dungeon_occupied(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].is_occupied);
    return(result);
}

internal void
set_dungeon_occupied(v2u pos, b32 value)
{
    dungeon.tiles[(pos.y * dungeon.w) + pos.x].is_occupied = value;
}

internal b32
is_dungeon_wall(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile >= tile_wall_start &&
                  dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile <= tile_wall_end);
    
    return(result);
}

internal void
set_dungeon_wall(game_state_t *game, v2u pos)
{
    u32 wall = random_number(&game->random, tile_stone_wall_1, tile_stone_wall_8);
    set_dungeon_tile(pos, wall);
}

internal b32
is_dungeon_floor(v2u pos)
{
    b32 result = (dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile >= tile_floor_start &&
                  dungeon.tiles[(pos.y * dungeon.w) + pos.x].tile <= tile_floor_end);
    
    return(result);
}

internal void
set_dungeon_floor(game_state_t *game, v2u pos)
{
    u32 floor = random_number(&game->random, tile_stone_floor_1, tile_stone_floor_4);
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
rand_rect_pos(game_state_t *game, v4u rect)
{
    v2u result =
    {
        result.x = random_number(&game->random, rect.x, rect.x + rect.w - 1),
        result.y = random_number(&game->random, rect.y, rect.y + rect.h - 1)
    };
    
    return(result);
}

internal v2u
random_dungeon_pos(game_state_t *game)
{
    // NOTE(rami): Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        random_number(&game->random, 1, dungeon.w - 2),
        random_number(&game->random, 1, dungeon.h - 2)
    };
    
    return(result);
}

internal void
set_player(entity_t *player, v2u pos)
{
    player->new_pos = player->pos = pos;
    set_dungeon_occupied(player->pos, true);
}

internal void
set_automaton_tile(automaton_t *automaton, v2u pos, u32 tile)
{
    automaton->ptr[(pos.y * automaton->width) + pos.x].tile = tile;
}

internal b32
is_automaton_wall(automaton_t *automaton, v2u pos)
{
    b32 result = (automaton->ptr[(pos.y * automaton->width) + pos.x].tile >= tile_wall_start &&
                  automaton->ptr[(pos.y * automaton->width) + pos.x].tile <= tile_wall_end);
    
    return(result);
}

internal void
set_automaton_wall(game_state_t *game, automaton_t *automaton, v2u pos)
{
    u32 wall = random_number(&game->random, tile_stone_wall_1, tile_stone_wall_8);
    set_automaton_tile(automaton, pos, wall);
}

internal b32
is_automaton_floor(automaton_t *automaton, v2u pos)
{
    b32 result = (automaton->ptr[(pos.y * automaton->width) + pos.x].tile >= tile_floor_start &&
                  automaton->ptr[(pos.y * automaton->width) + pos.x].tile <= tile_floor_end);
    
    return(result);
}

internal void
set_automaton_floor(game_state_t *game, automaton_t *automaton, v2u pos)
{
    u32 floor = random_number(&game->random, tile_stone_floor_1, tile_stone_floor_4);
    automaton->ptr[(pos.y * automaton->width) + pos.x].tile = floor;
}

internal void
place_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = {room.x + x, room.y + y};
            u32 tile = src->ptr[(y * src->width) + x].tile;
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
automaton_step(game_state_t *game, automaton_t *src, automaton_t *dest, v4u room)
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
                    set_automaton_wall(game, dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_floor(game, dest, dest_tile_pos);
                }
            }
            else if(is_automaton_wall(src, src_tile_pos))
            {
                if(wall_count >= 4)
                {
                    set_automaton_wall(game, dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_floor(game, dest, dest_tile_pos);
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
place_rectangle_room(game_state_t *game, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            v2u pos = {x, y};
            set_dungeon_floor(game, pos);
        }
    }
}

internal v4u_t
place_double_rectangle_room(game_state_t *game, v4u room_one)
{
    v4u_t result = {0};
    
    v4u room_two = {0};
    room_two.w = random_number(&game->random, dungeon.double_rect_min_size, dungeon.double_rect_max_size);
    room_two.h = random_number(&game->random, dungeon.double_rect_min_size, dungeon.double_rect_max_size);
    room_two.x = room_one.x + random_number(&game->random, 2, room_one.w - 2);
    room_two.y = room_one.y + random_number(&game->random, 2, room_one.h - 2);
    
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
            place_rectangle_room(game, room_one);
            place_rectangle_room(game, room_two);
            
            result.success = true;
        }
    }
    
    return(result);
}

internal void
generate_and_place_automaton_room(game_state_t *game, v4u room)
{
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            u32 random = random_number(&game->random, 0, 100);
            if(random <= 55)
            {
                v2u pos = {x, y};
                set_dungeon_floor(game, pos);
            }
        }
    }
    
    tile_t buff_one[dungeon.automaton_max_size * dungeon.automaton_max_size];
    tile_t buff_two[dungeon.automaton_max_size * dungeon.automaton_max_size];
    memset(buff_one, 0, sizeof(buff_one));
    memset(buff_two, 0, sizeof(buff_two));
    
    automaton_t dungeon_data = {dungeon.w, dungeon.tiles};
    automaton_t buff_one_data = {dungeon.automaton_max_size, buff_one};
    automaton_t buff_two_data = {dungeon.automaton_max_size, buff_two};
    
    automaton_step(game, &dungeon_data, &buff_one_data, room);
    
    v4u buff_room = {0, 0, room.w, room.h};
    automaton_step(game, &buff_one_data, &buff_two_data, buff_room);
    automaton_step(game, &buff_two_data, &buff_one_data, buff_room);
    automaton_step(game, &buff_one_data, &buff_two_data, buff_room);
    
    place_automaton_room(&buff_two_data, &dungeon_data, room);
}

internal v4u_t
generate_and_place_room(game_state_t *game)
{
    v4u_t result = {0};
    
    room_type type = random_number(&game->random, 1, 3);
    switch(type)
    {
        case room_rect:
        {
            result.rect.w = random_number(&game->random, dungeon.rect_min_size, dungeon.rect_max_size);
            result.rect.h = random_number(&game->random, dungeon.rect_min_size, dungeon.rect_max_size);
        } break;
        
        case room_double_rect:
        {
            result.rect.w = random_number(&game->random, dungeon.double_rect_min_size, dungeon.double_rect_max_size);
            result.rect.h = random_number(&game->random, dungeon.double_rect_min_size, dungeon.double_rect_max_size);
        } break;
        
        case room_automaton:
        {
            result.rect.w = random_number(&game->random, dungeon.automaton_min_size, dungeon.automaton_max_size);
            result.rect.h = random_number(&game->random, dungeon.automaton_min_size, dungeon.automaton_max_size);
        } break;
        
        invalid_default_case;
    }
    
    v2u random_pos = random_dungeon_pos(game);
    result.rect.x = random_pos.x - result.rect.w;
    result.rect.y = random_pos.y - result.rect.h;
    
    if(type == room_rect && dungeon.can_have_rect_rooms)
    {
        if(is_rect_wall(result.rect, 2))
        {
            result.success = true;
            place_rectangle_room(game, result.rect);
        }
    }
    else if(type == room_double_rect && dungeon.can_have_double_rect_rooms)
    {
        result = place_double_rectangle_room(game, result.rect);
    }
    else if(type == room_automaton && dungeon.can_have_automaton_rooms)
    {
        if(is_rect_wall(result.rect, 2))
        {
            result.success = true;
            generate_and_place_automaton_room(game, result.rect);
        }
    }
    
    return(result);
}

internal u32_t
get_closest_room_index(rooms_t *rooms, b32 *is_connected, u32 a_room_index)
{
    u32_t result = {0};
    u32 best_distance = 512;
    
    for(u32 b_room_index = 0;
        b_room_index < rooms->count;
        ++b_room_index)
    {
        if((a_room_index != b_room_index) && (!is_connected[b_room_index]))
        {
            v2u a_pos = center(rooms->array[a_room_index]);
            v2u b_pos = center(rooms->array[b_room_index]);
            
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
create_dungeon(game_state_t *game, entity_t *entities, item_t *items)
{
    entity_t *player = &entities[0];
    
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
            set_dungeon_wall(game, pos);
        }
    }
    
    // NOTE(rami): Leave dungeon blank.
#if 0
    return;
#endif
    
    rooms_t rooms = {0};
    
    u32 dungeon_area = dungeon.w * dungeon.h;
    u32 total_room_area = 0;
    
    while((f32)total_room_area / (f32)dungeon_area < 0.2f)
    {
        v4u_t room = generate_and_place_room(game);
        if(room.success)
        {
            rooms.array[rooms.count++] = room.rect;
            total_room_area += room.rect.w * room.rect.h;
            
            assert(rooms.count < MAX_DUNGEON_ROOMS);
        }
    }
    
#if 0
    printf("dungeon_area: %u\n", dungeon_area);
    printf("total_room_area: %u\n", total_room_area);
    printf("total_room_area / dungeon_area: %.02f\n", (f32)total_room_area / (f32)dungeon_area);
#endif
    
#if 0
    printf("\nRoom Count: %u\n\n", rooms.count);
    for(u32 room_index = 0;
        room_index < rooms.count;
        ++room_index)
    {
        printf("rooms[%u].x: %u\n", room_index, rooms.array[room_index].x);
        printf("rooms[%u].y: %u\n", room_index, rooms.array[room_index].y);
        printf("rooms[%u].w: %u\n", room_index, rooms.array[room_index].w);
        printf("rooms[%u].h: %u\n\n", room_index, rooms.array[room_index].h);
    }
#endif
    
    // NOTE(Rami): Connect Rooms
    b32 is_connected[rooms.count];
    memset(is_connected, 0, sizeof(is_connected));
    
    for(u32 start_room_index = 0;
        start_room_index < (rooms.count - 1);
        ++start_room_index)
    {
        u32_t end_room_index = get_closest_room_index(&rooms, is_connected, start_room_index);
        if(end_room_index.success)
        {
            for(;;)
            {
                v2u start_pos = rand_rect_pos(game, rooms.array[start_room_index]);
                if(is_dungeon_traversable(start_pos))
                {
                    v2u end_pos = rand_rect_pos(game, rooms.array[end_room_index.value]);
                    if(is_dungeon_traversable(end_pos))
                    {
                        s32 x_direction = 0;
                        
                        if(start_pos.x <= end_pos.x)
                        {
                            x_direction = 1;
                        }
                        else if(start_pos.x > end_pos.x)
                        {
                            x_direction = -1;
                        }
                        
                        while(start_pos.x != end_pos.x)
                        {
                            set_dungeon_floor(game, start_pos);
                            start_pos.x += x_direction;
                        }
                        
                        s32 y_direction = 0;
                        
                        if(start_pos.y <= end_pos.y)
                        {
                            y_direction = 1;
                        }
                        else if(start_pos.y > end_pos.y)
                        {
                            y_direction = -1;
                        }
                        
                        while(start_pos.y != end_pos.y)
                        {
                            set_dungeon_floor(game, start_pos);
                            start_pos.y += y_direction;
                        }
                        
                        is_connected[start_room_index] = true;
                        break;
                    }
                }
            }
        }
    }
    
    // NOTE(Rami): Fill Unreachable Tiles
    b32 fill_tiles[dungeon.h][dungeon.w];
    
    for(;;)
    {
        // NOTE(rami): If the fill fails, there's data already in the fill array,
        // so we clear it before starting on every iteration.
        memset(&fill_tiles, 0, sizeof(fill_tiles));
        
        u32 room_index = random_number(&game->random, 0, rooms.count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = rand_rect_pos(game, rooms.array[room_index]);
            if(is_dungeon_traversable(room_pos))
            {
                break;
            }
        }
        
        u32 tiles_flood_filled = flood_fill(room_pos.x, room_pos.y, 0, (b32 *)fill_tiles);
        u32 flood_fill_start_room_area = rooms.array[room_index].w * rooms.array[room_index].h;
        
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
                set_dungeon_wall(game, pos);
            }
        }
    }
    
    // NOTE(Rami): Place Details
    for(u32 i = 0; i < (f32)(dungeon.w * dungeon.h) * 0.02f; ++i)
    {
        for(;;)
        {
            v2u current = random_dungeon_pos(game);
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
                    u32 random_tile = random_number(&game->random, 1, 10);
                    switch(random_tile)
                    {
                        case 1: set_dungeon_tile(current, tile_stone_wall_torch_1); break;
                        case 2: set_dungeon_tile(current, tile_stone_wall_torch_2); break;
                        case 3: set_dungeon_tile(current, tile_stone_wall_torch_3); break;
                        case 4: set_dungeon_tile(current, tile_stone_wall_torch_4); break;
                        case 5: set_dungeon_tile(current, tile_stone_wall_torch_5); break;
                        
                        case 6: set_dungeon_tile(current, tile_stone_wall_grate_1); break;
                        case 7: set_dungeon_tile(current, tile_stone_wall_grate_2); break;
                        
                        case 8: set_dungeon_tile(current, tile_stone_wall_vines_1); break;
                        case 9: set_dungeon_tile(current, tile_stone_wall_vines_2); break;
                        case 10: set_dungeon_tile(current, tile_stone_wall_vines_3); break;
                        
#if 0
                        // TODO(rami): Banner art needs to be worked on.
                        case 11: set_dungeon_tile(current, tile_stone_wall_banner_1); break;
                        case 12: set_dungeon_tile(current, tile_stone_wall_banner_2); break;
                        case 13: set_dungeon_tile(current, tile_stone_wall_banner_3); break;
#endif
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
    
    for(u32 i = 0; i < (f32)(dungeon.w * dungeon.h) * 0.5f; ++i)
    {
        v2u current = random_dungeon_pos(game);
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
    
    for(u32 room_index = 0;
        room_index < rooms.count;
        ++room_index)
    {
        u32 random = random_number(&game->random, 1, 3);
        if(random == 1)
        {
            for(;;)
            {
                v2u pos = rand_rect_pos(game, rooms.array[room_index]);
                v4u rect = {0};
                
                u32 random_grate = random_number(&game->random, 1, 2);
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
    
    // NOTE(Rami): Place Start
    u32 start_room_index = random_number(&game->random, 0, rooms.count - 1);
    
    for(;;)
    {
        v2u start_pos = rand_rect_pos(game, rooms.array[start_room_index]);
        if(is_dungeon_traversable(start_pos))
        {
            set_dungeon_tile(start_pos, tile_stone_path_up);
            set_player(player, start_pos);
            break;
        }
    }
    
    // NOTE(Rami): Place End
    v2u start_room_pos =
    {
        rooms.array[start_room_index].x,
        rooms.array[start_room_index].y
    };
    
    u32 end_room_index = 0;
    u32 furthest_distance = 0;
    
    for(u32 room_index = 0;
        room_index < rooms.count;
        ++room_index)
    {
        v2u current_room_pos =
        {
            rooms.array[room_index].x,
            rooms.array[room_index].y
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
        v2u end_pos = rand_rect_pos(game, rooms.array[end_room_index]);
        if(is_dungeon_traversable(end_pos))
        {
            set_dungeon_tile(end_pos, tile_stone_path_down);
            break;
        }
    }
    
#if 1
    // NOTE(Rami): Place Enemies
    for(u32 entity_index = 1;
        entity_index < ENTITY_COUNT;
        ++entity_index)
    {
        remove_entity(entities + entity_index);
    }
    
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
    
    u32_t player_room_index = get_room_index_for_pos(player->pos, &rooms);
    assert(player_room_index.success);
    
    // TODO(rami): Figure out how many enemies we want to spawn for each level.
    for(u32 enemy_count = 0;
        enemy_count < 1;
        ++enemy_count)
    {
        for(;;)
        {
            u32 enemy_id = random_number(&game->random,
                                         entity_id_player + 1,
                                         entity_id_count - 1);
            
            if(enemy_levels[enemy_id] >= range_min &&
               enemy_levels[enemy_id] <= range_max)
            {
                v2u random_pos = random_dungeon_pos(game);
                if(is_dungeon_traversable(random_pos))
                {
                    if(!is_in_rectangle(random_pos, rooms.array[player_room_index.value]))
                    {
                        add_enemy_entity(entities, enemy_id, random_pos.x, random_pos.y);
                        printf("Enemy placed at %u, %u\n", random_pos.x, random_pos.y);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 0
    // NOTE(Rami): Place Items
    for(u32 item_count = 0;
        item_count < 4;
        ++item_count)
    {
        for(;;)
        {
            v2u item_pos = random_dungeon_pos(game);
            if(is_dungeon_traversable(item_pos))
            {
                // TODO(rami): Get random item type.
                item_type type = item_type_weapon;
                if(type == item_type_weapon)
                {
                    item_rarity rarity = item_rarity_none;
                    u32 rarity_chance = random_number(&game->random, 1, 100);
                    if(rarity_chance <= 5)
                    {
                        rarity = item_rarity_mythical;
                    }
                    else if(rarity_chance <= 40)
                    {
                        rarity = item_rarity_magical;
                    }
                    else if(rarity_chance <= 100)
                    {
                        rarity = item_rarity_common;
                    }
                    
                    // TODO(rami): Get random weapon type.
                    item_id id = item_dagger;
                    
                    add_weapon_item(game, items, id, rarity, item_pos.x, item_pos.y);
                    printf("Weapon placed at %u, %u\n", item_pos.x, item_pos.y);
                }
                else if(type == item_type_armor)
                {
                }
                else if(type == item_type_potion)
                {
                    item_id id = random_number(&game->random, item_potion_of_might, item_potion_of_flight);
                    add_potion_item(items, id, item_pos.x, item_pos.y);
                }
                else if(type == item_type_scroll)
                {
                    //add_scroll_item(id, item_pos.x, item_pos.y);
                }
                
                break;
            }
        }
    }
#endif
}