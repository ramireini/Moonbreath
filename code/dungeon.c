internal b32
dungeon_pos_has_multiple_examine_sources()
{
    b32 result = false;
    
    
    return(result);
}

internal v2u
get_random_dungeon_pos_with_type(Random *random,
                                 Dungeon *dungeon,
                                 ItemState *items,
                                 v4u rect,
                                   DungeonRandomType type)
{
    v2u result = {0};
    
    for(;;)
    {
        if(type == DungeonRandomType_TraversablePos)
            {
                result = get_random_dungeon_pos(random, dungeon->size);
                
                if(is_dungeon_pos_traversable(dungeon->tiles, result))
                {
                    break;
                }
            }
        else if(type == DungeonRandomType_TraversableRectPos)
            {
                assert(rect.x);
                assert(rect.y);
                assert(rect.w);
                assert(rect.h);
                
                result = get_random_dungeon_rect_pos(random, rect);
                
                if(is_dungeon_pos_traversable(dungeon->tiles, result))
                {
                    break;
                }
        }
        else if(type == DungeonRandomType_FeaturePos)
        {
            assert(items);
            
             result = get_random_dungeon_pos(random, dungeon->size);
            
            if(can_place_dungeon_feature_on_pos(dungeon, items, result) &&
                   !is_dungeon_pos_water(dungeon->tiles, result))
            {
                break;
            }
        }
        else
        {
            assert(0);
        }
        }
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_pos(Random *random, Dungeon *dungeon)
{
    v2u result = get_random_dungeon_pos_with_type(random,
                                                  dungeon,
                                                      0,
                                                      make_v4u(0, 0, 0, 0),
                                                      DungeonRandomType_TraversablePos);
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_rect_pos(Random *random, Dungeon *dungeon, v4u rect)
{
    v2u result = get_random_dungeon_pos_with_type(random,
                                                      dungeon,
                                                      0,
                                                  rect,
                                                  DungeonRandomType_TraversableRectPos);
    
    return(result);
}

internal v2u
get_random_dungeon_feature_pos(Random *random, Dungeon *dungeon, ItemState *items)
{
    v2u result = get_random_dungeon_pos_with_type(random,
                                                      dungeon,
                                                      items,
                                                      make_v4u(0, 0, 0, 0),
                                                      DungeonRandomType_FeaturePos);
    
    return(result);
}

internal u32
get_dungeon_rect_tile_type_count(DungeonTiles tiles, v4u rect, DungeonTileType type)
{
    u32 result = 0;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            v2u pos = {x, y};
            
            switch(type)
            {
                case DungeonTileType_Floor:
                {
                    if(is_dungeon_pos_floor(tiles, pos))
                    {
                        ++result;
                    }
                } break;
                
                case DungeonTileType_Water:
                {
                    if(is_dungeon_pos_water(tiles, pos))
                    {
                        ++result;
                    }
                } break;
                
                invalid_default_case;
            }
        }
    }
    
    return(result);
}

internal u32
get_dungeon_traversable_pos_count(Dungeon *dungeon)
{
    u32 result = 0;
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            if(is_dungeon_pos_traversable(dungeon->tiles, pos))
            {
                ++result;
            }
        }
    }
    
    return(result);
}

internal b32
is_dungeon_automaton_room_valid(DungeonSpec *spec, DungeonTiles automaton_tiles, v4u room_rect)
{
    b32 result = false;
    u32 floor_count = 0;
    
    for(u32 y = 0; y < room_rect.h; ++y)
    {
        for(u32 x = 0; x < room_rect.w; ++x)
        {
            v2u pos = {x, y};
            
            if(is_dungeon_pos_floor(automaton_tiles, pos))
            {
                ++floor_count;
            }
        }
    }
    
    f32 floor_area = (f32)floor_count / (f32)get_rect_area(room_rect);
    if(floor_area >= spec->automaton_room_min_valid_area)
    {
        result = true;
    }
    
    return(result);
}

