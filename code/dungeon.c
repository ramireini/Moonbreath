internal v4u
create_padded_rect(v4u rect, u32 padding)
{
    // NOTE(Rami): Width / height needs to be + 1.
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
get_room_index_for_pos(v2u pos, rooms_t *rooms)
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
set_dungeon_tile_blood(game_state_t *game, dungeon_t *dungeon, v2u pos)
{
    u32 blood_tile = random_number(&game->random, tile_blood_puddle_1, tile_blood_splatter_4);
    dungeon->tiles[(pos.y * dungeon->width) + pos.x].blood_tile = blood_tile;
}

internal u32
get_dungeon_tile_blood(dungeon_t *dungeon, v2u pos)
{
    u32 result = dungeon->tiles[(pos.y * dungeon->width) + pos.x].blood_tile;
    return(result);
}

internal b32
is_dungeon_tile(tile_t *tiles, u32 width, v2u pos, u32 tile)
{
    b32 result = (tiles[(pos.y * width) + pos.x].tile == tile);
    return(result);
}

internal u32
get_dungeon_tile(dungeon_t *dungeon, v2u pos)
{
    u32 result = dungeon->tiles[(pos.y * dungeon->width) + pos.x].tile;
    return(result);
}

internal void
set_dungeon_tile(tile_t *tiles, u32 width, v2u pos, u32 value)
{
    tiles[(pos.y * width) + pos.x].tile = value;
}

internal b32
is_dungeon_occupied(dungeon_t *dungeon, v2u pos)
{
    b32 result = (dungeon->tiles[(pos.y * dungeon->width) + pos.x].is_occupied);
    return(result);
}

internal void
set_dungeon_occupied(dungeon_t *dungeon, v2u pos, b32 value)
{
    dungeon->tiles[(pos.y * dungeon->width) + pos.x].is_occupied = value;
}

internal b32
is_dungeon_wall(tile_t *tiles, u32 width, v2u pos)
{
    b32 result = (tiles[(pos.y * width) + pos.x].tile >= tile_wall_start &&
                  tiles[(pos.y * width) + pos.x].tile <= tile_wall_end);
    
    return(result);
}

internal void
set_dungeon_wall(game_state_t *game, tile_t *tiles, u32 width, v2u pos)
{
    u32 wall = random_number(&game->random, tile_stone_wall_1, tile_stone_wall_8);
    set_dungeon_tile(tiles, width, pos, wall);
}

internal b32
is_dungeon_floor(tile_t *tiles, u32 width, v2u pos)
{
    b32 result = (tiles[(pos.y * width) + pos.x].tile >= tile_floor_start &&
                  tiles[(pos.y * width) + pos.x].tile <= tile_floor_end);
    
    return(result);
}

internal void
set_dungeon_floor(game_state_t *game, tile_t *tiles, u32 width, v2u pos)
{
    u32 floor = random_number(&game->random, tile_stone_floor_1, tile_stone_floor_4);
    set_dungeon_tile(tiles, width, pos, floor);
}

internal b32
is_dungeon_traversable(tile_t *tiles, u32 width, v2u pos)
{
    b32 result = (is_dungeon_floor(tiles, width, pos) ||
                  is_dungeon_tile(tiles, width, pos, tile_stone_door_open) ||
                  is_dungeon_tile(tiles, width, pos, tile_stone_path_up) ||
                  is_dungeon_tile(tiles, width, pos, tile_stone_path_down));
    
    return(result);
}


internal b32
is_rect_in_dungeon(dungeon_t *dungeon, v4u rect)
{
    b32 result = (rect.x >= 1 &&
                  rect.y >= 1 &&
                  (rect.x + rect.w) < (dungeon->width - 1) &&
                  (rect.y + rect.h) < (dungeon->height - 1));
    
    return(result);
}

internal b32
is_pos_in_dungeon(dungeon_t *dungeon, v2u pos)
{
    b32 result = (pos.x < dungeon->width &&
                  pos.y < dungeon->height);
    
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
            if(is_dungeon_wall(dungeon->tiles, dungeon->width, V2u(x, y)))
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
            if(!is_dungeon_wall(dungeon->tiles, dungeon->width, V2u(x, y)))
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
    // NOTE(rami): Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        random_number(&game->random, 1, dungeon->width - 2),
        random_number(&game->random, 1, dungeon->height - 2)
    };
    
    return(result);
}

