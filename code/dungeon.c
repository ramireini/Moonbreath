internal b32
is_passage_type_and_has_been_seen(Tiles tiles, Passage *passage, PassageType type)
{
    b32 result = (passage->type == type &&
                  has_tile_been_seen(tiles, passage->pos));
    
    return(result);
}

internal b32
is_down_passage_and_has_been_seen(Tiles tiles, Passage *passage)
{
    b32 result = (passage->type == PassageType_Down &&
                  has_tile_been_seen(tiles, passage->pos));
    
    return(result);
}

internal b32
is_tile_traversable_and_has_not_been_seen(Tiles tiles, v2u pos)
{
    b32 result = (is_tile_traversable(tiles, pos) &&
                  !has_tile_been_seen(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_explorable(Dungeon *dungeon)
{
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            if(is_tile_traversable_and_has_not_been_seen(dungeon->tiles, make_v2u(x, y)))
            {
                return(true);
            }
        }
    }
    
    return(false);
}

internal v4u
get_dimension_rect(Dungeon *dungeon, v2u pos, u32 dimension)
{
    v4u result =
    {
        pos.x - dimension,
        pos.y - dimension,
        dimension * 2,
        dimension * 2
    };
    
    // Clamping, note that whenever x or y are less than 0, the amount of
    // lost tiles will be added to the w and h respectively. This is just
    // so that the total area of the rect isn't being cut.
    u32 width_pos = result.x + result.w;
    if((s32)result.x < 0)
    {
        result.x = 0;
    }
    else if(width_pos >= dungeon->width)
    {
        u32 difference = (width_pos - dungeon->width) + 1;
        result.w -= difference;
    }
    
    u32 height_pos = result.y + result.h;
    if((s32)result.y < 0)
    {
        result.y = 0;
    }
    else if(height_pos >= dungeon->height)
    {
        u32 difference = (height_pos - dungeon->height) + 1;
        result.h -= difference;
    }
    
    return(result);
}

internal b32
is_tile_traversable_and_valid_for_passage(Dungeon *dungeon, v2u pos)
{
    b32 result = is_tile_traversable(dungeon->tiles, pos);
    if(result)
    {
        v4u rect = get_dimension_rect(dungeon, pos, dungeon->min_distance_between_passages);
        
        for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
        {
            if(dungeon->passages[index].type &&
               is_inside_rect(rect, dungeon->passages[index].pos))
            {
                result = false;
                break;
            }
        }
    }
    
    return(result);
}

internal v2u
random_dungeon_pos(Random *random, Dungeon *dungeon)
{
    // Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        random_number(random, 1, dungeon->width - 2),
        random_number(random, 1, dungeon->height - 2)
    };
    
    return(result);
}

internal v2u
random_rect_pos(Random *random, v4u rect)
{
    v2u result =
    {
        result.x = random_number(random, rect.x, (rect.x + rect.w) - 1),
        result.y = random_number(random, rect.y, (rect.y + rect.h) - 1)
    };
    
    return(result);
}

internal void
add_passage(Passage *passages, v2u pos, PassageType type)
{
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        if(!passages[index].type)
        {
            passages[index].type = type;
            passages[index].pos = pos;
            return;
        }
    }
    
    assert(0);
}

