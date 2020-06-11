internal b32
is_tile_value(tiles_t tiles, v2u pos, tile value)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].value == value);
    return(result);
}

internal b32
is_tile_floor(tiles_t tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].value >= tile_floor_start &&
                  tiles.array[(pos.y * tiles.width) + pos.x].value <= tile_floor_end);
    
    return(result);
}

internal b32
is_tile_traversable(tiles_t tiles, v2u pos)
{
    b32 result = (is_tile_floor(tiles, pos) ||
                  is_tile_value(tiles, pos, tile_stone_door_open) ||
                  is_tile_value(tiles, pos, tile_stone_path_up) ||
                  is_tile_value(tiles, pos, tile_stone_path_down) ||
                  is_tile_value(tiles, pos, tile_escape));
    
    return(result);
}

internal b32
pos_in_dungeon(dungeon_t *dungeon, v2u pos)
{
    b32 result = (pos.x > 0 &&
                  pos.y > 0 &&
                  pos.x < dungeon->w &&
                  pos.y < dungeon->h);
    
    return(result);
}

internal void
set_tile_is_seen(tiles_t tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_seen = value;
}

internal void
set_tile_has_been_seen(tiles_t tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen = value;
}

internal b32
tile_is_seen(tiles_t tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_seen);
    return(result);
}

internal b32
tile_has_been_seen(tiles_t tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen);
    return(result);
}

internal void
set_tile_is_seen_and_has_been_seen(tiles_t tiles, v2u pos, b32 value)
{
    set_tile_is_seen(tiles, pos, value);
    set_tile_has_been_seen(tiles, pos, value);
}

internal void
cast_light(dungeon_t *dungeon,
           v2u start,
           u32 fov_range,
           u32 row,
           f32 start_slope,
           f32 end_slope,
           v2u multiplier_x,
           v2u multiplier_y)
{
    if(start_slope >= end_slope)
    {
        f32 next_start_slope = start_slope;
        
        for(u32 y = row; y <= fov_range; ++y)
        {
            b32 is_current_blocked =  false;
            
            for(s32 dx = -y; dx <= 0; ++dx)
            {
                s32 dy = -y;
                
                // left_slope and right_slope store the
                // extremities of the square.
                f32 left_slope = (dx - 0.5f) / (dy + 0.5f);
                f32 right_slope = (dx + 0.5f) / (dy - 0.5f);
                
                if(start_slope < right_slope)
                {
                    continue;
                }
                else if(end_slope > left_slope)
                {
                    break;
                }
                
                // Get the offset for the current position in the
                // current sector by using the multipliers.
                v2s offset =
                {
                    (dx * multiplier_x.x) + (dy * multiplier_x.y),
                    (dx * multiplier_y.x) + (dy * multiplier_y.y)
                };
                
                v2u current = {start.x + offset.x, start.y + offset.y};
                if(pos_in_dungeon(dungeon, current))
                {
                    set_tile_is_seen_and_has_been_seen(dungeon->tiles, current, true);
                    
                    if(is_current_blocked)
                    {
                        if(is_tile_traversable(dungeon->tiles, current))
                        {
                            is_current_blocked = false;
                            start_slope = next_start_slope;
                        }
                        else
                        {
                            next_start_slope = right_slope;
                            continue;
                        }
                    }
                    else if(!is_tile_traversable(dungeon->tiles, current))
                    {
                        is_current_blocked = true;
                        next_start_slope = right_slope;
                        
                        // This position is blocking so start a child scan.
                        cast_light(dungeon,
                                   start,
                                   fov_range,
                                   y + 1,
                                   start_slope,
                                   left_slope,
                                   multiplier_x,
                                   multiplier_y);
                    }
                }
            }
            
            // Scan the next row unless if the last
            // position of the current row is blocking.
            if(is_current_blocked)
            {
                break;
            }
        }
    }
}

internal void
update_fov(dungeon_t *dungeon, entity_t *player)
{
#if MOONBREATH_SLOW
    if(debug_fov)
    {
        for(u32 y = 0; y < dungeon->h; ++y)
        {
            for(u32 x = 0; x < dungeon->w; ++x)
            {
                set_tile_is_seen(dungeon->tiles, V2u(x, y), true);
            }
        }
    }
    
    else
#endif
    
    {
        // Reset visibility.
        for(u32 y = 0; y < dungeon->h; ++y)
        {
            for(u32 x = 0; x < dungeon->w; ++x)
            {
                set_tile_is_seen(dungeon->tiles, V2u(x, y), false);
            }
        }
        
        // Player is visible by default.
        set_tile_is_seen_and_has_been_seen(dungeon->tiles, player->pos, true);
        
        // For transforming positions into other sectors.
        s32 multipliers[4][8] =
        {
            {1, 0, 0, -1, -1, 0, 0, 1},
            {0, 1, -1, 0, 0, -1, 1, 0},
            {0, 1, 1, 0, 0, -1, -1, 0},
            {1, 0, 0, 1, -1, 0, 0, -1}
        };
        
        for(u32 sector = 0; sector < 8; ++sector)
        {
            v2u multiplier_x = {multipliers[0][sector], multipliers[1][sector]};
            v2u multiplier_y = {multipliers[2][sector], multipliers[3][sector]};
            
            cast_light(dungeon,
                       player->pos,
                       player->p.fov,
                       1,
                       1.0f,
                       0.0f,
                       multiplier_x,
                       multiplier_y);
        }
    }
}

