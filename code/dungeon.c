internal void
print_all_dungeon_level_occupancies(Dungeons *dungeons)
{
    for(u32 index = 0; index < MAX_DUNGEON_LEVELS; ++index)
    {
        Dungeon *dungeon = &dungeons->levels[index];
        
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                v2u pos = {x, y};
                
                if(is_dungeon_pos_occupied(dungeon->tiles, pos))
                {
                    printf("Dungeon Level %u (Index %u): %u, %u is occupied\n", index + 1, index, pos.x, pos.y);
                }
            }
        }
    }
}

//
// Dungeon passage code
//
internal b32
is_dungeon_pos_passage(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, TileID_StoneStaircaseUp) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneStaircaseDown) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_ExitDungeon));
    
    return(result);
}

internal b32
is_dungeon_passage_last(DungeonPassages *passages,
                        DungeonPassageType search_type,
                        u32 search_index)
{
    b32 result = true;
    
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &passages->array[index];
        
        if(passage->type == search_type &&
           index > search_index)
        {
            result = false;
            break;
        }
    }
    
    return(result);
}

internal DungeonPassage *
get_dungeon_passage_from_type(DungeonPassages *passages, DungeonPassageType type)
{
    DungeonPassage *result = 0;
    
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &passages->array[index];
        
        if(passage->type == type)
        {
            result = passage;
            break;
        }
    }
    
    return(result);
}

internal DungeonPassage *
get_dungeon_pos_passage(DungeonPassages *passages, v2u pos)
{
    DungeonPassage *result = 0;
    
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &passages->array[index];
        if(passage->type && is_v2u_equal(passage->pos, pos))
        {
            result = passage;
            break;
        }
    }
    
    return(result);
}

internal void
add_dungeon_passage(DungeonPassages *passages, DungeonPassage new_passage)
{
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &passages->array[index];
        if(!passage->type)
        {
            *passage = new_passage;
            return;
        }
    }
    
    assert(0);
}
//
// Dungeon passage code
//

//
// Dungeon trap code
//
internal b32
is_dungeon_pos_trap(DungeonTraps *traps, v2u pos)
{
    b32 result = false;
    
    for(u32 index = 0; index < traps->count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        assert(trap->type);
        
        if(is_v2u_equal(trap->pos, pos))
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal DungeonTrap *
get_dungeon_pos_trap(DungeonTiles tiles, DungeonTraps *traps, v2u pos)
{
    DungeonTrap *result = 0;
    
    for(u32 index = 0; index < traps->count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        assert(trap->type);
        
        if(is_v2u_equal(trap->pos, pos))
        {
            result = trap;
            break;
        }
    }
    
    return(result);
}

internal void
add_dungeon_trap(DungeonSpec *spec, DungeonTraps *traps, DungeonTrap new_trap)
{
    assert(new_trap.type);
    
    // TODO(rami): Trap summoning more than one enemy? (value min max)
    // TODO(rami): Trap teleporting you into other levels?
    // TODO(rami): Trap that has a random effect?
    
    switch(new_trap.type)
    {
        case DungeonTrapType_Spike:
        {
            new_trap.name = "Spikes";
            
            sprintf(new_trap.description,
                    "Activating these spikes will deal %u - %u physical damage.",
                    spec->spike_trap_value.min, spec->spike_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_SpikeTrap);
        } break;
        
        case DungeonTrapType_Sword:
        {
            new_trap.name = "Swords";
            
            sprintf(new_trap.description,
                    "Activating these swords will deal %u - %u physical damage.",
                    spec->sword_trap_value.min, spec->sword_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_SwordTrap);
        } break;
        
        case DungeonTrapType_Arrow:
        {
            new_trap.name = "Arrows";
            
            sprintf(new_trap.description,
                    "Activating these arrows will deal %u - %u physical damage.",
                    spec->arrow_trap_value.min, spec->arrow_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_ArrowTrap);
        } break;
        
        case DungeonTrapType_Magic:
        {
            new_trap.name = "Magical Glyph";
            
            sprintf(new_trap.description,
                    "Stepping on this glyph will deal %u - %u damage of a random damage type.",
                    spec->magic_trap_value.min, spec->magic_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_MagicTrap);
        } break;
        
        case DungeonTrapType_Bind:
        {
            new_trap.name = "Binding Glyph";
            
            sprintf(new_trap.description,
                    "Stepping on this glyph will cause you to be bound for %u - %u turns.",
                    spec->bind_trap_value.min, spec->bind_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_BindTrap);
        } break;
        
        case DungeonTrapType_Shaft:
        {
            new_trap.name = "Shaft";
            
            sprintf(new_trap.description,
                    "Shafts cause you to fall %u - %u dungeon levels.",
                    spec->shaft_trap_value.min, spec->shaft_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_ShaftTrap);
        } break;
        
        case DungeonTrapType_Summon:
        {
            new_trap.name = "Summoning Glyph";
            
            sprintf(new_trap.description,
                    "This glyph will summon a random enemy of relative strength into your vicinity.");
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_SummonTrap);
        } break;
        
        case DungeonTrapType_Teleport:
        {
            new_trap.name = "Teleporting Glyph";
            
            sprintf(new_trap.description,
                    "This glyph will transport you to a random location in the dungeon level.");
            
            new_trap.tile_src = get_dungeon_tileset_rect(TileID_TeleportTrap);
        } break;
        
        invalid_default_case;
    }
    
    for(u32 index = 0; index < spec->trap_count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        
        if(!trap->type)
        {
            ++traps->count;
            *trap = new_trap;
            
            return;
        }
    }
    
    assert(0);
}
//
// Dungeon trap code
//

internal v4u
get_dungeon_tile_rect(v2u pos)
{
    v4u result =
    {
        tile_mul(pos.x),
        tile_mul(pos.y),
        32,
        32
    };
    
    return(result);
}

// TODO(rami): room
internal b32
add_dungeon_room_feature(DungeonRoom *room,
                                u32 max_feature_count,
                         DungeonRoomFeatureType type)
{
    b32 result = true;
    
    if(max_feature_count)
    {
        u32 feature_count = 0;
        
        switch(type)
        {
            case DungeonRoomFeatureType_Item: feature_count = room->item_count; break;
            case DungeonRoomFeatureType_Enemy: feature_count = room->enemy_count; break;
            
            invalid_default_case;
        }
        
        if(feature_count <= max_feature_count)
        {
            switch(type)
            {
                case DungeonRoomFeatureType_Item: ++room->item_count; break;
                case DungeonRoomFeatureType_Enemy: ++room->enemy_count; break;
                
                invalid_default_case;
            }
        }
        else
        {
             result = false;
        }
    }
    
    return(result);
}

internal TileID
get_dungeon_pos_tile(DungeonTiles tiles, v2u pos)
{
    TileID result = tiles.array[(pos.y * tiles.width) + pos.x].tile;
    return(result);
}

internal Dungeon *
get_dungeon_from_level(Dungeons *dungeons, u32 level)
{
    assert(level > 0);
    
    Dungeon *result = &dungeons->levels[level - 1];
    return(result);
}

internal EntityID
get_random_enemy_suitable_for_dungeon_level(Random *random,
                            u32 *entity_levels,
                            u32 dungeon_level)
{
    EntityID enemy_id = EntityID_None;
    
    for(;;)
    {
        u32 level_min = dungeon_level - 1;
        if(!level_min)
        {
            level_min = 1;
        }
        
        u32 level_max = dungeon_level + 1;
        if(level_max > MAX_DUNGEON_LEVELS)
        {
            level_max = MAX_DUNGEON_LEVELS;
        }
        
        enemy_id = get_random_number(random, ENEMY_START_ID, ENEMY_END_ID);
        
        if(entity_levels[enemy_id] >= level_min &&
               entity_levels[enemy_id] <= level_max)
        {
            break;
        }
    }
    
    return(enemy_id);
}

internal v2u
get_tileset_pos_from_dungeon_tile(TileID tile)
{
    v2u result = {0};
    
    switch(tile)
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
        
        case TileID_SpikeTrap: result = make_v2u(14, 4); break;
        case TileID_SwordTrap: result = make_v2u(15, 4); break;
        case TileID_ArrowTrap: result = make_v2u(16, 4); break;
        case TileID_MagicTrap: result = make_v2u(17, 4); break;
        case TileID_BindTrap: result = make_v2u(18, 4); break;
        case TileID_ShaftTrap: result = make_v2u(19, 4); break;
        case TileID_SummonTrap: result = make_v2u(20, 4); break;
        case TileID_TeleportTrap: result = make_v2u(21, 4); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_dungeon_tileset_pos(DungeonTiles tiles, v2u tile_pos)
{
    TileID tile = get_dungeon_pos_tile(tiles, tile_pos);
    v2u result = get_tileset_pos_from_dungeon_tile(tile);
    
    return(result);
}

internal v4u
get_dungeon_tileset_rect(TileID tile)
{
    v2u tileset_pos = get_tileset_pos_from_dungeon_tile(tile);
    v4u result = get_dungeon_tile_rect(tileset_pos);
    
    return(result);
}

internal u32
get_random_with_chances(Random *random,
                        u32 *chances,
                        u32 random_start,
                        u32 random_end,
                        RandomChanceType random_chance_type)
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
                
                u32 index = get_index(result);
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
get_dungeon_tile_name(TileID tile)
{
    char *result = "";
    
    switch(tile)
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
get_dungeon_tile_info_text(TileID tile)
{
    char *result = 0;
    
    switch(tile)
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

internal b32
is_dungeon_pos_traversable_or_closed_door(DungeonTiles tiles, v2u pos)
{
    b32 result = false;
    
    if(is_dungeon_pos_traversable(tiles, pos) ||
       is_dungeon_pos_closed_door(tiles, pos))
    {
        result = true;
    }
    
    return(result);
}

internal b32
is_dungeon_pos_occupied(DungeonTiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_occupied);
    return(result);
}

internal b32
is_dungeon_pos_traversable_and_not_occupied(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) && !is_dungeon_pos_occupied(tiles, pos));
    return(result);
}

internal b32
is_dungeon_pos_wall(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, TileID_StoneWall1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWall2) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWall3) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWall4) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWall5) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWall5) ||
                  
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallTorch1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallTorch2) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallTorch3) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallTorch4) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallTorch5) ||
                  
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallGrate1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallGrate2) ||
                  
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallVines1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallVines2) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallVines3) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallVines4) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneWallVines5));
    
    return(result);
}

