void lighting_update(entity_t entity)
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].val = lighting_min;
  }

  iv2_t ray_dest;
  i32 radius = entity.fov;
  for(ray_dest.x = player.entity.pos.x - radius; ray_dest.x <= player.entity.pos.x + radius; ray_dest.x++)
  {
    for(ray_dest.y = player.entity.pos.y - radius; ray_dest.y <= player.entity.pos.y + radius; ray_dest.y++)
    {
      if(ray_dest.x == player.entity.pos.x - radius || ray_dest.x == player.entity.pos.x + radius ||
         ray_dest.y == player.entity.pos.y - radius || ray_dest.y == player.entity.pos.y + radius)
      {
        iv2_t ray = player.entity.pos;
        iv2_t diff = {abs(ray.x - ray_dest.x), -abs(ray.y - ray_dest.y)};
        iv2_t dir;

        if(ray.x < ray_dest.x)
        {
          dir.x = 1;
        }
        else
        {
          dir.x = -1;
        }

        if(ray.y < ray_dest.y)
        {
          dir.y = 1;
        }
        else
        {
          dir.y = -1;
        }

        i32 err = diff.x + diff.y;
        i32 err_two;

        i32 lit_val_divider = 1;

        for(i32 i = 0; i <= radius; i++)
        {
          if(!is_inside_level(ray))
          {
            break;
          }

          level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].seen = true;

          if(lit_val_divider != 1)
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].val = lighting_max / lit_val_divider;
          }
          else
          {
            level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].val = lighting_max;
          }

          if(iv2_equal(ray, ray_dest) || !is_traversable(ray))
          {
            break;
          }

          err_two = err * 2;

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

          lit_val_divider++;
        }
      }
    }
  }
}

SDL_Color get_color_for_lighting_value(iv2_t p)
{
  SDL_Color color;
  color.r = level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val;
  color.g = level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val;
  color.b = level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val;

  return color;
}

bool is_lit(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val != lighting_min)
  {
    return true;
  }

  return false;
}

bool is_seen(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].seen)
  {
    return true;
  }

  return false;
}