internal v4u
create_padded_rect(v4u rect, u32 padding)
{
    // Width / height needs to be + 1.
    v4u result =
    {
        rect.x - padding,
        rect.y - padding,
        rect.w + padding + 1,
        rect.h + padding + 1
    };
    
    return(result);
}

internal u32_bool_t
room_index_from_pos(v2u pos, rooms_t *rooms)
{
    u32_bool_t result = {0};
    
    for(u32 room_index = 0;
        room_index < rooms->count;
        ++room_index)
    {
        if(is_inside_rectangle(pos, rooms->array[room_index]))
        {
            result.success = true;
            result.value = room_index;
            break;
        }
    }
    
    return(result);
}

internal void
set_tile_remains_value(tiles_t tiles, v2u pos, tile value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains = value;
}

internal tile
get_tile_remains_value(tiles_t tiles, v2u pos)
{
    tile remains = tiles.array[(pos.y * tiles.width) + pos.x].remains;
    return(remains);
}

internal v4u_bool_t
get_tile_remains_src(dungeon_t *dungeon, v2u render_pos, u32 tileset_tile_width)
{
    v4u_bool_t result = {0};
    
    tile remains_tile = get_tile_remains_value(dungeon->tiles, render_pos);
    if(remains_tile)
    {
        v2u remains_pos = v2u_from_index(remains_tile, tileset_tile_width);
        
        result.success = true;
        result.rect = tile_rect(remains_pos);
    }
    
    return(result);
}

internal void
set_tile_value(tiles_t tiles, v2u pos, tile value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].value = value;
}

internal tile
get_tile_value(tiles_t tiles, v2u pos)
{
    tile value = tiles.array[(pos.y * tiles.width) + pos.x].value;
    return(value);
}

internal void
set_tile_occupied(tiles_t tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_occupied = value;
}

internal b32
is_tile_occupied(tiles_t tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_occupied);
    return(result);
}

internal b32
is_tile_traversable_and_not_occupied(tiles_t tiles, v2u pos)
{
    b32 result = (is_tile_traversable(tiles, pos) && !is_tile_occupied(tiles, pos));
    return(result);
}

internal void
set_tile_wall(game_state_t *game, tiles_t tiles, v2u pos)
{
    u32 wall = random_number(&game->random, tile_stone_wall_1, tile_stone_wall_6);
    set_tile_value(tiles, pos, wall);
}

internal b32
is_tile_wall(tiles_t tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].value >= tile_wall_start &&
                  tiles.array[(pos.y * tiles.width) + pos.x].value <= tile_wall_end);
    
    return(result);
}

internal void
set_tile_floor(game_state_t *game, tiles_t tiles, v2u pos)
{
    u32 floor = random_number(&game->random, tile_stone_floor_1, tile_stone_floor_4);
    set_tile_value(tiles, pos, floor);
}

internal b32
is_rect_in_dungeon(dungeon_t *dungeon, v4u rect)
{
    b32 result = (rect.x >= 1 &&
                  rect.y >= 1 &&
                  (rect.x + rect.w) < (dungeon->w - 1) &&
                  (rect.y + rect.h) < (dungeon->h - 1));
    
    return(result);
}

internal u32
rect_wall_count(dungeon_t *dungeon, v4u rect)
{
    u32 wall_count = 0;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(is_tile_wall(dungeon->tiles, V2u(x, y)))
            {
                ++wall_count;
            }
        }
    }
    
    return(wall_count);
}

internal b32
is_rect_wall(dungeon_t *dungeon, v4u rect)
{
    for(u32 y = rect.y;
        y < (rect.y + rect.h);
        ++y)
    {
        for(u32 x = rect.x;
            x < (rect.x + rect.w);
            ++x)
        {
            if(!is_tile_wall(dungeon->tiles, V2u(x, y)))
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
        result.x = random_number(&game->random, rect.x, (rect.x + rect.w) - 1),
        result.y = random_number(&game->random, rect.y, (rect.y + rect.h) - 1)
    };
    
    return(result);
}

internal v2u
random_dungeon_pos(game_state_t *game, dungeon_t *dungeon)
{
    // Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        random_number(&game->random, 1, dungeon->w - 2),
        random_number(&game->random, 1, dungeon->h - 2)
    };
    
    return(result);
}