internal b32
is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, TileID_StoneDoorOpen));
    return(result);
}

internal b32
is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, TileID_StoneDoorClosed));
    return(result);
}

internal b32
is_dungeon_pos_inside_rect(v4u rect, v2u pos)
{
    b32 result = (pos.x >= rect.x &&
                  pos.y >= rect.y &&
                  pos.x < (rect.x + rect.w) &&
                  pos.y < (rect.y + rect.h));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable_and_has_been_seen(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) &&
                  has_tile_been_seen(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_floor(tiles, pos) ||
                  is_dungeon_pos_passage(tiles, pos) ||
                  is_dungeon_pos_open_door(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, TileID tile)
{
    b32 result = (get_dungeon_pos_tile(tiles, pos) == tile);
    return(result);
}

internal b32
is_dungeon_pos_floor(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, TileID_StoneFloor1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloor2) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloor3) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloor4) ||
                  
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate1) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate2) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate3) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate4) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate5) ||
                  is_dungeon_pos_tile(tiles, pos, TileID_StoneFloorGrate6));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable_and_has_not_been_seen(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) &&
                  !has_tile_been_seen(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_explorable(Dungeon *dungeon)
{
    b32 result = false;
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            if(is_dungeon_pos_traversable_and_has_not_been_seen(dungeon->tiles, make_v2u(x, y)))
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
    else if(result.x > dungeon->size.w - 1)
    {
        result.x = dungeon->size.w - 1;
    }
    
    if((s32)result.y < 0)
    {
        result.y = 0;
    }
    else if(result.y > dungeon->size.h - 1)
    {
        result.y = dungeon->size.h - 1;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_pos(Random *random, v2u dungeon_size)
{
    // Doesn't return a position on the edge of the dungeon
    v2u result =
    {
        get_random_number(random, 1, dungeon_size.w - 2),
        get_random_number(random, 1, dungeon_size.h - 2)
    };
    
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

internal TileID
get_dungeon_pos_remains(DungeonTiles tiles, v2u pos)
{
    TileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains;
    return(remains);
}

internal b32
can_place_dungeon_remains_on_pos(DungeonTiles tiles, v2u pos)
{
    b32 result = (!get_dungeon_pos_remains(tiles, pos) && !is_dungeon_pos_passage(tiles, pos));
    return(result);
}

internal void
set_dungeon_pos_remains(DungeonTiles tiles, v2u pos, TileID tile)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains = tile;
}

internal void
set_dungeon_pos_tile(DungeonTiles tiles, v2u pos, TileID tile)
{
    tiles.array[(pos.y * tiles.width) + pos.x].tile = tile;
}

internal void
set_dungeon_pos_occupied(DungeonTiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_occupied = value;
}

internal void
set_dungeon_pos_open_door(DungeonTiles tiles, v2u pos)
{
    set_dungeon_pos_tile(tiles, pos, TileID_StoneDoorOpen);
}

internal void
set_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos)
{
    set_dungeon_pos_tile(tiles, pos, TileID_StoneDoorClosed);
}

internal void
set_dungeon_pos_wall(Random *random, DungeonTiles tiles, v2u pos)
{
    u32 wall = get_random_number(random, TileID_StoneWall1, TileID_StoneWall5);
    set_dungeon_pos_tile(tiles, pos, wall);
}

internal void
set_tile_floor(Random *random, DungeonTiles tiles, v2u pos)
{
    u32 floor = get_random_number(random, TileID_StoneFloor1, TileID_StoneFloor4);
    set_dungeon_pos_tile(tiles, pos, floor);
}

internal u32
get_rect_wall_count(Dungeon *dungeon, v4u rect)
{
    u32 wall_count = 0;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(is_dungeon_pos_wall(dungeon->tiles, make_v2u(x, y)))
            {
                ++wall_count;
            }
        }
    }
    
    return(wall_count);
}