internal v2u
get_dungeon_remains_tile_pos(Tiles tiles, v2u pos)
{
    v2u result = {0};
    
    switch(tiles.array[(pos.y * tiles.width) + pos.x].remains_id)
    {
        case TileID_RedBlood1: result = make_v2u(33, 0); break;
        case TileID_RedBlood2: result = make_v2u(34, 0); break;
        case TileID_RedBlood3: result = make_v2u(35, 0); break;
        case TileID_RedBlood4: result = make_v2u(36, 0); break;
        case TileID_RedBlood5: result = make_v2u(37, 0); break;
        case TileID_RedBlood6: result = make_v2u(38, 0); break;
        case TileID_RedBlood7: result = make_v2u(39, 0); break;
        
        case TileID_GreenBlood1: result = make_v2u(40, 0); break;
        case TileID_GreenBlood2: result = make_v2u(41, 0); break;
        case TileID_GreenBlood3: result = make_v2u(42, 0); break;
        case TileID_GreenBlood4: result = make_v2u(43, 0); break;
        case TileID_GreenBlood5: result = make_v2u(44, 0); break;
        case TileID_GreenBlood6: result = make_v2u(45, 0); break;
        case TileID_GreenBlood7: result = make_v2u(46, 0); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_dungeon_tile_pos(Tiles tiles, v2u pos)
{
    v2u result = {0};
    
    switch(tiles.array[(pos.y * tiles.width) + pos.x].id)
    {
        case TileID_StoneWall1: result = make_v2u(1, 0); break;
        case TileID_StoneWall2: result = make_v2u(2, 0); break;
        case TileID_StoneWall3: result = make_v2u(3, 0); break;
        case TileID_StoneWall4: result = make_v2u(4, 0); break;
        case TileID_StoneWall5: result = make_v2u(5, 0); break;
        
        case TileID_StoneWallTorch1: result = make_v2u(6, 0); break;
        case TileID_StoneWallTorch2: result = make_v2u(7, 0); break;
        case TileID_StoneWallTorch3: result = make_v2u(8, 0); break;
        case TileID_StoneWallTorch4: result = make_v2u(9, 0); break;
        case TileID_StoneWallTorch5: result = make_v2u(10, 0); break;
        
        case TileID_StoneWallGrate1: result = make_v2u(11, 0); break;
        case TileID_StoneWallGrate2: result = make_v2u(12, 0); break;
        
        case TileID_StoneWallVines1: result = make_v2u(13, 0); break;
        case TileID_StoneWallVines2: result = make_v2u(14, 0); break;
        case TileID_StoneWallVines3: result = make_v2u(15, 0); break;
        case TileID_StoneWallVines4: result = make_v2u(16, 0); break;
        case TileID_StoneWallVines5: result = make_v2u(17, 0); break;
        
        case TileID_StoneFloor1: result = make_v2u(18, 0); break;
        case TileID_StoneFloor2: result = make_v2u(19, 0); break;
        case TileID_StoneFloor3: result = make_v2u(20, 0); break;
        case TileID_StoneFloor4: result = make_v2u(21, 0); break;
        
        case TileID_StoneFloorGrate1: result = make_v2u(22, 0); break;
        case TileID_StoneFloorGrate2: result = make_v2u(23, 0); break;
        case TileID_StoneFloorGrate3: result = make_v2u(24, 0); break;
        case TileID_StoneFloorGrate4: result = make_v2u(25, 0); break;
        case TileID_StoneFloorGrate5: result = make_v2u(26, 0); break;
        case TileID_StoneFloorGrate6: result = make_v2u(27, 0); break;
        
        case TileID_StoneDoorClosed: result = make_v2u(28, 0); break;
        case TileID_StoneDoorOpen: result = make_v2u(29, 0); break;
        
        case TileID_StoneStaircaseUp: result = make_v2u(30, 0); break;
        case TileID_StoneStaircaseDown: result = make_v2u(31, 0); break;
        case TileID_ExitDungeon: result = make_v2u(32, 0); break;
        
        case TileID_RedBlood1: result = make_v2u(33, 0); break;
        case TileID_RedBlood2: result = make_v2u(34, 0); break;
        case TileID_RedBlood3: result = make_v2u(35, 0); break;
        case TileID_RedBlood4: result = make_v2u(36, 0); break;
        case TileID_RedBlood5: result = make_v2u(37, 0); break;
        case TileID_RedBlood6: result = make_v2u(38, 0); break;
        case TileID_RedBlood7: result = make_v2u(39, 0); break;
        
        case TileID_GreenBlood1: result = make_v2u(40, 0); break;
        case TileID_GreenBlood2: result = make_v2u(41, 0); break;
        case TileID_GreenBlood3: result = make_v2u(42, 0); break;
        case TileID_GreenBlood4: result = make_v2u(43, 0); break;
        case TileID_GreenBlood5: result = make_v2u(44, 0); break;
        case TileID_GreenBlood6: result = make_v2u(45, 0); break;
        case TileID_GreenBlood7: result = make_v2u(46, 0); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal b32
is_tile_id(Tiles tiles, v2u pos, TileID id)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].id == id);
    return(result);
}

internal b32
is_tile_passage(Tiles tiles, v2u pos)
{
    b32 result = (is_tile_id(tiles, pos, TileID_StoneStaircaseUp) ||
                  is_tile_id(tiles, pos, TileID_StoneStaircaseDown) ||
                  is_tile_id(tiles, pos, TileID_ExitDungeon));
    
    return(result);
}

internal b32
is_tile_floor(Tiles tiles, v2u pos)
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
is_tile_traversable(Tiles tiles, v2u pos)
{
    b32 result = (is_tile_floor(tiles, pos) ||
                  is_tile_passage(tiles, pos) ||
                  is_tile_id(tiles, pos, TileID_StoneDoorOpen));
    
    return(result);
}

internal b32
is_inside_dungeon(Dungeon *dungeon, v2u pos)
{
    b32 result = (pos.x >= 0 &&
                  pos.y >= 0 &&
                  pos.x < dungeon->width &&
                  pos.y < dungeon->height);
    
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
    // Width and height need to be + 1.
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
get_room_index(Rooms *rooms, v2u pos)
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
set_tile_remains_value(Tiles tiles, v2u pos, TileID id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains_id = id;
}

internal TileID
get_tile_remains_value(Tiles tiles, v2u pos)
{
    TileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains_id;
    return(remains);
}

internal b32
can_place_remains_on_pos(Tiles tiles, v2u pos)
{
    b32 result = (!is_tile_passage(tiles, pos) &&
                  !get_tile_remains_value(tiles, pos));
    
    return(result);
}

internal RemainsSource
get_tile_remains_src(Dungeon *dungeon, v2u render_pos)
{
    RemainsSource result = {0};
    
    if(get_tile_remains_value(dungeon->tiles, render_pos))
    {
        result.found = true;
        v2u remains_pos = get_dungeon_remains_tile_pos(dungeon->tiles, render_pos);
        result.rect = get_tile_rect(remains_pos);
    }
    
    return(result);
}

internal void
set_tile_id(Tiles tiles, v2u pos, TileID id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].id = id;
}

internal void
set_tile_occupied(Tiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_occupied = value;
}

internal b32
is_tile_occupied(Tiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_occupied);
    return(result);
}

internal b32
is_tile_traversable_and_not_occupied(Tiles tiles, v2u pos)
{
    b32 result = (is_tile_traversable(tiles, pos) && !is_tile_occupied(tiles, pos));
    return(result);
}

internal void
set_tile_wall(Random *random, Tiles tiles, v2u pos)
{
    u32 wall = random_number(random, TileID_StoneWall1, TileID_StoneWall5);
    set_tile_id(tiles, pos, wall);
}

