internal TileID
tile_id(TileData tiles, v2u pos)
{
    TileID id = tiles.array[(pos.y * tiles.width) + pos.x].id;
    return(id);
}

internal b32
is_tile_id(TileData tiles, v2u pos, TileID id)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].id == id);
    return(result);
}

internal b32
is_tile_travel(TileData tiles, v2u pos)
{
    b32 result = (is_tile_id(tiles, pos, TileID_StonePathUp) ||
                  is_tile_id(tiles, pos, TileID_StonePathDown));
    
    return(result);
}

internal b32
is_tile_floor(TileData tiles, v2u pos)
{
    b32 result = (is_tile_id(tiles, pos, TileID_StoneFloor1) ||
                  is_tile_id(tiles, pos, TileID_StoneFloor2) ||
                  is_tile_id(tiles, pos, TileID_StoneFloor3) ||
                  is_tile_id(tiles, pos, TileID_StoneFloor4) ||
                  
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate1) ||
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate2) ||
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate3) ||
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate4) ||
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate5) ||
                  is_tile_id(tiles, pos, TileID_StoneFloorGrate6));
    
    return(result);
}

internal b32
is_tile_traversable(TileData tiles, v2u pos)
{
    b32 result = (is_tile_floor(tiles, pos) ||
                  is_tile_id(tiles, pos, TileID_StoneDoorOpen) ||
                  is_tile_id(tiles, pos, TileID_StonePathUp) ||
                  is_tile_id(tiles, pos, TileID_StonePathDown) ||
                  is_tile_id(tiles, pos, TileID_ExitDungeon));
    
    return(result);
}

internal b32
is_inside_dungeon(Dungeon *dungeon, v2u pos)
{
    b32 result = (pos.x > 0 &&
                  pos.y > 0 &&
                  pos.x < dungeon->w &&
                  pos.y < dungeon->h);
    
    return(result);
}

internal b32
is_inside_room(v4u room, v2u pos)
{
    b32 result = (pos.x >= room.x &&
                  pos.y >= room.y &&
                  pos.x < (room.x + room.w) &&
                  pos.y < (room.y + room.h));
    
    return(result);
}

internal v4u
get_padded_rect(v4u rect, u32 padding)
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

internal RoomIndex
get_room_index(RoomData *rooms, v2u pos)
{
    RoomIndex result = {0};
    
    for(u32 index = 0; index < rooms->count; ++index)
    {
        if(is_inside_room(rooms->array[index], pos))
        {
            result.found = true;
            result.index = index;
            break;
        }
    }
    
    return(result);
}

internal void
set_tile_remains_value(TileData tiles, v2u pos, TileID id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains_id = id;
}

internal TileID
get_tile_remains_value(TileData tiles, v2u pos)
{
    TileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains_id;
    return(remains);
}

internal b32
can_place_remains_on_pos(TileData tiles, v2u pos)
{
    b32 result = (!is_tile_id(tiles, pos, TileID_ExitDungeon) &&
                  !get_tile_remains_value(tiles, pos));
    
    return(result);
}

internal RemainsSource
get_tile_remains_src(Dungeon *dungeon, v2u render_pos, u32 tileset_tile_width)
{
    RemainsSource result = {0};
    
    TileID remains_id = get_tile_remains_value(dungeon->tiles, render_pos);
    if(remains_id)
    {
        v2u remains_pos = v2u_from_index(remains_id, tileset_tile_width);
        
        result.found = true;
        result.rect = tile_rect(remains_pos);
    }
    
    return(result);
}

internal void
set_tile_id(TileData tiles, v2u pos, TileID id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].id = id;
}

internal void
set_tile_occupied(TileData tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_occupied = value;
}

internal b32
is_tile_occupied(TileData tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_occupied);
    return(result);
}

internal b32
is_tile_traversable_and_not_occupied(TileData tiles, v2u pos)
{
    b32 result = (is_tile_traversable(tiles, pos) && !is_tile_occupied(tiles, pos));
    return(result);
}

