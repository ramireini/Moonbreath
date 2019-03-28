void lighting_update()
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].val = lighting_0;
  }

  #if 1
  iv2_t ray_dest;
  i32 radius = 4;
  // iv2_t ray_dest = {player.entity.pos.x, player.entity.pos.y - 1};
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

        i32 dist = -1;

        for(i32 i = 0; i < radius; i++)
        {
          dist++;

          if(!is_inside_level(ray))
          {
            break;
          }

          level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].seen = true;
          level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].val = dist;

          if(iv2_equal(ray, ray_dest))
          // if(iv2_equal(ray, ray_dest) || !is_traversable(ray))
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
        }
      }
    }
  }
  #endif

  #if 0
  for(i32 angle = 0; angle < 360; angle++)
  {
    rv2_t ray = {player.entity.pos.x, player.entity.pos.y};
    rv2_t ray_advance = {0.1 * cos(angle), 0.1 * sin(angle)};

    for(;;)
    {
      ray.x += ray_advance.x;
      ray.y += ray_advance.y;

      if(!is_inside_level((iv2_t){(i32)ray.x, (i32)ray.y}))
      {
        break;
      }

      level.lighting[((i32)ray.y * LEVEL_WIDTH_IN_TILES) + (i32)ray.x].seen = true;
      level.lighting[((i32)ray.y * LEVEL_WIDTH_IN_TILES) + (i32)ray.x].val = lighting_on;

      if(!traversable((iv2_t){(i32)ray.x, (i32)ray.y}))
      {
        break;
      }
    }
  }
  #endif

  #if 0
  for(i32 angle = 0; angle < 360; angle++)
  {
    iv2_t ray = player.entity.pos;
    iv2_t ray_dest = {ray.x + (4 * cos(angle)), ray.y + (4 * sin(angle))};

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

    i32 dist = -1;

    for(;;)
    {
      dist++;

      if(!is_inside_level(ray))
      {
        break;
      }

      level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].seen = true;
      level.lighting[(ray.y * LEVEL_WIDTH_IN_TILES) + ray.x].val = dist;

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
    }
  }
#endif
}

// NOTE(Rami): Remove.
void add_light(iv2_t p, i32 r)
{
  iv2_t t;
  for(t.x = p.x - r; t.x <= p.x + r; t.x++)
  {
    for(t.y = p.y - r; t.y <= p.y + r; t.y++)
    {
      level.lighting[(t.y * LEVEL_WIDTH_IN_TILES) + t.x].seen = true;
      level.lighting[(t.y * LEVEL_WIDTH_IN_TILES) + t.x].val = lighting_100;
    }
  }
}

bool is_lit(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val != lighting_0)
  {
    return true;
  }

  return false;
}

SDL_Color query_lit(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_100) {return LIGHTING_COLOR_100;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_90) {return LIGHTING_COLOR_90;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_80) {return LIGHTING_COLOR_80;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_70) {return LIGHTING_COLOR_70;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_60) {return LIGHTING_COLOR_60;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_50) {return LIGHTING_COLOR_50;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_40) {return LIGHTING_COLOR_40;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_30) {return LIGHTING_COLOR_30;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_20) {return LIGHTING_COLOR_20;}
  else if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_10) {return LIGHTING_COLOR_10;}
  else {return LIGHTING_COLOR_0;}
}

bool is_seen(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].seen)
  {
    return true;
  }

  return false;
}