internal void
place_automaton_room(tile_data_t *src, tile_data_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            set_dungeon_tile(dest->array,
                             dest->width,
                             V2u(room.x + x, room.y + y),
                             src->array[(y * src->width) + x].tile);
        }
    }
}

internal u32
get_neighbour_floor_count(tile_data_t *src, v2u pos, v4u room)
{
    u32 floor_count = 0;
    
    for(u32 y = pos.y - 1; y < pos.y + 2; ++y)
    {
        for(u32 x = pos.x - 1; x < pos.x + 2; ++x)
        {
            if(is_inside_rectangle(V2u(x, y), room))
            {
                // NOTE(Rami): Ignore starting pos.
                if(x != pos.x || y != pos.y)
                {
                    if(is_dungeon_floor(src->array, src->width, V2u(x, y)))
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
automaton_step(game_state_t *game, tile_data_t *src, tile_data_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u src_pos = {room.x + x, room.y + y};
            v2u dest_pos = {x, y};
            u32 floor_count = get_neighbour_floor_count(src, src_pos, room);
            
            if(is_dungeon_floor(src->array, src->width, src_pos))
            {
                if(floor_count <= 3)
                {
                    set_dungeon_wall(game, dest->array, dest->width, dest_pos);
                }
                else
                {
                    set_dungeon_floor(game, dest->array, dest->width, dest_pos);
                }
            }
            else if(is_dungeon_wall(src->array, src->width, src_pos))
            {
                if(floor_count >= 6)
                {
                    set_dungeon_floor(game, dest->array, dest->width, dest_pos);
                }
                else
                {
                    set_dungeon_wall(game, dest->array, dest->width, dest_pos);
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
            if(!is_dungeon_traversable(dungeon->tiles, dungeon->width, V2u(x, y)))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
place_rectangle_room(game_state_t *game, tile_t *tiles, u32 width, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            set_dungeon_floor(game, tiles, width, V2u(x, y));
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
            place_rectangle_room(game, dungeon->tiles, dungeon->width, room_one);
            place_rectangle_room(game, dungeon->tiles, dungeon->width, room_two);
            
            result.success = true;
        }
    }
    
    return(result);
}

internal u32
reset_non_flood_filled_tiles(game_state_t *game, tile_t *tiles, u32 width, v4u area, b32 *fill_tiles)
{
    for(u32 y = area.y; y < area.h; ++y)
    {
        for(u32 x = area.x; x < area.w; ++x)
        {
            if(!fill_tiles[(y * width) + x])
            {
                set_dungeon_wall(game, tiles, width, V2u(x, y));
            }
        }
    }
}

internal u32
flood_fill(tile_t *tiles, u32 width, b32 *fill_tiles, u32 fill_count, v2u pos)
{
    if(!fill_tiles[(pos.y * width) + pos.x] &&
       is_dungeon_floor(tiles, width, pos))
    {
        fill_tiles[(pos.y * width) + pos.x] = true;
        ++fill_count;
        
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x, pos.y - 1));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x, pos.y + 1));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x - 1, pos.y));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x + 1, pos.y));
        
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x - 1, pos.y - 1));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x + 1, pos.y - 1));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x - 1, pos.y + 1));
        fill_count = flood_fill(tiles, width, fill_tiles, fill_count, V2u(pos.x + 1, pos.y + 1));
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
    
    // TODO(Rami): Turn the tiles in dungeon_t into a tile_data_t and
    // allocate/free it on start/exit.
    tile_data_t dungeon_data = {dungeon->width, dungeon->tiles};
    tile_data_t buff_one_data = {dungeon->automaton_max_size, buff_one};
    tile_data_t buff_two_data = {dungeon->automaton_max_size, buff_two};
    
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            u32 floor_chance = random_number(&game->random, 0, 100);
            if(floor_chance <= 55)
            {
                set_dungeon_floor(game, buff_one_data.array, buff_one_data.width, V2u(x, y));
            }
        }
    }
    
    v4u buff_room = {0, 0, room.w, room.h};
    automaton_step(game, &buff_one_data, &buff_two_data, buff_room);
    automaton_step(game, &buff_two_data, &buff_one_data, buff_room);
    automaton_step(game, &buff_one_data, &buff_two_data, buff_room);
    automaton_step(game, &buff_two_data, &buff_one_data, buff_room);
    
    u32 floor_count = 0;
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            if(is_dungeon_floor(buff_one_data.array, buff_one_data.width, V2u(x, y)))
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
        if(is_dungeon_traversable(buff_one_data.array, buff_one_data.width, buff_pos))
        {
            break;
        }
    }
    
    b32 fill_tiles[buff_one_data.width * buff_one_data.width];
    memset(&fill_tiles, 0, sizeof(fill_tiles));
    u32 flood_fill_count = flood_fill(buff_one_data.array, buff_one_data.width, (b32 *)fill_tiles, 0, buff_pos);
    