internal b32
is_pos_inside_dungeon(v2u dungeon_size, v2u pos)
{
    b32 result = (pos.x >= 0 &&
                  pos.y >= 0 &&
                  pos.x < dungeon_size.w &&
                  pos.y < dungeon_size.h);
    
    return(result);
}

internal b32
is_rect_inside_dungeon(v2u dungeon_size, v4u rect)
{
    b32 result = (rect.x >= 1 &&
                  rect.y >= 1 &&
                      (rect.x + rect.w) < (dungeon_size.w - 1) &&
                      (rect.y + rect.h) < (dungeon_size.h - 1));
    
    return(result);
}

internal b32
is_rect_wall(DungeonTiles tiles, v4u rect)
{
    b32 result = true;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_dungeon_pos_wall(tiles, make_v2u(x, y)))
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
    b32 result = (is_rect_inside_dungeon(dungeon->size, rect) &&
                      is_rect_wall(dungeon->tiles, rect));
    
    return(result);
}

internal u32
get_dungeon_tile_neighbour_floor_count(DungeonTiles src_tiles, v2u center_pos, v4u room)
{
    u32 result = 0;
    
    for(u32 y = center_pos.y - 1; y < center_pos.y + 2; ++y)
    {
        for(u32 x = center_pos.x - 1; x < center_pos.x + 2; ++x)
        {
            v2u pos = {x, y};
            
            if(is_dungeon_pos_inside_rect(room, pos))
            {
                // Ignore center position
                if(pos.x != center_pos.x ||
                   pos.y != center_pos.y)
                {
                    if(is_dungeon_pos_floor(src_tiles, pos))
                    {
                        ++result;
                    }
                }
            }
        }
    }
    
    return(result);
}

internal b32
is_dungeon_rect_traversable(DungeonTiles tiles, v4u rect)
{
    b32 result = true;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            if(!is_dungeon_pos_traversable(tiles, make_v2u(x, y)))
            {
                result = false;
            }
        }
    }
    
    return(result);
}

internal void
reset_non_flood_filled_tiles(Random *random, Dungeon *dungeon, b32 *fill_tiles)
{
    for(u32 y = dungeon->rect.y; y < dungeon->rect.h; ++y)
    {
        for(u32 x = dungeon->rect.x; x < dungeon->rect.w; ++x)
        {
            if(!fill_tiles[(y * dungeon->tiles.width) + x])
            {
                set_dungeon_pos_wall(random, dungeon->tiles, make_v2u(x, y));
            }
        }
    }
}

internal u32
flood_fill(DungeonTiles tiles, b32 *fill_tiles, u32 fill_count, v2u pos)
{
    if(!fill_tiles[(pos.y * tiles.width) + pos.x] && is_dungeon_pos_traversable(tiles, pos))
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

internal b32
can_place_dungeon_feature_on_pos(Dungeon *dungeon, ItemState *items, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(dungeon->tiles, pos) &&
                      !is_dungeon_pos_trap(&dungeon->traps, pos) &&
                      !is_dungeon_pos_passage(dungeon->tiles, pos) &&
                      !is_dungeon_pos_occupied(dungeon->tiles, pos) &&
                      !get_dungeon_pos_item_count(items, pos, dungeon->level));
    
    return(result);
}

//
// Dungeon room code
//
internal DungeonRoom *
get_random_dungeon_room(Random *random, DungeonRooms *rooms)
{
    DungeonRoom *result = 0;
    
    u32 index = get_random_number(random, 0, rooms->count - 1);
    result = &rooms->array[index];
    
    return(result);
}

internal DungeonRoom *
get_dungeon_room_from_pos(DungeonRooms *rooms, v2u pos)
{
    DungeonRoom *result = 0;
    
    for(u32 index = 0; index < rooms->count; ++index)
    {
        DungeonRoom *room = &rooms->array[index];
        
        if(is_pos_inside_rect(room->rect, pos))
        {
            result = room;
            break;
        }
    }
    
    return(result);
}

