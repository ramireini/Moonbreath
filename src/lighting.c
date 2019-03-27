void lighting_update()
{
  for(i32 i = 0; i < LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES; i++)
  {
    level.lighting[i].val = lighting_off;
  }

  // for(i32 angle = 0; angle < 360; angle++)
  // {
  //   rv2_t ray = {player.entity.pos.x, player.entity.pos.y};
  //   rv2_t ray_advance = {0.1 * cos(angle), 0.1 * sin(angle)};

  //   for(;;)
  //   {
  //     ray.x += ray_advance.x;
  //     ray.y += ray_advance.y;

  //     if(!is_inside_level((iv2_t){(i32)ray.x, (i32)ray.y}))
  //     {
  //       break;
  //     }

  //     level.lighting[((i32)ray.y * LEVEL_WIDTH_IN_TILES) + (i32)ray.x].seen = true;
  //     level.lighting[((i32)ray.y * LEVEL_WIDTH_IN_TILES) + (i32)ray.x].val = lighting_on;

  //     if(!traversable((iv2_t){(i32)ray.x, (i32)ray.y}))
  //     {
  //       break;
  //     }
  //   }
  // }

  // for (i32 angle = 0; angle < 360; angle++)
  for (i32 angle = 0; angle < 1; angle++)
  {
    int x0 = player.entity.pos.x;
    int y0 = player.entity.pos.y;
    int x1 = (int)(100 * cos(angle));
    int y1 = (int)(100 * sin(angle));

    printf("dest x: %d\n", x1);
    printf("dest y: %d\n\n", y1);

    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for(;;)
    {
      if(!is_inside_level((iv2_t){x0, y0}))
      {
        break;
      }

      level.lighting[(y0 * LEVEL_WIDTH_IN_TILES) + x0].seen = true;
      level.lighting[(y0 * LEVEL_WIDTH_IN_TILES) + x0].val = lighting_on;
      if (x0 == x1 && y0 == y1) {break;}
      // if(!traversable((iv2_t){x0, y0})) {break;}
      e2 = err * 2;
      if (e2 >= dy) {err += dy; x0 += sx;}
      if (e2 <= dx) {err += dx; y0 += sy;}

      // printf("x: %d\n", x0);
      // printf("y: %d\n\n", y0);
    }
  }
}

bool line_of_sight(iv2_t a, iv2_t b)
{
  i32 dx = abs(a.x - b.x);
  i32 sx = a.x < b.x ? 1 : -1;

  i32 dy = -abs(a.y - b.y);
  i32 sy = a.y < b.y ? 1 : -1;

  i32 err = dx + dy;
  i32 err_two = 0;

  for(;;)
  {
    if(!traversable(a))
    {
      return false;
    }

    err_two = err * 2;
    if(err_two <= dx)
    {
      if(a.y == b.y)
      {
        break;
      }

      err += dx;
      a.y += sy;
    }

    if(err_two >= dy)
    {
      if(a.x == b.x)
      {
        break;
      }

      err += dy;
      a.x += sx;
    }
  }

  return true;
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