internal void
set_tile_wall(GameState *game, TileData tiles, v2u pos)
{
    u32 wall = random_number(&game->random, TileID_StoneWall1, TileID_StoneWall5);
    set_tile_id(tiles, pos, wall);
}

internal b32
is_tile_wall(TileData tiles, v2u pos)
{
    b32 result = (is_tile_id(tiles, pos, TileID_StoneWall1) ||
                  is_tile_id(tiles, pos, TileID_StoneWall2) ||
                  is_tile_id(tiles, pos, TileID_StoneWall3) ||
                  is_tile_id(tiles, pos, TileID_StoneWall4) ||
                  is_tile_id(tiles, pos, TileID_StoneWall5) ||
                  is_tile_id(tiles, pos, TileID_StoneWall5) ||
                  
                  is_tile_id(tiles, pos, TileID_StoneWallTorch1) ||
                  is_tile_id(tiles, pos, TileID_StoneWallTorch2) ||
                  is_tile_id(tiles, pos, TileID_StoneWallTorch3) ||
                  is_tile_id(tiles, pos, TileID_StoneWallTorch4) ||
                  is_tile_id(tiles, pos, TileID_StoneWallTorch5) ||
                  
                  is_tile_id(tiles, pos, TileID_StoneWallGrate1) ||
                  is_tile_id(tiles, pos, TileID_StoneWallGrate2) ||
                  
                  is_tile_id(tiles, pos, TileID_StoneWallVines1) ||
                  is_tile_id(tiles, pos, TileID_StoneWallVines2) ||
                  is_tile_id(tiles, pos, TileID_StoneWallVines3) ||
                  is_tile_id(tiles, pos, TileID_StoneWallVines4) ||
                  is_tile_id(tiles, pos, TileID_StoneWallVines5));
    
    return(result);
}

internal void
set_tile_floor(GameState *game, TileData tiles, v2u pos)
{
    u32 floor = random_number(&game->random, TileID_StoneFloor1, TileID_StoneFloor4);
    set_tile_id(tiles, pos, floor);
}

internal b32
is_rect_in_dungeon(Dungeon *dungeon, v4u rect)
{
    b32 result = (rect.x >= 1 &&
                  rect.y >= 1 &&
                  (rect.x + rect.w) < (dungeon->w - 1) &&
                  (rect.y + rect.h) < (dungeon->h - 1));
    
    return(result);
}

internal u32
rect_wall_count(Dungeon *dungeon, v4u rect)
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
is_rect_wall(Dungeon *dungeon, v4u rect)
{
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
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
rand_rect_pos(GameState *game, v4u rect)
{
    v2u result =
    {
        result.x = random_number(&game->random, rect.x, (rect.x + rect.w) - 1),
        result.y = random_number(&game->random, rect.y, (rect.y + rect.h) - 1)
    };
    
    return(result);
}

internal v2u
random_dungeon_pos(GameState *game, Dungeon *dungeon)
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
neighbour_floor_count(TileData src, v2u start, v4u room)
{
    u32 floor_count = 0;
    
    for(u32 y = start.y - 1; y < start.y + 2; ++y)
    {
        for(u32 x = start.x - 1; x < start.x + 2; ++x)
        {
            v2u pos = {x, y};
            if(is_inside_room(room, pos))
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
place_automaton_room(TileData src, TileData dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            set_tile_id(dest,
                        V2u(room.x + x, room.y + y),
                        src.array[(y * src.width) + x].id);
        }
    }
}

internal void
automaton_step(GameState *game, TileData src, TileData dest, v4u room)
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
is_rect_traversable(Dungeon *dungeon, v4u rect)
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
place_rect_room(GameState *game, TileData tiles, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            set_tile_floor(game, tiles, V2u(x, y));
        }
    }
}