internal char *
get_dungeon_room_type_string(DungeonRoomType type)
{
    char *result = 0;
    
    switch(type)
    {
        case DungeonRoomType_Rect: result = "DungeonRoomType_Rect"; break;
        case DungeonRoomType_DoubleRect: result = "DungeonRoomType_DoubleRect"; break;
        case DungeonRoomType_Circle: result = "DungeonRoomType_Circle"; break;
        case DungeonRoomType_Automaton: result = "DungeonRoomType_Automaton"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal DungeonTiles
get_dungeon_automaton_room_tiles(TemporaryMemory *memory, DungeonSpec *spec)
{
    memory_size buff_size = (spec->automaton_room_size.max * spec->automaton_room_size.max) * sizeof(DungeonTile);
    
    DungeonTiles buff_one = {0};
    buff_one.width = spec->automaton_room_size.max;
    buff_one.array = push_memory(memory->arena, buff_size);
    
    return(buff_one);
}

internal void
automaton_room_step(Random *random, DungeonTiles src_tiles, DungeonTiles dest_tiles, v4u rect)
{
    for(u32 y = 0; y < rect.h; ++y)
    {
        for(u32 x = 0; x < rect.w; ++x)
        {
            v2u src_pos = {rect.x + x, rect.y + y};
            v2u dest_pos = {x, y};
            
            u32 floor_count = get_dungeon_tile_neighbour_floor_count(src_tiles, src_pos, rect);
            
            if(is_dungeon_pos_floor(src_tiles, src_pos))
            {
                if(floor_count <= 3)
                {
                    set_dungeon_pos_wall(random, dest_tiles, dest_pos);
                }
                else
                {
                    set_tile_floor(random, dest_tiles, dest_pos);
                }
            }
            else if(is_dungeon_pos_wall(src_tiles, src_pos))
            {
                if(floor_count >= 6)
                {
                    set_tile_floor(random, dest_tiles, dest_pos);
                }
                else
                {
                    set_dungeon_pos_wall(random, dest_tiles, dest_pos);
                }
            }
        }
    }
}

internal void
place_dungeon_room_from_src_to_dest(DungeonTiles src_tiles, v4u src_room, DungeonTiles dest_tiles)
{
    for(u32 y = 0; y < src_room.h; ++y)
    {
        for(u32 x = 0; x < src_room.w; ++x)
        {
            v2u local_pos = {x, y};
            v2u dungeon_pos = {src_room.x + x, src_room.y + y};
            
            set_dungeon_pos_tile(dest_tiles, dungeon_pos, get_dungeon_pos_tile(src_tiles, local_pos));
        }
    }
}

internal void
place_dungeon_room(Random *random, DungeonTiles tiles, v4u room)
{
    for(u32 y = room.y; y < (room.y + room.h); ++y)
    {
        for(u32 x = room.x; x < (room.x + room.w); ++x)
        {
            set_tile_floor(random, tiles, make_v2u(x, y));
        }
    }
}

internal DungeonRoom
create_and_place_room(MemoryArena *memory, Random *random, Dungeon *dungeon)
{
    DungeonSpec *spec = &dungeon->spec;
    
    DungeonRoom result = {0};
    DungeonRoomType room_type = get_random_with_chances(random,
                                                            spec->room_type_chances,
                                                            DungeonRoomType_Rect,
                                                            DungeonRoomType_Automaton,
                                                            RandomChanceType_Normal);
    
    // Set room dimensions
    switch(room_type)
    {
        case DungeonRoomType_Rect:
        {
            result.rect.w = get_random_number_from_v2u(random, spec->rect_room_size);
            result.rect.h = get_random_number_from_v2u(random, spec->rect_room_size);
        } break;
        
        case DungeonRoomType_DoubleRect:
        {
            result.rect.w = get_random_number_from_v2u(random, spec->double_rect_room_size);
            result.rect.h = get_random_number_from_v2u(random, spec->double_rect_room_size);
        } break;
        
        case DungeonRoomType_Circle:
        {
            u32 radius = get_random_number_from_v2u(random, spec->circle_room_size);
            u32 size = radius * 2;
            
            result.rect.w = size;
            result.rect.h = size;
            } break;
        
        case DungeonRoomType_Automaton:
        {
            result.rect.w = get_random_number_from_v2u(random, spec->automaton_room_size);
            result.rect.h = get_random_number_from_v2u(random, spec->automaton_room_size);
        } break;
        
        invalid_default_case;
    }
    
    for(u32 count = 0; count <= spec->room_retry_count; ++count)
    {
        // Set room starting position
        v2u random_pos = get_random_dungeon_pos(random, dungeon->size);
        result.rect.x = random_pos.x;
        result.rect.y = random_pos.y;
        
        // Pad the rect so that there is at least a certain amount of tiles between rooms
        assert(result.rect.w);
        assert(result.rect.h);
        
        v4u padded_rect = get_padded_rect(result.rect, 1);
        
#if 0
        print_v2u(random_pos);
        print_v4u(result.rect);
        print_v4u(padded_rect);
#endif
        
        if(is_rect_inside_dungeon_and_wall(dungeon, padded_rect))
        {
            if(room_type == DungeonRoomType_Rect)
            {
                place_dungeon_room(random, dungeon->tiles, result.rect);
                
                result.success = true;
            }
            else if(room_type == DungeonRoomType_DoubleRect)
            {
                v4u room_one = result.rect;
                v4u room_two = {0};
                
                room_two.w = get_random_number_from_v2u(random, dungeon->spec.double_rect_room_size);
                room_two.h = get_random_number_from_v2u(random, dungeon->spec.double_rect_room_size);
                room_two.x = room_one.x + (room_one.w / 2);
                room_two.y = room_one.y + (room_one.h / 2);
                
#if 0
                print_v4u("room_one", room_one);
                print_v4u("room_two", room_two);
#endif
                
                // Set the correct final room width
                if(room_one.x + room_one.w >= room_two.x + room_two.w)
                {
                    result.rect.w = (room_one.x + room_one.w) - room_one.x;
                }
                else
                {
                    result.rect.w = (room_two.x + room_two.w) - room_one.x;
                }
                
                // Set the correct final room height
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
                    place_dungeon_room(random, dungeon->tiles, room_one);
                    place_dungeon_room(random, dungeon->tiles, room_two);
                    
                    result.success = true;
                }
            }
            else if(room_type == DungeonRoomType_Circle)
            {
                // result.rect is off by one on width and height which is why this is created
                // to be the accurate circle rectangle.
                v4u circle_rect = {result.rect.x, result.rect.y, result.rect.w + 1, result.rect.h + 1};
                
                if(is_rect_inside_dungeon_and_wall(dungeon, get_padded_rect(circle_rect, 1)))
                {
                    
                    #if 0
                    printf("\nresult.rect: %u, %u, %u, %u\n", result.rect.x, result.rect.y, result.rect.w, result.rect.h);
                    printf("circle_rect: %u, %u, %u, %u\n", circle_rect.x, circle_rect.y, circle_rect.w, circle_rect.h);
                    #endif
                    
                    // result.rect.w and result.rect.h both hold the radius of the circle and
                    // should therefore be equal
                    assert(result.rect.w == result.rect.h);
                    
                    s32 radius = result.rect.w / 2;
                    s32 double_radius = radius * 2;
                    
                    for(s32 y = 0; y <= double_radius; ++y)
                    {
                        for(s32 x = 0; x <= double_radius; ++x)
                        {
                            s32 y_difference = y - radius;
                            s32 x_difference = x - radius;
                            
                            s32 y_distance = y_difference * y_difference;
                            s32 x_distance = x_difference * x_difference;
                            
                            f32 distance = sqrt(y_distance + x_distance);
                            
                            if(distance < (radius + 0.5f))
                            {
                                v2u pos =
                                {
                                    result.rect.x + x,
                                    result.rect.y + y
                                };
                                
                                set_tile_floor(random, dungeon->tiles, pos);
                            }
                        }
                    }
                    
                    result.success = true;
                    result.rect = circle_rect;
                }
                }
            else if(room_type == DungeonRoomType_Automaton)
            {
                TemporaryMemory temporary_memory = start_temporary_memory(memory);
                
                DungeonTiles room_one = get_dungeon_automaton_room_tiles(&temporary_memory, &dungeon->spec);
                DungeonTiles room_two = get_dungeon_automaton_room_tiles(&temporary_memory, &dungeon->spec);
                
                // Lay out the initial shape by placing floor and wall tiles randomly
                for(u32 y = 0; y < result.rect.h; ++y)
                {
                    for(u32 x = 0; x < result.rect.w; ++x)
                    {
                        if(hit_random_chance(random, 55))
                        {
                            set_tile_floor(random, room_one, make_v2u(x, y));
                        }
                        else
                        {
                            set_dungeon_pos_wall(random, room_one, make_v2u(x, y));
                        }
                    }
                }
                
                // Create the final automaton room by applying automaton steps multiple times,
                // causing the initial random shape to turn into more of a blob shape.
                v4u room_rect = make_v4u(0, 0, result.rect.w, result.rect.h);
                
                automaton_room_step(random, room_one, room_two, room_rect);
                automaton_room_step(random, room_two, room_one, room_rect);
                automaton_room_step(random, room_one, room_two, room_rect);
                automaton_room_step(random, room_two, room_one, room_rect);
                
#if 0
                // This is to see the resulting automaton room before it is checked to be valid.
                place_room_from_src_to_dest(room_one, result.rect, dungeon->tiles);
                //place_room_from_src_to_dest(room_two, result.rect, dungeon->tiles);
                
                result.success = true;
                return(result);
#endif
                
                // Get the floor count of the automaton room
                u32 room_floor_count = 0;
                for(u32 y = 0; y < result.rect.h; ++y)
                {
                    for(u32 x = 0; x < result.rect.w; ++x)
                    {
                        if(is_dungeon_pos_floor(room_one, make_v2u(x, y)))
                        {
                            ++room_floor_count;
                        }
                    }
                }
                
                // The automaton room has to be at least a certain percentage of the original area.
                if((f32)room_floor_count / (f32)get_rect_area(result.rect) >= 0.25f)
                {
                    // Place the resulting automaton room
                    place_dungeon_room_from_src_to_dest(room_one, result.rect, dungeon->tiles);
                    
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
                            if(is_dungeon_pos_traversable(dungeon->tiles, make_v2u(x, y)))
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
                    
                    result.success = true;
                    result.rect = new_room_rect;
                    
#if 0
                    printf("Room floor count: %u\n", room_floor_count);
                    printf("Room rect size: %u\n", get_rect_size(result.rect));
                    print_v4u("Starting rect", result.rect);
                    print_v4u("Ending rect", new_room_rect);
#endif
                    
                }
                
                end_temporary_memory(temporary_memory);
            }
            }
        
        if(result.success)
        {
            result.type = room_type;
            break;
        }
        }
    
    return(result);
}
//
// Dungeon room code
//

internal Dungeon *
create_dungeon(Game *game,
               Entity *player,
               EntityState *entities,
               Dungeons *dungeons,
               ItemState *items,
               Inventory *inventory,
               UI *ui,
               u32 dungeon_level)
{
    
    #if 0
    printf("Creating dungeon level %u (index %u)\n", dungeon_level, dungeon_level - 1);
    #endif
    
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeon_level);
    dungeon->level = dungeon_level;
    
    assert(dungeon->level <= array_count(dungeons->levels));
    assert(dungeon->tiles.array);
    
    Random *random = &game->random;
    DungeonRooms *rooms = &dungeon->rooms;
    DungeonTraps *traps = &dungeon->traps;
    DungeonPassages *passages = &dungeon->passages;
    
    Dungeon *dungeon_above = 0;
    if(dungeon->level > 1)
    {
        dungeon_above = get_dungeon_from_level(dungeons, dungeon_level - 1);
    }
    
    dungeon->size.w = 64;
    dungeon->size.h = 64;
    dungeon->area = get_size_area(dungeon->size);
    dungeon->rect = make_v4u(0, 0, dungeon->size.w, dungeon->size.h);
    
    dungeon->tiles.width = dungeon->size.w;
    dungeon->ready_for_pathfinding = false;
    
    // TODO(rami): Spec Start
    
    DungeonSpec *spec = &dungeon->spec;
    
    spec->room_retry_count = 10;
    
    spec->room_max_items = 3;
    spec->room_max_enemies = 3;
    
    spec->room_type_chances[DungeonRoomType_Rect] = 30;
    spec->room_type_chances[DungeonRoomType_DoubleRect] = 30;
    spec->room_type_chances[DungeonRoomType_Circle] = 30;
    spec->room_type_chances[DungeonRoomType_Automaton] = 60;
    
    spec->corridor_type_chances[DungeonCorridorType_Turn] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Zigzag] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Diagonal] = 30;
    
    spec->rect_room_size = make_v2u(4, 8);
    spec->double_rect_room_size = make_v2u(4, 8);
    spec->circle_room_size = make_v2u(2, 5);
    spec->automaton_room_size = make_v2u(10, 18);
    
    spec->trap_count = get_random_number(random, 8, 16);
    
    spec->spike_trap_value = make_v2u(4, 12);
    spec->sword_trap_value = make_v2u(18, 35);
    spec->arrow_trap_value = make_v2u(2, 14);
    spec->magic_trap_value = make_v2u(4, 24);
    spec->bind_trap_value = make_v2u(3, 6);
    spec->shaft_trap_value = make_v2u(1, 2);
    
    spec->down_passage_count = get_random_number(random, 1, 3);
    
    spec->enemy_count = (dungeon->size.w + dungeon->size.h) * 0.15f;
    
    spec->item_count = (dungeon->size.w + dungeon->size.h) * 0.20f;
    spec->item_curse_chance = 5;
    
    spec->item_type_chances[get_index(ItemType_Weapon)] = 25;
    spec->item_type_chances[get_index(ItemType_Armor)] = 25;
    spec->item_type_chances[get_index(ItemType_Potion)] = 25;
    spec->item_type_chances[get_index(ItemType_Scroll)] = 25;
    spec->item_type_chances[get_index(ItemType_Ration)] = 25;
    
    spec->potion_chances[Potion_Might] = 25;
    spec->potion_chances[Potion_Wisdom] = 25;
    spec->potion_chances[Potion_Agility] = 25;
    spec->potion_chances[Potion_Elusion] = 25;
    spec->potion_chances[Potion_Healing] = 25;
    spec->potion_chances[Potion_Decay] = 25;
    spec->potion_chances[Potion_Confusion] = 25;
    
    spec->scroll_chances[Scroll_Identify] = 25;
    spec->scroll_chances[Scroll_EnchantWeapon] = 25;
    spec->scroll_chances[Scroll_EnchantArmor] = 25;
    spec->scroll_chances[Scroll_MagicMapping] = 25;
    spec->scroll_chances[Scroll_Teleportation] = 25;
    
    // TODO(rami): Spec End
    
    // Set dungeon up passage count
    if(dungeon->level == 1)
    {
        passages->up_count = 1;
    }
    else
    {
        passages->up_count = dungeon_above->passages.down_count;
        
        #if 0
        printf("Above dungeon up passage count: %u\n", dungeon_above->up_passage_count);
        printf("Above dungeon down passage count: %u\n\n", dungeon_above->down_passage_count);
        
        printf("Current dungeon up passage count: %u\n\n", passages->up_count);
#endif
        
    }
    
    // Set dungeon down passage count
    passages->down_count = spec->down_passage_count;
    
    #if 0
    printf("\nEnemy Count: %u\n", dungeon->enemy_count);
    printf("Item Count: %u\n", dungeon->item_count);
    
    printf("Up Passage Count: %u\n", dungeon->up_passage_count);
    printf("Down Passage Count: %u\n", dungeon->down_passage_count);
    
    printf("player_distance_from_enemy: %u\n", dungeon->player_distance_from_enemy);
    printf("player_distance_from_item: %u\n", dungeon->player_distance_from_item);
    printf("player_distance_from_passage: %u\n", dungeon->player_distance_from_passage);