internal u32
neighbour_floor_count(tiles_t src, v2u start, v4u room)
{
    u32 floor_count = 0;
    
    for(u32 y = start.y - 1; y < start.y + 2; ++y)
    {
        for(u32 x = start.x - 1; x < start.x + 2; ++x)
        {
            v2u pos = {x, y};
            if(is_inside_rectangle(pos, room))
            {
                // Ignore starting pos.
                if(pos.x != start.x || pos.y != start.y)
                {
                    if(is_tile_floor(src, pos))
                    {
                        ++floor_count;
                    }
                }
            }
        }
    }
    
    return(floor_count);
}

internal void
place_automaton_room(tiles_t src, tiles_t dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            set_tile_value(dest,
                           V2u(room.x + x, room.y + y),
                           src.array[(y * src.width) + x].value);
        }
    }
}

internal void
automaton_step(game_state_t *game, tiles_t src, tiles_t dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u src_pos = {room.x + x, room.y + y};
            v2u dest_pos = {x, y};
            u32 floor_count = neighbour_floor_count(src, src_pos, room);
            
            if(is_tile_floor(src, src_pos))
            {
                if(floor_count <= 3)
                {
                    set_tile_wall(game, dest, dest_pos);
                }
                else
                {
                    set_tile_floor(game, dest, dest_pos);
                }
            }
            else if(is_tile_wall(src, src_pos))
            {
                if(floor_count >= 6)
                {
                    set_tile_floor(game, dest, dest_pos);
                }
                else
                {
                    set_tile_wall(game, dest, dest_pos);
                }
            }
        }
    }
}

internal b32
is_rect_traversable(dungeon_t *dungeon, v4u rect)
{
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_tile_traversable(dungeon->tiles, V2u(x, y)))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
place_rectangle_room(game_state_t *game, tiles_t tiles, u32 width, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            set_tile_floor(game, tiles, V2u(x, y));
        }
    }
}

internal v4u_bool_t
create_and_place_double_rectangle_room(game_state_t *game, dungeon_t *dungeon, v4u room_one)
{
    v4u_bool_t result = {0};
    
#if 0
    printf("room_one.x: %u\n", room_one.x);
    printf("room_one.y: %u\n", room_one.y);
    printf("room_one.w: %u\n", room_one.w);
    printf("room_one.h: %u\n\n", room_one.h);
#endif
    
    v4u room_two = {0};
    room_two.w = random_number(&game->random, dungeon->double_rect_min_size, dungeon->double_rect_max_size);
    room_two.h = random_number(&game->random, dungeon->double_rect_min_size, dungeon->double_rect_max_size);
    room_two.x = room_one.x + (room_one.w / 2);
    room_two.y = room_one.y + (room_one.h / 2);
    
#if 0
    printf("room_two.x: %u\n", room_two.x);
    printf("room_two.y: %u\n", room_two.y);
    printf("room_two.w: %u\n", room_two.w);
    printf("room_two.h: %u\n\n", room_two.h);
#endif
    
    result.rect.x = room_one.x;
    result.rect.y = room_one.y;
    
    // Set the correct final room width.
    if(room_one.x + room_one.w >= room_two.x + room_two.w)
    {
        result.rect.w = (room_one.x + room_one.w) - room_one.x;
    }
    else
    {
        result.rect.w = (room_two.x + room_two.w) - room_one.x;
    }
    
    // Set the correct final room height.
    if(room_one.y + room_one.h >= room_two.y + room_two.h)
    {
        result.rect.h = (room_one.y + room_one.h) - room_one.y;
    }
    else
    {
        result.rect.h = (room_two.y + room_two.h) - room_one.y;
    }
    
#if 0
    printf("result.rect.x: %u\n", result.rect.x);
    printf("result.rect.y: %u\n", result.rect.y);
    printf("result.rect.w: %u\n", result.rect.w);
    printf("result.rect.h: %u\n\n", result.rect.h);
#endif
    
    v4u padded_rect = create_padded_rect(result.rect, 1);
    if(is_rect_in_dungeon(dungeon, padded_rect))
    {
        if(is_rect_wall(dungeon, padded_rect))
        {
            place_rectangle_room(game, dungeon->tiles, dungeon->w, room_one);
            place_rectangle_room(game, dungeon->tiles, dungeon->w, room_two);
            
            result.success = true;
        }
    }
    
    return(result);
}

internal u32
set_non_flood_filled_tiles_to_wall(game_state_t *game, tiles_t tiles, v4u area, b32 *fill_tiles)
{
    for(u32 y = area.y; y < area.h; ++y)
    {
        for(u32 x = area.x; x < area.w; ++x)
        {
            if(!fill_tiles[(y * tiles.width) + x])
            {
                set_tile_wall(game, tiles, V2u(x, y));
            }
        }
    }
}

internal u32
flood_fill(tiles_t tiles, b32 *fill_tiles, u32 fill_count, v2u pos)
{
    if(!fill_tiles[(pos.y * tiles.width) + pos.x] &&
       is_tile_floor(tiles, pos))
    {
        fill_tiles[(pos.y * tiles.width) + pos.x] = true;
        ++fill_count;
        
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x, pos.y + 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x - 1, pos.y));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x + 1, pos.y));
        
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x - 1, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x + 1, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x - 1, pos.y + 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, V2u(pos.x + 1, pos.y + 1));
    }
    
    return(fill_count);
}