internal v4u
get_dungeon_rect_min(DungeonTiles tiles, v4u rect)
{
    v4u result = {0};
    
    u32 highest_x = rect.x;
    u32 highest_y = rect.y;
    
    for(u32 y = rect.y; y < (rect.y + rect.h); ++y)
    {
        for(u32 x = rect.x; x < (rect.x + rect.w); ++x)
        {
            v2u pos = {x, y};
            
            if(is_dungeon_pos_traversable(tiles, pos))
            {
                if(!result.x || x < result.x)
                {
                    result.x = x;
                }
                
                if(!result.y || y < result.y)
                {
                    result.y = y;
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
    
    result.w = (highest_x - result.x) + 1;
    result.h = (highest_y - result.y) + 1;
    
    return(result);
}

internal DungeonTiles
get_dungeon_automaton_gen_tiles(TemporaryMemory *memory, DungeonSpec *spec)
{
    memory_size size = (spec->automaton_room_size.max * spec->automaton_room_size.max) * sizeof(DungeonTile);
    
    DungeonTiles buff_one = {0};
    buff_one.width = spec->automaton_room_size.max;
    buff_one.array = push_memory(memory->arena, size);
    
    return(buff_one);
}

internal DungeonTiles
get_dungeon_automaton_tiles(MemoryArena *memory,
                           Random *random,
                           DungeonSpec *spec,
                           v4u automaton_room_rect)
{
    TemporaryMemory temporary_memory = start_temporary_memory(memory);
    DungeonTiles src_tiles = get_dungeon_automaton_gen_tiles(&temporary_memory, spec);
    DungeonTiles dest_tiles = get_dungeon_automaton_gen_tiles(&temporary_memory, spec);
    
    v4u local_rect = make_v4u(0, 0, automaton_room_rect.w, automaton_room_rect.h);
    
    for(u32 y = local_rect.y; y < local_rect.h; ++y)
    {
        for(u32 x = local_rect.x; x < local_rect.w; ++x)
        {
            v2u pos = {x, y};
            
            if(hit_random_chance(random, spec->automaton_room_init_chance))
            {
                set_dungeon_pos_floor(random, src_tiles, pos);
            }
            else
            {
                set_dungeon_pos_wall(random, src_tiles, pos);
            }
        }
    }
    
    DungeonTiles *src = &src_tiles;
    DungeonTiles *dest = &dest_tiles;
    
    for(u32 index = 0; index < spec->automaton_room_step_count; ++index)
    {
        dungeon_automaton_room_step(random, *src, *dest, local_rect);
        
        DungeonTiles *temp = src;
        src = dest;
        dest = temp;
    }
    
    end_temporary_memory(temporary_memory);
    return(*src);
    }

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

internal b32
is_dungeon_pos_passage(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneStaircaseUp) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneStaircaseDown) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_ExitDungeon));
    
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

internal b32
is_dungeon_pos_trap(DungeonTraps *traps, v2u pos)
{
    b32 result = false;
    
    for(u32 index = 0; index < traps->count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        
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
    
    // TODO(rami): Dungeon trap ideas
    // Summoning traps summoning more than one enemy?
    // Teleporting traps teleporting you into other levels than current?
    // Traps with random effects?
    
    switch(new_trap.type)
    {
        case DungeonTrapType_Spike:
        {
            new_trap.name = "Spikes";
            
            sprintf(new_trap.description,
                    "Activating these spikes will deal %u - %u physical damage.",
                    spec->spike_trap_value.min, spec->spike_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_SpikeTrap);
        } break;
        
        case DungeonTrapType_Sword:
        {
            new_trap.name = "Swords";
            
            sprintf(new_trap.description,
                    "Activating these swords will deal %u - %u physical damage.",
                    spec->sword_trap_value.min, spec->sword_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_SwordTrap);
        } break;
        
        case DungeonTrapType_Arrow:
        {
            new_trap.name = "Arrows";
            
            sprintf(new_trap.description,
                    "Activating these arrows will deal %u - %u physical damage.",
                    spec->arrow_trap_value.min, spec->arrow_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_ArrowTrap);
        } break;
        
        case DungeonTrapType_Magic:
        {
            new_trap.name = "Magical Glyph";
            
            sprintf(new_trap.description,
                    "Stepping on this glyph will deal %u - %u damage of a random damage type.",
                    spec->magic_trap_value.min, spec->magic_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_MagicTrap);
        } break;
        
        case DungeonTrapType_Bind:
        {
            new_trap.name = "Binding Glyph";
            
            sprintf(new_trap.description,
                    "Stepping on this glyph will cause you to be bound for %u - %u turns.",
                    spec->bind_trap_value.min, spec->bind_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_BindTrap);
        } break;
        
        case DungeonTrapType_Shaft:
        {
            new_trap.name = "Shaft";
            
            sprintf(new_trap.description,
                    "Shafts cause you to fall %u - %u dungeon levels.",
                    spec->shaft_trap_value.min, spec->shaft_trap_value.max);
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_ShaftTrap);
        } break;
        
        case DungeonTrapType_Summon:
        {
            new_trap.name = "Summoning Glyph";
            
            sprintf(new_trap.description,
                    "This glyph will summon a random enemy of relative strength into your vicinity.");
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_SummonTrap);
        } break;
        
        case DungeonTrapType_Teleport:
        {
            new_trap.name = "Teleporting Glyph";
            
            sprintf(new_trap.description,
                    "This glyph will transport you to a random location in the dungeon level.");
            
            new_trap.tile_src = get_dungeon_tileset_rect(DungeonTileID_TeleportTrap);
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

internal DungeonTileID
get_dungeon_pos_tile(DungeonTiles tiles, v2u pos)
{
    DungeonTileID result = tiles.array[(pos.y * tiles.width) + pos.x].tile;
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
get_dungeon_tile_tileset_pos(DungeonTileID tile)
{
    v2u result = {0};
    
    switch(tile)
    {
        case DungeonTileID_StoneWall1: result = make_v2u(1, 0); break;
        case DungeonTileID_StoneWall2: result = make_v2u(2, 0); break;
        case DungeonTileID_StoneWall3: result = make_v2u(3, 0); break;
        case DungeonTileID_StoneWall4: result = make_v2u(4, 0); break;
        case DungeonTileID_StoneWall5: result = make_v2u(5, 0); break;
        
        case DungeonTileID_StoneWallTorch1: result = make_v2u(6, 0); break;
        case DungeonTileID_StoneWallTorch2: result = make_v2u(7, 0); break;
        case DungeonTileID_StoneWallTorch3: result = make_v2u(8, 0); break;
        case DungeonTileID_StoneWallTorch4: result = make_v2u(9, 0); break;
        case DungeonTileID_StoneWallTorch5: result = make_v2u(10, 0); break;
        
        case DungeonTileID_StoneWallGrate1: result = make_v2u(11, 0); break;
        case DungeonTileID_StoneWallGrate2: result = make_v2u(12, 0); break;
        
        case DungeonTileID_StoneWallVines1: result = make_v2u(13, 0); break;
        case DungeonTileID_StoneWallVines2: result = make_v2u(14, 0); break;
        case DungeonTileID_StoneWallVines3: result = make_v2u(15, 0); break;
        case DungeonTileID_StoneWallVines4: result = make_v2u(16, 0); break;
        case DungeonTileID_StoneWallVines5: result = make_v2u(17, 0); break;
        
        case DungeonTileID_StoneFloor1: result = make_v2u(18, 0); break;
        case DungeonTileID_StoneFloor2: result = make_v2u(19, 0); break;
        case DungeonTileID_StoneFloor3: result = make_v2u(20, 0); break;
        case DungeonTileID_StoneFloor4: result = make_v2u(21, 0); break;
        
        case DungeonTileID_StoneFloorGrate1: result = make_v2u(22, 0); break;
        case DungeonTileID_StoneFloorGrate2: result = make_v2u(23, 0); break;
        case DungeonTileID_StoneFloorGrate3: result = make_v2u(24, 0); break;
        case DungeonTileID_StoneFloorGrate4: result = make_v2u(25, 0); break;
        case DungeonTileID_StoneFloorGrate5: result = make_v2u(26, 0); break;
        case DungeonTileID_StoneFloorGrate6: result = make_v2u(27, 0); break;
        
        case DungeonTileID_StoneDoorClosed: result = make_v2u(28, 0); break;
        case DungeonTileID_StoneDoorOpen: result = make_v2u(29, 0); break;
        
        case DungeonTileID_StoneStaircaseUp: result = make_v2u(30, 0); break;
        case DungeonTileID_StoneStaircaseDown: result = make_v2u(31, 0); break;
        case DungeonTileID_ExitDungeon: result = make_v2u(32, 0); break;
        
        case DungeonTileID_EntityStatus: result = make_v2u(7, 15); break;
        case DungeonTileID_EntityAlert: result = make_v2u(8, 15); break;
        case DungeonTileID_EntityUnknown: result = make_v2u(9, 15); break;
        
        case DungeonTileID_FootstepsUp: result = make_v2u(14, 3); break;
        case DungeonTileID_FootstepsDown: result = make_v2u(15, 3); break;
        case DungeonTileID_FootstepsLeft: result = make_v2u(16, 3); break;
        case DungeonTileID_FootstepsRight: result = make_v2u(17, 3); break;
        
        case DungeonTileID_FootstepsUpLeft: result = make_v2u(18, 3); break;
        case DungeonTileID_FootstepsUpRight: result = make_v2u(19, 3); break;
        case DungeonTileID_FootstepsDownLeft: result = make_v2u(20, 3); break;
        case DungeonTileID_FootstepsDownRight: result = make_v2u(21, 3); break;
        
        case DungeonTileID_SpikeTrap: result = make_v2u(14, 4); break;
        case DungeonTileID_SwordTrap: result = make_v2u(15, 4); break;
        case DungeonTileID_ArrowTrap: result = make_v2u(16, 4); break;
        case DungeonTileID_MagicTrap: result = make_v2u(17, 4); break;
        case DungeonTileID_BindTrap: result = make_v2u(18, 4); break;
        case DungeonTileID_ShaftTrap: result = make_v2u(19, 4); break;
        case DungeonTileID_SummonTrap: result = make_v2u(20, 4); break;
        case DungeonTileID_TeleportTrap: result = make_v2u(21, 4); break;
        
        case DungeonTileID_Water1: result = make_v2u(52, 0); break;
        case DungeonTileID_Water2: result = make_v2u(53, 0); break;
        case DungeonTileID_Water3: result = make_v2u(54, 0); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_dungeon_tileset_pos(DungeonTiles tiles, v2u tile_pos)
{
    DungeonTileID tile = get_dungeon_pos_tile(tiles, tile_pos);
    v2u result = get_dungeon_tile_tileset_pos(tile);
    
    return(result);
}

internal v4u
get_dungeon_tileset_rect(DungeonTileID tile)
{
    v2u tileset_pos = get_dungeon_tile_tileset_pos(tile);
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
get_dungeon_tile_name(DungeonTileID tile)
{
    char *result = "";
    
    switch(tile)
    {
        case DungeonTileID_StoneWall1:
        case DungeonTileID_StoneWall2:
        case DungeonTileID_StoneWall3:
        case DungeonTileID_StoneWall4:
        case DungeonTileID_StoneWall5:
        
        case DungeonTileID_StoneWallTorch1:
        case DungeonTileID_StoneWallTorch2:
        case DungeonTileID_StoneWallTorch3:
        case DungeonTileID_StoneWallTorch4:
        case DungeonTileID_StoneWallTorch5:
        
        case DungeonTileID_StoneWallGrate1:
        case DungeonTileID_StoneWallGrate2:
        
        case DungeonTileID_StoneWallVines1:
        case DungeonTileID_StoneWallVines2:
        case DungeonTileID_StoneWallVines3:
        case DungeonTileID_StoneWallVines4:
        case DungeonTileID_StoneWallVines5: result = "Wall"; break;
        
        case DungeonTileID_StoneFloor1:
        case DungeonTileID_StoneFloor2:
        case DungeonTileID_StoneFloor3:
        case DungeonTileID_StoneFloor4:
        
        case DungeonTileID_StoneFloorGrate1:
        case DungeonTileID_StoneFloorGrate2:
        case DungeonTileID_StoneFloorGrate3:
        case DungeonTileID_StoneFloorGrate4:
        case DungeonTileID_StoneFloorGrate5:
        case DungeonTileID_StoneFloorGrate6: result = "Floor"; break;
        
        case DungeonTileID_StoneDoorClosed:
        case DungeonTileID_StoneDoorOpen: result = "Door"; break;
        
        case DungeonTileID_StoneStaircaseUp:
        case DungeonTileID_StoneStaircaseDown: result = "Staircase"; break;
        
        case DungeonTileID_ExitDungeon: result = "Dungeon Exit"; break;
        
        case DungeonTileID_Water1:
        case DungeonTileID_Water2:
        case DungeonTileID_Water3: result = "Water"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_dungeon_tile_info_text(DungeonTileID tile)
{
    char *result = 0;
    
    switch(tile)
    {
        case DungeonTileID_StoneWall1: break;
        case DungeonTileID_StoneWall2: break;
        case DungeonTileID_StoneWall3: break;
        case DungeonTileID_StoneWall4: break;
        case DungeonTileID_StoneWall5: break;
        
        case DungeonTileID_StoneWallTorch1: break;
        case DungeonTileID_StoneWallTorch2: break;
        case DungeonTileID_StoneWallTorch3: break;
        case DungeonTileID_StoneWallTorch4: break;
        case DungeonTileID_StoneWallTorch5: break;
        
        case DungeonTileID_StoneWallGrate1: break;
        case DungeonTileID_StoneWallGrate2: break;
        
        case DungeonTileID_StoneWallVines1: break;
        case DungeonTileID_StoneWallVines2: break;
        case DungeonTileID_StoneWallVines3: break;
        case DungeonTileID_StoneWallVines4: break;
        case DungeonTileID_StoneWallVines5: break;
        
        case DungeonTileID_StoneFloor1: break;
        case DungeonTileID_StoneFloor2: break;
        case DungeonTileID_StoneFloor3: break;
        case DungeonTileID_StoneFloor4: break;
        
        case DungeonTileID_StoneFloorGrate1: break;
        case DungeonTileID_StoneFloorGrate2: break;
        case DungeonTileID_StoneFloorGrate3: break;
        case DungeonTileID_StoneFloorGrate4: break;
        case DungeonTileID_StoneFloorGrate5: break;
        case DungeonTileID_StoneFloorGrate6: break;
        
        case DungeonTileID_StoneDoorClosed: break;
        case DungeonTileID_StoneDoorOpen: break;
        
        case DungeonTileID_StoneStaircaseUp:
        case DungeonTileID_StoneStaircaseDown: result = "A pathway leading somewhere."; break;
        
        case DungeonTileID_ExitDungeon: result = "A way out from the dungeon."; break;
        
        case DungeonTileID_Water1:
        case DungeonTileID_Water2:
        case DungeonTileID_Water3: result = "Flowing shallow water."; break;
        
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
is_dungeon_pos_torch(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch4) ||
                      is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch5));
    
    return(result);
}

internal b32
is_dungeon_pos_wall(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall4) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall5) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall5) ||
                  
                      is_dungeon_pos_torch(tiles, pos) ||
                  
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallGrate1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallGrate2) ||
                  
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallVines1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallVines2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallVines3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallVines4) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallVines5));
    
    return(result);
}

internal b32
is_dungeon_pos_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_open_door(tiles, pos) ||
                      is_dungeon_pos_closed_door(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorOpen));
    return(result);
}

