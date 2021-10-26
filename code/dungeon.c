#if MOONBREATH_SLOW
internal void
debug_place_dungeon_room(Random *random, DungeonTiles tiles, v4u room)
{
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            v2u pos = {x, y};
            set_dungeon_pos_floor(random, tiles, pos);
        }
    }
}
#endif

internal b32
process_new_seen_dungeon_traps(DungeonTrap *array, DungeonTiles tiles, UI *ui, b32 log_names)
{
    b32 result = false;
    
    for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
    {
        DungeonTrap *trap = &array[index];
        
        if(trap->type &&
           is_tile_seen(tiles, trap->pos) &&
           !is_set(trap->flags, DungeonTrapFlag_HasBeenSeen))
        {
            if(log_names)
            {
                log_add_comes_into_your_view_string(trap->name.s, ui);
            }
            
            set(trap->flags, DungeonTrapFlag_HasBeenSeen);
            result = true;
        }
    }
    
    return(result);
}

internal DungeonRemainsInfo
get_dungeon_pos_remains_info(DungeonTiles tiles, v2u tile_pos)
{
    DungeonRemainsInfo result = {0};
    
    result.tile_id = get_dungeon_pos_remains_tile_id(tiles, tile_pos);
    if(result.tile_id)
    {
        v2u tileset_pos = get_dungeon_tileset_pos_from_tile_id(result.tile_id);
        result.src = get_dungeon_tile_rect(tileset_pos);
    }
    
    return(result);
}

internal void
place_dungeon_corridor(Random *random,
                       DungeonTiles tiles,
                       v2u start,
                       v2u end,
                       DungeonCorridorType type)
{
    assert(random);
    assert(!is_v2u_zero(start));
    assert(!is_v2u_zero(end));
    assert(type);
    
    s32 x_direction = 0;
    if(start.x <= end.x)
    {
        x_direction = 1;
    }
    else
    {
        x_direction = -1;
    }
    
    s32 y_direction = 0;
    if(start.y <= end.y)
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
            while(start.x != end.x)
            {
                assert_loop_count();
                
                set_dungeon_pos_floor(random, tiles, start);
                start.x += x_direction;
            }
            
            while(start.y != end.y)
            {
                assert_loop_count();
                
                set_dungeon_pos_floor(random, tiles, start);
                start.y += y_direction;
            }
        } break;
        
        case DungeonCorridorType_Zigzag:
        {
            while(!is_v2u_equal(start, end))
            {
                assert_loop_count();
                
                if(start.x != end.x)
                {
                    set_dungeon_pos_floor(random, tiles, start);
                    start.x += x_direction;
                }
                
                if(start.y != end.y)
                {
                    set_dungeon_pos_floor(random, tiles, start);
                    start.y += y_direction;
                }
            }
        } break;
        
        case DungeonCorridorType_Diagonal:
        {
            while(!is_v2u_equal(start, end))
            {
                assert_loop_count();
                
                set_dungeon_pos_floor(random, tiles, start);
                if(start.x != end.x) start.x += x_direction;
                if(start.y != end.y) start.y += y_direction;
            }
            
        } break;
        
        invalid_default_case;
    }
}

internal b32
is_pos_on_rect_edge(v4u rect, v2u pos)
{
    assert(!is_v4u_zero(rect));
    assert(!is_v2u_zero(pos));
    
    b32 result = (pos.x == rect.x ||
                  pos.x == rect.x + rect.w - 1 ||
                  pos.y == rect.y ||
                  pos.y == rect.y + rect.h - 1);
    
    return(result);
}

internal v2u
get_new_dungeon_passage_pos(Random *random, Dungeon *dungeon, ItemState *item_state, u32 passage_min_spacing)
{
    assert(passage_min_spacing);
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_feature_pos(random, dungeon, item_state, false);
        if(is_dungeon_pos_traversable(dungeon->tiles, get_direction_pos(result, Direction_Up)) &&
           is_dungeon_pos_traversable(dungeon->tiles, get_direction_pos(result, Direction_Down)) &&
           is_dungeon_pos_traversable(dungeon->tiles, get_direction_pos(result, Direction_Left)) &&
           is_dungeon_pos_traversable(dungeon->tiles, get_direction_pos(result, Direction_Right)) &&
           !get_dungeon_area_tile_type_count(dungeon->tiles, result, passage_min_spacing, DungeonTileType_Passage))
        {
            break;
        }
    }
    
    return(result);
}

internal b32
is_type_info_valid(Random *random, ItemTypeInfo info)
{
    assert(random);
    
    b32 result = (info.valid && hit_random_chance(random, info.chance));
    return(result);
}

internal void
render_tile_on_dest(SDL_Renderer *renderer, SDL_Texture *texture, v4u tile_dest, DungeonTileID id)
{
    assert(renderer);
    assert(texture);
    assert(!is_v4u_zero(tile_dest));
    assert(id);
    
    v4u tile_src = get_dungeon_tileset_rect(id);
    render_texture(renderer, texture, &tile_src, &tile_dest, false, false);
}

internal u32
get_dungeon_pos_examine_source_count(Dungeon *dungeon, EntityState *entity_state, ItemState *item_state, v2u pos)
{
    assert(dungeon);
    assert(entity_state);
    assert(item_state);
    assert(!is_v2u_zero(pos));
    
    u32 result = 0;
    
    result += get_dungeon_pos_item_count(item_state, dungeon->level, pos);
    if(get_dungeon_pos_trap(&dungeon->traps, pos)) ++result;
    
    Entity *entity = get_dungeon_pos_entity(dungeon->tiles, pos);
    if(entity && entity->type != EntityType_Player)
    {
        ++result;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_pos(Random *random, Dungeon *dungeon)
{
    assert(random);
    assert(dungeon);
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_pos(random, dungeon->size);
        if(is_dungeon_pos_traversable(dungeon->tiles, result)) break;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_unoccupied_pos(Random *random, Dungeon *dungeon)
{
    assert(random);
    assert(dungeon);
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_pos(random, dungeon->size);
        if(is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, result)) break;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_rect_pos(Random *random, Dungeon *dungeon, v4u rect)
{
    assert(random);
    assert(dungeon);
    assert(!is_v4u_zero(rect));
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_rect_pos(random, rect);
        if(is_dungeon_pos_traversable(dungeon->tiles, result)) break;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_traversable_unoccupied_rect_pos(Random *random, Dungeon *dungeon, v4u rect)
{
    assert(random);
    assert(dungeon);
    assert(!is_v4u_zero(rect));
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_rect_pos(random, rect);
        if(is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, result)) break;
    }
    
    return(result);
}

internal v2u
get_random_dungeon_feature_pos(Random *random, Dungeon *dungeon, ItemState *item_state, b32 accept_water)
{
    assert(random);
    assert(dungeon);
    assert(item_state);
    
    v2u result = {0};
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random_dungeon_pos(random, dungeon->size);
        if(is_dungeon_pos_traversable(dungeon->tiles, result) &&
           !get_dungeon_pos_entity(dungeon->tiles, result) &&
           !get_dungeon_pos_item_count(item_state, dungeon->level, result) &&
           !get_dungeon_pos_trap(&dungeon->traps, result) &&
           !is_dungeon_pos_passage(dungeon->tiles, result))
        {
            if(!accept_water && is_dungeon_pos_water(dungeon->tiles, result)) continue;
            break;
        }
    }
    
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
    
    f32 floor_area = (f32)floor_count / (f32)rect_area(room_rect);
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
                set_dungeon_pos_wall(random, spec, src_tiles, pos);
            }
        }
    }
    
    DungeonTiles *src = &src_tiles;
    DungeonTiles *dest = &dest_tiles;
    
    for(u32 index = 0; index < spec->automaton_room_step_count; ++index)
    {
        dungeon_automaton_room_step(random, spec, *src, *dest, local_rect);
        
        DungeonTiles *temp = src;
        src = dest;
        dest = temp;
    }
    
    end_temporary_memory(temporary_memory);
    return(*src);
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
    assert(passages);
    assert(new_passage.type);
    assert(!is_v2u_zero(new_passage.pos));
    
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

internal DungeonTrap *
get_dungeon_pos_trap(DungeonTraps *traps, v2u pos)
{
    DungeonTrap *result = 0;
    
    for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        if(trap->type && is_v2u_equal(trap->pos, pos))
        {
            result = trap;
            break;
        }
    }
    
    return(result);
}

internal void
remove_dungeon_trap(DungeonTrap *trap)
{
    zero_struct(*trap);
}