internal b32
is_tile_wall(Tiles tiles, v2u pos)
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
set_tile_floor(Random *random, Tiles tiles, v2u pos)
{
    u32 floor = random_number(random, TileID_StoneFloor1, TileID_StoneFloor4);
    set_tile_id(tiles, pos, floor);
}

internal u32
rect_wall_count(Dungeon *dungeon, v4u rect)
{
    u32 wall_count = 0;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(is_tile_wall(dungeon->tiles, make_v2u(x, y)))
            {
                ++wall_count;
            }
        }
    }
    
    return(wall_count);
}

internal b32
is_rect_in_dungeon(Dungeon *dungeon, v4u rect)
{
    b32 result = (rect.x >= 1 &&
                  rect.y >= 1 &&
                  (rect.x + rect.w) < (dungeon->width - 1) &&
                  (rect.y + rect.h) < (dungeon->height - 1));
    
    return(result);
}

internal b32
is_rect_wall(Dungeon *dungeon, v4u rect)
{
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_tile_wall(dungeon->tiles, make_v2u(x, y)))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal b32
is_rect_in_dungeon_and_wall(Dungeon *dungeon, v4u rect)
{
    b32 result = (is_rect_in_dungeon(dungeon, rect) && is_rect_wall(dungeon, rect));
    return(result);
}

internal u32
neighbour_floor_count(Tiles src, v2u start, v4u room)
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
automaton_step(Random *random, Tiles src, Tiles dest, v4u room)
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
                    set_tile_wall(random, dest, dest_pos);
                }
                else
                {
                    set_tile_floor(random, dest, dest_pos);
                }
            }
            else if(is_tile_wall(src, src_pos))
            {
                if(floor_count >= 6)
                {
                    set_tile_floor(random, dest, dest_pos);
                }
                else
                {
                    set_tile_wall(random, dest, dest_pos);
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
            if(!is_tile_traversable(dungeon->tiles, make_v2u(x, y)))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
place_rect_room(Random *random, Tiles tiles, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            set_tile_floor(random, tiles, make_v2u(x, y));
        }
    }
}

internal u32
set_not_flood_filled_tiles_as_wall(Random *random, Tiles tiles, v4u area, b32 *fill_tiles)
{
    for(u32 y = area.y; y < area.h; ++y)
    {
        for(u32 x = area.x; x < area.w; ++x)
        {
            if(!fill_tiles[(y * tiles.width) + x])
            {
                set_tile_wall(random, tiles, make_v2u(x, y));
            }
        }
    }
}

internal u32
flood_fill(Tiles tiles, b32 *fill_tiles, u32 fill_count, v2u pos)
{
    if(!fill_tiles[(pos.y * tiles.width) + pos.x] &&
       is_tile_floor(tiles, pos))
    {
        fill_tiles[(pos.y * tiles.width) + pos.x] = true;
        ++fill_count;
        
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x, pos.y + 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x - 1, pos.y));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x + 1, pos.y));
        
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x - 1, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x + 1, pos.y - 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x - 1, pos.y + 1));
        fill_count = flood_fill(tiles, fill_tiles, fill_count, make_v2u(pos.x + 1, pos.y + 1));
    }
    
    return(fill_count);
}