internal b32
is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorClosed));
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
is_dungeon_pos_water(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_Water1) ||
                      is_dungeon_pos_tile(tiles, pos, DungeonTileID_Water2) ||
                      is_dungeon_pos_tile(tiles, pos, DungeonTileID_Water3));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_floor(tiles, pos) ||
                  is_dungeon_pos_passage(tiles, pos) ||
                      is_dungeon_pos_open_door(tiles, pos) ||
                      is_dungeon_pos_water(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile)
{
    b32 result = (get_dungeon_pos_tile(tiles, pos) == tile);
    return(result);
}

internal b32
is_dungeon_pos_floor(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor4) ||
                  
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate4) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate5) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloorGrate6));
    
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
get_dungeon_dimension_rect(v2u dungeon_size, v2u pos, u32 dimension)
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
    else if(result.x > dungeon_size.w - 1)
    {
        result.x = dungeon_size.w - 1;
    }
    
    if((s32)result.y < 0)
    {
        result.y = 0;
    }
    else if(result.y > dungeon_size.h - 1)
    {
        result.y = dungeon_size.h - 1;
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
    
    assert(is_pos_inside_dungeon(dungeon_size, result));
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
get_random_dungeon_rect_pos(Random *random, v4u rect)
{
    v2u result =
    {
        result.x = get_random_number(random, rect.x, (rect.x + rect.w) - 1),
        result.y = get_random_number(random, rect.y, (rect.y + rect.h) - 1)
    };
    
    return(result);
}

internal DungeonTileID
get_dungeon_pos_remains(DungeonTiles tiles, v2u pos)
{
    DungeonTileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains;
    return(remains);
}

internal b32
can_place_dungeon_remains_on_pos(DungeonTiles tiles, v2u pos)
{
    b32 result = (!get_dungeon_pos_remains(tiles, pos) && !is_dungeon_pos_passage(tiles, pos));
    return(result);
}

internal void
set_dungeon_pos_remains(DungeonTiles tiles, v2u pos, DungeonTileID tile)
{
    tiles.array[(pos.y * tiles.width) + pos.x].remains = tile;
}

internal void
set_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile)
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
    set_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorOpen);
}