#if 0
    printf("flood_fill_count: %u\n", flood_fill_count);
#endif
    
    if(((f32)flood_fill_count / (f32)floor_count) >= 0.8f)
    {
        reset_non_flood_filled_tiles(game,
                                     buff_one_data.array,
                                     buff_one_data.width,
                                     V4u(0, 0, room.w, room.h),
                                     fill_tiles);
        
        place_automaton_room(&buff_one_data, &dungeon_data, room);
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
                place_rectangle_room(game, dungeon->tiles, dungeon->width, result.rect);
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
                result.success = create_and_place_automaton_room(game, dungeon, result.rect);
            }
        }
    }
    
    
    return(result);
}

internal void
create_dungeon(game_state_t *game, dungeon_t *dungeon, entity_t *entities, item_t *items, u32 *enemy_levels)
{
    entity_t *player = &entities[0];
    
    dungeon->type = dungeon_type_cavern;
    dungeon->width = 128;
    dungeon->height = 128;
    
    dungeon->can_have_rect_rooms = true;
    dungeon->rect_min_size = 4;
    dungeon->rect_max_size = 8;
    
    dungeon->can_have_double_rect_rooms = true;
    dungeon->double_rect_min_size = 4;
    dungeon->double_rect_max_size = 8;
    
    dungeon->can_have_automaton_rooms = true;
    dungeon->automaton_min_size = 12;
    dungeon->automaton_max_size = 18;
    
    assert(dungeon->width <= MAX_DUNGEON_SIZE && dungeon->height <= MAX_DUNGEON_SIZE);
    
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            v2u pos = {x, y};
            set_seen(dungeon, pos, false);
            set_dungeon_occupied(dungeon, pos, false);
            set_dungeon_wall(game, dungeon->tiles, dungeon->width, pos);
        }
    }
    
    // NOTE(rami): Leave dungeon blank.
#if 0
    return;
#endif
    
    u32 dungeon_area = dungeon->width * dungeon->height;
    u32 total_room_area = 0;
    
    rooms_t *rooms = &dungeon->rooms;
    while((f32)total_room_area / (f32)dungeon_area < 0.4f)
#if 0
    while(rooms->count < 32)
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
    // TODO(Rami): Debug return!
    printf("Debug Return\n");
    return;
