internal void
update_lighting()
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].value = lighting_max;
  }

  for(i32 x = player.x - player.fov; x <= player.x + player.fov; x++)
  {
    for(i32 y = player.y - player.fov; y <= player.y + player.fov; y++)
    {
      if(x == player.x - player.fov || x == player.x + player.fov ||
         y == player.y - player.fov || y == player.y + player.fov)
      {
        iv2 ray = v2(player.x, player.y);
        iv2 diff = v2(abs(ray.x - x), -abs(ray.y - y));
        iv2 dir = {0};

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

        for(i32 i = 0; i <= player.fov; i++)
        {
          if(!inside_level(ray))
          {
            break;
          }

          level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].seen = 1;

          if(lit_value_divider != 1)
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].value = player.brightness / lit_value_divider;
          }
          else
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].value = player.brightness;
          }

          if(v2_equal(ray, v2(x, y)) || !traversable(ray))
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

internal iv4
get_color_for_lighting_value(iv2 pos)
{
  iv4 color = {0};
  color.r = color.g = color.b = level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].value;
  return color;
}

internal i32
is_lit(iv2 pos)
{
  i32 result = 0;

  if(level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].value != lighting_min)
  {
    result = 1;
  }

  return result;
}

internal i32
is_seen(iv2 pos)
{
  i32 result = 0;

  if(level.lighting[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x].seen)
  {
    result = 1;
  }

  return result;
}