internal void
set_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos)
{
    set_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorClosed);
}

internal void
set_dungeon_pos_wall(Random *random, DungeonTiles tiles, v2u pos)
{
     DungeonTileID tile = get_random_number(random, DungeonTileID_StoneWall1, DungeonTileID_StoneWall5);
    set_dungeon_pos_tile(tiles, pos, tile);
}

internal void
set_dungeon_pos_floor(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile = get_random_number(random, DungeonTileID_StoneFloor1, DungeonTileID_StoneFloor4);
    set_dungeon_pos_tile(tiles, pos, tile);
}

internal void
set_dungeon_pos_torch(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile = get_random_number(random, DungeonTileID_StoneWallTorch1, DungeonTileID_StoneWallTorch5);
    set_dungeon_pos_tile(tiles, pos, tile);
}

internal void
set_dungeon_pos_water(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile = get_random_number(random, DungeonTileID_Water1, DungeonTileID_Water3);
    set_dungeon_pos_tile(tiles, pos, tile);
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
is_dungeon_rect_wall(DungeonTiles tiles, v4u rect)
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
                      is_dungeon_rect_wall(dungeon->tiles, rect));
    
    return(result);
    }

internal u32
get_dungeon_pos_area_tile_type_count(DungeonTiles tiles,
                                     v2u pos,
                                     u32 area,
                                     DungeonTileType type,
                                     b32 ignore_center)
{
    u32 result = 0;
    
    s32 start_x = pos.x - area;
    if(start_x < 0)
    {
        start_x = 0;
    }
    
    s32 start_y = pos.y - area;
    if(start_y < 0)
    {
        start_y = 0;
    }
    
    for(u32 y = start_y; y < (pos.y + area + 1); ++y)
    {
        for(u32 x = start_x; x < (pos.x + area + 1); ++x)
        {
            v2u local_pos = {x, y};
            
            if(ignore_center &&
               is_v2u_equal(local_pos, pos))
                {
                    continue;
                }
            
                switch(type)
                {
                    case DungeonTileType_Floor:
                    {
                        if(is_dungeon_pos_floor(tiles, local_pos))
                        {
                            ++result;
                        }
                    } break;
                    
                    case DungeonTileType_Torch:
                    {
                        if(is_dungeon_pos_torch(tiles, local_pos))
                        {
                            ++result;
                        }
                    } break;
                    
                    case DungeonTileType_Door:
                    {
                        if(is_dungeon_pos_door(tiles, local_pos))
                        {
                            ++result;
                        }
                    } break;
                    
                    invalid_default_case;
            }
        }
        }

        return(result);
}

