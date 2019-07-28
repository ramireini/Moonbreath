internal b32
is_cell_in_shadow(f32 cell_slope, shadow_data_t *data)
{
    b32 result = false;
    
    for(u32 i = 0; i < data->shadow_count; ++i)
    {
        shadow_t shadow = data->shadows[i];
        if(shadow.start <= cell_slope && shadow.end >= cell_slope)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal v2u
map_cell_for_local_cell(u32 sector, v2u player, v2u cell)
{
    v2u result = {0};
    
    switch(sector)
    {
        case 0: result = V2u(player.x + cell.x, player.y - cell.y); break;
        case 1: result = V2u(player.x + cell.y, player.y - cell.x); break;
        case 2: result = V2u(player.x + cell.y, player.y + cell.x); break;
        case 3: result = V2u(player.x + cell.x, player.y + cell.y); break;
        case 4: result = V2u(player.x - cell.x, player.y + cell.y); break;
        case 5: result = V2u(player.x - cell.y, player.y + cell.x); break;
        case 6: result = V2u(player.x - cell.y, player.y - cell.x); break;
        case 7: result = V2u(player.x - cell.x, player.y - cell.y); break;
    }
    
    return(result);
}

internal void
add_shadow(shadow_t shadow, shadow_data_t *data)
{
    data->shadows[data->shadow_count] = shadow;
    ++data->shadow_count;
}

internal void
set_as_visible(v2u pos)
{
    level.fov_map[(pos.y * LEVEL_TILE_WIDTH) + pos.x].seen = true;
    level.fov_map[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value = 1;
}

internal void
update_fov()
{
    for(u32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
    {
        level.fov_map[i].value = 0;
    }
    
    set_as_visible(player.pos);
    
    for(u32 sector = 0; sector < 8; ++sector)
    {
        shadow_data_t data = {0};
        b32 previous_blocking = false;
        f32 shadow_start = 0.0f;
        f32 shadow_end = 0.0f;
        
        v2u cell = {0};
        for(cell.y = 0; cell.y < player.fov; ++cell.y)
        {
            previous_blocking = false;
            
            for(cell.x = 0; cell.x <= cell.y; ++cell.x)
            {
                v2u map_cell = map_cell_for_local_cell(sector, player.pos, cell);
                
                if(is_inside_level(map_cell))
                {
                    if(distance_between(0, 0, cell.x, cell.y) <= player.fov)
                    {
                        f32 cell_slope = line_slope(0, 0, cell.x, cell.y);
                        if(!is_cell_in_shadow(cell_slope, &data))
                        {
                            set_as_visible(map_cell);
                            
                            if(!is_traversable(map_cell))
                            {
                                if(!previous_blocking)
                                {
                                    shadow_start = line_slope(0, 0, cell.x, cell.y);
                                    previous_blocking = true;
                                }
                            }
                            else
                            {
                                if(previous_blocking)
                                {
                                    shadow_end = line_slope(0, 0, cell.x + 0.5f, cell.y);
                                    shadow_t shadow = {shadow_start, shadow_end};
                                    add_shadow(shadow, &data);
                                }
                            }
                        }
                    }
                }
            }
            
            if(previous_blocking)
            {
                shadow_end = line_slope(0, 0, cell.y + 0.5f, cell.y);
                shadow_t shadow = {shadow_start, shadow_end};
                add_shadow(shadow, &data);
            }
        }
    }
}

internal b32
is_seen(v2u pos)
{
    b32 result = false;
    
    if(level.fov_map[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value == 1)
    {
        result = true;
    }
    
    return(result);
}

internal b32
has_been_seen(v2u pos)
{
    b32 result = false;
    
    if(level.fov_map[(pos.y * LEVEL_TILE_WIDTH) + pos.x].seen)
    {
        result = true;
    }
    
    return(result);
}
