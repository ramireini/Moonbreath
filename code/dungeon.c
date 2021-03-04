internal b32
is_tile_trap(Dungeon *dungeon, v2u pos)
{
    b32 result = false;
    
    for(u32 trap_index = 0; trap_index < dungeon->trap_count; ++trap_index)
    {
        Trap *trap = &dungeon->traps[trap_index];
        if(trap->type && is_v2u_equal(trap->pos, pos))
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal u32
get_random_with_chances(Random *random, u32 *chances, u32 random_start, u32 random_end, RandomChanceType random_chance_type)
{
    u32 result = 0;
    u32 counter = 0;
    
    for(;;)
    {
        switch(random_chance_type)
        {
            case RandomChanceType_Normal:
            {
                result = get_random_number(random, random_start, random_end);
                counter += chances[result];
            } break;
            
            case RandomChanceType_ItemType:
            {
                result = random_item_type(random);
                
                u32 index = item_type_chance_index(result);
                counter += chances[index];
            } break;
            
            case RandomChanceType_Potion:
            {
                result = random_potion(random);
                
                u32 index = potion_chance_index(result);
                counter += chances[index];
            } break;
            
            case RandomChanceType_Scroll:
            {
                result = random_scroll(random);
                
                u32 index = scroll_chance_index(result);
                counter += chances[index];
            } break;
            
            invalid_default_case;
        }
        
        if(counter >= 100)
        {
            break;
        }
    }
    
    assert(result);
    return(result);
}

internal char *
get_tile_name(TileID id)
{
    char *result = "";
    
    switch(id)
    {
        case TileID_StoneWall1:
        case TileID_StoneWall2:
        case TileID_StoneWall3:
        case TileID_StoneWall4:
        case TileID_StoneWall5:
        
        case TileID_StoneWallTorch1:
        case TileID_StoneWallTorch2:
        case TileID_StoneWallTorch3:
        case TileID_StoneWallTorch4:
        case TileID_StoneWallTorch5:
        
        case TileID_StoneWallGrate1:
        case TileID_StoneWallGrate2:
        
        case TileID_StoneWallVines1:
        case TileID_StoneWallVines2:
        case TileID_StoneWallVines3:
        case TileID_StoneWallVines4:
        case TileID_StoneWallVines5: result = "Wall"; break;
        
        case TileID_StoneFloor1:
        case TileID_StoneFloor2:
        case TileID_StoneFloor3:
        case TileID_StoneFloor4:
        
        case TileID_StoneFloorGrate1:
        case TileID_StoneFloorGrate2:
        case TileID_StoneFloorGrate3:
        case TileID_StoneFloorGrate4:
        case TileID_StoneFloorGrate5:
        case TileID_StoneFloorGrate6: result = "Floor"; break;
        
        case TileID_StoneDoorClosed:
        case TileID_StoneDoorOpen: result = "Door"; break;;
        
        case TileID_StoneStaircaseUp:
        case TileID_StoneStaircaseDown: result = "Staircase"; break;
        
        case TileID_ExitDungeon:  result = "Dungeon Exit"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_tile_info_text(TileID id)
{
    char *result = 0;
    
    switch(id)
    {
        case TileID_StoneWall1: break;
        case TileID_StoneWall2: break;
        case TileID_StoneWall3: break;
        case TileID_StoneWall4: break;
        case TileID_StoneWall5: break;
        
        case TileID_StoneWallTorch1: break;
        case TileID_StoneWallTorch2: break;
        case TileID_StoneWallTorch3: break;
        case TileID_StoneWallTorch4: break;
        case TileID_StoneWallTorch5: break;
        
        case TileID_StoneWallGrate1: break;
        case TileID_StoneWallGrate2: break;
        
        case TileID_StoneWallVines1: break;
        case TileID_StoneWallVines2: break;
        case TileID_StoneWallVines3: break;
        case TileID_StoneWallVines4: break;
        case TileID_StoneWallVines5: break;
        
        case TileID_StoneFloor1: break;
        case TileID_StoneFloor2: break;
        case TileID_StoneFloor3: break;
        case TileID_StoneFloor4: break;
        
        case TileID_StoneFloorGrate1: break;
        case TileID_StoneFloorGrate2: break;
        case TileID_StoneFloorGrate3: break;
        case TileID_StoneFloorGrate4: break;
        case TileID_StoneFloorGrate5: break;
        case TileID_StoneFloorGrate6: break;
        
        case TileID_StoneDoorClosed: break;
        case TileID_StoneDoorOpen: break;
        
        case TileID_StoneStaircaseUp:
        case TileID_StoneStaircaseDown: result = "A pathway leading somewhere."; break;
        
        case TileID_ExitDungeon:  result = "A way out from the dungeon."; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal TileID
get_pos_tile_id(Tiles tiles, v2u pos)
{
    TileID result = tiles.array[(pos.y * tiles.width) + pos.x].id;
    return(result);
}

internal b32
is_tile_traversable_and_has_been_seen(Tiles tiles, v2u pos)
{
    b32 result = (is_tile_traversable(tiles, pos) &&
                  has_tile_been_seen(tiles, pos));
    
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
    b32 result = false;
    
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            if(is_tile_traversable_and_has_not_been_seen(dungeon->tiles, make_v2u(x, y)))
            {
                result = true;
                break;
            }
        }
    }
    
    return(result);
}

internal v4u
get_dimension_rect(Dungeon *dungeon, v2u pos, u32 dimension)
{
    assert(dimension);
    
    v4u result =
    {
        pos.x - dimension,
        pos.y - dimension,
        dimension * 2,
        dimension * 2
    };
    
    if((s32)result.x < 0)
    {
        result.x = 0;
    }
    else if(result.x > dungeon->width - 1)
    {
        result.x = dungeon->width - 1;
    }
    
    if((s32)result.y < 0)
    {
        result.y = 0;
    }
    else if(result.y > dungeon->height - 1)
    {
        result.y = dungeon->height - 1;
    }
    
    return(result);
}

internal b32
is_tile_traversable_and_valid_for_passage(Dungeon *dungeon, v2u pos)
{
    b32 result = is_tile_traversable(dungeon->tiles, pos);
    if(result)
    {
        v4u rect = get_dimension_rect(dungeon, pos, dungeon->player_distance_from_passage);
        
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
get_random_dungeon_pos(Random *random, Dungeon *dungeon)
{
    // Doesn't return a position on the edge of the dungeon.
    v2u result =
    {
        get_random_number(random, 1, dungeon->width - 2),
        get_random_number(random, 1, dungeon->height - 2)
    };
    
    return(result);
}

internal v2u
get_random_rect_pos(Random *random, v4u rect)
{
    v2u result =
    {
        result.x = get_random_number(random, rect.x, (rect.x + rect.w) - 1),
        result.y = get_random_number(random, rect.y, (rect.y + rect.h) - 1)
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
get_tileset_pos_from_tile_id(TileID id)
{
    v2u result = {0};
    
    switch(id)
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
        
        case TileID_StatusMark: result = make_v2u(8, 15); break;
        case TileID_QuestionMark: result = make_v2u(7, 15); break;
        
        case TileID_FootstepsUp: result = make_v2u(14, 3); break;
        case TileID_FootstepsDown: result = make_v2u(15, 3); break;
        case TileID_FootstepsLeft: result = make_v2u(16, 3); break;
        case TileID_FootstepsRight: result = make_v2u(17, 3); break;
        
        case TileID_FootstepsUpLeft: result = make_v2u(18, 3); break;
        case TileID_FootstepsUpRight: result = make_v2u(19, 3); break;
        case TileID_FootstepsDownLeft: result = make_v2u(20, 3); break;
        case TileID_FootstepsDownRight: result = make_v2u(21, 3); break;
        
        case TileID_SwordTrap: result = make_v2u(14, 4); break;
        case TileID_ArrowTrap: result = make_v2u(15, 4); break;
        case TileID_MagicTrap: result = make_v2u(16, 4); break;
        case TileID_BindTrap: result = make_v2u(17, 4); break;
        case TileID_ShaftTrap: result = make_v2u(18, 4); break;
        case TileID_SummonTrap: result = make_v2u(19, 4); break;
        case TileID_TeleportTrap: result = make_v2u(20, 4); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_tileset_pos_from_tile_pos(Tiles tiles, v2u pos)
{
    TileID tile_id = tiles.array[(pos.y * tiles.width) + pos.x].id;
    v2u result = get_tileset_pos_from_tile_id(tile_id);
    return(result);
}

internal v4u
get_tileset_rect_from_tile_id(TileID tile_id)
{
    v2u tileset_pos = get_tileset_pos_from_tile_id(tile_id);
    v4u result = get_tile_rect(tileset_pos);
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

internal u32
is_pos_valid_for_items_and_traps(v4u player_rect, Dungeon *dungeon, ItemState *items, v2u pos)
{
    b32 result = (!is_inside_rect(player_rect, pos) &&
                      is_tile_traversable(dungeon->tiles, pos) &&
                      !is_tile_passage(dungeon->tiles, pos) &&
                  !get_pos_item_count(items, pos) &&
                  !is_tile_trap(dungeon, pos));
    
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
is_inside_room(v4u room_rect, v2u pos)
{
    b32 result = (pos.x >= room_rect.x &&
                      pos.y >= room_rect.y &&
                      pos.x < (room_rect.x + room_rect.w) &&
                      pos.y < (room_rect.y + room_rect.h));
    
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
get_room_index(Room *rooms, u32 room_count, v2u pos)
{
    RoomIndex result = {0};
    
    for(u32 index = 0; index < room_count; ++index)
    {
        if(is_inside_room(rooms[index].rect, pos))
        {
            result.found = true;
            result.value = index;
            break;
        }
    }
    
    return(result);
}

internal void
set_tile_remains(Tiles tiles, v2u pos, TileID id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains_id = id;
}

internal TileID
get_tile_remains(Tiles tiles, v2u pos)
{
    TileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains_id;
    return(remains);
}

internal b32
can_place_remains_on_pos(Tiles tiles, v2u pos)
{
    b32 result = (!get_tile_remains(tiles, pos) && !is_tile_passage(tiles, pos));
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
    u32 wall = get_random_number(random, TileID_StoneWall1, TileID_StoneWall5);
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
    u32 floor = get_random_number(random, TileID_StoneFloor1, TileID_StoneFloor4);
    set_tile_id(tiles, pos, floor);
}

internal u32
get_rect_wall_count(Dungeon *dungeon, v4u rect)
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
is_rect_inside_dungeon(Dungeon *dungeon, v4u rect)
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
    b32 result = true;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_tile_wall(dungeon->tiles, make_v2u(x, y)))
            {
                result = false;
                break;
            }
        }
    }
    
    return(result);
}

internal b32
is_rect_inside_dungeon_and_wall(Dungeon *dungeon, v4u rect)
{
    b32 result = (is_rect_inside_dungeon(dungeon, rect) && is_rect_wall(dungeon, rect));
    return(result);
}

internal u32
get_neighbour_floor_count(Tiles src, v2u center_pos, v4u room)
{
    u32 floor_count = 0;
    
    for(u32 y = center_pos.y - 1; y < center_pos.y + 2; ++y)
    {
        for(u32 x = center_pos.x - 1; x < center_pos.x + 2; ++x)
        {
            v2u pos = {x, y};
            if(is_inside_room(room, pos))
            {
                // Ignore the position that we loop around
                if(pos.x != center_pos.x ||
                       pos.y != center_pos.y)
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
            u32 floor_count = get_neighbour_floor_count(src, src_pos, room);
            
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
    b32 result = true;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_tile_traversable(dungeon->tiles, make_v2u(x, y)))
            {
                result = false;
            }
        }
    }
    
    return(result);
}

internal void
place_room_from_src_to_dest(Tiles src_tiles, v4u src_room, Tiles dest_tiles)
{
    for(u32 y = 0; y < src_room.h; ++y)
    {
        for(u32 x = 0; x < src_room.w; ++x)
        {
            v2u local_pos = {x, y};
            v2u dungeon_pos = {src_room.x + x, src_room.y + y};
            
            set_tile_id(dest_tiles, dungeon_pos, get_pos_tile_id(src_tiles, local_pos));
        }
    }
}

internal void
place_room(Random *random, Tiles tiles, v4u room)
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

internal Room
create_and_place_room(Random *random, Dungeon *dungeon)
{
    Room result = {0};
    RoomType room_type = get_random_with_chances(random, dungeon->room_type_chances, RoomType_Rect, RoomType_Automaton, RandomChanceType_Normal);
    
    // Set room starting position.
    v2u random_pos = get_random_dungeon_pos(random, dungeon);
    result.rect.x = random_pos.x;
    result.rect.y = random_pos.y;
    
    // Set room dimensions based on the parameters from the dungeon struct.
    switch(room_type)
    {
        case RoomType_Rect:
        {
            result.rect.w = get_random_number(random, dungeon->rect_room_size.min, dungeon->rect_room_size.max);
            result.rect.h = get_random_number(random, dungeon->rect_room_size.min, dungeon->rect_room_size.max);
        } break;
        
        case RoomType_DoubleRect:
        {
            result.rect.w = get_random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            result.rect.h = get_random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
        } break;
        
        case RoomType_Automaton:
        {
            result.rect.w = get_random_number(random, dungeon->automaton_room_size.min, dungeon->automaton_room_size.min);
            result.rect.h = get_random_number(random, dungeon->automaton_room_size.min, dungeon->automaton_room_size.min);
        } break;
        
        invalid_default_case;
    }
    
    // Pad the rect so that there is at least a certain amount of tiles between rooms.
    v4u padded_rect = get_padded_rect(result.rect, 1);
    
#if 0
    print_v2u("random_pos", random_pos);
    print_v4u("result.rect", result.rect);
    print_v4u("padded_rect", padded_rect);
#endif
    
    // Set the room based on room type.
    if(is_rect_inside_dungeon_and_wall(dungeon, padded_rect))
    {
        if(room_type == RoomType_Rect)
        {
            place_room(random, dungeon->tiles, result.rect);
            result.success = true;
        }
        else if(room_type == RoomType_DoubleRect)
        {
            v4u room_one = result.rect;
            v4u room_two = {0};
            
            room_two.w = get_random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            room_two.h = get_random_number(random, dungeon->double_rect_room_size.min, dungeon->double_rect_room_size.max);
            room_two.x = room_one.x + (room_one.w / 2);
            room_two.y = room_one.y + (room_one.h / 2);
            
#if 0
            print_v4u("room_one", room_one);
            print_v4u("room_two", room_two);
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
            
            // Place the resulting double rectangle room
            if(is_rect_inside_dungeon_and_wall(dungeon, get_padded_rect(result.rect, 1)))
            {
                place_room(random, dungeon->tiles, room_one);
                place_room(random, dungeon->tiles, room_two);
                
                result.success = true;
            }
        }
        else if(room_type == RoomType_Automaton)
        {
            // Create two buffers used to hold the current version of the automaton room.
            Tile buff_one[dungeon->automaton_room_size.max * dungeon->automaton_room_size.max];
            Tile buff_two[dungeon->automaton_room_size.max * dungeon->automaton_room_size.max];
            
            zero_struct(buff_one);
            zero_struct(buff_two);
            
            Tiles buff_one_tiles = {dungeon->automaton_room_size.max, buff_one};
            Tiles buff_two_tiles = {dungeon->automaton_room_size.max, buff_two};
            
            // Lay out the initial shape by placing floor and wall tiles randomly.
            for(u32 y = 0; y < result.rect.h; ++y)
            {
                for(u32 x = 0; x < result.rect.w; ++x)
                {
                    if(hit_random_chance(random, 55))
                    {
                        set_tile_floor(random, buff_one_tiles, make_v2u(x, y));
                    }
                    else
                    {
                        set_tile_wall(random, buff_one_tiles, make_v2u(x, y));
                    }
                }
            }
            
            // Create the final room by calling automaton_step() multiple times,
            // causing the random initial shape to coalesce into a blob we call a room.
            v4u buff_room = make_v4u(0, 0, result.rect.w, result.rect.h);
            automaton_step(random, buff_one_tiles, buff_two_tiles, buff_room);
            automaton_step(random, buff_two_tiles, buff_one_tiles, buff_room);
            automaton_step(random, buff_one_tiles, buff_two_tiles, buff_room);
            automaton_step(random, buff_two_tiles, buff_one_tiles, buff_room);
            
#if 0
            // This is to see the resulting automaton room before it is checked to be valid.
            place_room_from_src_to_dest(buff_one_tiles, result.rect, dungeon->tiles);
            //place_room_from_src_to_dest(buff_two_tiles, result.rect, dungeon->tiles);
            
            result.success = true;
            return(result);
#endif
            
            // Get the floor count of the automaton room
            u32 room_floor_count = 0;
            for(u32 y = 0; y < result.rect.h; ++y)
            {
                for(u32 x = 0; x < result.rect.w; ++x)
                {
                    if(is_tile_floor(buff_one_tiles, make_v2u(x, y)))
                    {
                        ++room_floor_count;
                    }
                }
            }
            
            // The automaton room has to be at least a certain percentage of the original area.
            if((f32)room_floor_count / (f32)get_rect_area(result.rect) >= 0.25f)
            {
                // Place the resulting automaton room
                place_room_from_src_to_dest(buff_one_tiles, result.rect, dungeon->tiles);
                
                // The automaton room is now completely different when compared to what it
                // started from. We want the room rectangle to be as accurate as possible
                // which is why the below code gets us the minimum size rectangle of the
                // automaton room.
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
                
                result.rect = new_room_rect;
                result.success = true;
                
#if 0
                printf("Room floor count: %u\n", room_floor_count);
                printf("Room rect area: %u\n", get_rect_area(result.rect));
                print_v4u("Starting rect", result.rect);
                print_v4u("Ending rect", new_room_rect);
#endif
                
            }
        }
    }
    
    return(result);
}

internal void
create_dungeon(Game *game,
               Entity *player,
               EntityState *entities,
               Dungeon *dungeon,
               ItemState *items,
               Inventory *inventory,
               UI *ui)
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
    
    dungeon->tiles.width = dungeon->width;
    dungeon->pathfind.width = dungeon->width;
    dungeon->has_corridors = false;
    
    dungeon->cursed_item_chance = 5;
    
    dungeon->room_type_chances[RoomType_Rect] = 20;
    dungeon->room_type_chances[RoomType_DoubleRect] = 20;
    dungeon->room_type_chances[RoomType_Automaton] = 80;
    
    dungeon->corridor_type_chances[CorridorType_Turn] = 30;
    dungeon->corridor_type_chances[CorridorType_Zigzag] = 30;
    dungeon->corridor_type_chances[CorridorType_Diagonal] = 30;
    
    dungeon->enemy_count = (dungeon->width + dungeon->height) * 0.15f;
    dungeon->max_room_enemy_count = 2;
    dungeon->player_distance_from_enemy = 12;
    
    dungeon->item_count = (dungeon->width + dungeon->height) * 0.20f;
    dungeon->max_room_item_count = get_random_number(&game->random, 2, 3);
    dungeon->player_distance_from_item = 12;
    
    dungeon->up_passage_count = dungeon->down_passage_count;
    dungeon->down_passage_count = get_random_number(&game->random, 1, 3);
    dungeon->player_distance_from_passage = 12;
    
    // There has to be at least one up passage in the dungeon level.
    if(!dungeon->up_passage_count)
    {
        dungeon->up_passage_count = 1;
    }
    
    dungeon->trap_count = 32;
    dungeon->player_distance_from_trap = 12;
    
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
    
    #if 0
    printf("Enemy Count: %u\n", dungeon->enemy_count);
    printf("Item Count: %u\n", dungeon->item_count);
    printf("Up Passage Count: %u\n", dungeon->up_passage_count);
    printf("Down Passage Count: %u\n", dungeon->down_passage_count);
    printf("Min Tiles Between Passages: %u\n\n", dungeon->min_tiles_between_passages);
#endif
    
    { // Reset dungeon data
        for(u32 y = 0; y < MAX_DUNGEON_SIZE; ++y)
        {
            for(u32 x = 0; x < MAX_DUNGEON_SIZE; ++x)
            {
                v2u pos = {x, y};
                set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
                set_tile_occupied(dungeon->tiles, pos, false);
                set_tile_wall(&game->random, dungeon->tiles, pos);
            }
        }
        
        dungeon->room_count = 0;
        zero_struct(dungeon->rooms);
        zero_struct(dungeon->passages);
        
        for(u32 index = 0; index < EntityID_Count; ++index)
        {
            Entity *entity = &entities->array[index];
            if(entity->id != EntityID_Player)
            {
                remove_entity(entity);
            }
        }
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            if(is_item_valid_and_not_in_inventory(item))
            {
                zero_struct(*item);
            }
        }
        
        for(u32 trap_index = 0; trap_index < dungeon->trap_count; ++trap_index)
        {
            Trap *trap = &dungeon->traps[trap_index];
            zero_struct(*trap);
        }
    }
    
    assert(dungeon->width <= MAX_DUNGEON_SIZE);
    assert(dungeon->height <= MAX_DUNGEON_SIZE);
    
    assert(!dungeon->room_count);
    
    assert(dungeon->player_distance_from_enemy);
    assert(dungeon->player_distance_from_item);
    assert(dungeon->player_distance_from_passage);
    assert(dungeon->player_distance_from_trap);
    
    #if 0
    // Create a test room
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            if(!x || x == (dungeon->width - 1) ||
               !y || y == (dungeon->height - 1))
            {
                set_tile_wall(&game->random, dungeon->tiles, make_v2u(x, y));
            }
            else
            {
                set_tile_floor(&game->random, dungeon->tiles, make_v2u(x, y));
            }
        }
    }
    
    move_entity(player, dungeon, make_v2u(8, 1));
    
    add_enemy_entity(entities, dungeon->tiles, EntityID_Python, 7, 1);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_OrcWarrior, 7, 1);
    
    //add_enemy_entity(entities, dungeon->tiles, EntityID_SkeletonMage, 6, 1);
    
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Dummy, 5, 5);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_ShadowWalker, 5, 5);
    
    //add_enemy_entity(entities, dungeon->tiles, EntityID_KoboldShaman, 5, 5);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_OrcWarrior, 6, 5);
    
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Rat, 7, 1);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Bat, 15, 5);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Dummy, 6, 7);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_KoboldShaman, 5, 7);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Python, 5, 5);
    