internal b32
create_and_place_automaton_room(game_state_t *game, dungeon_t *dungeon, v4u room)
{
    b32 result = false;
    
    tile_t buff_one[dungeon->automaton_max_size * dungeon->automaton_max_size];
    tile_t buff_two[dungeon->automaton_max_size * dungeon->automaton_max_size];
    
    memset(buff_one, 0, sizeof(buff_one));
    memset(buff_two, 0, sizeof(buff_two));
    
    tiles_t buff_one_data = {dungeon->automaton_max_size, buff_one};
    tiles_t buff_two_data = {dungeon->automaton_max_size, buff_two};
    
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            u32 floor_chance = random_number(&game->random, 0, 100);
            if(floor_chance <= 55)
            {
                set_tile_floor(game, buff_one_data, V2u(x, y));
            }
            else
            {
                set_tile_wall(game, buff_one_data, V2u(x, y));
            }
        }
    }
    
    v4u buff_room = {0, 0, room.w, room.h};
    automaton_step(game, buff_one_data, buff_two_data, buff_room);
    automaton_step(game, buff_two_data, buff_one_data, buff_room);
    automaton_step(game, buff_one_data, buff_two_data, buff_room);
    automaton_step(game, buff_two_data, buff_one_data, buff_room);
    
    u32 floor_count = 0;
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            if(is_tile_floor(buff_one_data, V2u(x, y)))
            {
                ++floor_count;
            }
        }
    }
    
#if 0
    printf("floor_count: %u\n", floor_count);
#endif
    
    v2u buff_pos = {0};
    for(;;)
    {
        buff_pos = rand_rect_pos(game, buff_room);
        if(is_tile_traversable(buff_one_data, buff_pos))
        {
            break;
        }
    }
    
    b32 fill_tiles[buff_one_data.width * buff_one_data.width];
    memset(&fill_tiles, 0, sizeof(fill_tiles));
    u32 flood_fill_count = flood_fill(buff_one_data, (b32 *)fill_tiles, 0, buff_pos);
    
#if 0
    printf("flood_fill_count: %u\n", flood_fill_count);
#endif
    
    if(((f32)flood_fill_count / (f32)floor_count) >= 0.8f)
    {
        set_non_flood_filled_tiles_to_wall(game,
                                           buff_one_data,
                                           V4u(0, 0, room.w, room.h),
                                           fill_tiles);
        
        place_automaton_room(buff_one_data, dungeon->tiles, room);
        result = true;
    }
    
    return(result);
}

internal v4u_bool_t
create_and_place_room(game_state_t *game, dungeon_t *dungeon)
{
    v4u_bool_t result = {0};
    
    room_type type = random_number(&game->random, room_type_rect, room_type_automaton);
    switch(type)
    {
        case room_type_rect:
        {
            result.rect.w = random_number(&game->random, dungeon->rect_min_size, dungeon->rect_max_size);
            result.rect.h = random_number(&game->random, dungeon->rect_min_size, dungeon->rect_max_size);
        } break;
        
        case room_type_double_rect:
        {
            result.rect.w = random_number(&game->random, dungeon->double_rect_min_size, dungeon->double_rect_max_size);
            result.rect.h = random_number(&game->random, dungeon->double_rect_min_size, dungeon->double_rect_max_size);
        } break;
        
        case room_type_automaton:
        {
            result.rect.w = random_number(&game->random, dungeon->automaton_min_size, dungeon->automaton_max_size);
            result.rect.h = random_number(&game->random, dungeon->automaton_min_size, dungeon->automaton_max_size);
        } break;
        
        invalid_default_case;
    }
    
    v2u random_pos = random_dungeon_pos(game, dungeon);
    result.rect.x = random_pos.x;
    result.rect.y = random_pos.y;
    
#if 0
    printf("random_pos: %u, %u\n", random_pos.x, random_pos.y);
    printf("result.rect.x: %u\n", result.rect.x);
    printf("result.rect.y: %u\n", result.rect.y);
    printf("result.rect.w: %u\n", result.rect.w);
    printf("result.rect.h: %u\n\n", result.rect.h);
#endif
    
    v4u padded_rect = create_padded_rect(result.rect, 1);
    
#if 0
    printf("padded_rect.x: %u\n", padded_rect.x);
    printf("padded_rect.y: %u\n", padded_rect.y);
    printf("padded_rect.w: %u\n", padded_rect.w);
    printf("padded_rect.h: %u\n\n", padded_rect.h);
#endif
    
    if(type == room_type_rect && dungeon->can_have_rect_rooms)
    {
        if(is_rect_in_dungeon(dungeon, padded_rect))
        {
            if(is_rect_wall(dungeon, padded_rect))
            {
                result.success = true;
                place_rectangle_room(game, dungeon->tiles, dungeon->w, result.rect);
            }
        }
    }
    else if(type == room_type_double_rect && dungeon->can_have_double_rect_rooms)
    {
        result = create_and_place_double_rectangle_room(game, dungeon, result.rect);
    }
    else if(type == room_type_automaton && dungeon->can_have_automaton_rooms)
    {
        if(is_rect_in_dungeon(dungeon, padded_rect))
        {
            if(is_rect_wall(dungeon, padded_rect))
            {
                result.success = create_and_place_automaton_room(game,
                                                                 dungeon,
                                                                 result.rect);
            }
        }
    }
    
    return(result);
}