#endif
    
    // Set dungeon tile defaults
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
                v2u pos = {x, y};
                set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
            set_dungeon_pos_occupied(dungeon->tiles, pos, false);
                set_dungeon_pos_wall(random, dungeon->tiles, pos);
            }
        }
        
    assert(dungeon->size.w <= MAX_DUNGEON_SIZE);
    assert(dungeon->size.h <= MAX_DUNGEON_SIZE);
    
#if 0
    // Create a test room
    dungeon->ready_for_pathfinding = true;
    
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            if(!x || x == (dungeon->width - 1) ||
               !y || y == (dungeon->height - 1))
            {
                set_dungeon_pos_wall(random, dungeon->tiles, make_v2u(x, y));
            }
            else
            {
                set_tile_floor(random, dungeon->tiles, make_v2u(x, y));
            }
        }
    }
    
        move_entity(random, player, dungeon->tiles, ui, make_v2u(8, 1));
    
    //add_enemy_entity(entities, dungeon->tiles, EntityID_Python, 7, 1);
    //add_enemy_entity(entities, dungeon->tiles, EntityID_OrcWarrior, 5, 15);
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
    StatusEffect test_effect = {0};
    test_effect.type = StatusEffectType_Confusion;
    test_effect.chance = 50;
    test_effect.duration = 15;
    start_entity_status_effect(test_entity, test_effect);