internal CreatedRoom
create_and_place_double_rect_room(GameState *game, Dungeon *dungeon, v4u room_one)
{
    CreatedRoom result = {0};
    
#if 0
    printf("room_one.x: %u\n", room_one.x);
    printf("room_one.y: %u\n", room_one.y);
    printf("room_one.w: %u\n", room_one.w);
    printf("room_one.h: %u\n\n", room_one.h);
#endif
    
    v4u room_two = {0};
    room_two.w = random_number(&game->random, dungeon->double_rect_size.min, dungeon->double_rect_size.max);
    room_two.h = random_number(&game->random, dungeon->double_rect_size.min, dungeon->double_rect_size.max);
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
    
    v4u padded_rect = get_padded_rect(result.rect, 1);
    if(is_rect_in_dungeon(dungeon, padded_rect))
    {
        if(is_rect_wall(dungeon, padded_rect))
        {
            place_rect_room(game, dungeon->tiles, room_one);
            place_rect_room(game, dungeon->tiles, room_two);
            
            result.success = true;
        }
    }
    
    return(result);
}

internal u32
set_not_flood_filled_tiles_as_wall(GameState *game, TileData tiles, v4u area, b32 *fill_tiles)
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
flood_fill(TileData tiles, b32 *fill_tiles, u32 fill_count, v2u pos)
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
create_and_place_automaton_room(GameState *game, Dungeon *dungeon, v4u room)
{
    b32 result = false;
    
    Tile buff_one[dungeon->automaton_size.max * dungeon->automaton_size.max];
    Tile buff_two[dungeon->automaton_size.max * dungeon->automaton_size.max];
    
    memset(buff_one, 0, sizeof(buff_one));
    memset(buff_two, 0, sizeof(buff_two));
    
    TileData buff_one_data = {dungeon->automaton_size.max, buff_one};
    TileData buff_two_data = {dungeon->automaton_size.max, buff_two};
    
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            u32 floor_chance = random_number(&game->random, 1, 100);
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
        set_not_flood_filled_tiles_as_wall(game,
                                           buff_one_data,
                                           V4u(0, 0, room.w, room.h),
                                           fill_tiles);
        
        place_automaton_room(buff_one_data, dungeon->tiles, room);
        result = true;
    }
    
    return(result);
}

internal CreatedRoom
create_and_place_room(GameState *game, Dungeon *dungeon)
{
    CreatedRoom result = {0};
    
    RoomType type = RoomType_None;
    u32 break_value = random_number(&game->random, 1, 100);
    u32 counter = 0;
    
    for(;;)
    {
        type = random_number(&game->random, RoomType_Rect, RoomType_Automaton);
        
        counter += dungeon->room_type_spawn_chances[type];
        if(counter >= break_value)
        {
            break;
        }
    }
    
    switch(type)
    {
        case RoomType_Rect:
        {
            result.rect.w = random_number(&game->random, dungeon->rect_size.min, dungeon->rect_size.max);
            result.rect.h = random_number(&game->random, dungeon->rect_size.min, dungeon->rect_size.max);
        } break;
        
        case RoomType_DoubleRect:
        {
            result.rect.w = random_number(&game->random, dungeon->double_rect_size.min, dungeon->double_rect_size.max);
            result.rect.h = random_number(&game->random, dungeon->double_rect_size.min, dungeon->double_rect_size.max);
        } break;
        
        case RoomType_Automaton:
        {
            result.rect.w = random_number(&game->random, dungeon->automaton_size.min, dungeon->automaton_size.min);
            result.rect.h = random_number(&game->random, dungeon->automaton_size.min, dungeon->automaton_size.min);
        } break;
        
        invalid_default_case;
    }
    
    v2u random_pos = random_dungeon_pos(game, dungeon);
    result.rect.x = random_pos.x;
    result.rect.y = random_pos.y;
    
    v4u padded_rect = get_padded_rect(result.rect, 1);
    
#if 0
    printf("random_pos: %u, %u\n", random_pos.x, random_pos.y);
    printf("result.rect.x: %u\n", result.rect.x);
    printf("result.rect.y: %u\n", result.rect.y);
    printf("result.rect.w: %u\n", result.rect.w);
    printf("result.rect.h: %u\n\n", result.rect.h);
#endif
    
#if 0
    printf("padded_rect.x: %u\n", padded_rect.x);
    printf("padded_rect.y: %u\n", padded_rect.y);
    printf("padded_rect.w: %u\n", padded_rect.w);
    printf("padded_rect.h: %u\n\n", padded_rect.h);
#endif
    
    assert(dungeon->room_type_spawn_chances[type]);
    
    if(type == RoomType_Rect)
    {
        if(is_rect_in_dungeon(dungeon, padded_rect) &&
           is_rect_wall(dungeon, padded_rect))
        {
            result.success = true;
            place_rect_room(game, dungeon->tiles, result.rect);
        }
    }
    else if(type == RoomType_DoubleRect)
    {
        result = create_and_place_double_rect_room(game, dungeon, result.rect);
    }
    else if(type == RoomType_Automaton)
    {
        if(is_rect_in_dungeon(dungeon, padded_rect) &&
           is_rect_wall(dungeon, padded_rect))
        {
            result.success = create_and_place_automaton_room(game, dungeon, result.rect);
        }
    }
    
    return(result);
}