internal CreatedRoom
create_and_place_room(Random *random, Dungeon *dungeon)
{
    CreatedRoom result = {0};
    
    RoomType type = RoomType_None;
    u32 break_value = 100;
    u32 counter = 0;
    
    for(;;)
    {
        type = random_number(random, RoomType_Rect, RoomType_Automaton);
        
        counter += dungeon->room_type_chances[type];
        if(counter >= break_value)
        {
            break;
        }
    }
    
    switch(type)
    {
        case RoomType_Rect:
        {
            result.rect.w = random_number(random, dungeon->rect_room_size.min, dungeon->rect_room_size.max);
            result.rect.h = random_number(random, dungeon->rect_room_size.min, dungeon->rect_room_size.max);
        } break;
        
        case RoomType_DoubleRect:
        {
            result.rect.w = random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            result.rect.h = random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
        } break;
        
        case RoomType_Automaton:
        {
            result.rect.w = random_number(random, dungeon->automaton_room_size.min, dungeon->automaton_room_size.min);
            result.rect.h = random_number(random, dungeon->automaton_room_size.min, dungeon->automaton_room_size.min);
        } break;
        
        invalid_default_case;
    }
    
    v2u random_pos = random_dungeon_pos(random, dungeon);
    result.rect.x = random_pos.x;
    result.rect.y = random_pos.y;
    
    v4u padded_rect = get_padded_rect(result.rect, 1);
    
#if 0
    printf("\nrandom_pos: %u, %u\n\n", random_pos.x, random_pos.y);
    printf("result.rect.x: %u\n", result.rect.x);
    printf("result.rect.y: %u\n", result.rect.y);
    printf("result.rect.w: %u\n", result.rect.w);
    printf("result.rect.h: %u\n\n", result.rect.h);
    
    printf("padded_rect.x: %u\n", padded_rect.x);
    printf("padded_rect.y: %u\n", padded_rect.y);
    printf("padded_rect.w: %u\n", padded_rect.w);
    printf("padded_rect.h: %u\n\n", padded_rect.h);
#endif
    
    if(is_rect_in_dungeon_and_wall(dungeon, padded_rect))
    {
        if(type == RoomType_Rect)
        {
            place_rect_room(random, dungeon->tiles, result.rect);
            result.success = true;
        }
        else if(type == RoomType_DoubleRect)
        {
            v4u room_one = result.rect;
            v4u room_two = {0};
            
            room_two.w = random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            room_two.h = random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            room_two.x = room_one.x + (room_one.w / 2);
            room_two.y = room_one.y + (room_one.h / 2);
            
#if 0
            printf("\nroom_one.x: %u\n", room_one.x);
            printf("room_one.y: %u\n", room_one.y);
            printf("room_one.w: %u\n", room_one.w);
            printf("room_one.h: %u\n\n", room_one.h);
            
            printf("room_two.x: %u\n", room_two.x);
            printf("room_two.y: %u\n", room_two.y);
            printf("room_two.w: %u\n", room_two.w);
            printf("room_two.h: %u\n\n", room_two.h);
#endif
            
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
            
            if(is_rect_in_dungeon_and_wall(dungeon, get_padded_rect(result.rect, 1)))
            {
                place_rect_room(random, dungeon->tiles, room_one);
                place_rect_room(random, dungeon->tiles, room_two);
                
                result.success = true;
            }
        }
        else if(type == RoomType_Automaton)
        {
            Tile buff_one[dungeon->automaton_room_size.max * dungeon->automaton_room_size.max];
            Tile buff_two[dungeon->automaton_room_size.max * dungeon->automaton_room_size.max];
            
            memset(buff_one, 0, sizeof(buff_one));
            memset(buff_two, 0, sizeof(buff_two));
            
            Tiles buff_one_data = {dungeon->automaton_room_size.max, buff_one};
            Tiles buff_two_data = {dungeon->automaton_room_size.max, buff_two};
            
            for(u32 y = 0; y < result.rect.h; ++y)
            {
                for(u32 x = 0; x < result.rect.w; ++x)
                {
                    if(random_number(random, 1, 100) <= 55)
                    {
                        set_tile_floor(random, buff_one_data, make_v2u(x, y));
                    }
                    else
                    {
                        set_tile_wall(random, buff_one_data, make_v2u(x, y));
                    }
                }
            }
            
            v4u buff_room = make_v4u(0, 0, result.rect.w, result.rect.h);
            automaton_step(random, buff_one_data, buff_two_data, buff_room);
            automaton_step(random, buff_two_data, buff_one_data, buff_room);
            automaton_step(random, buff_one_data, buff_two_data, buff_room);
            automaton_step(random, buff_two_data, buff_one_data, buff_room);
            
#if 0
            // To see the room before any checks.
            for(u32 y = 0; y < result.rect.h; ++y)
            {
                for(u32 x = 0; x < result.rect.w; ++x)
                {
                    set_tile_id(dungeon->tiles,
                                make_v2u(result.rect.x + x, result.rect.y + y),
                                buff_one_data.array[(y * buff_one_data.width) + x].id);
                    //buff_two_data.array[(y * buff_one_data.width) + x].id);
                }
            }
            
            result.success = true;
            return(result);
#endif
            
            u32 room_floor_count = 0;
            for(u32 y = 0; y < result.rect.h; ++y)
            {
                for(u32 x = 0; x < result.rect.w; ++x)
                {
                    if(is_tile_floor(buff_one_data, make_v2u(x, y)))
                    {
                        ++room_floor_count;
                    }
                }
            }
            
#if 0
            printf("room_floor_count: %u\n", room_floor_count);
            printf("result_rect_area: %u\n", result.rect.w * result.rect.h);
#endif
            
            // Accept the room if the floor count is at least a certain percent
            // of the original area.
            if((f32)room_floor_count / (f32)(result.rect.w * result.rect.h) >= 0.25f)
            {
                // Place automaton room.
                for(u32 y = 0; y < result.rect.h; ++y)
                {
                    for(u32 x = 0; x < result.rect.w; ++x)
                    {
                        set_tile_id(dungeon->tiles,
                                    make_v2u(result.rect.x + x, result.rect.y + y),
                                    buff_one_data.array[(y * buff_one_data.width) + x].id);
                    }
                }
                
                // Calculate rectangle around the automaton room for more correct room data.
                v4u new_room_rect = {0};
                u32 highest_x = result.rect.x;
                u32 highest_y = result.rect.y;
                
                for(u32 y = result.rect.y; y < (result.rect.y + result.rect.h); ++y)
                {
                    for(u32 x = result.rect.x; x < (result.rect.x + result.rect.w); ++x)
                    {
                        if(is_tile_traversable(dungeon->tiles, make_v2u(x, y)))
                        {
                            if(!new_room_rect.x || x < new_room_rect.x)
                            {
                                new_room_rect.x = x;
                            }
                            
                            if(!new_room_rect.y || y < new_room_rect.y)
                            {
                                new_room_rect.y = y;
                            }
                            
                            if(x > highest_x)
                            {
                                highest_x = x;
                            }
                            
                            if(y > highest_y)
                            {
                                highest_y = y;
                            }
                        }
                    }
                }
                
                new_room_rect.w = (highest_x - new_room_rect.x) + 1;
                new_room_rect.h = (highest_y - new_room_rect.y) + 1;
                
#if 0
                printf("result.rect.x: %u\n", result.rect.x);
                printf("result.rect.y: %u\n", result.rect.y);
                printf("result.rect.w: %u\n", result.rect.w);
                printf("result.rect.h: %u\n\n", result.rect.h);
                
                printf("new_room_rect.x: %u\n", new_room_rect.x);
                printf("new_room_rect.y: %u\n", new_room_rect.y);
                printf("new_room_rect.w: %u\n", new_room_rect.w);
                printf("new_room_rect.h: %u\n\n", new_room_rect.h);
#endif
                
                result.rect = new_room_rect;
                result.success = true;
            }
        }
    }
    
    return(result);
}