internal u32
get_dungeon_pos_neighbour_tile_count(DungeonTiles tiles, v2u pos, DungeonTileType type)
{
    u32 result = get_dungeon_pos_area_tile_type_count(tiles, pos, 1, type, true);
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
reset_not_flooded_dungeon_tiles(Random *random, Dungeon *dungeon, b32 *fill_tiles)
{
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            if(!fill_tiles[(y * dungeon->tiles.width) + x])
            {
                v2u pos = {x, y};
                set_dungeon_pos_wall(random, dungeon->tiles, pos);
                }
        }
    }
}

internal u32
flood_dungeon(DungeonTiles tiles, b32 *flood_array, u32 flood_count, v2u pos)
{
    if(!flood_array[(pos.y * tiles.width) + pos.x] &&
           is_dungeon_pos_traversable(tiles, pos))
    {
        flood_array[(pos.y * tiles.width) + pos.x] = true;
        ++flood_count;
        
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_Up));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_Down));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_Left));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_Right));
        
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_UpLeft));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_UpRight));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_DownLeft));
        flood_count = flood_dungeon(tiles, flood_array, flood_count, get_direction_pos(pos, Direction_DownRight));
    }
    
    return(flood_count);
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

internal void
dungeon_automaton_room_step(Random *random,
                            DungeonTiles src_tiles,
                            DungeonTiles dest_tiles,
                            v4u rect)
{
    for(u32 y = 0; y < rect.h; ++y)
    {
        for(u32 x = 0; x < rect.w; ++x)
        {
            v2u src_pos = {rect.x + x, rect.y + y};
            v2u dest_pos = {x, y};
            
            u32 floor_count = get_dungeon_pos_neighbour_tile_count(src_tiles, src_pos,
                                                                   DungeonTileType_Floor);
            
            if(is_dungeon_pos_floor(src_tiles, src_pos))
            {
                if(floor_count <= 3)
                {
                    set_dungeon_pos_wall(random, dest_tiles, dest_pos);
                }
                else
                {
                    set_dungeon_pos_floor(random, dest_tiles, dest_pos);
                }
            }
            else if(is_dungeon_pos_wall(src_tiles, src_pos))
            {
                if(floor_count >= 6)
                {
                    set_dungeon_pos_floor(random, dest_tiles, dest_pos);
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
place_dungeon_automaton_room(DungeonTiles src_tiles,
                             DungeonTiles dest_tiles,
                             v4u src_room,
                             b32 is_water)
{
    for(u32 y = 0; y < src_room.h; ++y)
    {
        for(u32 x = 0; x < src_room.w; ++x)
        {
            v2u src_pos = {x, y};
            v2u dest_pos = {src_room.x + x, src_room.y + y};
            
            DungeonTileID src_tile = get_dungeon_pos_tile(src_tiles, src_pos);
            
            if(is_water && !is_dungeon_pos_water(src_tiles, src_pos))
            {
                continue;
            }
            
            set_dungeon_pos_tile(dest_tiles, dest_pos, src_tile);
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
            set_dungeon_pos_floor(random, tiles, make_v2u(x, y));
        }
    }
}

internal DungeonRoom
create_and_place_dungeon_room(MemoryArena *memory, Random *random, Dungeon *dungeon)
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
        
        assert(result.rect.w);
        assert(result.rect.h);
        
        // Padding ensures rooms won't touch eachother
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
                v4u circle_rect =
                {
                    result.rect.x,
                    result.rect.y,
                    result.rect.w + 1,
                    result.rect.h + 1
                };
                
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
                                
                                set_dungeon_pos_floor(random, dungeon->tiles, pos);
                            }
                        }
                    }
                    
                    result.success = true;
                    result.rect = circle_rect;
                }
                }
            else if(room_type == DungeonRoomType_Automaton)
            {
                
                DungeonTiles automaton_tiles = get_dungeon_automaton_tiles(memory, random, spec, result.rect);
                
#if 0
                // Automaton room before tests for being valid
                place_dungeon_automaton_room(automaton_tiles, result.rect, dungeon->tiles);
                
                result.success = true;
                return(result);
#endif
                
                    if(is_dungeon_automaton_room_valid(spec, automaton_tiles, result.rect))
                {
                    place_dungeon_automaton_room(automaton_tiles, dungeon->tiles, result.rect, false);
                    result.rect = get_dungeon_rect_min(dungeon->tiles, result.rect);
                    
                    result.success = true;
                }
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
    
    // TODO(rami): Spec Start
    
    DungeonSpec *spec = &dungeon->spec;
    
    spec->size.w = 64;
    spec->size.h = 64;
    spec->area = get_size_area(spec->size);
    
    spec->flood_traversable_min = 0.9f;
    
    spec->has_water = true;
    spec->water_min_total_area = 0.1f;
    spec->water_placement_min_floor_count = 2;
    
    spec->torch_count = spec->area * 0.02f;
    spec->door_count = spec->area * 0.004f;
    spec->door_min_spacing = 4;
    
    spec->room_retry_count = 10;
    
    spec->automaton_room_min_valid_area = 0.4f;
    spec->automaton_room_init_chance = 55;
    spec->automaton_room_step_count = 4;
    
    spec->room_max_items = 3;
    spec->room_max_enemies = 3;
    
    spec->room_type_chances[DungeonRoomType_Rect] = 20;
    spec->room_type_chances[DungeonRoomType_DoubleRect] = 40;
    spec->room_type_chances[DungeonRoomType_Circle] = 40;
    spec->room_type_chances[DungeonRoomType_Automaton] = 60;
    
    spec->corridor_type_chances[DungeonCorridorType_Turn] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Zigzag] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Diagonal] = 30;
    
    spec->rect_room_size = make_v2u(4, 8);
    spec->double_rect_room_size = make_v2u(4, 8);
    spec->circle_room_size = make_v2u(2, 5);
    spec->automaton_room_size = make_v2u(10, 18);
    spec->automaton_water_size = make_v2u(8, 16);
    
    spec->trap_count = get_random_number(random, 8, 16);
    
    spec->spike_trap_value = make_v2u(4, 12);
    spec->sword_trap_value = make_v2u(18, 35);
    spec->arrow_trap_value = make_v2u(2, 14);
    spec->magic_trap_value = make_v2u(4, 24);
    spec->bind_trap_value = make_v2u(3, 6);
    spec->shaft_trap_value = make_v2u(1, 2);
    
    spec->down_passage_count = get_random_number(random, 1, 3);
    
    spec->enemy_count = (spec->size.w + spec->size.h) * 0.15f;
    
    spec->item_count = (spec->size.w + spec->size.h) * 0.2f;
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
    
    dungeon->size.w = 64;
    dungeon->size.h = 64;
    dungeon->area = spec->area;
    dungeon->rect = make_v4u(0, 0, dungeon->size.w, dungeon->size.h);
    
    dungeon->tiles.width = dungeon->size.w;
    dungeon->ready_for_pathfinding = false;
    
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
    