internal void
create_dungeon(GameState *game,
               Dungeon *dungeon,
               Entity *player,
               String128 *log,
               Entity *entities,
               Item *items,
               ConsumableData *consumable_data,
               u32 *enemy_levels)
{
    ++dungeon->level;
    
    dungeon->type = DungeonType_Cavern;
    dungeon->w = 64;
    dungeon->h = 64;
    
    dungeon->pathfind.width = dungeon->w;
    dungeon->tiles.width = dungeon->w;
    
    dungeon->room_type_spawn_chances[RoomType_Rect] = 50;
    dungeon->rect_size.min = 4;
    dungeon->rect_size.max = 8;
    
    dungeon->room_type_spawn_chances[RoomType_DoubleRect] = 50;
    dungeon->double_rect_size.min = 4;
    dungeon->double_rect_size.max = 8;
    
    dungeon->room_type_spawn_chances[RoomType_Automaton] = 50;
    dungeon->automaton_size.min = 12;
    dungeon->automaton_size.max = 18;
    
    assert(dungeon->w <= MAX_DUNGEON_SIZE &&
           dungeon->h <= MAX_DUNGEON_SIZE);
    
    { // Reset Dungeon Data
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
        
        memset(&dungeon->rooms, 0, sizeof(dungeon->rooms));
    }
    
    { // Reset Entity And Item Data
        for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
        {
            remove_entity(entities + index);
        }
        
        memset(items, 0, sizeof(Item) * MAX_ITEM_COUNT);
    }
    
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
    
    move_entity(dungeon->tiles, V2u(6, 1), player);
    add_enemy_entity(entities, dungeon->tiles, enemy_levels, EntityID_Dummy, player->pos.x, player->pos.y + 1);
    
#if 0
    // Test Entities
    u32 entity_x_start = 15;
    u32 entity_x = entity_x_start;
    u32 entity_y = 23;
    
    // First row
    for(u32 index = EntityID_None + 1; index < EntityID_Count; ++index)
    {
        add_enemy_entity(entities,
                         dungeon->tiles,
                         enemy_levels,
                         index,
                         entity_x,
                         entity_y);
        
        ++entity_x;
    }
    
    entity_x = entity_x_start;
    
    // Second row
    for(u32 index = EntityID_None + 1; index < EntityID_Count; ++index)
    {
        add_enemy_entity(entities,
                         dungeon->tiles,
                         enemy_levels,
                         index,
                         entity_x,
                         entity_y + 1);
        
        ++entity_x;
    }
    
    // Kill second row entities
    for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities[index];
        if(entity->pos.y == entity_y + 1)
        {
            kill_entity(game, dungeon, log, entity);
        }
    }