#endif
    
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
    
    // NOTE(Rami): Connect Rooms
    b32 is_connected[rooms->count];
    memset(is_connected, 0, sizeof(is_connected));
    
    // NOTE(Rami): Loop through all rooms and find the closest room to the current one.
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
                if(is_dungeon_traversable(dungeon->tiles, dungeon->width, start_pos))
                {
                    v2u end_pos = rand_rect_pos(game, rooms->array[end_room_index.value]);
                    if(is_dungeon_traversable(dungeon->tiles, dungeon->width, end_pos))
                    {
                        // NOTE(Rami): Set corridor from start to end.
                        u32 corridor_chance = random_number(&game->random, 1, 10);
                        s32 x_direction = start_pos.x <= end_pos.x ? 1 : -1;
                        s32 y_direction = start_pos.y <= end_pos.y ? 1 : -1;
                        
                        if(corridor_chance <= 4)
                        {
                            while(start_pos.x != end_pos.x)
                            {
                                set_dungeon_floor(game, dungeon->tiles, dungeon->width, start_pos);
                                start_pos.x += x_direction;
                            }
                            
                            while(start_pos.y != end_pos.y)
                            {
                                set_dungeon_floor(game, dungeon->tiles, dungeon->width, start_pos);
                                start_pos.y += y_direction;
                            }
                        }
                        else if(corridor_chance <= 7)
                        {
                            for(;;)
                            {
                                if(start_pos.x != end_pos.x)
                                {
                                    set_dungeon_floor(game, dungeon->tiles, dungeon->width, start_pos);
                                    start_pos.x += x_direction;
                                }
                                
                                if(start_pos.y != end_pos.y)
                                {
                                    set_dungeon_floor(game, dungeon->tiles, dungeon->width, start_pos);
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
                                set_dungeon_floor(game, dungeon->tiles, dungeon->width, start_pos);
                                
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
    
#if 0
    // TODO(Rami): Debug return!
    printf("Debug Return\n");
    return;
#endif
    
    // NOTE(Rami): Fill Unreachable Tiles
    b32 fill_tiles[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    
    for(;;)
    {
        // NOTE(rami): If the fill fails, there's data already in the fill array,
        // so we clear it before starting on every iteration.
        memset(&fill_tiles, 0, sizeof(fill_tiles));
        
        u32 room_index = random_number(&game->random, 0, rooms->count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = rand_rect_pos(game, rooms->array[room_index]);
            if(is_dungeon_traversable(dungeon->tiles, dungeon->width, room_pos))
            {
                break;
            }
        }
        
        u32 flood_fill_count = flood_fill(dungeon->tiles, dungeon->width, (b32 *)fill_tiles, 0, room_pos);
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
    
    reset_non_flood_filled_tiles(game,
                                 dungeon->tiles,
                                 dungeon->width,
                                 V4u(0, 0, dungeon->width, dungeon->height),
                                 fill_tiles);
    
#if 0
    // TODO(Rami): Debug return!
    printf("Debug Return\n");
    return;
#endif
    
    // NOTE(Rami): Place Details
    for(u32 i = 0; i < (f32)(dungeon->width * dungeon->height) * 0.02f; ++i)
    {
        for(;;)
        {
            v2u current = random_dungeon_pos(game, dungeon);
            if(is_dungeon_wall(dungeon->tiles, dungeon->width, current))
            {
                v2u up = {current.x, current.y - 1};
                v2u down = {current.x, current.y + 1};
                v2u left = {current.x - 1, current.y};
                v2u right = {current.x + 1, current.y};
                
                if(is_dungeon_floor(dungeon->tiles, dungeon->width, up) ||
                   is_dungeon_floor(dungeon->tiles, dungeon->width, down)||
                   is_dungeon_floor(dungeon->tiles, dungeon->width, left)||
                   is_dungeon_floor(dungeon->tiles, dungeon->width, right))
                {
                    u32 random_tile = random_number(&game->random, 1, 10);
                    switch(random_tile)
                    {
                        case 1: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_torch_1); break;
                        case 2: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_torch_2); break;
                        case 3: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_torch_3); break;
                        case 4: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_torch_4); break;
                        case 5: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_torch_5); break;
                        
                        case 6: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_grate_1); break;
                        case 7: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_grate_2); break;
                        
                        case 8: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_vines_1); break;
                        case 9: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_vines_2); break;
                        case 10: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_vines_3); break;
                        
#if 0
                        // TODO(rami): Banner art needs to be worked on.
                        case 11: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_banner_1); break;
                        case 12: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_banner_2); break;
                        case 13: set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_wall_banner_3); break;
