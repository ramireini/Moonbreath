void lighting_update()
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].val = lighting_off;
  }

  for(i32 x = player.entity.pos.x - player.entity.fov; x <= player.entity.pos.x + player.entity.fov; x++)
  {
    for(i32 y = player.entity.pos.y - player.entity.fov; y <= player.entity.pos.y + player.entity.fov; y++)
    {
      level.lighting[(y * LEVEL_WIDTH_IN_TILES) + x].seen = true;
      level.lighting[(y * LEVEL_WIDTH_IN_TILES) + x].val = lighting_on;
    }
  }
}

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