#endif
    
#if 1
    // Test traps
    v2u trap_pos = {25, 2};
    
    for(DungeonTrapType type = DungeonTrapType_None + 1; type < DungeonTrapType_Count; ++type)
    {
        DungeonTrap new_trap = {0};
        new_trap.type = type;
        new_trap.pos = trap_pos;
        
        add_dungeon_trap(&dungeon->spec, &dungeon->traps, new_trap);
        
        ++trap_pos.x;
    }
    #endif
    
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
    // Test entity remains
    entity_x = entity_x_start;
    
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
                kill_entity(random, entity, dungeon, ui);
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
        add_weapon_item(random, items, dungeon_level, weapon_id, ItemRarity_Common, weapon.x + 1, weapon.y, false);
        add_weapon_item(random, items, dungeon_level, weapon_id, ItemRarity_Magical, weapon.x + 2, weapon.y, false);
        add_weapon_item(random, items, dungeon_level, weapon_id, ItemRarity_Mythical, weapon.x + 3, weapon.y, false);
        
        ++weapon.y;
    }
    
    v2u armor = {weapon.x + 4, 1};
    
    add_armor_item(random, items, dungeon_level, ItemID_LeatherHelmet, armor.x + 1, armor.y, false);
    add_armor_item(random, items, dungeon_level, ItemID_LeatherChestplate, armor.x + 2, armor.y, false);
    add_armor_item(random, items, dungeon_level, ItemID_LeatherGreaves, armor.x + 3, armor.y, false);
    add_armor_item(random, items, dungeon_level, ItemID_LeatherBoots, armor.x + 4, armor.y, false);
    
    add_armor_item(random, items, dungeon_level, ItemID_SteelHelmet, armor.x + 1, armor.y + 1, false);
    add_armor_item(random, items, dungeon_level, ItemID_SteelChestplate, armor.x + 2, armor.y + 1, false);
    add_armor_item(random, items, dungeon_level, ItemID_SteelGreaves, armor.x + 3, armor.y + 1, false);
    add_armor_item(random, items, dungeon_level, ItemID_SteelBoots, armor.x + 4, armor.y + 1, false);
    
    v2u potion = {armor.x + 5, 1};
    
    for(ItemID potion_id = ItemID_PotionStart + 1; potion_id < ItemID_PotionEnd; ++potion_id)
    {
        add_consumable_item(random, items, dungeon_level, potion_id, potion.x + 1, potion.y, 2);
        add_consumable_item(random, items, dungeon_level, potion_id, potion.x + 2, potion.y, 7);
        
        ++potion.y;
    }
    
    add_consumable_item(random, items, dungeon_level, ItemID_Ration, potion.x + 1, potion.y, 1);
    add_consumable_item(random, items, dungeon_level, ItemID_Ration, potion.x + 2, potion.y, 1);
    
    v2u scroll = {potion.x + 3, 1};
    
    for(ItemID scroll_id = ItemID_ScrollStart + 1; scroll_id < ItemID_ScrollEnd; ++scroll_id)
    {
        add_consumable_item(random, items, dungeon_level, scroll_id, scroll.x + 1, scroll.y, 1);
        add_consumable_item(random, items, dungeon_level, scroll_id, scroll.x + 2, scroll.y, 1);
        
        ++scroll.y;
    }
#endif
    
    return(dungeon);
    
#elif 0
    
    // Leave dungeon blank
    return(dungeon);
    
#endif
    
    // Place rooms
    u32 total_room_size = 0;
    
    //while(dungeon->room_count < 10)
    while((f32)total_room_size / (f32)dungeon->area < 0.4f)
    {
        DungeonRoom created_room = create_and_place_room(&game->memory_arena,
                                                             random,
                                                             dungeon);
        
        if(created_room.success)
        {
            rooms->array[rooms->count++] = created_room;
            assert(rooms->count < MAX_DUNGEON_ROOM_COUNT);
            
            total_room_size += get_rect_area(created_room.rect);
        }
    }
    
#if 0
    printf("dungeon_size: %u\n", dungeon->size);
    printf("total_room_size: %u\n", total_room_size);
    printf("total_room_size / dungeon_size: %.02f\n\n", (f32)total_room_size / (f32)dungeon->size);
    #endif
    
#if 0
    DungeonRoomType room_type_count[DungeonRoomType_Count] = {0};
    
    for(u32 index = 0; index < rooms->count; ++index)
    {
        DungeonRoom *room = &rooms->array[index];
        DungeonRoomType type = room->type;
        
        switch(type)
        {
            case DungeonRoomType_Rect: ++room_type_count[type]; break;
            case DungeonRoomType_DoubleRect: ++room_type_count[type]; break;
            case DungeonRoomType_Circle: ++room_type_count[type]; break;
            case DungeonRoomType_Automaton: ++room_type_count[type]; break;
            
            invalid_default_case;
        }
    }
    
    printf("\n");
    
    for(DungeonRoomType type = DungeonRoomType_None + 1;
            type < DungeonRoomType_Count;
            ++type)
    {
        printf("type_counts[%s]: %u\n", get_dungeon_room_type_string(type), room_type_count[type]);
    }
    
    printf("\n");
    #endif
    
#if 0
    printf("\nRoom Count: %u\n", rooms->count);
    
    for(u32 room_index = 0; room_index < rooms->count; ++room_index)
    {
        DungeonRoom *room = &rooms->array[room_index];
        
        printf("rooms[%u].x: %u\n", room_index, room->rect.x);
        printf("rooms[%u].y: %u\n", room_index, room->rect.y);
        printf("rooms[%u].w: %u\n", room_index, room->rect.w);
        printf("rooms[%u].h: %u\n\n", room_index, room->rect.h);
    }
#endif
    
