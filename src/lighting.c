void lighting_update()
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].val = lighting_off;
  }

  // for(i32 x = player.entity.pos.x - player.entity.fov; x <= player.entity.pos.x + player.entity.fov; x++)
  // {
  //   for(i32 y = player.entity.pos.y - player.entity.fov; y <= player.entity.pos.y + player.entity.fov; y++)
  //   {
  //     level.lighting[(y * LEVEL_WIDTH_IN_TILES) + x].seen = true;
  //     level.lighting[(y * LEVEL_WIDTH_IN_TILES) + x].val = lighting_on;
  //   }
  // }

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
}

// ---
void lighting_add_light(iv2_t p, i32 r)
{
  iv2_t t;
  for(t.x = p.x - r; t.x <= p.x + r; t.x++)
  {
    for(t.y = p.y - r; t.y <= p.y + r; t.y++)
    {
      level.lighting[(t.y * LEVEL_WIDTH_IN_TILES) + t.x].seen = true;
      level.lighting[(t.y * LEVEL_WIDTH_IN_TILES) + t.x].val = lighting_on;
    }
  }
}
// ---

bool lighting_lit(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].val == lighting_on)
  {
    return true;
  }

  return false;
}

bool lighting_seen(iv2_t p)
{
  if(level.lighting[(p.y * LEVEL_WIDTH_IN_TILES) + p.x].seen)
  {
    return true;
  }

  return false;
}