internal void
create_dungeon(Random *random,
               Dungeon *dungeon,
               Entity *player,
               Log *log,
               Entity *entities,
               Item *items,
               Inventory *inventory,
               ItemInfo *item_info,
               u32 *entity_levels)
{
    ++dungeon->level;
    
#if 0
    dungeon->width = 128;
    dungeon->height = 128;
#elif 1
    dungeon->width = 64;
    dungeon->height = 64;
#else
    dungeon->width = 32;
    dungeon->height = 32;
#endif
    
    dungeon->pathfind_map.width = dungeon->width;
    dungeon->tiles.width = dungeon->width;
    
    dungeon->room_type_chances[RoomType_Rect] = 20;
    dungeon->room_type_chances[RoomType_DoubleRect] = 20;
    dungeon->room_type_chances[RoomType_Automaton] = 80;
    
    dungeon->corridor_type_chances[CorridorType_Turn] = 30;
    dungeon->corridor_type_chances[CorridorType_Zigzag] = 30;
    dungeon->corridor_type_chances[CorridorType_Diagonal] = 30;
    
    dungeon->enemy_count = (u32)((dungeon->width + dungeon->height) * 0.15f);
    dungeon->item_count = (u32)((dungeon->width + dungeon->height) * 0.20f);
    
#if 0
    printf("Enemy Count: %u\n", dungeon->enemy_count);
    printf("Item Count: %u\n", dungeon->item_count);
#endif
    
    dungeon->room_enemy_count = 2;
    dungeon->room_item_count = random_number(random, 2, 3);
    
    dungeon->min_distance_between_passages = 12;
    dungeon->up_passage_count = dungeon->down_passage_count;
    dungeon->down_passage_count = random_number(random, 1, 3);
    
    if(!dungeon->up_passage_count)
    {
        dungeon->up_passage_count = 1;
    }
    
#if 0
    printf("Up Passage Count: %u\n", dungeon->up_passage_count);
    printf("Down Passage Count: %u\n", dungeon->down_passage_count);
    printf("Min Distance Between Passages: %u\n\n", dungeon->min_distance_between_passages);
#endif
    
    dungeon->item_type_chances[item_type_chance_index(ItemType_Weapon)] = 25;
    dungeon->item_type_chances[item_type_chance_index(ItemType_Armor)] = 25;
    dungeon->item_type_chances[item_type_chance_index(ItemType_Potion)] = 25;
    dungeon->item_type_chances[item_type_chance_index(ItemType_Scroll)] = 25;
    dungeon->item_type_chances[item_type_chance_index(ItemType_Ration)] = 25;
    
    dungeon->potion_chances[Potion_Might] = 25;
    dungeon->potion_chances[Potion_Wisdom] = 25;
    dungeon->potion_chances[Potion_Agility] = 25;
    dungeon->potion_chances[Potion_Elusion] = 25;
    dungeon->potion_chances[Potion_Healing] = 25;
    dungeon->potion_chances[Potion_Decay] = 25;
    dungeon->potion_chances[Potion_Confusion] = 25;
    
    dungeon->scroll_chances[Scroll_Identify] = 25;
    dungeon->scroll_chances[Scroll_EnchantWeapon] = 25;
    dungeon->scroll_chances[Scroll_EnchantArmor] = 25;
    dungeon->scroll_chances[Scroll_MagicMapping] = 25;
    dungeon->scroll_chances[Scroll_Teleportation] = 25;
    
    dungeon->rect_room_size.min = 4;
    dungeon->rect_room_size.max = 8;
    dungeon->double_rect_room_size.min = 4;
    dungeon->double_rect_room_size.max = 8;
    dungeon->automaton_room_size.min = 10;
    dungeon->automaton_room_size.max = 18;
    
    assert(dungeon->width <= MAX_DUNGEON_SIZE &&
           dungeon->height <= MAX_DUNGEON_SIZE);
    
    { // Reset Dungeon Data
        for(u32 y = 0; y < MAX_DUNGEON_SIZE; ++y)
        {
            for(u32 x = 0; x < MAX_DUNGEON_SIZE; ++x)
            {
                v2u pos = {x, y};
                set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
                set_tile_occupied(dungeon->tiles, pos, false);
                set_tile_wall(random, dungeon->tiles, pos);
            }
        }
        
        memset(&dungeon->rooms, 0, sizeof(dungeon->rooms));
        memset(&dungeon->passages, 0, sizeof(dungeon->passages));
    }
    
    { // Reset Entity And Item Data
        for(u32 index = 0; index < EntityID_Count; ++index)
        {
            Entity *entity = &entities[index];
            if(entity->id != EntityID_Player)
            {
                remove_entity(entity);
            }
        }
        
        memset(items, 0, sizeof(Item) * MAX_ITEM_COUNT);
    }
    
#if 0
    // Test Room
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            if(!x || x == (dungeon->width - 1) ||
               !y || y == (dungeon->height - 1))
            {
                set_tile_wall(random, dungeon->tiles, make_v2u(x, y));
            }
            else
            {
                set_tile_floor(random, dungeon->tiles, make_v2u(x, y));
            }
        }
    }
    
    v2u seen_one = {8, 8};
    v2u seen_two = {10, 8};
    v2u unseen = {9, 14};
    
    add_passage(dungeon->passages, seen_one);
    set_tile_id(dungeon->tiles, seen_one, TileID_ExitDungeon);
    
    add_passage(dungeon->passages, seen_two);
    set_tile_id(dungeon->tiles, unseen, TileID_StoneStaircaseDown);
    
    add_passage(dungeon->passages, unseen);
    set_tile_id(dungeon->tiles, seen_two, TileID_StoneStaircaseDown);
    
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_SkeletonWarrior, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Rat, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Bat, 15, 5);
    
    move_entity(dungeon->tiles, player, make_v2u(5, 5));
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Dummy, 5, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_SkeletonWarrior, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_SkeletonArcher, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Rat, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Bat, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_SkeletonMage, 5, 7);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_KoboldShaman, 5, 7);
    //add_enemy_entity(entities, dungeon->tiles, entity_levels, EntityID_Python, 5, 7);
    
