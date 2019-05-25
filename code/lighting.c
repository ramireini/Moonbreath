internal void
update_lighting(entity_t entity)
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].value = lighting_min;
  }

  for(i32 x = entity.x - entity.fov; x <= entity.x + entity.fov; x++)
  {
    for(i32 y = entity.y - entity.fov; y <= entity.y + entity.fov; y++)
    {
      if(x == entity.x - entity.fov || x == entity.x + entity.fov ||
         y == entity.y - entity.fov || y == entity.y + entity.fov)
      {
        v2_t ray = {{entity.x, entity.y}};
        v2_t diff = {{abs(ray.x - x), -abs(ray.y - y)}};
        v2_t dir = {0};

        if(ray.x < x)
        {
          dir.x = 1;
        }
        else
        {
          dir.x = -1;
        }

        if(ray.y < y)
        {
          dir.y = 1;
        }
        else
        {
          dir.y = -1;
        }

        i32 err = diff.x + diff.y;
        i32 lit_value_divider = 1;

        for(i32 i = 0; i <= entity.fov; i++)
        {
          if(!is_inside_level(ray))
          {
            break;
          }

          level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].seen = true;

          if(lit_value_divider != 1)
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].value = entity.brightness / lit_value_divider;
          }
          else
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].value = entity.brightness;
          }

          if(v2_equal(ray, v2(x, y)) || !is_traversable(ray))
          {
            break;
          }

          i32 err_two = err * 2;

          if(err_two >= diff.y)
          {
            err += diff.y;
            ray.x += dir.x;
          }

          if(err_two <= diff.x)
          {
            err += diff.x;
            ray.y += dir.y;
          }

          lit_value_divider++;
        }
      }
    }
  }
}

internal v4_t
get_color_for_lighting_value(v2_t pos)
{
  v4_t color;
  color.r = color.g = color.b = level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].value;
  return color;
}

internal i32
is_lit(v2_t pos)
{
  i32 result = 0;

  if(level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].value != lighting_min)
  {
    result = 1;
  }

  return result;
}

internal i32
is_seen(v2_t pos)
{
  i32 result = 0;

  if(level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].seen)
  {
    result = 1;
  }

  return result;
}