#if 1
    // Create a test room
    dungeon->ready_for_pathfinding = true;
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            if(!x || x == (dungeon->size.w - 1) ||
                   !y || y == (dungeon->size.h - 1))
            {
                set_dungeon_pos_wall(random, dungeon->tiles, make_v2u(x, y));
            }
            else
            {
                set_dungeon_pos_floor(random, dungeon->tiles, make_v2u(x, y));
            }
        }
    }
    
    #if 0
    set_dungeon_pos_tile(dungeon->tiles, make_v2u(10, 15), DungeonTileID_Water1);
    set_dungeon_pos_tile(dungeon->tiles, make_v2u(10, 16), DungeonTileID_Water1);
    set_dungeon_pos_tile(dungeon->tiles, make_v2u(11, 15), DungeonTileID_Water1);
    set_dungeon_pos_tile(dungeon->tiles, make_v2u(11, 16), DungeonTileID_Water1);
    #endif
    
        move_entity(random, player, dungeon->tiles, ui, make_v2u(24, 2));
    
    add_enemy_entity(entities, dungeon, EntityID_OrcWarrior, 12, 15);
    //add_enemy_entity(entities, dungeon, EntityID_Rat, 12, 15);
    
    //add_enemy_entity(entities, dungeon, EntityID_Python, 7, 1);
    //add_enemy_entity(entities, dungeon, EntityID_SkeletonMage, 6, 1);
    
    //add_enemy_entity(entities, dungeon, EntityID_Dummy, 5, 5);
    //add_enemy_entity(entities, dungeon, EntityID_ShadowWalker, 5, 5);
    
    //add_enemy_entity(entities, dungeon, EntityID_KoboldShaman, 5, 5);
    //add_enemy_entity(entities, dungeon, EntityID_OrcWarrior, 6, 5);
    
    //add_enemy_entity(entities, dungeon, EntityID_Rat, 7, 1);
    //add_enemy_entity(entities, dungeon, EntityID_Bat, 15, 5);
    //add_enemy_entity(entities, dungeon, EntityID_Dummy, 6, 7);
    //add_enemy_entity(entities, dungeon, EntityID_KoboldShaman, 5, 7);
    //add_enemy_entity(entities, dungeon, EntityID_Python, 5, 5);
    
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
    
    // Place Rooms
    u32 total_room_size = 0;
    
    //while(dungeon->room_count < 10)
    while((f32)total_room_size / (f32)dungeon->area < 0.4f)
    {
        DungeonRoom created_room = create_and_place_dungeon_room(&game->memory_arena,
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
    // Connect rooms
    dungeon->ready_for_pathfinding = true;
    
    b32 is_connected[rooms->count];
    zero_struct(is_connected);
    
    for(u32 start_index = 0; start_index < (rooms->count - 1); ++start_index)
    {
        u32 closest_distance = U32_MAX;
        
        DungeonRoom *start_room = &rooms->array[start_index];
        v2u start_room_center = get_rect_center(start_room->rect);
        
        DungeonRoom *end_room = 0;
        
        // Find closest room to start room
        for(u32 end_index = 0; end_index < rooms->count; ++end_index)
        {
            if((start_index != end_index) && !is_connected[end_index])
            {
                DungeonRoom *loop_end_room = &rooms->array[end_index];
                v2u end_room_center = get_rect_center(loop_end_room->rect);
                
                u32 new_distance = get_cardinal_distance(start_room_center, end_room_center);
                if(new_distance < closest_distance)
                {
                    closest_distance = new_distance;
                    
                    end_room = loop_end_room;
                }
            }
        }
        
        // Connect start and end room
        if(end_room)
        {
            v2u start_pos = get_random_dungeon_traversable_rect_pos(random, dungeon, start_room->rect);
            v2u end_pos = get_random_dungeon_traversable_rect_pos(random, dungeon, end_room->rect);
            
                        DungeonCorridorType type = get_random_with_chances(random,
                                                                                 spec->corridor_type_chances,
                                                                               DungeonCorridorType_None + 1,
                                                                               DungeonCorridorType_Count - 1,
                                                                                 RandomChanceType_Normal);
            
            #if 0
            printf("Start Pos: %u, %u\n", start_pos.x, start_pos.y);
            printf("End Pos: %u, %u\n", end_pos.x, end_pos.y);
            printf("Type: %u\n\n", type);
            #endif
            
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
                                    set_dungeon_pos_floor(random, dungeon->tiles, start_pos);
                                    start_pos.x += x_direction;
                                }
                                
                                while(start_pos.y != end_pos.y)
                                {
                                    set_dungeon_pos_floor(random, dungeon->tiles, start_pos);
                                    start_pos.y += y_direction;
                                }
                            } break;
                            
                            case DungeonCorridorType_Zigzag:
                            {
                                while(!is_v2u_equal(start_pos, end_pos))
                                {
                                    if(start_pos.x != end_pos.x)
                                    {
                                        set_dungeon_pos_floor(random, dungeon->tiles, start_pos);
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        set_dungeon_pos_floor(random, dungeon->tiles, start_pos);
                                        start_pos.y += y_direction;
                                    }
                                }
                            } break;
                            
                            case DungeonCorridorType_Diagonal:
                            {
                                while(!is_v2u_equal(start_pos, end_pos))
                                {
                                    set_dungeon_pos_floor(random, dungeon->tiles, start_pos);
                                    
                                    if(start_pos.x != end_pos.x)
                                    {
                                        start_pos.x += x_direction;
                                    }
                                    
                                    if(start_pos.y != end_pos.y)
                                    {
                                        start_pos.y += y_direction;
                                    }
                                }
                                
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        is_connected[start_index] = true;
                    }
                }
    #endif
    
    #if 1
    // Place Water
    if(spec->has_water)
    {
        f32 total_water_area = 0.0f;
        
        while(total_water_area < spec->water_min_total_area)
        {
            v2u random_pos = get_random_dungeon_pos(random, dungeon->size);
            
            v4u water_rect = {0};
            water_rect.x = random_pos.x;
            water_rect.y = random_pos.y;
            water_rect.w = get_random_number_from_v2u(random, spec->automaton_water_size);
            water_rect.h = get_random_number_from_v2u(random, spec->automaton_water_size);
            
            if(is_rect_inside_dungeon(dungeon->size, water_rect))
            {
                DungeonTiles automaton_tiles = get_dungeon_automaton_tiles(&game->memory_arena, random, spec, water_rect);
                
                if(is_dungeon_automaton_room_valid(spec, automaton_tiles, water_rect))
                {
                    // Set all automaton floor tiles as water
                    for(u32 y = 0; y < water_rect.h; ++y)
                    {
                        for(u32 x = 0; x < water_rect.w; ++x)
                        {
                            v2u pos = {x, y};
                            
                            if(is_dungeon_pos_floor(automaton_tiles, pos))
                            {
                                set_dungeon_pos_water(random, automaton_tiles, pos);
                            }
                        }
                    }
                    
                    place_dungeon_automaton_room(automaton_tiles, dungeon->tiles, water_rect, true);
                    water_rect = get_dungeon_rect_min(dungeon->tiles, water_rect);
                    
                    u32 water_count = get_dungeon_rect_tile_type_count(dungeon->tiles, water_rect, DungeonTileType_Water);
                    f32 water_area = (f32)water_count / (f32)dungeon->area;
                    total_water_area += water_area;
                }
            }
        }
    }
    #endif
    
    #if 1
    // Reset Unreachable Tiles
    TemporaryMemory temporary_memory = start_temporary_memory(&game->memory_arena);
    
    memory_size flood_size = dungeon->area * sizeof(b32);
    b32 *flood_array = push_memory(temporary_memory.arena, flood_size);
    
    u32 dungeon_traversable_count = get_dungeon_traversable_pos_count(dungeon);
    
    for(;;)
    {
        zero_size(flood_array, flood_size);
        
        v2u flood_pos = get_random_dungeon_traversable_pos(random, dungeon);
        u32 flood_count = flood_dungeon(dungeon->tiles, flood_array, 0, flood_pos);
        
#if 0
        printf("Flood Pos: %u, %u\n", flood_pos.x, flood_pos.y);
        printf("Flood Count: %u\n", flood_count);
        printf("Dungeon Traversable Count: %u\n\n", dungeon_traversable_count);
#endif
        
        if(flood_count >= (dungeon_traversable_count * spec->flood_traversable_min))
        {
            break;
        }
    }
    
    reset_not_flooded_dungeon_tiles(random, dungeon, flood_array);
    end_temporary_memory(temporary_memory);
#endif
    
#if 1
    // Place Torches
    for(u32 count = 0; count < spec->torch_count; ++count)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(is_dungeon_pos_wall(dungeon->tiles, pos))
            {
                if(get_dungeon_pos_neighbour_tile_count(dungeon->tiles, pos, DungeonTileType_Floor))
                {
                    if(!get_dungeon_pos_neighbour_tile_count(dungeon->tiles, pos, DungeonTileType_Torch))
                    {
                        set_dungeon_pos_torch(random, dungeon->tiles, pos);
                        break;
                    }
                }
            }
        }
    }