#if 0
    // Test entities
    u32 entity_x_start = 15;
    u32 entity_x = entity_x_start;
    u32 entity_y = 23;
    
    for(u32 id = ENEMY_START_ID; id < ENEMY_END_ID; ++id)
    {
        add_enemy_entity(entities, dungeon->tiles, id, entity_x, entity_y);
        ++entity_x;
    }
    
    #if 0
    entity_x = entity_x_start;
    
    // Test entity remains
    for(u32 id = ENEMY_START_ID; id < ENEMY_END_ID; ++id)
    {
        add_enemy_entity(entities, dungeon->tiles, id, entity_x, entity_y + 1);
        ++entity_x;
    }
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        if(is_entity_valid_and_not_player(entity->type))
        {
            if(entity->pos.y == entity_y + 1)
            {
                kill_entity(&game->random, entity, dungeon, ui);
            }
        }
    }
    #endif
#endif
    
#if 1
    // Test Items
    v2u weapon = {8, 1};
    
    for(ItemID weapon_id = ItemID_WeaponStart + 1; weapon_id < ItemID_WeaponEnd; ++weapon_id)
    {
        add_weapon_item(&game->random, items, weapon_id, ItemRarity_Common, weapon.x + 1, weapon.y, false);
        add_weapon_item(&game->random, items, weapon_id, ItemRarity_Magical, weapon.x + 2, weapon.y, false);
        add_weapon_item(&game->random, items, weapon_id, ItemRarity_Mythical, weapon.x + 3, weapon.y, false);
        
        ++weapon.y;
    }
    
    v2u armor = {weapon.x + 4, 1};
    
    add_armor_item(&game->random, items, ItemID_LeatherHelmet, armor.x + 1, armor.y, false);
    add_armor_item(&game->random, items, ItemID_LeatherChestplate, armor.x + 2, armor.y, false);
    add_armor_item(&game->random, items, ItemID_LeatherGreaves, armor.x + 3, armor.y, false);
    add_armor_item(&game->random, items, ItemID_LeatherBoots, armor.x + 4, armor.y, false);
    
    add_armor_item(&game->random, items, ItemID_SteelHelmet, armor.x + 1, armor.y + 1, false);
    add_armor_item(&game->random, items, ItemID_SteelChestplate, armor.x + 2, armor.y + 1, false);
    add_armor_item(&game->random, items, ItemID_SteelGreaves, armor.x + 3, armor.y + 1, false);
    add_armor_item(&game->random, items, ItemID_SteelBoots, armor.x + 4, armor.y + 1, false);
    
    v2u potion = {armor.x + 5, 1};
    
    for(ItemID potion_id = ItemID_PotionStart + 1; potion_id < ItemID_PotionEnd; ++potion_id)
    {
        add_consumable_item(&game->random, items, potion_id, potion.x + 1, potion.y, 2);
        add_consumable_item(&game->random, items, potion_id, potion.x + 2, potion.y, 7);
        
        ++potion.y;
    }
    
    add_consumable_item(&game->random, items, ItemID_Ration, potion.x + 1, potion.y, 1);
    add_consumable_item(&game->random, items, ItemID_Ration, potion.x + 2, potion.y, 1);
    
    v2u scroll = {potion.x + 3, 1};
    
    for(ItemID scroll_id = ItemID_ScrollStart + 1; scroll_id < ItemID_ScrollEnd; ++scroll_id)
    {
        add_consumable_item(&game->random, items, scroll_id, scroll.x + 1, scroll.y, 1);
        add_consumable_item(&game->random, items, scroll_id, scroll.x + 2, scroll.y, 1);
        
        ++scroll.y;
    }