#if 0
    // Test Entities
    u32 entity_x_start = 15;
    u32 entity_x = entity_x_start;
    u32 entity_y = 23;
    
    // First row
    for(u32 index = ENEMY_START_ID; index < ENEMY_ENTITY_COUNT; ++index)
    {
        add_enemy_entity(entities,
                         dungeon->tiles,
                         entity_levels,
                         index,
                         entity_x,
                         entity_y);
        
        ++entity_x;
    }
    
    entity_x = entity_x_start;
    
    // Second row
    for(u32 index = ENEMY_START_ID; index < ENEMY_ENTITY_COUNT; ++index)
    {
        add_enemy_entity(entities,
                         dungeon->tiles,
                         entity_levels,
                         index,
                         entity_x,
                         entity_y + 1);
        
        ++entity_x;
    }
    
    // Kill second row entities
    for(u32 index = ENEMY_START_ID; index < ENEMY_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities[index];
        if(entity->pos.y == entity_y + 1)
        {
            kill_entity(random, dungeon->tiles, log, entity);
        }
    }
#endif
    
#if 1
    // Test Items
    u32 weapon_y = 1;
    for(ItemID weapon_id = ItemID_WeaponStart + 1; weapon_id < ItemID_WeaponEnd; ++weapon_id)
    {
        add_weapon_item(random, items, weapon_id, ItemRarity_Common, 8, weapon_y);
        add_weapon_item(random, items, weapon_id, ItemRarity_Magical, 9, weapon_y);
        add_weapon_item(random, items, weapon_id, ItemRarity_Mythical, 10, weapon_y);
        
        ++weapon_y;
    }
    
    add_armor_item(random, items, ItemID_LeatherHelmet, 12, 1);
    add_armor_item(random, items, ItemID_LeatherChestplate, 13, 1);
    add_armor_item(random, items, ItemID_LeatherGreaves, 14, 1);
    add_armor_item(random, items, ItemID_LeatherBoots, 15, 1);
    
    add_armor_item(random, items, ItemID_SteelHelmet, 12, 2);
    add_armor_item(random, items, ItemID_SteelChestplate, 13, 2);
    add_armor_item(random, items, ItemID_SteelGreaves, 14, 2);
    add_armor_item(random, items, ItemID_SteelBoots, 15, 2);
    
    u32 potion_y = 1;
    for(ItemID potion_id = ItemID_PotionStart + 1; potion_id < ItemID_PotionEnd; ++potion_id)
    {
        add_consumable_item(random, items, item_info, potion_id, 17, potion_y);
        add_consumable_item(random, items, item_info, potion_id, 18, potion_y);
        
        ++potion_y;
    }
    
    add_consumable_item(random, items, item_info, ItemID_Ration, 17, potion_y);
    add_consumable_item(random, items, item_info, ItemID_Ration, 18, potion_y);
    
    u32 scroll_y = 1;
    for(ItemID scroll_id = ItemID_ScrollStart + 1; scroll_id < ItemID_ScrollEnd; ++scroll_id)
    {
        add_consumable_item(random, items, item_info, scroll_id, 20, scroll_y);
        add_consumable_item(random, items, item_info, scroll_id, 21, scroll_y);
        
        ++scroll_y;
    }
#endif
    
    return;
#elif 0
    // Leave dungeon blank.
    return;
#endif
    
    Rooms *rooms = &dungeon->rooms;
    u32 dungeon_area = dungeon->width * dungeon->height;
    u32 total_room_area = 0;
    
#if 1
    while((f32)total_room_area / (f32)dungeon_area < 0.4f)
#else
    while(rooms->count < 1)
#endif
    {
        CreatedRoom room = create_and_place_room(random, dungeon);
        if(room.success)
        {
            rooms->array[rooms->count++] = room.rect;
            total_room_area += room.rect.w * room.rect.h;
            
            assert(rooms->count < MAX_DUNGEON_ROOM_COUNT);
        }
    }
    
#if 0
    printf("dungeon_area: %u\n", dungeon_area);
    printf("total_room_area: %u\n", total_room_area);
    printf("total_room_area / dungeon_area: %.02f\n", (f32)total_room_area / (f32)dungeon_area);
#endif
    
#if 0
    printf("\nRoom Count: %u\n", rooms->count);
    for(u32 index = 0; index < rooms->count; ++index)
    {
        printf("rooms[%u].x: %u\n", index, rooms->array[index].x);
        printf("rooms[%u].y: %u\n", index, rooms->array[index].y);
        printf("rooms[%u].w: %u\n", index, rooms->array[index].w);
        printf("rooms[%u].h: %u\n\n", index, rooms->array[index].h);
    }
