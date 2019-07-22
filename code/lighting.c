internal void
update_lighting()
{
    for(i32 i = 0; i < LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT; ++i)
    {
        level.lighting[i].value = lighting_min;
    }
    
#if 0
    for(i32 x = player.pos.x - player.fov; x <= player.pos.x + player.fov; ++x)
    {
        for(i32 y = player.pos.y - player.fov; y <= player.pos.y + player.fov; ++y)
        {
            level.lighting[(y * LEVEL_TILE_WIDTH) + x].seen = 1;
            level.lighting[(y * LEVEL_TILE_WIDTH) + x].value = lighting_max;
        }
    }
#else
    for(i32 x = player.pos.x - player.fov; x <= player.pos.x + player.fov; ++x)
    {
        for(i32 y = player.pos.y - player.fov; y <= player.pos.y + player.fov; ++y)
        {
            if(x == player.pos.x - player.fov || x == player.pos.x + player.fov ||
               y == player.pos.y - player.fov || y == player.pos.y + player.fov)
            {
                v2i ray = player.pos;
                v2i difference = V2i(abs(ray.x - x), -abs(ray.y - y));
                v2i direction = {0};
                
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
                
                i32 err = difference.x + difference.y;
                i32 lit_value_divider = 1;
                
                for(i32 i = 0; i <= player.fov; ++i)
                {
                    if(!is_inside_level(ray))
                    {
                        break;
                    }
                    
                    level.lighting[(ray.y * LEVEL_TILE_WIDTH) + ray.x].seen = 1;
                    
                    if(lit_value_divider != 1)
                    {
                        level.lighting[(ray.y * LEVEL_TILE_WIDTH) + ray.x].value = player.brightness / lit_value_divider;
                    }
                    else
                    {
                        level.lighting[(ray.y * LEVEL_TILE_WIDTH) + ray.x].value = player.brightness;
                    }
                    
                    if(V2i_equal(ray, V2i(x, y)) || !is_traversable(ray))
                    {
                        break;
                    }
                    
                    i32 err_two = err * 2;
                    
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

internal v4i
get_color_from_lighting_value(v2i pos)
{
    v4i color = {0};
    color.r = color.g = color.b = level.lighting[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value;
    return(color);
}

internal i32
is_lit(v2i pos)
{
    i32 result = 0;
    
    if(level.lighting[(pos.y * LEVEL_TILE_WIDTH) + pos.x].value != lighting_min)
    {
        result = 1;
    }
    
    return(result);
}

internal i32
is_seen(v2i pos)
{
    i32 result = 0;
    
    if(level.lighting[(pos.y * LEVEL_TILE_WIDTH) + pos.x].seen)
    {
        result = 1;
    }
    
    return(result);
}