#endif
    
    return;
    
#elif 0
    
    // Leave dungeon blank
    return;
    
    #endif
    
    // Place rooms
    u32 total_room_area = 0;
    u32 dungeon_area = dungeon->width * dungeon->height;
    
    //while(dungeon->room_count < 10)
    while((f32)total_room_area / (f32)dungeon_area < 0.4f)
    {
        Room created_room = create_and_place_room(&game->random, dungeon);
        if(created_room.success)
        {
            dungeon->rooms[dungeon->room_count++] = created_room;
            total_room_area += get_rect_area(created_room.rect);
            
            assert(dungeon->room_count < MAX_DUNGEON_ROOM_COUNT);
        }
    }
    
#if 0
    printf("dungeon_area: %u\n", dungeon_area);
    printf("total_room_area: %u\n", total_room_area);
    printf("total_room_area / dungeon_area: %.02f\n\n", (f32)total_room_area / (f32)dungeon_area);
    #endif
    
#if 0
    printf("\nRoom Count: %u\n", dungeon->room_count);
    
    for(u32 room_index = 0; room_index < dungeon->room_count; ++room_index)
    {
        Room *room = &dungeon->rooms[room_index];
        
        printf("rooms[%u].x: %u\n", room_index, room->rect.x);
        printf("rooms[%u].y: %u\n", room_index, room->rect.y);
        printf("rooms[%u].w: %u\n", room_index, room->rect.w);
        printf("rooms[%u].h: %u\n\n", room_index, room->rect.h);
    }