#endif
    
#if 1
    // Test Items
    u32 weapon_y = 1;
    for(ItemID weapon_id = ItemID_WeaponStart + 1; weapon_id < ItemID_WeaponEnd; ++weapon_id)
    {
        add_weapon_item(game, items, weapon_id, ItemRarity_Common, 8, weapon_y);
        add_weapon_item(game, items, weapon_id, ItemRarity_Magical, 9, weapon_y);
        add_weapon_item(game, items, weapon_id, ItemRarity_Mythical, 10, weapon_y);
        
        ++weapon_y;
    }
    
    add_armour_item(game, items, ItemID_LeatherHelmet, 12, 1);
    add_armour_item(game, items, ItemID_LeatherChestplate, 13, 1);
    add_armour_item(game, items, ItemID_LeatherGreaves, 14, 1);
    add_armour_item(game, items, ItemID_LeatherBoots, 15, 1);
    
    add_armour_item(game, items, ItemID_SteelHelmet, 12, 2);
    add_armour_item(game, items, ItemID_SteelChestplate, 13, 2);
    add_armour_item(game, items, ItemID_SteelGreaves, 14, 2);
    add_armour_item(game, items, ItemID_SteelBoots, 15, 2);
    
    u32 potion_y = 1;
    for(ItemID potion_id = ItemID_PotionStart + 1; potion_id < ItemID_PotionEnd; ++potion_id)
    {
        add_consumable_item(items, &game->random, consumable_data, potion_id, 17, potion_y);
        add_consumable_item(items, &game->random, consumable_data, potion_id, 18, potion_y);
        
        ++potion_y;
    }
    
    u32 scroll_y = 1;
    for(ItemID scroll_id = ItemID_ScrollStart + 1; scroll_id < ItemID_ScrollEnd; ++scroll_id)
    {
        add_consumable_item(items, &game->random, consumable_data, scroll_id, 20, scroll_y);
        add_consumable_item(items, &game->random, consumable_data, scroll_id, 21, scroll_y);
        
        ++scroll_y;
    }
#endif
    
    return;
#elif 0
    // Leave dungeon blank.
    return;
#endif
    
    RoomData *rooms = &dungeon->rooms;
    u32 dungeon_area = dungeon->w * dungeon->h;
    u32 total_room_area = 0;
    
#if 1
    while((f32)total_room_area / (f32)dungeon_area < 0.4f)
#else
    while(rooms->count < 2)