#if 1
    // Place Corridors
    dungeon->ready_for_pathfinding = true;
    
    b32 is_connected[rooms->count];
    zero_struct(is_connected);
    
    // Find the room positions to connect
    for(u32 start_index = 0; start_index < (rooms->count - 1); ++start_index)
    {
        RoomIndex end_room_index = {0};
        u32 best_distance = U32_MAX;
        
        for(u32 end_index = 0; end_index < rooms->count; ++end_index)
        {
            if((start_index != end_index) && !is_connected[end_index])
            {
                v2u start_pos = get_rect_center(rooms->array[start_index].rect);
                v2u end_pos = get_rect_center(rooms->array[end_index].rect);
                
                u32 new_distance = get_cardinal_distance(start_pos, end_pos);
                if(new_distance < best_distance)
                {
                    best_distance = new_distance;
                    
                    end_room_index.found = true;
                    end_room_index.value = end_index;
                }
            }
        }
        
        // Connect the chosen positions
        if(end_room_index.found)
        {
            for(;;)
            {
                v2u start_pos = get_random_rect_pos(random, rooms->array[start_index].rect);
                if(is_dungeon_pos_traversable(dungeon->tiles, start_pos))
                {
                    v2u end_pos = get_random_rect_pos(random, rooms->array[end_room_index.value].rect);
                    if(is_dungeon_pos_traversable(dungeon->tiles, end_pos))
                    {
                        DungeonCorridorType type = get_random_with_chances(random,
                                                                                 spec->corridor_type_chances,
                                                                               DungeonCorridorType_None + 1,
                                                                               DungeonCorridorType_Count - 1,
                                                                                 RandomChanceType_Normal);
                        
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
                            case DungeonCorridorType_Turn:
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
                            
                            case DungeonCorridorType_Zigzag:
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
                                    
                                    if(is_v2u_equal(start_pos, end_pos))
                                    {
                                        break;
                                    }
                                }
                            } break;
                            
                            case DungeonCorridorType_Diagonal:
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
    TemporaryMemory temporary_memory = start_temporary_memory(&game->memory_arena);
    
    memory_size fill_tiles_size = dungeon->area * sizeof(b32);
    b32 *fill_tiles = push_memory(temporary_memory.arena, fill_tiles_size);
    
    for(;;)
    {
        // Clear in case we looped
        zero_size(fill_tiles, fill_tiles_size);
        
        // Get a room and a pos in the room to start the flood fill from
        DungeonRoom *room = get_random_dungeon_room(random, &dungeon->rooms);
        v2u room_pos = {0};
        
        for(;;)
        {
            room_pos = get_random_rect_pos(random, room->rect);
            if(is_dungeon_pos_traversable(dungeon->tiles, room_pos))
            {
                break;
            }
        }
        
        // See how much we flood filled
        u32 flood_fill_count = flood_fill(dungeon->tiles, fill_tiles, 0, room_pos);
        u32 flood_fill_start_room_size = get_rect_area(room->rect);
        
#if 0
        printf("Flood Fill Room Pos: %u, %u\n", room->pos.x, room->pos.y);
        printf("Flood Fill Room Area: %u\n", flood_fill_start_room_size);
        printf("Flood Fill Count: %u\n\n", flood_fill_count);
#endif
        
        if(flood_fill_count > flood_fill_start_room_size)
        {
            break;
        }
    }
    
    reset_non_flood_filled_tiles(random, dungeon, fill_tiles);
    end_temporary_memory(temporary_memory);
#endif
    
#if 1
    // Place Details
    for(u32 count = 0; count < (f32)(dungeon->area) * 0.02f; ++count)
    {
        for(;;)
        {
            v2u current = get_random_dungeon_pos(random, dungeon->size);
            
            if(is_dungeon_pos_wall(dungeon->tiles, current))
            {
                v2u up = {current.x, current.y - 1};
                v2u down = {current.x, current.y + 1};
                v2u left = {current.x - 1, current.y};
                v2u right = {current.x + 1, current.y};
                
                if(is_dungeon_pos_floor(dungeon->tiles, up) ||
                   is_dungeon_pos_floor(dungeon->tiles, down)||
                   is_dungeon_pos_floor(dungeon->tiles, left)||
                   is_dungeon_pos_floor(dungeon->tiles, right))
                {
                    TileID tile = get_random_number(random, TileID_StoneWallTorch1, TileID_StoneWallVines5);
                    set_dungeon_pos_tile(dungeon->tiles, current, tile);
                    
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    // Place Doors
    for(u32 count = 0; count < (f32)(dungeon->area) * 0.5f; ++count)
    {
        v2u current = get_random_dungeon_pos(random, dungeon->size);
        
        if(is_dungeon_pos_floor(dungeon->tiles, current))
        {
            v2u up = {current.x, current.y - 1};
            v2u down = {current.x, current.y + 1};
            v2u left = {current.x - 1, current.y};
            v2u right = {current.x + 1, current.y};
            
            v2u up_up = {current.x, current.y - 2};
            v2u down_down = {current.x, current.y + 2};
            v2u left_left = {current.x - 2, current.y};
            v2u right_right = {current.x + 2, current.y};
            
            if(is_dungeon_pos_floor(dungeon->tiles, left) &&
               is_dungeon_pos_floor(dungeon->tiles, right) &&
               is_dungeon_pos_floor(dungeon->tiles, left_left) &&
               is_dungeon_pos_floor(dungeon->tiles, right_right) &&
               is_dungeon_pos_wall(dungeon->tiles, up) &&
               is_dungeon_pos_wall(dungeon->tiles, down))
            {
                v2u left_up = {left.x, left.y - 1};
                v2u left_down = {left.x, left.y + 1};
                v2u right_up = {right.x, right.y - 1};
                v2u right_down = {right.x, right.y + 1};
                
                if((is_dungeon_pos_floor(dungeon->tiles, left_up) && is_dungeon_pos_floor(dungeon->tiles, left_down)) ||
                   (is_dungeon_pos_floor(dungeon->tiles, right_up) && is_dungeon_pos_floor(dungeon->tiles, right_down)))
                {
                    set_dungeon_pos_closed_door(dungeon->tiles, current);
                }
            }
            else if(is_dungeon_pos_floor(dungeon->tiles, up) &&
                    is_dungeon_pos_floor(dungeon->tiles, down) &&
                    is_dungeon_pos_floor(dungeon->tiles, up_up) &&
                    is_dungeon_pos_floor(dungeon->tiles, down_down) &&
                    is_dungeon_pos_wall(dungeon->tiles, left) &&
                    is_dungeon_pos_wall(dungeon->tiles, right))
            {
                v2u up_left = {up.x - 1, up.y};
                v2u up_right = {up.x + 1, up.y};
                v2u down_left = {down.x - 1, down.y};
                v2u down_right = {down.x + 1, down.y};
                
                if((is_dungeon_pos_floor(dungeon->tiles, up_left) && is_dungeon_pos_floor(dungeon->tiles, up_right) ||
                    (is_dungeon_pos_floor(dungeon->tiles, down_left) && is_dungeon_pos_floor(dungeon->tiles, down_right))))
                {
                    set_dungeon_pos_closed_door(dungeon->tiles, current);
                }
            }
        }
    }
#endif
    
#if 1
    // Place Up Passages
    for(u32 count = 0; count < passages->up_count; ++count)
    {
        DungeonPassage up_passage = {0};
        up_passage.type = DungeonPassageType_Up;
        
        for(;;)
        {
            up_passage.pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, up_passage.pos))
            {
                if(dungeon->level == 1)
                {
                    set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, TileID_ExitDungeon);
                }
                else
                {
                    set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, TileID_StoneStaircaseUp);
                }
                
                // Connect all the up passages in the current dungeon to down passages
                // in the dungeon above.
                if(dungeon_above)
                {
                    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                    {
                        DungeonPassage *dungeon_above_passage = &dungeon_above->passages.array[index];
                        
                        if(dungeon_above_passage->type == DungeonPassageType_Down &&
                               is_v2u_zero(dungeon_above_passage->dest_pos))
                    {
                            dungeon_above_passage->dest_pos = up_passage.pos;
                            up_passage.dest_pos = dungeon_above_passage->pos;
                            
                            #if 0
                            printf("Passage at %u, %u has been linked to %u, %u above.\n",
                                       up_passage.pos.x, up_passage.pos.y,
                                       dungeon_above_passage->pos.x, dungeon_above_passage->pos.y);
                            #endif
                            
                            break;
                        }
                    }
                    
                    // No passage should be left unconnected
                    assert(!is_v2u_zero(up_passage.dest_pos));
                }
                
                add_dungeon_passage(&dungeon->passages, up_passage);
                break;
            }
        }
    }
    #endif
    
    #if 1
    // Place Down Passages
    for(u32 count = 0; count < spec->down_passage_count; ++count)
    {
        DungeonPassage down_passage = {0};
        down_passage.type = DungeonPassageType_Down;
        
        for(;;)
        {
            down_passage.pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, down_passage.pos))
            {
                set_dungeon_pos_tile(dungeon->tiles, down_passage.pos, TileID_StoneStaircaseDown);
                add_dungeon_passage(&dungeon->passages, down_passage);
                
                break;
            }
        }
    }
    #endif
    
#if 0
    // Print passages
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &dungeon->passages.array[index];
        
        if(passage->type)
        {
            printf("\nPassage[%u]\n", index);
            printf("Type: %s\n", passage->type == DungeonPassageType_Up ? "Up" : "Down");
            printf("Pos: %u, %u\n", passage->pos.x, passage->pos.y);
            printf("Destination Pos: %u, %u\n\n", passage->dest_pos.x, passage->dest_pos.y);
        }
    }
    #endif
    
    #if 1
    // Place Enemies
    
    for(u32 count = 0; count < spec->enemy_count; ++count)
    {
        EntityID enemy_id = get_random_enemy_suitable_for_dungeon_level(random,
                                                                            entities->levels,
                                                                            dungeon->level);
        
        for(;;)
        {
                v2u pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, pos))
            {
                b32 add_enemy = false;
                
                    DungeonRoom *room = get_dungeon_room_from_pos(&dungeon->rooms, pos);
                if(room)
                {
                    add_enemy = add_dungeon_room_feature(room,
                                                             spec->room_max_enemies,
                                                             DungeonRoomFeatureType_Enemy);
                }
                else
                {
                    add_enemy = true;
                }
                
                if(add_enemy)
                {
                    add_enemy_entity(entities, dungeon, enemy_id, pos.x, pos.y);
                    break;
                }
                }
        }
        }