#endif
    
#if 1
    // Place Corridors
    b32 is_connected[rooms->count];
    memset(is_connected, 0, sizeof(is_connected));
    
    // Find the room positions to connect.
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
                
                u32 new_distance = tile_dist_cardinal(start_pos, end_pos);
                if(new_distance < best_distance)
                {
                    best_distance = new_distance;
                    
                    end_room.found = true;
                    end_room.index = end_index;
                }
            }
        }
        
        // Connect the chosen positions.
        if(end_room.found)
        {
            for(;;)
            {
                v2u start_pos = random_rect_pos(random, rooms->array[start_index]);
                if(is_tile_traversable(dungeon->tiles, start_pos))
                {
                    v2u end_pos = random_rect_pos(random, rooms->array[end_room.index]);
                    if(is_tile_traversable(dungeon->tiles, end_pos))
                    {
                        CorridorType type = CorridorType_None;
                        u32 break_value = 100;
                        u32 counter = 0;
                        
                        for(;;)
                        {
                            type = random_number(random, CorridorType_Turn, CorridorType_Diagonal);
                            
                            counter += dungeon->corridor_type_chances[type];
                            if(counter >= break_value)
                            {
                                break;
                            }
                        }
                        
                        s32 x_direction = 0;
                        if(start_pos.x <= end_pos.x)
                        {
                            x_direction = 1;
                        }
                        else
                        {
                            x_direction = -1;
                        }
                        
                        s32 y_direction = 0;
                        if(start_pos.y <= end_pos.y)
                        {
                            y_direction = 1;
                        }
                        else
                        {
                            y_direction = -1;
                        }
                        
                        switch(type)
                        {
                            case CorridorType_Turn:
                            {
                                while(start_pos.x != end_pos.x)
                                {
                                    set_tile_floor(random, dungeon->tiles, start_pos);
                                    start_pos.x += x_direction;
                                }
                                
                                while(start_pos.y != end_pos.y)
                                {
                                    set_tile_floor(random, dungeon->tiles, start_pos);
                                    start_pos.y += y_direction;
                                }
                            } break;
                            
                            case CorridorType_Zigzag:
                            {
                                for(;;)
                                {
                                    if(start_pos.x != end_pos.x)
                                    {
                                        set_tile_floor(random, dungeon->tiles, start_pos);
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        set_tile_floor(random, dungeon->tiles, start_pos);
                                        start_pos.y += y_direction;
                                    }
                                    
                                    if(equal_v2u(start_pos, end_pos))
                                    {
                                        break;
                                    }
                                }
                            } break;
                            
                            case CorridorType_Diagonal:
                            {
                                for(;;)
                                {
                                    set_tile_floor(random, dungeon->tiles, start_pos);
                                    
                                    if(start_pos.x != end_pos.x)
                                    {
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        start_pos.y += y_direction;
                                    }
                                    
                                    if(equal_v2u(start_pos, end_pos))
                                    {
                                        break;
                                    }
                                }
                                
                            } break;
                            
                            invalid_default_case;
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
        
        u32 room_index = random_number(random, 0, rooms->count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = random_rect_pos(random, rooms->array[room_index]);
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
    
    set_not_flood_filled_tiles_as_wall(random, dungeon->tiles, make_v4u(0, 0, dungeon->width, dungeon->height), fill_tiles);
#endif
    
#if 1
    // Place Details
    for(u32 index = 0; index < (f32)(dungeon->width * dungeon->height) * 0.02f; ++index)
    {
        for(;;)
        {
            v2u current = random_dungeon_pos(random, dungeon);
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
                    TileID id = random_number(random, TileID_StoneWallTorch1, TileID_StoneWallVines5);
                    set_tile_id(dungeon->tiles, current, id);
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    // Place Doors
    for(u32 index = 0; index < (f32)(dungeon->width * dungeon->height) * 0.5f; ++index)
    {
        v2u current = random_dungeon_pos(random, dungeon);
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
#endif
    
#if 1
    // Place Up Passages
    for(u32 count = 0; count < dungeon->up_passage_count; ++count)
    {
        for(;;)
        {
            v2u pos = random_dungeon_pos(random, dungeon);
            if(is_tile_traversable_and_valid_for_passage(dungeon, pos))
            {
                if(dungeon->level == 1)
                {
                    set_tile_id(dungeon->tiles, pos, TileID_ExitDungeon);
                }
                else
                {
                    set_tile_id(dungeon->tiles, pos, TileID_StoneStaircaseUp);
                }
                
                //printf("Entrance Set: %u, %u\n", pos.x, pos.y);
                add_passage(dungeon->passages, pos, PassageType_Up);
                break;
            }
        }
    }
    
    // Place Player
    move_entity(dungeon->tiles, player, dungeon->passages[0].pos);
    add_player_starting_item(random, items, item_info, inventory, ItemID_Sword, player->pos.x, player->pos.y);
    add_player_starting_item(random, items, item_info, inventory, ItemID_MightPotion, player->pos.x, player->pos.y);
    
    // Place Down Passages
    for(u32 count = 0; count < dungeon->down_passage_count; ++count)
    {
        for(;;)
        {
            v2u pos = random_dungeon_pos(random, dungeon);
            if(is_tile_traversable_and_valid_for_passage(dungeon, pos))
            {
                //printf("Staircase Set: %u, %u\n", pos.x, pos.y);
                
                set_tile_id(dungeon->tiles, pos, TileID_StoneStaircaseDown);
                add_passage(dungeon->passages, pos, PassageType_Down);
                break;
            }
        }
    }
#endif
    
#if 1
    // Place Enemies
    u32 range_min = dungeon->level - 1;
    if(!range_min)
    {
        range_min = 1;
    }
    
    u32 range_max = dungeon->level + 1;
    if(range_max > MAX_DUNGEON_LEVEL)
    {
        range_max = MAX_DUNGEON_LEVEL;
    }
    
    for(u32 count = 0; count < dungeon->enemy_count; ++count)
    {
        for(;;)
        {
            EntityID enemy_id = random_number(random, ENEMY_START_ID, ENEMY_ENTITY_COUNT);
            
            if(entity_levels[enemy_id] >= range_min &&
               entity_levels[enemy_id] <= range_max)
            {
                v2u pos = random_dungeon_pos(random, dungeon);
                v4u rect = get_dimension_rect(dungeon, player->pos, 12);
                
                if(!is_inside_rect(rect, pos) &&
                   is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
                {
                    RoomIndex enemy_room = get_room_index(rooms, pos);
                    if(enemy_room.found)
                    {
                        if(rooms->enemy_count[enemy_room.index] <
                           dungeon->room_enemy_count)
                        {
                            ++rooms->enemy_count[enemy_room.index];
                            
                            add_enemy_entity(entities, dungeon->tiles, entity_levels, enemy_id, pos.x, pos.y);
                            break;
                        }
                    }
                    else
                    {
                        add_enemy_entity(entities, dungeon->tiles, entity_levels, enemy_id, pos.x, pos.y);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 1
    // Place Items
    for(u32 count = 0; count < dungeon->item_count; ++count)
    {
        u32 break_value = 100;
        u32 counter = 0;
        
        for(;;)
        {
            // TODO(rami): Add curse chance.
            v2u pos = random_dungeon_pos(random, dungeon);
            v4u rect = get_dimension_rect(dungeon, player->pos, 12);
            
            if(!is_inside_rect(rect, pos) &&
               is_tile_traversable(dungeon->tiles, pos) &&
               !is_tile_passage(dungeon->tiles, pos) &&
               !is_pos_occupied_by_item(items, pos))
            {
                b32 should_add_item = false;
                RoomIndex room = get_room_index(rooms, pos);
                if(room.found)
                {
                    if(rooms->item_count[room.index] < 
                       dungeon->room_item_count)
                    {
                        should_add_item = true;
                        ++rooms->item_count[room.index];
                    }
                }
                else
                {
                    should_add_item = true;
                }
                
                if(should_add_item)
                {
                    ItemType type = ItemType_None;
                    counter = 0;
                    
                    for(;;)
                    {
                        type = random_item_type(random);
                        u32 index = item_type_chance_index(type);
                        
                        counter += dungeon->item_type_chances[index];
                        if(counter >= break_value)
                        {
                            break;
                        }
                    }
                    
                    assert(type);
                    
                    if(type == ItemType_Weapon)
                    {
                        ItemRarity rarity = ItemRarity_Common;
                        
                        if(dungeon->level >= 4)
                        {
                            b32 is_mythical = false;
                            u32 rarity_chance = random_number(random, 1, 100);
                            
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
                        ItemID weapon_id = random_weapon(random);
                        add_weapon_item(random, items, weapon_id, rarity, pos.x, pos.y);
                    }
                    else if(type == ItemType_Armor)
                    {
                        ItemID armor_id = random_leather_armor(random);
                        
                        if(dungeon->level >= 4)
                        {
                            if(random_number(random, 1, 100) <= 50)
                            {
                                armor_id = random_steel_armor(random);
                            }
                        }
                        
                        assert((armor_id > ItemID_ArmorStart) && (armor_id < ItemID_ArmorEnd));
                        add_armor_item(random, items, armor_id, pos.x, pos.y);
                    }
                    else if(type == ItemType_Potion)
                    {
                        ItemID potion_id = ItemID_None;
                        counter = 0;
                        
                        for(;;)
                        {
                            potion_id = random_potion(random);
                            u32 index = potion_chance_index(potion_id);
                            
                            counter += dungeon->potion_chances[index];
                            if(counter >= break_value)
                            {
                                break;
                            }
                        }
                        
                        assert((potion_id > ItemID_PotionStart) && (potion_id < ItemID_PotionEnd));
                        add_consumable_item(random, items, item_info, potion_id, pos.x, pos.y);
                    }
                    else if(type == ItemType_Scroll)
                    {
                        ItemID scroll_id = ItemID_None;
                        counter = 0;
                        
                        for(;;)
                        {
                            scroll_id = random_scroll(random);
                            u32 index = scroll_chance_index(scroll_id);
                            
                            counter += dungeon->scroll_chances[index];
                            if(counter >= break_value)
                            {
                                break;
                            }
                        }
                        
                        assert((scroll_id > ItemID_ScrollStart) && (scroll_id < ItemID_ScrollEnd));
                        add_consumable_item(random, items, item_info, scroll_id, pos.x, pos.y);
                    }
                    else if(type == ItemType_Ration)
                    {
                        add_consumable_item(random, items, item_info, ItemID_Ration, pos.x, pos.y);
                    }
                    
                    break;
                }
            }
        }
    }
#endif
}