#endif
    {
        CreatedRoom room = create_and_place_room(game, dungeon);
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
    for(u32 room_index = 0; room_index < rooms->count; ++room_index)
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
    for(u32 start_index = 0; start_index < (rooms->count - 1); ++start_index)
    {
        RoomIndex end_room = {0};
        u32 best_distance = U32_MAX;
        
        for(u32 end_index = 0; end_index < rooms->count; ++end_index)
        {
            if((start_index != end_index) && !is_connected[end_index])
            {
                v2u start_pos = center(rooms->array[start_index]);
                v2u end_pos = center(rooms->array[end_index]);
                
                u32 distance = tile_dist_cardinal(start_pos, end_pos);
                if(distance < best_distance)
                {
                    best_distance = distance;
                    
                    end_room.found = true;
                    end_room.index = end_index;
                }
            }
        }
        
        if(end_room.found)
        {
            for(;;)
            {
                v2u start_pos = rand_rect_pos(game, rooms->array[start_index]);
                if(is_tile_traversable(dungeon->tiles, start_pos))
                {
                    v2u end_pos = rand_rect_pos(game, rooms->array[end_room.index]);
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
    
    set_not_flood_filled_tiles_as_wall(game,
                                       dungeon->tiles,
                                       V4u(0, 0, dungeon->w, dungeon->h),
                                       fill_tiles);
    
    // Place Details
    for(u32 index = 0; index < (f32)(dungeon->w * dungeon->h) * 0.02f; ++index)
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
                        case 1: set_tile_id(dungeon->tiles, current, TileID_StoneWallTorch1); break;
                        case 2: set_tile_id(dungeon->tiles, current, TileID_StoneWallTorch2); break;
                        case 3: set_tile_id(dungeon->tiles, current, TileID_StoneWallTorch3); break;
                        case 4: set_tile_id(dungeon->tiles, current, TileID_StoneWallTorch4); break;
                        case 5: set_tile_id(dungeon->tiles, current, TileID_StoneWallTorch5); break;
                        
                        case 6: set_tile_id(dungeon->tiles, current, TileID_StoneWallGrate1); break;
                        case 7: set_tile_id(dungeon->tiles, current, TileID_StoneWallGrate2); break;
                        
                        case 8: set_tile_id(dungeon->tiles, current, TileID_StoneWallVines1); break;
                        case 9: set_tile_id(dungeon->tiles, current, TileID_StoneWallVines2); break;
                        case 10: set_tile_id(dungeon->tiles, current, TileID_StoneWallVines3); break;
                        case 11: set_tile_id(dungeon->tiles, current, TileID_StoneWallVines4); break;
                        case 12: set_tile_id(dungeon->tiles, current, TileID_StoneWallVines5); break;
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
    
    // Place Doors
    for(u32 index = 0; index < (f32)(dungeon->w * dungeon->h) * 0.5f; ++index)
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
                    set_tile_id(dungeon->tiles, current, TileID_StoneDoorClosed);
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
                    set_tile_id(dungeon->tiles, current, TileID_StoneDoorClosed);
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
            move_entity(dungeon->tiles, start_pos, player);
            
            if(dungeon->level == 1)
            {
                set_tile_id(dungeon->tiles, start_pos, TileID_ExitDungeon);
            }
            else
            {
                set_tile_id(dungeon->tiles, start_pos, TileID_StonePathUp);
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
    u32 biggest_distance = 0;
    
    for(u32 room_index = 0; room_index < rooms->count; ++room_index)
    {
        v2u current_room_pos =
        {
            rooms->array[room_index].x,
            rooms->array[room_index].y
        };
        
        u32 distance = tile_dist_cardinal(start_room_pos, current_room_pos);
        if(distance > biggest_distance)
        {
            end_room_index = room_index;
            biggest_distance = distance;
        }
    }
    
    for(;;)
    {
        v2u end_pos = rand_rect_pos(game, rooms->array[end_room_index]);
        if(is_tile_traversable(dungeon->tiles, end_pos))
        {
#if 0
            // Place player at end of level.
            move_entity(dungeon->tiles, end_pos, player);
#endif
            
            set_tile_id(dungeon->tiles, end_pos, TileID_StonePathDown);
            break;
        }
    }
    
#if 1
    // Place Enemies
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
    printf("dungeon->level: %u\n", dungeon->level);
    printf("range_min: %u\n", range_min);
    printf("range_max: %u\n", range_max);
#endif
    
    RoomIndex player_room = get_room_index(rooms, player->pos);
    assert(player_room.found);
    
    for(u32 count = 0; count < (dungeon->w + dungeon->h) / 6; ++count)
    {
        for(;;)
        {
            EntityID enemy_id = random_number(&game->random,
                                              EntityID_None + 1,
                                              EntityID_Count - 1);
            
            if(enemy_levels[enemy_id] >= range_min &&
               enemy_levels[enemy_id] <= range_max)
            {
                v2u enemy_pos = random_dungeon_pos(game, dungeon);
                if(!is_inside_room(rooms->array[player_room.index], enemy_pos))
                {
                    if(is_tile_traversable(dungeon->tiles, enemy_pos))
                    {
                        add_enemy_entity(entities, dungeon->tiles, enemy_levels, enemy_id, enemy_pos.x, enemy_pos.y);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 1
    // Place Items
    // TODO(rami): How many items do we want to place?
    for(u32 item_count = 0; item_count < (dungeon->w + dungeon->h) / 8; ++item_count)
    {
        for(;;)
        {
            // TODO(rami): Add curse chance.
            // TODO(rami): Do we want to limit the amount of items that can be
            // inside of each room?
            v2u item_pos = random_dungeon_pos(game, dungeon);
            
            if(!is_tile_travel(dungeon->tiles, item_pos) &&
               is_tile_traversable_and_not_occupied(dungeon->tiles, item_pos))
            {
                ItemType type = ItemType_None;
                
                u32 type_chance = random_number(&game->random, 1, 100);
                if(type_chance <= 35) type = ItemType_Weapon;
                else if(type_chance <= 70) type = ItemType_Armour;
                else if(type_chance <= 85) type = ItemType_Potion;
                else if(type_chance <= 100) type = ItemType_Scroll;
                
                assert(type);
                
                if(type == ItemType_Weapon)
                {
                    ItemRarity rarity = ItemRarity_Common;
                    
                    if(dungeon->level >= 4)
                    {
                        b32 is_mythical = false;
                        u32 rarity_chance = random_number(&game->random, 1, 100);
                        
                        if(dungeon->level >= 8)
                        {
                            if(rarity_chance <= 10)
                            {
                                is_mythical = true;
                                rarity = ItemRarity_Mythical;
                            }
                        }
                        
                        if(!is_mythical)
                        {
                            if(rarity_chance <= 20)
                            {
                                rarity = ItemRarity_Magical;
                            }
                        }
                    }
                    
                    assert(rarity);
                    ItemID weapon_id = random_weapon(&game->random);
                    add_weapon_item(game, items, weapon_id, rarity, item_pos.x, item_pos.y);
                }
                else if(type == ItemType_Armour)
                {
                    ItemID armour_id = random_leather_armour(&game->random);
                    
                    if(dungeon->level >= 4)
                    {
                        u32 steel_armour_chance = random_number(&game->random, 1, 100);
                        if(steel_armour_chance <= 50)
                        {
                            armour_id = random_steel_armour(&game->random);
                        }
                    }
                    
                    assert((armour_id > ItemID_ArmourStart) && (armour_id < ItemID_ArmourEnd));
                    add_armour_item(game, items, armour_id, item_pos.x, item_pos.y);
                }
                else if(type == ItemType_Potion)
                {
                    ItemID potion_id = ItemID_None;
                    u32 break_value = random_number(&game->random, 1, 100);
                    u32 counter = 0;
                    
                    for(;;)
                    {
                        potion_id = random_potion(&game->random);
                        u32 index = potion_chance_index(potion_id);
                        
                        counter += consumable_data->scroll_spawn_chances[index];
                        if(counter >= break_value)
                        {
                            break;
                        }
                    }
                    
                    assert((potion_id > ItemID_PotionStart) && (potion_id < ItemID_PotionEnd));
                    add_consumable_item(items, &game->random, consumable_data, potion_id, item_pos.x, item_pos.y);
                }
                else if(type == ItemType_Scroll)
                {
                    ItemID scroll_id = ItemID_None;
                    u32 break_value = random_number(&game->random, 1, 100);
                    u32 counter = 0;
                    
                    for(;;)
                    {
                        scroll_id = random_scroll(&game->random);
                        u32 index = scroll_chance_index(scroll_id);
                        
                        counter += consumable_data->scroll_spawn_chances[index];
                        if(counter >= break_value)
                        {
                            break;
                        }
                    }
                    
                    assert((scroll_id > ItemID_ScrollStart) && (scroll_id < ItemID_ScrollEnd));
                    add_consumable_item(items, &game->random, consumable_data, scroll_id, item_pos.x, item_pos.y);
                }
                
                break;
            }
        }
    }
#endif
}