#endif
    
#if 0
    // Place Corridors
    dungeon->has_corridors = true;
    
    b32 is_connected[dungeon->room_count];
    zero_struct(is_connected);
    
    // Find the room positions to connect.
    for(u32 start_index = 0; start_index < (dungeon->room_count - 1); ++start_index)
    {
        RoomIndex end_room_index = {0};
        u32 best_distance = U32_MAX;
        
        for(u32 end_index = 0; end_index < dungeon->room_count; ++end_index)
        {
            if((start_index != end_index) && !is_connected[end_index])
            {
                v2u start_pos = get_rect_center(dungeon->rooms[start_index].rect);
                v2u end_pos = get_rect_center(dungeon->rooms[end_index].rect);
                
                u32 new_distance = get_cardinal_distance(start_pos, end_pos);
                if(new_distance < best_distance)
                {
                    best_distance = new_distance;
                    
                    end_room_index.found = true;
                    end_room_index.value = end_index;
                }
            }
        }
        
        // Connect the chosen positions.
        if(end_room_index.found)
        {
            for(;;)
            {
                v2u start_pos = get_random_rect_pos(&game->random, dungeon->rooms[start_index].rect);
                if(is_tile_traversable(dungeon->tiles, start_pos))
                {
                    v2u end_pos = get_random_rect_pos(&game->random, dungeon->rooms[end_room_index.value].rect);
                    if(is_tile_traversable(dungeon->tiles, end_pos))
                    {
                        CorridorType corridor_type = get_random_with_chances(&game->random, dungeon->corridor_type_chances, CorridorType_Turn, CorridorType_Diagonal, RandomChanceType_Normal);
                        
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
                        
                        switch(corridor_type)
                        {
                            case CorridorType_Turn:
                            {
                                while(start_pos.x != end_pos.x)
                                {
                                    set_tile_floor(&game->random, dungeon->tiles, start_pos);
                                    start_pos.x += x_direction;
                                }
                                
                                while(start_pos.y != end_pos.y)
                                {
                                    set_tile_floor(&game->random, dungeon->tiles, start_pos);
                                    start_pos.y += y_direction;
                                }
                            } break;
                            
                            case CorridorType_Zigzag:
                            {
                                for(;;)
                                {
                                    if(start_pos.x != end_pos.x)
                                    {
                                        set_tile_floor(&game->random, dungeon->tiles, start_pos);
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        set_tile_floor(&game->random, dungeon->tiles, start_pos);
                                        start_pos.y += y_direction;
                                    }
                                    
                                    if(is_v2u_equal(start_pos, end_pos))
                                    {
                                        break;
                                    }
                                }
                            } break;
                            
                            case CorridorType_Diagonal:
                            {
                                for(;;)
                                {
                                    set_tile_floor(&game->random, dungeon->tiles, start_pos);
                                    
                                    if(start_pos.x != end_pos.x)
                                    {
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        start_pos.y += y_direction;
                                    }
                                    
                                    if(is_v2u_equal(start_pos, end_pos))
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
        zero_struct(fill_tiles);
        
        u32 room_index = get_random_number(&game->random, 0, dungeon->room_count - 1);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = get_random_rect_pos(&game->random, dungeon->rooms[room_index].rect);
            if(is_tile_traversable(dungeon->tiles, room_pos))
            {
                break;
            }
        }
        
        u32 flood_fill_count = flood_fill(dungeon->tiles, (b32 *)fill_tiles, 0, room_pos);
        u32 flood_fill_start_room_area = get_rect_area(dungeon->rooms[room_index].rect);
        
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
    
    set_not_flood_filled_tiles_as_wall(&game->random, dungeon->tiles, make_v4u(0, 0, dungeon->width, dungeon->height), fill_tiles);
#endif
    
#if 0
    // Place Details
    for(u32 index = 0; index < (f32)(dungeon->width * dungeon->height) * 0.02f; ++index)
    {
        for(;;)
        {
            v2u current = get_random_dungeon_pos(&game->random, dungeon);
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
                    TileID id = get_random_number(&game->random, TileID_StoneWallTorch1, TileID_StoneWallVines5);
                    set_tile_id(dungeon->tiles, current, id);
                    break;
                }
            }
        }
    }
#endif
    
#if 0
    // Place Doors
    for(u32 index = 0; index < (f32)(dungeon->width * dungeon->height) * 0.5f; ++index)
    {
        v2u current = get_random_dungeon_pos(&game->random, dungeon);
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
    
#if 0
    // Place Up Passages
    for(u32 count = 0; count < dungeon->up_passage_count; ++count)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(&game->random, dungeon);
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
                
                //printf("Up Passage Set: %u, %u\n", pos.x, pos.y);
                
                add_passage(dungeon->passages, pos, PassageType_Up);
                break;
            }
        }
    }
    
    // Place Player
    move_entity(player, dungeon, dungeon->passages[0].pos);
    
    if(dungeon->level == 1)
    {
        add_player_starting_item(game, player, items, inventory, ui, ItemID_Sword, player->pos.x, player->pos.y);
        add_player_starting_item(game, player, items, inventory, ui, ItemID_MightPotion, player->pos.x, player->pos.y);
    }
#endif
    
    #if 0
    // Place Down Passages
    for(u32 count = 0; count < dungeon->down_passage_count; ++count)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(&game->random, dungeon);
            if(is_tile_traversable_and_valid_for_passage(dungeon, pos))
            {
                //printf("Down Passage Set: %u, %u\n", pos.x, pos.y);
                
                set_tile_id(dungeon->tiles, pos, TileID_StoneStaircaseDown);
                add_passage(dungeon->passages, pos, PassageType_Down);
                break;
            }
        }
    }
    #endif
    
    #if 0
    // Place Enemies
    u32 level_min = dungeon->level - 1;
    if(!level_min)
    {
        level_min = 1;
    }
    
    u32 level_max = dungeon->level + 1;
    if(level_max > MAX_DUNGEON_LEVEL)
    {
        level_max = MAX_DUNGEON_LEVEL;
    }
    
    for(u32 enemy_index = 0; enemy_index < dungeon->enemy_count; ++enemy_index)
    {
        for(;;)
        {
            // Get a random enemy and see if its level would fit this dungeon level.
            EntityID enemy_id = get_random_number(&game->random, ENEMY_START_ID, ENEMY_END_ID);
            
            if(entities->levels[enemy_id] >= level_min &&
                   entities->levels[enemy_id] <= level_max)
            {
                // Get a position and only accept it if it's far away enough from the player.
                // We don't want enemies to be placed too close to the player, which would feel unfair.
                v2u pos = get_random_dungeon_pos(&game->random, dungeon);
                v4u player_rect = get_dimension_rect(dungeon, player->pos, dungeon->player_distance_from_enemy);
                
                if(!is_inside_rect(player_rect, pos) &&
                   is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
                {
                    // If the enemy is going to be set in a dungeon room,
                    // only place the enemy if the room has space for it.
                    RoomIndex room_index = get_room_index(dungeon->rooms, dungeon->room_count, pos);
                    if(room_index.found)
                    {
                        Room *enemy_room = &dungeon->rooms[room_index.value];
                        if(enemy_room->enemy_count <= dungeon->max_room_enemy_count)
                        {
                            ++enemy_room->enemy_count;
                            
                            add_enemy_entity(entities, dungeon->tiles, enemy_id, pos.x, pos.y);
                            break;
                        }
                    }
                    else
                    {
                        add_enemy_entity(entities, dungeon->tiles, enemy_id, pos.x, pos.y);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 0
    // Place Items
    for(u32 item_index = 0; item_index < dungeon->item_count; ++item_index)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(&game->random, dungeon);
            v4u player_rect = get_dimension_rect(dungeon, player->pos, dungeon->player_distance_from_item);
            
            if(is_pos_valid_for_items_and_traps(player_rect, dungeon, items, pos))
            {
                // The item will be placed if it's not placed in a room or if the room has
                // enough space for the item.
                b32 should_add_item = false;
                
                RoomIndex room_index = get_room_index(dungeon->rooms, dungeon->room_count, pos);
                if(room_index.found)
                {
                    Room *item_room = &dungeon->rooms[room_index.value];
                    if(item_room->item_count <= dungeon->max_room_item_count)
                    {
                        ++item_room->item_count;
                        should_add_item = true;
                    }
                }
                else
                {
                    should_add_item = true;
                }
                
                if(should_add_item)
                {
                    b32 is_item_cursed = false;
                    ItemType item_type = get_random_with_chances(&game->random, dungeon->item_type_chances, 0, 0, RandomChanceType_ItemType);
                    
                    if(is_item_equipment(item_type) &&
                       hit_random_chance(&game->random, dungeon->cursed_item_chance))
                    {
                        is_item_cursed = true;
                        
                        //printf("Cursed item at %u, %u.\n", pos.x, pos.y);
                        }
                    
                    switch(item_type)
                    {
                        case ItemType_Weapon:
                        {
                            ItemRarity rarity = ItemRarity_Common;
                            
                            if(dungeon->level >= 4)
                            {
                                b32 is_mythical = false;
                                u32 rarity_chance = get_random_chance(&game->random);
                                
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
                            add_weapon_item(&game->random, items, weapon_id, rarity, pos.x, pos.y, is_item_cursed);
                        } break;
                        
                        case ItemType_Armor:
                        {
                            ItemID armor_id = random_leather_armor(&game->random);
                            
                            if(dungeon->level >= 4)
                            {
                                if(hit_random_chance(&game->random, 50))
                                {
                                    armor_id = random_steel_armor(&game->random);
                                }
                            }
                            
                            assert((armor_id > ItemID_ArmorStart) && (armor_id < ItemID_ArmorEnd));
                            add_armor_item(&game->random, items, armor_id, pos.x, pos.y, is_item_cursed);
                        } break;
                        
                        case ItemType_Potion:
                        {
                            ItemID potion_id = get_random_with_chances(&game->random, dungeon->potion_chances, 0, 0, RandomChanceType_Potion);
                            assert((potion_id > ItemID_PotionStart) && (potion_id < ItemID_PotionEnd));
                            
                            add_consumable_item(&game->random, items, potion_id, pos.x, pos.y, 1);
                        } break;
                        
                        case ItemType_Scroll:
                        {
                            ItemID scroll_id = get_random_with_chances(&game->random, dungeon->scroll_chances, 0, 0, RandomChanceType_Scroll);
                            assert((scroll_id > ItemID_ScrollStart) && (scroll_id < ItemID_ScrollEnd));
                            
                            add_consumable_item(&game->random, items, scroll_id, pos.x, pos.y, 1);
                        } break;
                        
                        case ItemType_Ration:
                        {
                            add_consumable_item(&game->random, items, ItemID_Ration, pos.x, pos.y, 1);
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
#endif
    
    // TODO(rami): Next make traps actually function.
    
#if 0
    // Place Traps
    for(u32 trap_index = 0; trap_index < dungeon->trap_count; ++trap_index)
    {
        Trap *trap = &dungeon->traps[trap_index];
        
        for(;;)
        {
            v4u player_rect = get_dimension_rect(dungeon, player->pos, dungeon->player_distance_from_trap);
            v2u pos = get_random_dungeon_pos(&game->random, dungeon);
            
            if(is_pos_valid_for_items_and_traps(player_rect, dungeon, items, pos))
            {
                trap->pos = pos;
                trap->type = get_random_number(&game->random, TrapType_None + 1, TrapType_Count - 1);
                
                switch(trap->type)
                {
                    case TrapType_Sword: trap->tile_src = get_tileset_rect_from_tile_id(TileID_SwordTrap); break;
                    case TrapType_Arrow: trap->tile_src = get_tileset_rect_from_tile_id(TileID_ArrowTrap); break;
                    case TrapType_Magic: trap->tile_src = get_tileset_rect_from_tile_id(TileID_MagicTrap); break;
                    case TrapType_Bind: trap->tile_src = get_tileset_rect_from_tile_id(TileID_BindTrap); break;
                    case TrapType_Shaft: trap->tile_src = get_tileset_rect_from_tile_id(TileID_ShaftTrap); break;
                    case TrapType_Summon: trap->tile_src = get_tileset_rect_from_tile_id(TileID_SummonTrap); break;
                    case TrapType_Teleport: trap->tile_src = get_tileset_rect_from_tile_id(TileID_TeleportTrap); break;
                    
                    invalid_default_case;
                }
                
                break;
                }
        }
        
#if 1
        printf("\nTrap Count: %u\n", dungeon->trap_count);
        
        for(u32 trap_index = 0; trap_index < dungeon->trap_count; ++trap_index)
        {
            Trap *trap = &dungeon->traps[trap_index];
            
            printf("traps[%u].type: %u\n", trap_index, trap->type);
            printf("traps[%u].pos: %u, %u\n\n", trap_index, trap->pos.x, trap->pos.y);
        }
#endif
    }
    #endif
}