#endif
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
    
    for(u32 i = 0; i < (f32)(dungeon->width * dungeon->height) * 0.5f; ++i)
    {
        v2u current = random_dungeon_pos(game, dungeon);
        if(is_dungeon_floor(dungeon->tiles, dungeon->width, current))
        {
            v2u up = {current.x, current.y - 1};
            v2u down = {current.x, current.y + 1};
            v2u left = {current.x - 1, current.y};
            v2u right = {current.x + 1, current.y};
            
            v2u up_up = {current.x, current.y - 2};
            v2u down_down = {current.x, current.y + 2};
            v2u left_left = {current.x - 2, current.y};
            v2u right_right = {current.x + 2, current.y};
            
            if(is_dungeon_floor(dungeon->tiles, dungeon->width, left) &&
               is_dungeon_floor(dungeon->tiles, dungeon->width, right) &&
               is_dungeon_floor(dungeon->tiles, dungeon->width, left_left) &&
               is_dungeon_floor(dungeon->tiles, dungeon->width, right_right) &&
               is_dungeon_wall(dungeon->tiles, dungeon->width, up) &&
               is_dungeon_wall(dungeon->tiles, dungeon->width, down))
            {
                v2u left_up = {left.x, left.y - 1};
                v2u left_down = {left.x, left.y + 1};
                v2u right_up = {right.x, right.y - 1};
                v2u right_down = {right.x, right.y + 1};
                
                if((is_dungeon_floor(dungeon->tiles, dungeon->width, left_up) && is_dungeon_floor(dungeon->tiles, dungeon->width, left_down)) ||
                   (is_dungeon_floor(dungeon->tiles, dungeon->width, right_up) && is_dungeon_floor(dungeon->tiles, dungeon->width, right_down)))
                {
                    set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_door_closed);
                }
            }
            else if(is_dungeon_floor(dungeon->tiles, dungeon->width, up) &&
                    is_dungeon_floor(dungeon->tiles, dungeon->width, down) &&
                    is_dungeon_floor(dungeon->tiles, dungeon->width, up_up) &&
                    is_dungeon_floor(dungeon->tiles, dungeon->width, down_down) &&
                    is_dungeon_wall(dungeon->tiles, dungeon->width, left) &&
                    is_dungeon_wall(dungeon->tiles, dungeon->width, right))
            {
                v2u up_left = {up.x - 1, up.y};
                v2u up_right = {up.x + 1, up.y};
                v2u down_left = {down.x - 1, down.y};
                v2u down_right = {down.x + 1, down.y};
                
                if((is_dungeon_floor(dungeon->tiles, dungeon->width, up_left) && is_dungeon_floor(dungeon->tiles, dungeon->width, up_right) ||
                    (is_dungeon_floor(dungeon->tiles, dungeon->width, down_left) && is_dungeon_floor(dungeon->tiles, dungeon->width, down_right))))
                {
                    set_dungeon_tile(dungeon->tiles, dungeon->width, current, tile_stone_door_closed);
                }
            }
        }
    }
    
    for(u32 room_index = 0;
        room_index < rooms->count;
        ++room_index)
    {
        u32 random = random_number(&game->random, 1, 3);
        if(random == 1)
        {
            for(;;)
            {
                v2u pos = rand_rect_pos(game, rooms->array[room_index]);
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
                
                if(is_rect_traversable(dungeon, rect))
                {
                    v4u padded_rect = create_padded_rect(rect, 1);
                    u32 wall_count = rect_wall_count(dungeon, padded_rect);
                    
                    if(wall_count >= 4)
                    {
                        if(random_grate == 1)
                        {
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x, pos.y), tile_stone_floor_grate_1);
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x + 1, pos.y), tile_stone_floor_grate_2);
                        }
                        else
                        {
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x, pos.y), tile_stone_floor_grate_3);
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x + 1, pos.y), tile_stone_floor_grate_4);
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x, pos.y + 1), tile_stone_floor_grate_5);
                            set_dungeon_tile(dungeon->tiles, dungeon->width, V2u(pos.x + 1, pos.y + 1), tile_stone_floor_grate_6);
                        }
                        
                        break;
                    }
                }
            }
        }
    }
    
    // NOTE(Rami): Place Start
    u32 start_room_index = random_number(&game->random, 0, rooms->count - 1);
    
    for(;;)
    {
        v2u start_pos = rand_rect_pos(game, rooms->array[start_room_index]);
        if(is_dungeon_traversable(dungeon->tiles, dungeon->width, start_pos))
        {
            player->new_pos = start_pos;
            move_entity(dungeon, player);
            set_dungeon_tile(dungeon->tiles, dungeon->width, start_pos, tile_stone_path_up);
            
            break;
        }
    }
    
    // NOTE(Rami): Place End
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
        if(is_dungeon_traversable(dungeon->tiles, dungeon->width, end_pos))
        {
            set_dungeon_tile(dungeon->tiles, dungeon->width, end_pos, tile_stone_path_down);
            break;
        }
    }
    
#if 0
    // NOTE(Rami): Place Enemies
    for(u32 entity_index = 1;
        entity_index < ENTITY_COUNT;
        ++entity_index)
    {
        remove_entity(entities + entity_index);
    }
    
    s32 range_min = dungeon->level - 2;
    if(range_min < 1)
    {
        range_min = 1;
    }
    
    s32 range_max = dungeon->level + 2;
    if(range_max > MAX_DUNGEON_LEVEL)
    {
        range_max = MAX_DUNGEON_LEVEL;
    }
    
    u32_bool_t player_room_index = get_room_index_for_pos(player->pos, rooms);
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
                v2u random_pos = random_dungeon_pos(game, dungeon);
                if(is_dungeon_traversable(dungeon->tiles, dungeon->width, random_pos))
                {
                    if(!is_in_rectangle(random_pos, rooms->array[player_room_index.value]))
                    {
                        add_enemy_entity(entities, dungeon, enemy_id, random_pos.x, random_pos.y, enemy_levels);
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