internal v4u
get_dungeon_trap_tile_src(DungeonTrapType type)
{
    v4u result = {0};
    
    switch(type)
    {
        case DungeonTrapType_Spike: result = get_dungeon_tileset_rect(DungeonTileID_SpikeTrap); break;
        case DungeonTrapType_Sword: result = get_dungeon_tileset_rect(DungeonTileID_SwordTrap); break;
        case DungeonTrapType_Arrow: result = get_dungeon_tileset_rect(DungeonTileID_ArrowTrap); break;
        case DungeonTrapType_Magic: result = get_dungeon_tileset_rect(DungeonTileID_MagicTrap); break;
        case DungeonTrapType_Bind: result = get_dungeon_tileset_rect(DungeonTileID_BindTrap); break;
        case DungeonTrapType_Summon: result = get_dungeon_tileset_rect(DungeonTileID_SummonTrap); break;
        case DungeonTrapType_Teleport: result = get_dungeon_tileset_rect(DungeonTileID_TeleportTrap); break;
        case DungeonTrapType_Shaft: result = get_dungeon_tileset_rect(DungeonTileID_ShaftTrap); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
add_dungeon_trap(DungeonSpec *spec, DungeonTraps *traps, DungeonTrapType type, v2u pos)
{
    assert(spec);
    assert(traps);
    assert(type);
    assert(!is_v2u_zero(pos));
    
    for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        
        if(!trap->type)
        {
            trap->type = type;
            trap->pos = pos;
            trap->tile_src = get_dungeon_trap_tile_src(type);
            
            switch(type)
            {
                case DungeonTrapType_Spike:
                {
                    strcpy(trap->name.s, "Spike Trap");
                    trap->damage = spec->spike_trap_damage;
                    sprintf(trap->description.s, "A pit of spikes dealing %u-%u %s damage when stepped on.", spec->spike_trap_damage.min, spec->spike_trap_damage.max, get_entity_damage_type_string(spec->spike_trap_damage.type));
                } break;
                
                case DungeonTrapType_Sword:
                {
                    strcpy(trap->name.s, "Sword Trap");
                    trap->damage = spec->sword_trap_damage;
                    sprintf(trap->description.s, "A pit of swords dealing %u-%u %s damage when stepped on.", spec->sword_trap_damage.min, spec->sword_trap_damage.max, get_entity_damage_type_string(spec->sword_trap_damage.type));
                } break;
                
                case DungeonTrapType_Arrow:
                {
                    strcpy(trap->name.s, "Arrow Trap");
                    trap->damage = spec->arrow_trap_damage;
                    sprintf(trap->description.s, "A pit of arrows dealing %u-%u %s damage when stepped on.", spec->arrow_trap_damage.min, spec->arrow_trap_damage.max, get_entity_damage_type_string(spec->arrow_trap_damage.type));
                } break;
                
                case DungeonTrapType_Magic:
                {
                    strcpy(trap->name.s, "Magic Trap");
                    trap->damage = spec->magic_trap_damage;
                    sprintf(trap->description.s, "A glyph that explodes dealing %u-%u random type damage when stepped on.", spec->magic_trap_damage.min, spec->magic_trap_damage.max);
                } break;
                
                case DungeonTrapType_Bind:
                {
                    strcpy(trap->name.s, "Bind Trap");
                    trap->value_range = spec->bind_trap_value_range;
                    sprintf(trap->description.s, "A glyph that stops you from moving for %u-%u turns when stepped on.", spec->bind_trap_value_range.min, spec->bind_trap_value_range.max);
                } break;
                
                case DungeonTrapType_Summon:
                {
                    strcpy(trap->name.s, "Summon Trap");
                    strcpy(trap->description.s, "A glyph that summons a random enemy to the vicinity when stepped on.");
                } break;
                
                case DungeonTrapType_Teleport:
                {
                    strcpy(trap->name.s, "Teleport Trap");
                    strcpy(trap->description.s, "A glyph that teleports you to a random location in the dungeon when stepped on.");
                } break;
                
                case DungeonTrapType_Shaft:
                {
                    strcpy(trap->name.s, "Shaft");
                    trap->value_range = spec->shaft_trap_value_range;
                    sprintf(trap->description.s, "Stepping into shafts will cause you to fall %u-%u dungeon levels.", spec->shaft_trap_value_range.min, spec->shaft_trap_value_range.max);
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(0);
}

internal v4u
get_dungeon_tile_rect_from_entity_id(EntityID id)
{
    v2u pos = get_entity_tile_pos(id);
    v4u result = get_dungeon_tile_rect(pos);
    return(result);
}

internal v4u
get_dungeon_tile_rect(v2u pos)
{
    v4u result =
    {
        tile_mul(pos.x),
        tile_mul(pos.y),
        TILE_SIZE,
        TILE_SIZE
    };
    
    return(result);
}

internal b32
add_dungeon_room_feature(DungeonRoom *room, u32 max_feature_count, DungeonRoomFeatureType type)
{
    assert(room);
    assert(max_feature_count);
    assert(type);
    
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

internal DungeonTile *
get_dungeon_pos_tile(DungeonTiles tiles, v2u pos)
{
    DungeonTile *result = &tiles.array[(pos.y * tiles.width) + pos.x];
    return(result);
}

internal DungeonTileID
get_dungeon_pos_tile_id(DungeonTiles tiles, v2u pos)
{
    DungeonTileID result = tiles.array[(pos.y * tiles.width) + pos.x].id;
    return(result);
}

internal b32
entity_level_fits_dungeon_level(u32 entity_level, u32 dungeon_level)
{
    assert(entity_level);
    assert(is_dungeon_level_valid(dungeon_level));
    
    u32 min_dungeon_level = dungeon_level - 1;
    if(!min_dungeon_level)
    {
        min_dungeon_level = 1;
    }
    
    u32 max_dungeon_level = dungeon_level + 1;
    if(max_dungeon_level > MAX_DUNGEON_LEVEL)
    {
        max_dungeon_level = MAX_DUNGEON_LEVEL;
    }
    
    b32 result = (entity_level >= min_dungeon_level &&
                  entity_level <= max_dungeon_level);
    
    return(result);
}

internal EntityID
get_random_enemy_id_for_dungeon_level(Random *random, u32 dungeon_level)
{
    assert(random);
    assert(is_dungeon_level_valid(dungeon_level));
    
    EntityID result = EntityID_None;
    
    for(;;)
    {
        assert_loop_count();
        
        EntityInfo enemy_info = get_random_enemy_entity_info(random);
        if(entity_level_fits_dungeon_level(enemy_info.level, dungeon_level))
        {
            result = enemy_info.id;
            break;
        }
    }
    
    return(result);
}

internal v2u
get_dungeon_tileset_pos_from_tile_id(DungeonTileID id)
{
    v2u result = {0};
    
    v2u red_blood_pos = {37, 3};
    v2u green_blood_pos = {red_blood_pos.x, red_blood_pos.y + 1};
    
    switch(id)
    {
        case DungeonTileID_StoneWall1: result = make_v2u(1, 0); break;
        case DungeonTileID_StoneWall2: result = make_v2u(2, 0); break;
        case DungeonTileID_StoneWall3: result = make_v2u(3, 0); break;
        case DungeonTileID_StoneWall4: result = make_v2u(4, 0); break;
        case DungeonTileID_StoneWall5: result = make_v2u(5, 0); break;
        case DungeonTileID_StoneWall6: result = make_v2u(6, 0); break;
        case DungeonTileID_StoneWall7: result = make_v2u(7, 0); break;
        case DungeonTileID_StoneWall8: result = make_v2u(8, 0); break;
        case DungeonTileID_StoneWall9: result = make_v2u(9, 0); break;
        case DungeonTileID_StoneWall10: result = make_v2u(10, 0); break;
        
        case DungeonTileID_StoneWallTorch1: result = make_v2u(11, 0); break;
        case DungeonTileID_StoneWallTorch2: result = make_v2u(12, 0); break;
        case DungeonTileID_StoneWallTorch3: result = make_v2u(13, 0); break;
        case DungeonTileID_StoneWallTorch4: result = make_v2u(14, 0); break;
        
        case DungeonTileID_StoneFloor1: result = make_v2u(15, 0); break;
        case DungeonTileID_StoneFloor2: result = make_v2u(16, 0); break;
        case DungeonTileID_StoneFloor3: result = make_v2u(17, 0); break;
        case DungeonTileID_StoneFloor4: result = make_v2u(18, 0); break;
        
        case DungeonTileID_StoneDoorClosed: result = make_v2u(19, 0); break;
        case DungeonTileID_StoneDoorOpen: result = make_v2u(20, 0); break;
        
        case DungeonTileID_StoneStaircaseUp: result = make_v2u(21, 0); break;
        case DungeonTileID_StoneStaircaseDown: result = make_v2u(22, 0); break;
        case DungeonTileID_ExitDungeon: result = make_v2u(23, 0); break;
        
        case DungeonTileID_EntityStatus: result = make_v2u(7, 15); break;
        case DungeonTileID_EntityAlerted: result = make_v2u(8, 15); break;
        case DungeonTileID_EntityInvisible: result = make_v2u(9, 15); break;
        
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
        case DungeonTileID_SummonTrap: result = make_v2u(19, 4); break;
        case DungeonTileID_TeleportTrap: result = make_v2u(20, 4); break;
        case DungeonTileID_ShaftTrap: result = make_v2u(21, 4); break;
        
        case DungeonTileID_Water1: result = make_v2u(1, 1); break;
        case DungeonTileID_Water2: result = make_v2u(2, 1); break;
        case DungeonTileID_Water3: result = make_v2u(3, 1); break;
        
        //
        
        case DungeonTileID_RedBloodGroundSmall1: result = make_v2u(red_blood_pos.x, red_blood_pos.y); break;
        case DungeonTileID_RedBloodGroundSmall2: result = make_v2u(red_blood_pos.x + 1, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodGroundMedium1: result = make_v2u(red_blood_pos.x + 2, red_blood_pos.y); break;
        case DungeonTileID_RedBloodGroundMedium2: result = make_v2u(red_blood_pos.x + 3, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodGroundLarge1: result = make_v2u(red_blood_pos.x + 4, red_blood_pos.y); break;
        case DungeonTileID_RedBloodGroundLarge2: result = make_v2u(red_blood_pos.x + 5, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallUp1: result = make_v2u(red_blood_pos.x + 6, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUp2: result = make_v2u(red_blood_pos.x + 7, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUp3: result = make_v2u(red_blood_pos.x + 8, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallDown1: result = make_v2u(red_blood_pos.x + 9, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDown2: result = make_v2u(red_blood_pos.x + 10, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDown3: result = make_v2u(red_blood_pos.x + 11, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallLeft1: result = make_v2u(red_blood_pos.x + 12, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallLeft2: result = make_v2u(red_blood_pos.x + 13, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallLeft3: result = make_v2u(red_blood_pos.x + 14, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallRight1: result = make_v2u(red_blood_pos.x + 15, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallRight2: result = make_v2u(red_blood_pos.x + 16, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallRight3: result = make_v2u(red_blood_pos.x + 17, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallUpLeft1: result = make_v2u(red_blood_pos.x + 18, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUpLeft2: result = make_v2u(red_blood_pos.x + 19, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUpLeft3: result = make_v2u(red_blood_pos.x + 20, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallUpRight1: result = make_v2u(red_blood_pos.x + 21, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUpRight2: result = make_v2u(red_blood_pos.x + 22, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallUpRight3: result = make_v2u(red_blood_pos.x + 23, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallDownLeft1: result = make_v2u(red_blood_pos.x + 24, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDownLeft2: result = make_v2u(red_blood_pos.x + 25, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDownLeft3: result = make_v2u(red_blood_pos.x + 26, red_blood_pos.y); break;
        
        case DungeonTileID_RedBloodWallDownRight1: result = make_v2u(red_blood_pos.x + 27, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDownRight2: result = make_v2u(red_blood_pos.x + 28, red_blood_pos.y); break;
        case DungeonTileID_RedBloodWallDownRight3: result = make_v2u(red_blood_pos.x + 29, red_blood_pos.y); break;
        
        //
        
        case DungeonTileID_GreenBloodGroundSmall1: result = make_v2u(green_blood_pos.x, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodGroundSmall2: result = make_v2u(green_blood_pos.x + 1, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodGroundMedium1: result = make_v2u(green_blood_pos.x + 2, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodGroundMedium2: result = make_v2u(green_blood_pos.x + 3, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodGroundLarge1: result = make_v2u(green_blood_pos.x + 4, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodGroundLarge2: result = make_v2u(green_blood_pos.x + 5, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallUp1: result = make_v2u(green_blood_pos.x + 6, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUp2: result = make_v2u(green_blood_pos.x + 7, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUp3: result = make_v2u(green_blood_pos.x + 8, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallDown1: result = make_v2u(green_blood_pos.x + 9, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDown2: result = make_v2u(green_blood_pos.x + 10, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDown3: result = make_v2u(green_blood_pos.x + 11, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallLeft1: result = make_v2u(green_blood_pos.x + 12, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallLeft2: result = make_v2u(green_blood_pos.x + 13, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallLeft3: result = make_v2u(green_blood_pos.x + 14, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallRight1: result = make_v2u(green_blood_pos.x + 15, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallRight2: result = make_v2u(green_blood_pos.x + 16, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallRight3: result = make_v2u(green_blood_pos.x + 17, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallUpLeft1: result = make_v2u(green_blood_pos.x + 18, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUpLeft2: result = make_v2u(green_blood_pos.x + 19, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUpLeft3: result = make_v2u(green_blood_pos.x + 20, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallUpRight1: result = make_v2u(green_blood_pos.x + 21, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUpRight2: result = make_v2u(green_blood_pos.x + 22, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallUpRight3: result = make_v2u(green_blood_pos.x + 23, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallDownLeft1: result = make_v2u(green_blood_pos.x + 24, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDownLeft2: result = make_v2u(green_blood_pos.x + 25, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDownLeft3: result = make_v2u(green_blood_pos.x + 26, green_blood_pos.y); break;
        
        case DungeonTileID_GreenBloodWallDownRight1: result = make_v2u(green_blood_pos.x + 27, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDownRight2: result = make_v2u(green_blood_pos.x + 28, green_blood_pos.y); break;
        case DungeonTileID_GreenBloodWallDownRight3: result = make_v2u(green_blood_pos.x + 29, green_blood_pos.y); break;
        
        //
        
        case DungeonTileID_RedBloodWater1: result = make_v2u(4, 1); break;
        case DungeonTileID_RedBloodWater2: result = make_v2u(5, 1); break;
        
        case DungeonTileID_GreenBloodWater1: result = make_v2u(6, 1); break;
        case DungeonTileID_GreenBloodWater2: result = make_v2u(7, 1); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_dungeon_tileset_pos_from_tile_pos(DungeonTiles tiles, v2u tile_pos)
{
    DungeonTileID id = get_dungeon_pos_tile_id(tiles, tile_pos);
    v2u result = get_dungeon_tileset_pos_from_tile_id(id);
    
    return(result);
}

internal v4u
get_dungeon_tileset_rect(DungeonTileID id)
{
    v2u pos = get_dungeon_tileset_pos_from_tile_id(id);
    v4u result = get_dungeon_tile_rect(pos);
    
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
    
    while(counter < 100)
    {
        switch(random_chance_type)
        {
            case RandomChanceType_Default:
            {
                result = get_random(random, random_start, random_end);
                counter += chances[result];
            } break;
            
            case RandomChanceType_ItemType:
            {
                result = get_random_item_type(random);
                
                u32 index = get_index(result);
                counter += chances[index];
            } break;
            
            case RandomChanceType_Potion:
            {
                result = get_random_potion(random);
                
                u32 index = result - ItemID_PotionStart - 1;
                counter += chances[index];
            } break;
            
            case RandomChanceType_Scroll:
            {
                result = get_random_scroll(random);
                
                u32 index = result - ItemID_ScrollStart - 1;
                counter += chances[index];
            } break;
            
            invalid_default_case;
        }
    }
    
    assert(result);
    return(result);
}

internal char *
get_dungeon_tile_name(DungeonTileID tile_id)
{
    char *result = "";
    
    switch(tile_id)
    {
        case DungeonTileID_StoneWall1:
        case DungeonTileID_StoneWall2:
        case DungeonTileID_StoneWall3:
        case DungeonTileID_StoneWall4:
        case DungeonTileID_StoneWall5:
        case DungeonTileID_StoneWall6:
        case DungeonTileID_StoneWall7:
        case DungeonTileID_StoneWall8:
        case DungeonTileID_StoneWall9:
        case DungeonTileID_StoneWall10:
        
        case DungeonTileID_StoneWallTorch1:
        case DungeonTileID_StoneWallTorch2:
        case DungeonTileID_StoneWallTorch3:
        case DungeonTileID_StoneWallTorch4: result = "Wall"; break;
        
        case DungeonTileID_StoneFloor1:
        case DungeonTileID_StoneFloor2:
        case DungeonTileID_StoneFloor3:
        case DungeonTileID_StoneFloor4: result = "Floor"; break;
        
        case DungeonTileID_StoneDoorClosed: result = "Closed Door"; break;
        case DungeonTileID_StoneDoorOpen: result = "Open Door"; break;
        
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
get_dungeon_tile_description(DungeonTileID tile_id)
{
    char *result = 0;
    
    switch(tile_id)
    {
        case DungeonTileID_StoneWall1: break;
        case DungeonTileID_StoneWall2: break;
        case DungeonTileID_StoneWall3: break;
        case DungeonTileID_StoneWall4: break;
        case DungeonTileID_StoneWall5: break;
        case DungeonTileID_StoneWall6: break;
        case DungeonTileID_StoneWall7: break;
        case DungeonTileID_StoneWall8: break;
        case DungeonTileID_StoneWall9: break;
        case DungeonTileID_StoneWall10: break;
        
        case DungeonTileID_StoneWallTorch1: break;
        case DungeonTileID_StoneWallTorch2: break;
        case DungeonTileID_StoneWallTorch3: break;
        case DungeonTileID_StoneWallTorch4: break;
        
        case DungeonTileID_StoneFloor1: break;
        case DungeonTileID_StoneFloor2: break;
        case DungeonTileID_StoneFloor3: break;
        case DungeonTileID_StoneFloor4: break;
        
        case DungeonTileID_StoneDoorClosed: break;
        case DungeonTileID_StoneDoorOpen: break;
        
        case DungeonTileID_StoneStaircaseUp: result = "A pathway leading upwards."; break;
        case DungeonTileID_StoneStaircaseDown: result = "A pathway leading downwards."; break;
        
        case DungeonTileID_ExitDungeon: result = "A pathway leading out of the dungeon."; break;
        
        case DungeonTileID_Water1:
        case DungeonTileID_Water2:
        case DungeonTileID_Water3: result = "Flowing shallow water."; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal Entity *
get_dungeon_pos_entity(DungeonTiles tiles, v2u pos)
{
    Entity *result = tiles.array[(pos.y * tiles.width) + pos.x].entity;
    return(result);
}

internal b32
is_dungeon_pos_traversable_or_closed_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) ||
                  is_dungeon_pos_closed_door(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable_and_occupied(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) &&
                  get_dungeon_pos_entity(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_traversable_and_unoccupied(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) &&
                  !get_dungeon_pos_entity(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_torch(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWallTorch4));
    
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
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall6) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall7) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall8) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall9) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneWall10) ||
                  is_dungeon_pos_torch(tiles, pos));
    
    return(result);
}

internal b32
is_dungeon_pos_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_open_door(tiles, pos) || is_dungeon_pos_closed_door(tiles, pos));
    return(result);
}

internal b32
is_dungeon_pos_open_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorOpen));
    return(result);
}

internal b32
is_dungeon_pos_traversable_or_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable(tiles, pos) || is_dungeon_pos_door(tiles, pos));
    return(result);
}

internal b32
is_dungeon_pos_closed_door(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneDoorClosed));
    return(result);
}

internal b32
is_dungeon_pos_inside_rect(v2u pos, v4u rect)
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
is_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile_id)
{
    b32 result = (get_dungeon_pos_tile_id(tiles, pos) == tile_id);
    return(result);
}

internal b32
is_dungeon_pos_floor(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor1) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor2) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor3) ||
                  is_dungeon_pos_tile(tiles, pos, DungeonTileID_StoneFloor4));
    
    return(result);
}

internal b32
has_tile_not_been_seen_and_can_entity_move_there(Entity *player, Dungeon *dungeon, v2u pos)
{
    assert(is_player_entity_valid(player));
    assert(dungeon);
    
    b32 result = (!has_tile_been_seen(dungeon->tiles, pos) &&
                  can_entity_move_to_pos(player, dungeon, pos, false));
    
    return(result);
}

internal b32
can_player_auto_explore_dungeon(Entity *player, Dungeon *dungeon)
{
    assert(is_player_entity_valid(player));
    assert(dungeon);
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            if(has_tile_not_been_seen_and_can_entity_move_there(player, dungeon, pos))
            {
                return(true);
            }
        }
    }
    
    return(false);
}

internal v4u
get_dungeon_dimension_rect(v2u dungeon_size, v2u pos, u32 dimension)
{
    assert(!is_v2u_zero(dungeon_size));
    assert(!is_v2u_zero(pos));
    assert(dimension);
    
    v4u result = {0};
    result.x = pos.x - dimension;
    result.y = pos.y - dimension;
    
    if((s32)result.x < 0) result.x = 0;
    if((s32)result.y < 0) result.y = 0;
    
    result.w = (pos.x - result.x) + dimension + 1;
    result.h = (pos.y - result.y) + dimension + 1;
    
    if(get_rect_width(result) > dungeon_size.w - 1) result.w = (dungeon_size.w - 1) - result.x;
    if(get_rect_height(result) > dungeon_size.h - 1) result.h = (dungeon_size.h - 1) - result.y;
    
#if 0
    printf("pos: %u, %u\n", pos.x, pos.y);
    printf("dimension: %u\n", dimension);
    printf("result: %u, %u, %u, %u\n\n", result.x, result.y, result.w, result.h);
#endif
    
    return(result);
}

internal v2u
get_random_dungeon_pos(Random *random, v2u dungeon_size)
{
    // Doesn't return a position on the edge of the dungeon
    v2u result =
    {
        get_random(random, 1, dungeon_size.w - 2),
        get_random(random, 1, dungeon_size.h - 2)
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
        result.x = get_random(random, rect.x, (rect.x + rect.w) - 1),
        result.y = get_random(random, rect.y, (rect.y + rect.h) - 1)
    };
    
    return(result);
}

internal DungeonTileID
get_dungeon_pos_remains_tile_id(DungeonTiles tiles, v2u pos)
{
    DungeonTileID remains = tiles.array[(pos.y * tiles.width) + pos.x].remains_id;
    return(remains);
}

internal void
set_dungeon_pos_remains_tile_id(DungeonTiles tiles, v2u pos, DungeonTileID tile_id)
{
    assert(tile_id);
    tiles.array[(pos.y * tiles.width) + pos.x].remains_id = tile_id;
}

internal b32
is_dungeon_pos_valid_for_base_entity_remains(Dungeon *dungeon, v2u pos)
{
    b32 result = (!is_dungeon_pos_passage(dungeon->tiles, pos) &&
                  !get_dungeon_pos_trap(&dungeon->traps, pos) &&
                  !is_dungeon_pos_door(dungeon->tiles, pos));
    
    return(result);
}

internal b32
can_place_entity_remains_on_dungeon_pos(Dungeon *dungeon, v2u pos, EntityRemainsPlaceType type)
{
    assert(dungeon);
    assert(type == EntityRemainsPlaceType_Wound ||
           type == EntityRemainsPlaceType_Kill);
    
    b32 result = false;
    
    if(type == EntityRemainsPlaceType_Wound &&
       is_dungeon_pos_valid_for_base_entity_remains(dungeon, pos) &&
       !get_dungeon_pos_entity(dungeon->tiles, pos) &&
       !get_dungeon_pos_remains_tile_id(dungeon->tiles, pos))
    {
        result = true;
    }
    else if(type == EntityRemainsPlaceType_Kill &&
            is_dungeon_pos_valid_for_base_entity_remains(dungeon, pos))
    {
        result = true;
    }
    
    return(result);
}

internal void
set_dungeon_pos_tile(DungeonTiles tiles, v2u pos, DungeonTileID tile_id)
{
    tiles.array[(pos.y * tiles.width) + pos.x].id = tile_id;
}

internal void
set_dungeon_pos_entity(DungeonTiles tiles, v2u pos, Entity *entity)
{
    tiles.array[(pos.y * tiles.width) + pos.x].entity = entity;
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
set_dungeon_pos_wall(Random *random, DungeonSpec *spec, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile_id = get_random_with_chances(random,
                                                    spec->wall_tile_chances,
                                                    DungeonWallTileType_None + 1,
                                                    DungeonWallTileType_Count - 1,
                                                    RandomChanceType_Default);
    
    set_dungeon_pos_tile(tiles, pos, tile_id);
}

internal void
set_dungeon_pos_floor(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile_id = get_random(random, DungeonTileID_StoneFloor1, DungeonTileID_StoneFloor4);
    set_dungeon_pos_tile(tiles, pos, tile_id);
}

internal void
set_dungeon_pos_torch(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile_id = get_random(random, DungeonTileID_StoneWallTorch1, DungeonTileID_StoneWallTorch4);
    set_dungeon_pos_tile(tiles, pos, tile_id);
}

internal void
set_dungeon_pos_water(Random *random, DungeonTiles tiles, v2u pos)
{
    DungeonTileID tile_id = get_random(random, DungeonTileID_Water1, DungeonTileID_Water3);
    set_dungeon_pos_tile(tiles, pos, tile_id);
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
is_pos_inside_dungeon_rect(v2u pos, v4u rect)
{
    b32 result = (pos.x >= rect.x &&
                  pos.y >= rect.y &&
                  pos.x < (rect.x + rect.w) &&
                  pos.y < (rect.y + rect.h));
    
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
get_dungeon_area_tile_type_count(DungeonTiles tiles, v2u pos, u32 area, DungeonTileType type)
{
    assert(area);
    assert(type);
    
    u32 result = 0;
    
    s32 start_x = pos.x - area;
    s32 start_y = pos.y - area;
    
    if(start_x < 0) start_x = 0;
    if(start_y < 0) start_y = 0;
    
    for(u32 y = start_y; y < (pos.y + area + 1); ++y)
    {
        for(u32 x = start_x; x < (pos.x + area + 1); ++x)
        {
            v2u local_pos = {x, y};
            
            switch(type)
            {
                case DungeonTileType_Passage: if(is_dungeon_pos_passage(tiles, local_pos)) ++result; break;
                case DungeonTileType_Floor: if(is_dungeon_pos_floor(tiles, local_pos)) ++result; break;
                case DungeonTileType_Wall: if(is_dungeon_pos_wall(tiles, local_pos)) ++result; break;
                case DungeonTileType_Torch: if(is_dungeon_pos_torch(tiles, local_pos)) ++result; break;
                case DungeonTileType_Door: if(is_dungeon_pos_door(tiles, local_pos)) ++result; break;
                
                invalid_default_case;
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
reset_not_flooded_dungeon_tiles(Random *random, Dungeon *dungeon, b32 *fill_tiles)
{
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            if(!fill_tiles[(y * dungeon->tiles.width) + x])
            {
                v2u pos = {x, y};
                set_dungeon_pos_wall(random, &dungeon->spec, dungeon->tiles, pos);
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

internal DungeonRoom *
get_random_dungeon_room(Random *random, DungeonRooms *rooms)
{
    DungeonRoom *result = 0;
    
    u32 index = get_random_index(random, rooms->count);
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
                            DungeonSpec *spec,
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
            
            u32 floor_count = get_dungeon_area_tile_type_count(src_tiles, src_pos, 1, DungeonTileType_Floor);
            
            if(is_dungeon_pos_floor(src_tiles, src_pos))
            {
                if(floor_count <= 3)
                {
                    set_dungeon_pos_wall(random, spec, dest_tiles, dest_pos);
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
                    set_dungeon_pos_wall(random, spec, dest_tiles, dest_pos);
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
            
            DungeonTileID src_tile = get_dungeon_pos_tile_id(src_tiles, src_pos);
            
            if(is_water && !is_dungeon_pos_water(src_tiles, src_pos)) continue;
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
                                                        RandomChanceType_Default);
    
    // Set room dimensions
    switch(room_type)
    {
        case DungeonRoomType_Rect:
        {
            result.rect.w = get_random_from_v2u(random, spec->rect_room_size);
            result.rect.h = get_random_from_v2u(random, spec->rect_room_size);
        } break;
        
        case DungeonRoomType_DoubleRect:
        {
            result.rect.w = get_random_from_v2u(random, spec->double_rect_room_size);
            result.rect.h = get_random_from_v2u(random, spec->double_rect_room_size);
        } break;
        
        case DungeonRoomType_Circle:
        {
            u32 radius = get_random_from_v2u(random, spec->circle_room_size);
            u32 size = radius * 2;
            
            result.rect.w = size;
            result.rect.h = size;
        } break;
        
        case DungeonRoomType_Automaton:
        {
            result.rect.w = get_random_from_v2u(random, spec->automaton_room_size);
            result.rect.h = get_random_from_v2u(random, spec->automaton_room_size);
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
        
        // Padding ensures rooms won't touch each other
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
                
                room_two.w = get_random_from_v2u(random, dungeon->spec.double_rect_room_size);
                room_two.h = get_random_from_v2u(random, dungeon->spec.double_rect_room_size);
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

internal b32
is_dungeon_level_valid(u32 dungeon_level)
{
    b32 result = (dungeon_level > 0 && dungeon_level <= MAX_DUNGEON_LEVEL);
    return(result);
}

internal Dungeon *
get_dungeon_from_level(DungeonState *dungeon_state, u32 dungeon_level)
{
    is_dungeon_level_valid(dungeon_level);
    
    Dungeon *result = &dungeon_state->levels[dungeon_level - 1];
    return(result);
}

internal Dungeon *
create_dungeon(Game *game,
               EntityState *entity_state,
               ItemState *item_state,
               Inventory *inventory,
               DungeonState *dungeon_state,
               UI *ui,
               u32 dungeon_level,
               v2u player_came_from_pos)
{
    
#if 0
    printf("Creating dungeon level %u (index %u)\n", dungeon_level, dungeon_level - 1);
#endif
    
    assert(dungeon_state->current_level != dungeon_level);
    
    Dungeon *dungeon = get_dungeon_from_level(dungeon_state, dungeon_level);
    dungeon->level = dungeon_level;
    assert(dungeon->tiles.array);
    
    dungeon_state->current_level = dungeon_level;
    
    Entity *player = get_player_entity();
    if(!is_player_entity_valid(player))
    {
        // Player will be moved later to a proper position by this function.
        add_entity(entity_state, EntityID_Player, 0, 0, dungeon);
    }
    
    Random *random = &game->random;
    DungeonRooms *rooms = &dungeon->rooms;
    DungeonTraps *traps = &dungeon->traps;
    DungeonPassages *passages = &dungeon->passages;
    
    Dungeon *dungeon_above = 0;
    if(dungeon->level > 1)
    {
        dungeon_above = get_dungeon_from_level(dungeon_state, dungeon_level - 1);
    }
    
    DungeonSpec *spec = &dungeon->spec;
    
    spec->size.w = 64;
    spec->size.h = 64;
    spec->area = area_size(spec->size);
    
    // Flood fill
    spec->flood_fill_area_min = 0.9f;
    
    // Water
    spec->has_water = true;
    spec->water_min_total_area = 0.04f;
    
    // Torch
    spec->torch_count = spec->area * 0.02f;
    spec->torch_retry_count = 100;
    spec->torch_min_spacing = 4;
    
    // Door
    spec->door_count = spec->area * 0.004f;
    spec->door_retry_count = 100;
    spec->door_min_spacing = 4;
    
    // Room
    spec->room_retry_count = 10;
    
    spec->automaton_room_min_valid_area = 0.4f;
    spec->automaton_room_init_chance = 55;
    spec->automaton_room_step_count = 4;
    
    spec->room_max_items = 3;
    spec->room_max_enemies = 2;
    
    spec->wall_tile_chances[DungeonWallTileType_StoneWall1] = 80;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall2] = 80;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall3] = 80;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall4] = 80;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall5] = 80;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall6] = 2;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall7] = 2;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall8] = 2;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall9] = 10;
    spec->wall_tile_chances[DungeonWallTileType_StoneWall10] = 10;
    
    spec->room_type_chances[DungeonRoomType_Rect] = 20;
    spec->room_type_chances[DungeonRoomType_DoubleRect] = 30;
    spec->room_type_chances[DungeonRoomType_Circle] = 40;
    spec->room_type_chances[DungeonRoomType_Automaton] = 60;
    
    spec->corridor_type_chances[DungeonCorridorType_Turn] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Zigzag] = 30;
    spec->corridor_type_chances[DungeonCorridorType_Diagonal] = 30;
    
    spec->rect_room_size = make_v2u(4, 8);
    spec->double_rect_room_size = make_v2u(4, 8);
    spec->circle_room_size = make_v2u(2, 5);
    spec->automaton_room_size = make_v2u(10, 18);
    spec->automaton_water_size = make_v2u(6, 14);
    
    // Trap
    spec->trap_count = get_random(random, 4, 8);
    spec->trap_minimum_distance = 6;
    
    spec->spike_trap_damage.min = 4;
    spec->spike_trap_damage.max = 12;
    spec->spike_trap_damage.type = EntityDamageType_Physical;
    
    spec->sword_trap_damage.min = 8;
    spec->sword_trap_damage.max = 24;
    spec->sword_trap_damage.type = EntityDamageType_Physical;
    
    spec->arrow_trap_damage.min = 2;
    spec->arrow_trap_damage.max = 10;
    spec->arrow_trap_damage.type = EntityDamageType_Physical;
    
    spec->magic_trap_damage.min = 4;
    spec->magic_trap_damage.max = 24;
    spec->magic_trap_damage.type = EntityDamageType_Physical;
    
    spec->bind_trap_value_range = make_v2u(3, 6);
    spec->shaft_trap_value_range = make_v2u(1, 2);
    
    // Passage
    spec->down_passage_count = get_random(random, 1, 3);
    spec->passage_min_spacing = 15;
    
    // Entity
    spec->enemy_count = (spec->size.w + spec->size.h) * 0.14f;
    
    // Item
    spec->item_count = (spec->size.w + spec->size.h) * 0.10f;
    spec->item_curse_chance = 5;
    
    spec->item_type_chances[get_index(ItemType_Weapon)] = 30;
    spec->item_type_chances[get_index(ItemType_Armor)] = 30;
    spec->item_type_chances[get_index(ItemType_Potion)] = 20;
    spec->item_type_chances[get_index(ItemType_Scroll)] = 20;
    spec->item_type_chances[get_index(ItemType_Ration)] = 5;
    
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
    spec->scroll_chances[Scroll_Teleport] = 25;
    
    dungeon->size.w = 64;
    dungeon->size.h = 64;
    dungeon->area = spec->area;
    dungeon->rect = make_v4u(0, 0, dungeon->size.w, dungeon->size.h);
    
    dungeon->tiles.width = dungeon->size.w;
    dungeon->can_pathfind = false;
    
    // Set dungeon up passage count
    if(dungeon->level == 1)
    {
        passages->up_count = 1;
    }
    else
    {
        passages->up_count = dungeon_above->passages.down_count;
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
    
    assert(dungeon->size.w <= MAX_DUNGEON_SIZE);
    assert(dungeon->size.h <= MAX_DUNGEON_SIZE);
    
    // Set dungeon tile defaults
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            set_dungeon_pos_wall(random, spec, dungeon->tiles, pos);
            set_dungeon_pos_entity(dungeon->tiles, pos, 0);
            set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
        }
    }
    
#if 1
    // Test room
    dungeon->can_pathfind = true;
    
    for(u32 y = 1; y < dungeon->size.h - 1; ++y)
    {
        for(u32 x = 1; x < dungeon->size.w - 1; ++x)
        {
            set_dungeon_pos_floor(random, dungeon->tiles, make_v2u(x, y));
        }
    }
    
    entity_move(random, player, dungeon, ui, make_v2u(10, 1));
    //player->hp = player->max_hp - 1;
    //player->hp = 1;
    
#if 0
    // Test blood
    
    u32 blood_x = 25;
    u32 blood_y = 4;
    for(DungeonTileID blood_id = DungeonTileID_RedBloodGroundSmall1;
        blood_id <= DungeonTileID_RedBloodWallDownRight3;
        ++blood_id)
    {
        v2u blood_pos = {blood_x, blood_y};
        set_dungeon_pos_remains(dungeon->tiles, blood_pos, blood_id);
        
        ++blood_x;
    }
    
    blood_x = 25;
    blood_y = 5;
    for(DungeonTileID blood_id = DungeonTileID_GreenBloodGroundSmall1;
        blood_id <= DungeonTileID_GreenBloodWallDownRight3;
        ++blood_id)
    {
        v2u blood_pos = {blood_x, blood_y};
        set_dungeon_pos_remains(dungeon->tiles, blood_pos, blood_id);
        
        ++blood_x;
    }
    
    blood_x = 25;
    blood_y = 7;
    set_dungeon_pos_remains(dungeon->tiles, make_v2u(blood_x, blood_y), DungeonTileID_RedBloodWater1);
    set_dungeon_pos_remains(dungeon->tiles, make_v2u(blood_x + 1, blood_y), DungeonTileID_RedBloodWater2);
    
    blood_x = 25;
    blood_y = 8;
    set_dungeon_pos_remains(dungeon->tiles, make_v2u(blood_x, blood_y), DungeonTileID_GreenBloodWater1);
    set_dungeon_pos_remains(dungeon->tiles, make_v2u(blood_x + 1, blood_y), DungeonTileID_GreenBloodWater2);
#endif
    
#if 0
    // Test traps
    v2u trap_pos = {25, 2};
    
    for(DungeonTrapType trap_type = DungeonTrapType_None + 1;
        trap_type < DungeonTrapType_Count;
        ++trap_type)
    {
        add_dungeon_trap(&dungeon->spec, &dungeon->traps, trap_type, trap_pos);
        
        ++trap_pos.x;
    }
#endif
    
#if 0
    // Test all entities
    u32 entity_x_start = 15;
    u32 entity_x = entity_x_start;
    u32 entity_y = 23;
    
    for(u32 id = ENEMY_START_ID; id < ENEMY_END_ID; ++id)
    {
        add_enemy_entity(entities, dungeon->tiles, id, entity_x, entity_y);
        ++entity_x;
    }
#endif
    
#if 0
    // Test all entity remains
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
    
#if 1
    // Test items
    v2u weapon = {8, 1};
    {
        for(ItemID weapon_id = ItemID_WeaponStart + 1; weapon_id < ItemID_WeaponEnd; ++weapon_id)
        {
            add_weapon_item(random, item_state, dungeon_level, weapon_id, ItemRarity_Common, weapon.x + 1, weapon.y, true);
            add_weapon_item(random, item_state, dungeon_level, weapon_id, ItemRarity_Magical, weapon.x + 2, weapon.y, false);
            add_weapon_item(random, item_state, dungeon_level, weapon_id, ItemRarity_Mythical, weapon.x + 3, weapon.y, false);
            
            ++weapon.y;
        }
    }
    
    v2u armor = {weapon.x + 4, 1};
    {
        add_armor_item(random, item_state, dungeon_level, ItemID_LeatherHelmet, armor.x + 1, armor.y, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_LeatherChestplate, armor.x + 2, armor.y, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_LeatherGreaves, armor.x + 3, armor.y, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_LeatherBoots, armor.x + 4, armor.y, false);
        
        add_armor_item(random, item_state, dungeon_level, ItemID_SteelHelmet, armor.x + 1, armor.y + 1, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_SteelChestplate, armor.x + 2, armor.y + 1, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_SteelGreaves, armor.x + 3, armor.y + 1, false);
        add_armor_item(random, item_state, dungeon_level, ItemID_SteelBoots, armor.x + 4, armor.y + 1, false);
    }
    
    v2u potion = {armor.x + 5, 1};
    {
        for(ItemID potion_id = ItemID_PotionStart + 1; potion_id < ItemID_PotionEnd; ++potion_id)
        {
            add_consumable_item(random, item_state, dungeon_level, potion_id, potion.x + 1, potion.y, 2);
            add_consumable_item(random, item_state, dungeon_level, potion_id, potion.x + 2, potion.y, 7);
            
            ++potion.y;
        }
        
        add_consumable_item(random, item_state, dungeon_level, ItemID_Ration, potion.x + 1, potion.y, 1);
        add_consumable_item(random, item_state, dungeon_level, ItemID_Ration, potion.x + 2, potion.y, 1);
    }
    
    v2u scroll = {potion.x + 3, 1};
    {
        for(ItemID scroll_id = ItemID_ScrollStart + 1; scroll_id < ItemID_ScrollEnd; ++scroll_id)
        {
            add_consumable_item(random, item_state, dungeon_level, scroll_id, scroll.x + 1, scroll.y, 1);
            add_consumable_item(random, item_state, dungeon_level, scroll_id, scroll.x + 2, scroll.y, 1);
            
            ++scroll.y;
        }
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
        assert_loop_count();
        
        DungeonRoom created_room = create_and_place_dungeon_room(&game->memory, random, dungeon);
        if(created_room.success)
        {
            rooms->array[rooms->count++] = created_room;
            assert(rooms->count < MAX_DUNGEON_ROOM_COUNT);
            
            total_room_size += rect_area(created_room.rect);
        }
    }
    
#if 0
    printf("spec->area: %u\n", spec->area);
    printf("total_room_size: %u\n", total_room_size);
    printf("total_room_size / dungeon_size: %.02f\n", (f32)total_room_size / (f32)spec->area);
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
    dungeon->can_pathfind = true;
    
    b32 is_connected[rooms->count];
    zero_struct(is_connected);
    
    for(u32 start_index = 0; start_index < (rooms->count - 1); ++start_index)
    {
        u32 closest_distance = U32_MAX;
        
        DungeonRoom *start_room = &rooms->array[start_index];
        v2u start_room_center = rect_center(start_room->rect);
        
        DungeonRoom *end_room = 0;
        
        // Find closest room to start room
        for(u32 end_index = 0; end_index < rooms->count; ++end_index)
        {
            if((start_index != end_index) && !is_connected[end_index])
            {
                DungeonRoom *loop_end_room = &rooms->array[end_index];
                v2u end_room_center = rect_center(loop_end_room->rect);
                
                u32 new_distance = cardinal_distance(start_room_center, end_room_center);
                if(new_distance < closest_distance)
                {
                    closest_distance = new_distance;
                    end_room = loop_end_room;
                }
            }
        }
        
        // Place corridor between start and end room
        if(end_room)
        {
            v2u corridor_start = get_random_dungeon_traversable_rect_pos(random, dungeon, start_room->rect);
            v2u corridor_end = get_random_dungeon_traversable_rect_pos(random, dungeon, end_room->rect);
            
            DungeonCorridorType corridor_type = get_random_with_chances(random,
                                                                        spec->corridor_type_chances,
                                                                        DungeonCorridorType_None + 1,
                                                                        DungeonCorridorType_Count - 1,
                                                                        RandomChanceType_Default);
            
#if 0
            printf("Corridor start: %u, %u\n", corridor_start.x, corridor_start.y);
            printf("Corridor end: %u, %u\n", corridor_end.x, corridor_end.y);
            printf("Corridor type: %u\n\n", corridor_type);
#endif
            
            is_connected[start_index] = true;
            place_dungeon_corridor(random, dungeon->tiles, corridor_start, corridor_end, corridor_type);
        }
    }
#endif
    
#if 1
    // Place water
    if(spec->has_water)
    {
        f32 total_water_area = 0.0f;
        
        while(total_water_area < spec->water_min_total_area)
        {
            assert_loop_count();
            v2u random_pos = get_random_dungeon_pos(random, dungeon->size);
            
            v4u water_rect = {0};
            water_rect.x = random_pos.x;
            water_rect.y = random_pos.y;
            water_rect.w = get_random_from_v2u(random, spec->automaton_water_size);
            water_rect.h = get_random_from_v2u(random, spec->automaton_water_size);
            
            if(is_rect_inside_dungeon(dungeon->size, water_rect))
            {
                DungeonTiles automaton_tiles = get_dungeon_automaton_tiles(&game->memory, random, spec, water_rect);
                
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
    TemporaryMemory temporary_memory = start_temporary_memory(&game->memory);
    
    memory_size flood_size = dungeon->area * sizeof(b32);
    b32 *flood_array = push_memory(temporary_memory.arena, flood_size);
    
    u32 dungeon_traversable_count = get_dungeon_traversable_pos_count(dungeon);
    
    for(;;)
    {
        assert_loop_count();
        
        zero_size(flood_array, flood_size);
        v2u flood_pos = get_random_dungeon_traversable_pos(random, dungeon);
        u32 flood_count = flood_dungeon(dungeon->tiles, flood_array, 0, flood_pos);
        
#if 0
        printf("Flood Pos: %u, %u\n", flood_pos.x, flood_pos.y);
        printf("Flood Count: %u\n", flood_count);
        printf("Dungeon Traversable Count: %u\n\n", dungeon_traversable_count);
#endif
        
        if(flood_count >= (dungeon_traversable_count * spec->flood_fill_area_min)) break;
    }
    
    reset_not_flooded_dungeon_tiles(random, dungeon, flood_array);
    end_temporary_memory(temporary_memory);
#endif
    
#if 1
    // Place torches
    assert(!spec->torches_generated_count);
    
    for(u32 count = 0; count < spec->torch_count; ++count)
    {
        for(u32 retry_count = 0; retry_count < spec->torch_retry_count; ++retry_count)
        {
            v2u pos = get_random_dungeon_pos(random, dungeon->size);
            
            if(is_dungeon_pos_wall(dungeon->tiles, pos) &&
               !is_dungeon_pos_torch(dungeon->tiles, pos) &&
               get_dungeon_area_tile_type_count(dungeon->tiles, pos, 1, DungeonTileType_Floor))
            {
                
                u32 torch_count = get_dungeon_area_tile_type_count(dungeon->tiles,
                                                                   pos,
                                                                   spec->torch_min_spacing,
                                                                   DungeonTileType_Torch);
                
                if(!torch_count)
                {
                    ++spec->torches_generated_count;
                    set_dungeon_pos_torch(random, dungeon->tiles, pos);
                    
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    // Place doors
    assert(!spec->doors_generated_count);
    
    for(u32 count = 0; count < spec->door_count; ++count)
    {
        for(u32 retry_count = 0; retry_count < spec->door_retry_count; ++retry_count)
        {
            v2u pos = get_random_dungeon_traversable_pos(random, dungeon);
            u32 area_door_count = get_dungeon_area_tile_type_count(dungeon->tiles,
                                                                   pos,
                                                                   spec->door_min_spacing,
                                                                   DungeonTileType_Door);
            
            if(!area_door_count)
            {
                v2u up = get_direction_pos(pos, Direction_Up);
                v2u down = get_direction_pos(pos, Direction_Down);
                v2u left = get_direction_pos(pos, Direction_Left);
                v2u right = get_direction_pos(pos, Direction_Right);
                
                b32 can_place = false;
                
                if(is_dungeon_pos_wall(dungeon->tiles, up) &&
                   is_dungeon_pos_wall(dungeon->tiles, down) &&
                   !is_dungeon_pos_wall(dungeon->tiles, left) &&
                   !is_dungeon_pos_wall(dungeon->tiles, right))
                {
                    can_place = true;
                }
                else if(is_dungeon_pos_wall(dungeon->tiles, left) &&
                        is_dungeon_pos_wall(dungeon->tiles, right) &&
                        !is_dungeon_pos_wall(dungeon->tiles, up) &&
                        !is_dungeon_pos_wall(dungeon->tiles, down))
                {
                    can_place = true;
                }
                
                if(can_place)
                {
                    ++spec->doors_generated_count;
                    set_dungeon_pos_closed_door(dungeon->tiles, pos);
                    
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    b32 has_connected_player_used_passage = false;
    //printf("player_came_from_pos: %u, %u\n", player_came_from_pos.x, player_came_from_pos.y);
    
    // Place up passages
    for(u32 count = 0; count < passages->up_count; ++count)
    {
        DungeonPassage up_passage = {0};
        up_passage.type = DungeonPassageType_Up;
        up_passage.pos = get_new_dungeon_passage_pos(random, dungeon, item_state, spec->passage_min_spacing);
        up_passage.dungeon_level = dungeon_level;
        
        if(dungeon->level == 1)
        {
            set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, DungeonTileID_ExitDungeon);
        }
        else
        {
            set_dungeon_pos_tile(dungeon->tiles, up_passage.pos, DungeonTileID_StoneStaircaseUp);
        }
        
        // Connect current up passage to a passage in the above dungeon that goes down.
        if(dungeon_above)
        {
            assert(!is_v2u_zero(player_came_from_pos));
            
            for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
            {
                DungeonPassage *dungeon_above_passage = &dungeon_above->passages.array[index];
                
                if(dungeon_above_passage->type == DungeonPassageType_Down &&
                   is_v2u_zero(dungeon_above_passage->dest_pos))
                {
                    if(!has_connected_player_used_passage &&
                       is_v2u_equal(dungeon_above_passage->pos, player_came_from_pos))
                    {
                        // Connect the passage the player used with the current up passage.
                        has_connected_player_used_passage = true;
                        
                        dungeon_above_passage->dest_pos = up_passage.pos;
                        up_passage.dest_pos = player_came_from_pos;
                        break;
                    }
                    else
                    {
                        dungeon_above_passage->dest_pos = up_passage.pos;
                        up_passage.dest_pos = dungeon_above_passage->pos;
                        break;
                    }
                }
            }
        }
        
        add_dungeon_passage(&dungeon->passages, up_passage);
    }
#endif
    
#if 1
    // Place down passages
    for(u32 count = 0; count < spec->down_passage_count; ++count)
    {
        DungeonPassage down_passage = {0};
        down_passage.type = DungeonPassageType_Down;
        down_passage.pos = get_new_dungeon_passage_pos(random, dungeon, item_state, spec->passage_min_spacing);
        down_passage.dungeon_level = dungeon_level;
        
        set_dungeon_pos_tile(dungeon->tiles, down_passage.pos, DungeonTileID_StoneStaircaseDown);
        add_dungeon_passage(&dungeon->passages, down_passage);
    }
#endif
    
#if 1
    // Place player
    v2u player_passage_pos = {0};
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        // Find the passage that's connected to player_came_from_pos and place the player there.
        DungeonPassage *passage = &dungeon->passages.array[index];
        if(passage->type == DungeonPassageType_Up &&
           is_v2u_equal(passage->dest_pos, player_came_from_pos))
        {
            player_passage_pos = passage->pos;
            break;
        }
    }
    
    assert(!is_v2u_zero(player_passage_pos));
    entity_move(random, player, dungeon, ui, player_passage_pos);
    if(dungeon->level == 1)
    {
        add_player_starting_item(game, player, item_state, inventory, ui, ItemID_Sword);
        add_player_starting_item(game, player, item_state, inventory, ui, ItemID_MightPotion);
    }
#endif
    
#if 1
    // Place traps
    for(u32 count = 0; count < spec->trap_count; ++count)
    {
        for(;;)
        {
            assert_loop_count();
            
            b32 can_place = false;
            v2u trap_pos = get_random_dungeon_feature_pos(random, dungeon, item_state, false);
            
            if(!get_dungeon_area_tile_type_count(dungeon->tiles, trap_pos, 1, DungeonTileType_Wall))
            {
                can_place = true;
            }
            
            if(can_place)
            {
                // Make sure the trap isn't too close to other traps.
                v4u trap_rect = get_dungeon_dimension_rect(dungeon->size, trap_pos, spec->trap_minimum_distance);
                for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
                {
                    DungeonTrap *trap = &traps->array[index];
                    if(trap->type && is_dungeon_pos_inside_rect(trap->pos, trap_rect))
                    {
                        can_place = false;
                        break;
                    }
                }
                
                if(can_place)
                {
                    DungeonTrapType trap_type = get_random(random, DungeonTrapType_None + 1, DungeonTrapType_Count - 1);
                    add_dungeon_trap(&dungeon->spec, &dungeon->traps, trap_type, trap_pos);
                    
                    break;
                }
            }
        }
    }
#endif
    
#if 1
    // Place items
    u32 percent_progress = ((f32)dungeon_level / (f32)MAX_DUNGEON_LEVEL) * 100;
    u32 chance_per_level = 100 / MAX_DUNGEON_LEVEL;
    u32 level_chance = dungeon_level * chance_per_level;
    
    // TODO(rami): The prints at the end of this function and for item rarity info are pretty spammy.
    // We should change it. Like we fill the rarity info struct when it would be nicer to have a debug
    // tab or something and have all the information there.
    
    ItemRarity item_rarity_counts[ItemRarity_Count] = {0};
    ItemTypeInfo item_rarity_info[ItemRarity_Count] = {0};
    
    item_rarity_info[ItemRarity_Magical].valid = percent_progress >= 30;
    item_rarity_info[ItemRarity_Magical].chance = level_chance * 0.5f;
    assert(item_rarity_info[ItemRarity_Magical].chance);
    
    item_rarity_info[ItemRarity_Mythical].valid = percent_progress >= 60;
    item_rarity_info[ItemRarity_Mythical].chance = level_chance * 0.2f;
    assert(item_rarity_info[ItemRarity_Mythical].chance);
    
    ItemTypeInfo armor_type_info[ArmorItemType_Count] = {0};
    armor_type_info[ArmorItemType_Steel].valid = percent_progress >= 40;
    armor_type_info[ArmorItemType_Steel].chance = level_chance * 0.5f;
    
    for(u32 count = 0; count < spec->item_count; ++count)
    {
        b32 should_add_item = true;
        v2u pos = get_random_dungeon_feature_pos(random, dungeon, item_state, true);
        DungeonRoom *room = get_dungeon_room_from_pos(&dungeon->rooms, pos);
        if(room)
        {
            should_add_item = add_dungeon_room_feature(room, spec->room_max_items, DungeonRoomFeatureType_Item);
        }
        
        if(should_add_item)
        {
            ItemType item_type = get_random_with_chances(random, spec->item_type_chances, 0, 0, RandomChanceType_ItemType);
            
#if 0
            // Test item type
            //item_type = ItemType_Weapon;
            item_type = ItemType_Armor;
            is_item_cursed = false;
#endif
            
            Item *added_item = 0;
            switch(item_type)
            {
                case ItemType_Weapon:
                {
                    // Set item rarity
                    ItemRarity item_rarity = ItemRarity_None;
                    
                    if(is_type_info_valid(random, item_rarity_info[ItemRarity_Magical]))
                    {
                        item_rarity = ItemRarity_Magical;
                        ++item_rarity_counts[ItemRarity_Magical];
                    }
                    else if(is_type_info_valid(random, item_rarity_info[ItemRarity_Mythical]))
                    {
                        item_rarity = ItemRarity_Mythical;
                        ++item_rarity_counts[ItemRarity_Mythical];
                    }
                    else
                    {
                        item_rarity = ItemRarity_Common;
                        ++item_rarity_counts[ItemRarity_Common];
                    }
                    
                    ItemID weapon_id = get_random_weapon(random);
                    added_item = add_weapon_item(random, item_state, dungeon_level, weapon_id, item_rarity, pos.x, pos.y, false);
                } break;
                
                case ItemType_Armor:
                {
                    // Set armor type
                    ItemID armor_id = get_random_leather_armor(random);
                    
                    if(is_type_info_valid(random, armor_type_info[ArmorItemType_Steel]))
                    {
                        armor_id = get_random_steel_armor(random);
                    }
                    
                    assert(is_value_in_range(armor_id, ItemID_ArmorStart, ItemID_ArmorEnd));
                    added_item = add_armor_item(random, item_state, dungeon_level, armor_id, pos.x, pos.y, false);
                } break;
                
                case ItemType_Potion:
                {
                    ItemID potion_id = get_random_with_chances(random, spec->potion_chances, 0, 0, RandomChanceType_Potion);
                    assert(is_value_in_range(potion_id, ItemID_PotionStart, ItemID_PotionEnd));
                    
                    added_item = add_consumable_item(random, item_state, dungeon_level, potion_id, pos.x, pos.y, 1);
                } break;
                
                case ItemType_Scroll:
                {
                    ItemID scroll_id = get_random_with_chances(random, spec->scroll_chances, 0, 0, RandomChanceType_Scroll);
                    assert(is_value_in_range(scroll_id, ItemID_ScrollStart, ItemID_ScrollEnd));
                    
                    added_item = add_consumable_item(random, item_state, dungeon_level, scroll_id, pos.x, pos.y, 1);
                } break;
                
                case ItemType_Ration:
                {
                    added_item = add_consumable_item(random, item_state, dungeon_level, ItemID_Ration, pos.x, pos.y, 1);
                } break;
                
                invalid_default_case;
            }
            
            // Set cursed
            assert(added_item);
            if(is_set(added_item->flags, ItemFlag_CanEquip) &&
               hit_random_chance(random, spec->item_curse_chance))
            {
                set(added_item->flags, ItemFlag_IsCursed);
            }
        }
    }
#endif
    
#if 1
    // Place enemies
    for(u32 count = 0; count < spec->enemy_count; ++count)
    {
        v2u enemy_pos = get_random_dungeon_feature_pos(random, dungeon, item_state, true);
        EntityID enemy_id = get_random_enemy_id_for_dungeon_level(random, dungeon->level);
        
        // We don't want enemies inside the player view range.
        v4u player_view_rect = get_dungeon_dimension_rect(dungeon->size, player->pos, player->stats.fov);
        if(!is_dungeon_pos_inside_rect(enemy_pos, player_view_rect))
        {
            b32 add_enemy = true;
            
            // If the position is inside a room, see if the room can fit another enemy.
            DungeonRoom *room = get_dungeon_room_from_pos(&dungeon->rooms, enemy_pos);
            if(room)
            {
                add_enemy = add_dungeon_room_feature(room, spec->room_max_enemies,
                                                     DungeonRoomFeatureType_Enemy);
            }
            
            if(add_enemy)
            {
                add_entity(entity_state, enemy_id, enemy_pos.x, enemy_pos.y, dungeon);
            }
        }
    }
#endif
    
#if 0
    printf("spec->torch_count: %u\n", spec->torch_count);
    printf("spec->torches_generated_count: %u\n", spec->torches_generated_count);
    printf("spec->torch_retry_count: %u\n", spec->torch_retry_count);
    printf("spec->torch_min_spacing: %u\n\n", spec->torch_min_spacing);
    
    printf("spec->door_count: %u\n", spec->door_count);
    printf("spec->doors_generated_count: %u\n", spec->doors_generated_count);
    printf("spec->door_retry_count: %u\n", spec->door_retry_count);
    printf("spec->door_min_spacing: %u\n\n", spec->door_min_spacing);
    
    // Print item rarity information
    printf("Dungeon Level: %u\n", dungeon_level);
    printf("Max Dungeon Level: %u\n", MAX_DUNGEON_LEVEL);
    printf("Level %u out of %u is %u%% progress\n\n", dungeon_level, MAX_DUNGEON_LEVEL, percent_progress);
    
    for(ItemRarity item_rarity = ItemRarity_Common + 1; item_rarity < ItemRarity_Count; ++item_rarity)
    {
        printf("%s Rarity: Valid: %s, Chance: %u\n", get_item_rarity_string(item_rarity), item_rarity_info[item_rarity].valid ? "True" : "False", item_rarity_info[item_rarity].chance);
    }
    printf("\n");
    
    for(ItemRarity item_rarity = ItemRarity_None + 1; item_rarity < ItemRarity_Count; ++item_rarity)
    {
        printf("%s Item Rarity Count: %u\n", get_item_rarity_string(item_rarity), item_rarity_counts[item_rarity]);
    }
    printf("\n");
    
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
    printf("\n");
    
    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
    {
        DungeonPassage *passage = &dungeon->passages.array[index];
        
        if(passage->type)
        {
            printf("Passage[%u]: %s (Pos: %u, %u) (Dest: %u %u)\n",
                   index, passage->type == DungeonPassageType_Up ? "Up" : "Down",
                   passage->pos.x, passage->pos.y, passage->dest_pos.x, passage->dest_pos.y);
            
        }
    }
    
    printf("\nSpec Trap Count: %u\n", spec->trap_count);
    
    for(u32 index = 0; index < spec->trap_count; ++index)
    {
        DungeonTrap *trap = &traps->array[index];
        printf("Trap[%u]: %s (Pos: %u, %u)\n",
               index, trap->name.s, trap->pos.x, trap->pos.y);
    }
#endif
    
    return(dungeon);
}