#endif
    
#if 1
    // Place Doors
    for(u32 count = 0; count < spec->door_count; ++count)
    {
        for(;;)
        {
            v2u pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(is_dungeon_pos_traversable(dungeon->tiles, pos))
            {
                u32 door_count = get_dungeon_pos_area_tile_type_count(dungeon->tiles, pos,
                                                                    spec->door_min_spacing,
                                                                          DungeonTileType_Door, false);
                
                if(!door_count)
                {
                    v2u up = get_direction_pos(pos, Direction_Up);
                    v2u down = get_direction_pos(pos, Direction_Down);
                    v2u left = get_direction_pos(pos, Direction_Left);
                    v2u right = get_direction_pos(pos, Direction_Right);
                    
                    b32 can_place = false;
                    
                    if(is_dungeon_pos_wall(dungeon->tiles, up) &&
                       is_dungeon_pos_wall(dungeon->tiles, down))
                    {
                        if(!is_dungeon_pos_wall(dungeon->tiles, left) &&
                               !is_dungeon_pos_wall(dungeon->tiles, right))
                        {
                            can_place = true;
                        }
                    }
                    else if(is_dungeon_pos_wall(dungeon->tiles, left) &&
                            is_dungeon_pos_wall(dungeon->tiles, right))
                    {
                        if(!is_dungeon_pos_wall(dungeon->tiles, up) &&
                               !is_dungeon_pos_wall(dungeon->tiles, down))
                        {
                        can_place = true;
                        }
                    }
                    
                    if(can_place)
                    {
                        set_dungeon_pos_closed_door(dungeon->tiles, pos);
                        break;
                    }
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
        up_passage.pos = get_random_dungeon_feature_pos(random, dungeon, items);
            
                if(dungeon->level == 1)
                {
                    set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, DungeonTileID_ExitDungeon);
                }
                else
                {
                    set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, DungeonTileID_StoneStaircaseUp);
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
        }
    #endif
    
    #if 1
    // Place Down Passages
    for(u32 count = 0; count < spec->down_passage_count; ++count)
    {
        DungeonPassage down_passage = {0};
        down_passage.type = DungeonPassageType_Down;
        
            down_passage.pos = get_random_dungeon_feature_pos(random, dungeon, items);
            
                set_dungeon_pos_tile(dungeon->tiles, down_passage.pos, DungeonTileID_StoneStaircaseDown);
                add_dungeon_passage(&dungeon->passages, down_passage);
    }
    #endif
    
#if 1
    // Place Traps
    for(u32 count = 0; count < spec->trap_count; ++count)
    {
        DungeonTrap new_trap = {0};
        
            new_trap.type = get_random_number(random, DungeonTrapType_None + 1, DungeonTrapType_Count - 1);
            new_trap.pos = get_random_dungeon_feature_pos(random, dungeon, items);
            
                add_dungeon_trap(&dungeon->spec, &dungeon->traps, new_trap);
        }
#endif
    
#if 1
    // Place Items
    for(u32 count = 0; count < spec->item_count; ++count)
    {
            v2u pos = get_random_dungeon_feature_pos(random, dungeon, items);
            
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
        
            v2u pos = get_random_dungeon_feature_pos(random, dungeon, items);
            
                b32 add_enemy = false;
                
                DungeonRoom *room = get_dungeon_room_from_pos(&dungeon->rooms, pos);
                if(room)
                {
            add_enemy = add_dungeon_room_feature(room, spec->room_max_enemies,
                                                 DungeonRoomFeatureType_Enemy);
                }
                else
                {
                    add_enemy = true;
                }
                
                if(add_enemy)
                {
                    add_enemy_entity(entities, dungeon, enemy_id, pos.x, pos.y);
                }
    }
#endif
    
#if 0
    printf("Spec Torch Count: %u\n", spec->torch_count);
    printf("Spec Door Count: %u\n\n", spec->door_count);
    
    // Print room types
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
    
    for(DungeonRoomType type = DungeonRoomType_None + 1;
        type < DungeonRoomType_Count;
        ++type)
    {
        printf("%s: %u\n", get_dungeon_room_type_string(type), room_type_count[type]);
    }
    
    // Print passages
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &dungeon->passages.array[index];
        
        if(passage->type)
        {
            printf("\nPassage[%u]\n", index);
            printf("Type: %s\n", passage->type == DungeonPassageType_Up ? "Up" : "Down");
            printf("Pos: %u, %u\n", passage->pos.x, passage->pos.y);
            printf("Destination Pos: %u, %u\n", passage->dest_pos.x, passage->dest_pos.y);
        }
    }
    
    printf("\nSpec Trap Count: %u\n\n", spec->trap_count);
    
    for(u32 index = 0; index < spec->trap_count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        
        printf("Index %u\n", index);
        printf("Name: %s\n", trap->name);
        printf("Pos: %u, %u\n\n", trap->pos.x, trap->pos.y);
    }
    #endif
    
    return(dungeon);
}