internal void
create_dungeon(game_state_t *game,
               dungeon_t *dungeon,
               entity_t *player,
               entity_t *entities,
               item_t *items,
               consumable_info_t *consumable,
               u32 *enemy_levels)
{
    ++dungeon->level;
    
    dungeon->type = dungeon_type_cavern;
    dungeon->tiles.width = dungeon->w = 64;
    dungeon->h = 64;
    
    dungeon->can_have_rect_rooms = true;
    dungeon->rect_min_size = 4;
    dungeon->rect_max_size = 8;
    
    dungeon->can_have_double_rect_rooms = true;
    dungeon->double_rect_min_size = 4;
    dungeon->double_rect_max_size = 8;
    
    dungeon->can_have_automaton_rooms = true;
    dungeon->automaton_min_size = 12;
    dungeon->automaton_max_size = 18;
    
    assert(dungeon->w <= MAX_DUNGEON_SIZE &&
           dungeon->h <= MAX_DUNGEON_SIZE);
    
    // Reset dungeon tiles.
    for(u32 y = 0; y < MAX_DUNGEON_SIZE; ++y)
    {
        for(u32 x = 0; x < MAX_DUNGEON_SIZE; ++x)
        {
            v2u pos = {x, y};
            set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
            set_tile_occupied(dungeon->tiles, pos, false);
            set_tile_wall(game, dungeon->tiles, pos);
        }
    }
    
    // Reset dungeon room data.
    memset(&dungeon->rooms, 0, sizeof(dungeon->rooms));
    
#if 1
    // Test Room
    for(u32 y = 0; y < dungeon->h; ++y)
    {
        for(u32 x = 0; x < dungeon->w; ++x)
        {
            if(x == 0 || x == (dungeon->w - 1) ||
               y == 0 || y == (dungeon->h - 1))
            {
                set_tile_wall(game, dungeon->tiles, V2u(x, y));
            }
            else
            {
                set_tile_floor(game, dungeon->tiles, V2u(x, y));
            }
        }
    }
    
    move_entity(dungeon, V2u(8, 1), player);
    
#if 0
    // Test Entities
    u32 entity_x_start = 15;
    u32 entity_x = entity_x_start;
    u32 entity_y = 23;
    
    // First row
    for(u32 entity_index = entity_player + 1;
        entity_index < entity_count;
        ++entity_index)
    {
        add_enemy_entity(entities,
                         dungeon,
                         enemy_levels,
                         entity_index,
                         entity_x,
                         entity_y);
        
        ++entity_x;
    }
    
    entity_x = entity_x_start;
    
    // Second row
    for(u32 entity_index = entity_player + 1;
        entity_index < entity_count;
        ++entity_index)
    {
        add_enemy_entity(entities,
                         dungeon,
                         enemy_levels,
                         entity_index,
                         entity_x,
                         entity_y + 1);
        
        ++entity_x;
    }
    
    // Kill second row entities
    for(u32 entity_index = 1;
        entity_index < MAX_ENTITIES;
        ++entity_index)
    {
        entity_t *entity = &entities[entity_index];
        if(entity->pos.y == entity_y + 1)
        {
            kill_enemy_entity(game, dungeon, entity);
        }
    }
#endif
    
#if 1
    // Test Items
    add_weapon_item(item_sword, item_rarity_common, 10, 1, game, items);
    
    u32 potion_y = 1;
    for(u32 potion_id = item_potion_start + 1;
        potion_id < item_potion_end;
        ++potion_id)
    {
        add_consumable_item(potion_id, 12, potion_y, items, consumable);
        add_consumable_item(potion_id, 13, potion_y, items, consumable);
        
        ++potion_y;
    }
    
    u32 scroll_y = 1;
    for(u32 scroll_id = item_scroll_start + 1;
        scroll_id < item_scroll_end;
        ++scroll_id)
    {
        add_consumable_item(scroll_id, 15, scroll_y, items, consumable);
        add_consumable_item(scroll_id, 16, scroll_y, items, consumable);
        
        ++scroll_y;
    }
#endif
    
    return;
#elif 0
    // Leave dungeon blank.
    return;
#endif
    
    rooms_t *rooms = &dungeon->rooms;
    u32 dungeon_area = (dungeon->w * dungeon->h);
    u32 total_room_area = 0;
    
#if 1
    while((f32)total_room_area / (f32)dungeon_area < 0.4f)
#else
    while(rooms->count < 2)
#endif
    {
        v4u_bool_t room = create_and_place_room(game, dungeon);
        if(room.success)
        {
            rooms->array[rooms->count++] = room.rect;
            total_room_area += room.rect.w * room.rect.h;
            
            assert(rooms->count < MAX_DUNGEON_ROOMS);
        }
    }
    
#if 0
    printf("dungeon_area: %u\n", dungeon_area);
    printf("total_room_area: %u\n", total_room_area);
    printf("total_room_area / dungeon_area: %.02f\n", (f32)total_room_area / (f32)dungeon_area);
#endif
    
#if 0
    printf("\nRoom Count: %u\n", rooms->count);
    for(u32 room_index = 0;
        room_index < rooms->count;
        ++room_index)
    {
        printf("rooms[%u].x: %u\n", room_index, rooms->array[room_index].x);
        printf("rooms[%u].y: %u\n", room_index, rooms->array[room_index].y);
        printf("rooms[%u].w: %u\n", room_index, rooms->array[room_index].w);
        printf("rooms[%u].h: %u\n\n", room_index, rooms->array[room_index].h);
    }
#endif
    
    // Connect Rooms
    b32 is_connected[rooms->count];
    memset(is_connected, 0, sizeof(is_connected));
    
    // Loop through all rooms and find the closest room to the current one.
    for(u32 start_index = 0;
        start_index < (rooms->count - 1);
        ++start_index)
    {
        u32_bool_t end_room_index = {0};
        u32 best_distance = 1024;
        
        for(u32 end_index = 0;
            end_index < rooms->count;
            ++end_index)
        {
            if((start_index != end_index) && (!is_connected[end_index]))
            {
                v2u a_pos = center(rooms->array[start_index]);
                v2u b_pos = center(rooms->array[end_index]);
                
                u32 distance = tile_dist_cardinal(a_pos, b_pos);
                if(distance < best_distance)
                {
                    best_distance = distance;
                    end_room_index.success = true;
                    end_room_index.value = end_index;
                }
            }
        }
        
        if(end_room_index.success)
        {
            for(;;)
            {
                v2u start_pos = rand_rect_pos(game, rooms->array[start_index]);
                if(is_tile_traversable(dungeon->tiles, start_pos))
                {
                    v2u end_pos = rand_rect_pos(game, rooms->array[end_room_index.value]);
                    if(is_tile_traversable(dungeon->tiles, end_pos))
                    {
                        // Set corridor from start to end.
                        u32 corridor_chance = random_number(&game->random, 1, 10);
                        s32 x_direction = start_pos.x <= end_pos.x ? 1 : -1;
                        s32 y_direction = start_pos.y <= end_pos.y ? 1 : -1;
                        
                        if(corridor_chance <= 4)
                        {
                            while(start_pos.x != end_pos.x)
                            {
                                set_tile_floor(game, dungeon->tiles, start_pos);
                                start_pos.x += x_direction;
                            }
                            
                            while(start_pos.y != end_pos.y)
                            {
                                set_tile_floor(game, dungeon->tiles, start_pos);
                                start_pos.y += y_direction;
                            }
                        }
                        else if(corridor_chance <= 7)
                        {
                            for(;;)
                            {
                                if(start_pos.x != end_pos.x)
                                {
                                    set_tile_floor(game, dungeon->tiles, start_pos);
                                    start_pos.x += x_direction;
                                }
                                
                                if(start_pos.y != end_pos.y)
                                {
                                    set_tile_floor(game, dungeon->tiles, start_pos);
                                    start_pos.y += y_direction;
                                }
                                
                                if(start_pos.x == end_pos.x &&
                                   start_pos.y == end_pos.y)
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            for(;;)
                            {
                                set_tile_floor(game, dungeon->tiles, start_pos);
                                
                                if(start_pos.x != end_pos.x)
                                {
                                    start_pos.x += x_direction;
                                }
                                
                                if(start_pos.y != end_pos.y)
                                {
                                    start_pos.y += y_direction;
                                }
                                
                                if(start_pos.x == end_pos.x &&
                                   start_pos.y == end_pos.y)
                                {
                                    break;
                                }
                            }
                        }
                        
                        is_connected[start_index] = true;
                        break;
                    }
                }
            }
        }
    }
    
    // Fill Unreachable Tiles
    b32 fill_tiles[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    
    for(;;)
    {
        // If the fill fails, there's data already in the fill array,
        // so we clear it before starting on every iteration.
        memset(&fill_tiles, 0, sizeof(fill_tiles));
        
        u32 room_index = random_number(&game->random, 0, rooms->count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = rand_rect_pos(game, rooms->array[room_index]);
            if(is_tile_traversable(dungeon->tiles, room_pos))
            {
                break;
            }
        }
        
        u32 flood_fill_count = flood_fill(dungeon->tiles, (b32 *)fill_tiles, 0, room_pos);
        u32 flood_fill_start_room_area = rooms->array[room_index].w * rooms->array[room_index].h;
#if 0
        printf("Flood fill start room index: %u\n", room_index);
        printf("Flood fill start room pos: %u, %u\n", room_pos.x, room_pos.y);
        printf("flood_fill_count: %u\n", flood_fill_count);
        printf("Flood fill start room area: %u\n\n", flood_fill_start_room_area);
#endif
        
        if(flood_fill_count > flood_fill_start_room_area)
        {
            break;
        }
    }
    
    set_non_flood_filled_tiles_to_wall(game,
                                       dungeon->tiles,
                                       V4u(0, 0, dungeon->w, dungeon->h),
                                       fill_tiles);
    
    // Place Details
    for(u32 i = 0; i < (f32)(dungeon->w * dungeon->h) * 0.02f; ++i)
    {
        for(;;)
        {
            v2u current = random_dungeon_pos(game, dungeon);
            if(is_tile_wall(dungeon->tiles, current))
            {
                v2u up = {current.x, current.y - 1};
                v2u down = {current.x, current.y + 1};
                v2u left = {current.x - 1, current.y};
                v2u right = {current.x + 1, current.y};
                
                if(is_tile_floor(dungeon->tiles, up) ||
                   is_tile_floor(dungeon->tiles, down)||
                   is_tile_floor(dungeon->tiles, left)||
                   is_tile_floor(dungeon->tiles, right))
                {
                    u32 random_tile = random_number(&game->random, 1, 12);
                    switch(random_tile)
                    {
                        case 1: set_tile_value(dungeon->tiles, current, tile_stone_wall_torch_1); break;
                        case 2: set_tile_value(dungeon->tiles, current, tile_stone_wall_torch_2); break;
                        case 3: set_tile_value(dungeon->tiles, current, tile_stone_wall_torch_3); break;
                        case 4: set_tile_value(dungeon->tiles, current, tile_stone_wall_torch_4); break;
                        case 5: set_tile_value(dungeon->tiles, current, tile_stone_wall_torch_5); break;
                        
                        case 6: set_tile_value(dungeon->tiles, current, tile_stone_wall_grate_1); break;
                        case 7: set_tile_value(dungeon->tiles, current, tile_stone_wall_grate_2); break;
                        
                        case 8: set_tile_value(dungeon->tiles, current, tile_stone_wall_vines_1); break;
                        case 9: set_tile_value(dungeon->tiles, current, tile_stone_wall_vines_2); break;
                        case 10: set_tile_value(dungeon->tiles, current, tile_stone_wall_vines_3); break;
                        case 11: set_tile_value(dungeon->tiles, current, tile_stone_wall_vines_4); break;
                        case 12: set_tile_value(dungeon->tiles, current, tile_stone_wall_vines_5); break;
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
    
    // Place Doors
    for(u32 i = 0; i < (f32)(dungeon->w * dungeon->h) * 0.5f; ++i)
    {
        v2u current = random_dungeon_pos(game, dungeon);
        if(is_tile_floor(dungeon->tiles, current))
        {
            v2u up = {current.x, current.y - 1};
            v2u down = {current.x, current.y + 1};
            v2u left = {current.x - 1, current.y};
            v2u right = {current.x + 1, current.y};
            
            v2u up_up = {current.x, current.y - 2};
            v2u down_down = {current.x, current.y + 2};
            v2u left_left = {current.x - 2, current.y};
            v2u right_right = {current.x + 2, current.y};
            
            if(is_tile_floor(dungeon->tiles, left) &&
               is_tile_floor(dungeon->tiles, right) &&
               is_tile_floor(dungeon->tiles, left_left) &&
               is_tile_floor(dungeon->tiles, right_right) &&
               is_tile_wall(dungeon->tiles, up) &&
               is_tile_wall(dungeon->tiles, down))
            {
                v2u left_up = {left.x, left.y - 1};
                v2u left_down = {left.x, left.y + 1};
                v2u right_up = {right.x, right.y - 1};
                v2u right_down = {right.x, right.y + 1};
                
                if((is_tile_floor(dungeon->tiles, left_up) && is_tile_floor(dungeon->tiles, left_down)) ||
                   (is_tile_floor(dungeon->tiles, right_up) && is_tile_floor(dungeon->tiles, right_down)))
                {
                    set_tile_value(dungeon->tiles, current, tile_stone_door_closed);
                }
            }
            else if(is_tile_floor(dungeon->tiles, up) &&
                    is_tile_floor(dungeon->tiles, down) &&
                    is_tile_floor(dungeon->tiles, up_up) &&
                    is_tile_floor(dungeon->tiles, down_down) &&
                    is_tile_wall(dungeon->tiles, left) &&
                    is_tile_wall(dungeon->tiles, right))
            {
                v2u up_left = {up.x - 1, up.y};
                v2u up_right = {up.x + 1, up.y};
                v2u down_left = {down.x - 1, down.y};
                v2u down_right = {down.x + 1, down.y};
                
                if((is_tile_floor(dungeon->tiles, up_left) && is_tile_floor(dungeon->tiles, up_right) ||
                    (is_tile_floor(dungeon->tiles, down_left) && is_tile_floor(dungeon->tiles, down_right))))
                {
                    set_tile_value(dungeon->tiles, current, tile_stone_door_closed);
                }
            }
        }
    }
    
    // Place Start
    u32 start_room_index = random_number(&game->random, 0, rooms->count - 1);
    
    for(;;)
    {
        v2u start_pos = rand_rect_pos(game, rooms->array[start_room_index]);
        if(is_tile_traversable(dungeon->tiles, start_pos))
        {
            move_entity(dungeon, start_pos, player);
            
            if(dungeon->level == 1)
            {
                set_tile_value(dungeon->tiles, start_pos, tile_escape);
            }
            else
            {
                set_tile_value(dungeon->tiles, start_pos, tile_stone_path_up);
            }
            
            break;
        }
    }
    
    // Place End
    v2u start_room_pos =
    {
        rooms->array[start_room_index].x,
        rooms->array[start_room_index].y
    };
    
    u32 end_room_index = 0;
    u32 furthest_distance = 0;
    
    for(u32 room_index = 0;
        room_index < rooms->count;
        ++room_index)
    {
        v2u current_room_pos =
        {
            rooms->array[room_index].x,
            rooms->array[room_index].y
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
        v2u end_pos = rand_rect_pos(game, rooms->array[end_room_index]);
        if(is_tile_traversable(dungeon->tiles, end_pos))
        {
#if 0
            // Place player at end of level.
            move_entity(dungeon, end_pos, player);
#endif
            
            set_tile_value(dungeon->tiles, end_pos, tile_stone_path_down);
            break;
        }
    }
    
#if 1
    // Place Enemies
    for(u32 entity_index = 1;
        entity_index < MAX_ENTITIES;
        ++entity_index)
    {
        remove_entity(entities + entity_index);
    }
    
    u32 range_min = dungeon->level - 1;
    if(range_min == 0)
    {
        range_min = 1;
    }
    
    u32 range_max = dungeon->level + 1;
    if(range_max > MAX_DUNGEON_LEVEL)
    {
        range_max = MAX_DUNGEON_LEVEL;
    }
    
#if 0
    printf("level: %u\n", dungeon->level);
    printf("range_min: %u\n", range_min);
    printf("range_max: %u\n", range_max);
#endif
    
    u32_bool_t player_room_index = room_index_from_pos(player->pos, rooms);
    assert(player_room_index.success);
    
    for(u32 enemy_count = 0;
        enemy_count < (dungeon->w + dungeon->h) / 8;
        ++enemy_count)
    {
        for(;;)
        {
            u32 enemy_id = random_number(&game->random,
                                         entity_none + 2,
                                         entity_count - 1);
            
            if(enemy_levels[enemy_id] >= range_min &&
               enemy_levels[enemy_id] <= range_max)
            {
                v2u enemy_pos = random_dungeon_pos(game, dungeon);
                if(!is_inside_rectangle(enemy_pos, rooms->array[player_room_index.value]))
                {
                    if(is_tile_traversable(dungeon->tiles, enemy_pos))
                    {
                        add_enemy_entity(entities, dungeon, enemy_levels, enemy_id, enemy_pos.x, enemy_pos.y);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 1
    // Place Items
    for(u32 item_count = 0;
        // TODO(Rami): How many items do we want to place?
        item_count < (dungeon->w + dungeon->h) / 8;
        ++item_count)
    {
        for(;;)
        {
            // TODO(Rami): Do we want to limit the amount of items that can be
            // inside of each room?
            v2u item_pos = random_dungeon_pos(game, dungeon);
            if(!V2u_equal(item_pos, player->pos) &&
               is_tile_traversable(dungeon->tiles, item_pos))
            {
                item_type type = random_number(&game->random,
                                               item_type_none + 1,
                                               item_type_count - 1);
                
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
                    
                    // TODO(Rami): Chance?
                    // TODO(rami): Random weapon type.
                    item id = item_dagger;
                    //item id = random_number(&game->random,
                    //item_weapon_start + 1,
                    //item_weapon_end - 1);
                    
                    add_weapon_item(id, rarity, item_pos.x, item_pos.y, game, items);
                }
                else if(type == item_type_armor)
                {
                }
                else if(type == item_type_potion)
                {
                    // TODO(Rami): Add chance.
                    item id = random_number(&game->random,
                                            item_potion_start + 1,
                                            item_potion_end - 1);
                    
                    add_consumable_item(id, item_pos.x, item_pos.y, items, consumable);
                }
                else if(type == item_type_scroll)
                {
                    // TODO(Rami): Add chance.
                    item id = random_number(&game->random,
                                            item_scroll_start + 1,
                                            item_scroll_end - 1);
                    
                    add_consumable_item(id, item_pos.x, item_pos.y, items, consumable);
                }
                
                break;
            }
        }
    }
#endif
}