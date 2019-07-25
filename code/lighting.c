// TODO(rami): Lighting is probably a better name considering
// that multiple things like the player/torches etc also use this

internal void
update_fov(v2u pos, u32 dist)
{
    for(u32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
    {
        level.fov_tiles[i].value = 0;
    }
    
    // TODO(rami): Remove when time is right
    /*u32 x1 = 0;
    if(pos.x >= dist)
    {
        x1 = pos.x - dist;
    }
    
    u32 x2 = pos.x + dist;
    if(x2 >= LEVEL_TILE_WIDTH)
    {
        x2 = LEVEL_TILE_WIDTH - 1;
    }
    
    u32 y1 = 0;
    if(pos.y >= dist)
    {
        y1 = pos.y - dist;
    }
    
    u32 y2 = pos.y + dist;
    if(pos.y >= LEVEL_TILE_HEIGHT)
    {
        y2 = LEVEL_TILE_HEIGHT - 1;
    }*/
    
    b32 previous_is_blocking = 0;
    
    for(u32 i = 1; i < dist; ++i)
    {
        // TODO(rami): Check
        for(u32 x = 0; x <= i; ++x)
        {
            level.fov_tiles[((pos.y - i) * LEVEL_TILE_WIDTH) + (pos.x + x)].seen = 1;
            level.fov_tiles[((pos.y - i) * LEVEL_TILE_WIDTH) + (pos.x + x)].value = 10;
            
            // v2u cell = 
        }
    }
    
    
    /*for(u32 x = x1; x <= x2; ++x)
    {
        for(u32 y = y1; y <= y2; ++y)
        {
            level.fov_tiles[(y * LEVEL_TILE_WIDTH) + x].seen = 1;
            level.fov_tiles[(y * LEVEL_TILE_WIDTH) + x].value = 10;
        }
    }*/
}

internal void
update_light_point(v2u pos)
{
#if 0
    for(u32 x = pos.x - light.fov; x <= pos.x + light.fov; ++x)
    {
        for(u32 y = pos.y - light.fov; y <= pos.y + light.fov; ++y)
        {
            level.lights[(y * LEVEL_TILE_WIDTH) + x].seen = 1;
            level.lights[(y * LEVEL_TILE_WIDTH) + x].value = 128;
        }
    }
#elif 0
    for(u32 x = pos.x - light.fov; x <= pos.x + light.fov; ++x)
    {
        for(u32 y = pos.y - light.fov; y <= pos.y + light.fov; ++y)
        {
            if(x == pos.x - light.fov || x == pos.x + light.fov ||
               y == pos.y - light.fov || y == pos.y + light.fov)
            {
                v2u ray = pos;
                v2u difference = V2i(abs(ray.x - x), -abs(ray.y - y));
                v2u direction = {0};
                
                if(ray.x < x)
                {
                    direction.x = 1;
                }
                else
                {
                    direction.x = -1;
                }
                
                if(ray.y < y)
                {
                    direction.y = 1;
                }
                else
                {
                    direction.y = -1;
                }
                
                u32 err = difference.x + difference.y;
                u32 lit_value_divider = 1;
                
                for(u32 i = 0; i <= light.fov; ++i)
                {
                    if(!is_inside_level(ray))
                    {
                        break;
                    }
                    
                    level.lights[(ray.y * LEVEL_TILE_WIDTH) + ray.x].seen = 1;
                    
                    if(lit_value_divider == 1)
                    {
                        level.lights[(ray.y * LEVEL_TILE_WIDTH) + ray.x].value = light.brightness;
                    }
                    else
                    {
                        level.lights[(ray.y * LEVEL_TILE_WIDTH) + ray.x].value = light.brightness / lit_value_divider;
                    }
                    
                    if(V2i_equal(ray, V2i(x, y)) || !is_traversable(ray))
                    {
                        break;
                    }
                    
                    u32 err_two = err * 2;
                    
                    if(err_two >= difference.y)
                    {
                        err += difference.y;
                        ray.x += direction.x;
                    }
                    
                    if(err_two <= difference.x)
                    {
                        err += difference.x;
                        ray.y += direction.y;
                    }
                    
                    ++lit_value_divider;
                }
            }
        }
    }
#endif
}

internal void
update_lighting()
{
    { // Reset lighting
        for(u32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
        {
            level.fov_tiles[i].value = lighting_min;
        }
    }
    
    { // Update player lighting
        update_light_point(player.pos);
    }
}

// TODO(rami):
internal v4u
get_color_from_light_value(v2u pos)
{
    /*v4i color = {0};
    color.r = color.g = color.b = level.lights[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value;
    return(color);*/
}

internal b32
is_lit(v2u pos)
{
    b32 result = false;
    
    if(level.fov_tiles[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value > 0)
    {
        result = true;
    }
    
    return(result);
}

internal b32
is_seen(v2u pos)
{
    b32 result = false;
    
    if(level.fov_tiles[(pos.y * LEVEL_TILE_WIDTH) + pos.x].seen)
    {
        result = true;
    }
    
    return(result);
}