#endif
    
#if 1
    // Place Items
    for(u32 count = 0; count < spec->item_count; ++count)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, pos))
            {
                b32 add_item = true;
                
                DungeonRoom *room = get_dungeon_room_from_pos(&dungeon->rooms, pos);
                if(room)
                {
                    add_item = add_dungeon_room_feature(room,
                                                            spec->room_max_items,
                                                            DungeonRoomFeatureType_Item);
                    }
                
                if(add_item)
                {
                    b32 is_item_cursed = false;
                    ItemType item_type = get_random_with_chances(random, spec->item_type_chances, 0, 0, RandomChanceType_ItemType);
                    
                    if(is_item_equipment(item_type) &&
                           hit_random_chance(random, spec->item_curse_chance))
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
                                u32 rarity_chance = get_random_chance(random);
                                
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
                            add_weapon_item(random, items, dungeon_level, weapon_id, rarity, pos.x, pos.y, is_item_cursed);
                        } break;
                        
                        case ItemType_Armor:
                        {
                            ItemID armor_id = random_leather_armor(random);
                            
                            if(dungeon->level >= 4)
                            {
                                if(hit_random_chance(random, 50))
                                {
                                    armor_id = random_steel_armor(random);
                                }
                            }
                            
                            assert(is_value_in_range(armor_id, ItemID_ArmorStart, ItemID_ArmorEnd));
                            add_armor_item(random, items, dungeon_level, armor_id, pos.x, pos.y, is_item_cursed);
                        } break;
                        
                        case ItemType_Potion:
                        {
                            ItemID potion_id = get_random_with_chances(random, spec->potion_chances, 0, 0, RandomChanceType_Potion);
                            assert(is_value_in_range(potion_id, ItemID_PotionStart, ItemID_PotionEnd));
                            
                            add_consumable_item(random, items, dungeon_level, potion_id, pos.x, pos.y, 1);
                        } break;
                        
                        case ItemType_Scroll:
                        {
                            ItemID scroll_id = get_random_with_chances(random, spec->scroll_chances, 0, 0, RandomChanceType_Scroll);
                            assert(is_value_in_range(scroll_id, ItemID_ScrollStart, ItemID_ScrollEnd));
                            
                            add_consumable_item(random, items, dungeon_level, scroll_id, pos.x, pos.y, 1);
                        } break;
                        
                        case ItemType_Ration:
                        {
                            add_consumable_item(random, items, dungeon_level, ItemID_Ration, pos.x, pos.y, 1);
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    // Add Traps
    for(u32 count = 0; count < spec->trap_count; ++count)
    {
        DungeonTrap new_trap = {0};
        
        for(;;)
        {
            new_trap.type = get_random_number(random, DungeonTrapType_None + 1, DungeonTrapType_Count - 1);
            new_trap.pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, new_trap.pos))
            {
                add_dungeon_trap(&dungeon->spec, &dungeon->traps, new_trap);
                break;
                }
        }
    }
    
    #if 0
    printf("\nDungeon Traps Created: %u\n\n", spec->create_traps_count);
    
    for(u32 index = 0; index < spec->create_traps_count; ++index)
    {
        Trap *trap = &traps->array[index];
        
        printf("Index %u\n", index);
        printf("Name: %s\n", trap->name);
        printf("Pos: %u, %u\n\n", trap->pos.x, trap->pos.y);
    }
#endif
#endif
    
    return(dungeon);
}