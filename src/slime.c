#include <slime.h>

slime_t *slimes[SLIME_COUNT];

// NOTE(Rami): instead of manually giving information about the monster,
// have some kind of array which holds the information already
// we can then choose to fill that in from a config file or have it hard coded
void create_slimes(int32 tile, int32 hp, int32 damage, int32 armor, int32 fov, int32 x, int32 y, int32 w, int32 h)
{
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(!slimes[i])
    {
      slimes[i] = malloc(sizeof(slime_t));
      slimes[i]->entity = malloc(sizeof(entity_t));

      slimes[i]->in_combat = false;

      slimes[i]->entity->tile = tile;
      slimes[i]->entity->hp = hp;
      slimes[i]->entity->damage = damage;
      slimes[i]->entity->armor = armor;
      slimes[i]->entity->fov = fov;
      slimes[i]->entity->x = x;
      slimes[i]->entity->y = y;
      slimes[i]->entity->w = w;
      slimes[i]->entity->h = h;
      return;
    }
  }

  // NOTE(Rami): 
  printf("[Error] Slime array is already full\n");
}
void update_slimes(char *level)
{
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(slimes[i]->entity->hp <= 0)
      {
        free_slimes(i);
        continue;
      }

      if(slimes[i]->in_combat)
      {
        // NOTE(Rami): Could probably make line_of_sight return 0 for
        // no line of sight and a positive int for any line of sight,
        // that way we wouldn't have to do distance().
        // depends on if we really want that though.
        if(line_of_sight(level, slimes[i]->entity->x, slimes[i]->entity->y, player->entity->x, player->entity->y))
        {
          int32 dist = to_tiles(distance(slimes[i]->entity->x, slimes[i]->entity->y, player->entity->x, player->entity->y));
          if(dist == 1)
          {
            attack(slimes[i]->entity, player->entity);
            add_console_msg("Slime attacks you for %d damage", TEXT_COLOR_WHITE, slimes[i]->entity->damage);
          }
          else
          {
            int32 sx = slimes[i]->entity->x < player->entity->x ? TILE_SIZE : -TILE_SIZE;
            int32 sy = slimes[i]->entity->y < player->entity->y ? TILE_SIZE : -TILE_SIZE;

            if(slimes[i]->entity->x != player->entity->x)
            {
              slimes[i]->entity->x += sx;
            }

            if(slimes[i]->entity->y != player->entity->y)
            {
              slimes[i]->entity->y += sy;
            }
          }
        }

        continue;
      }

      // NOTE(Rami): Have some kinda AI function so we can have different
      // out of combat behaviours like the one below.
      int new_x = to_tiles(slimes[i]->entity->x);
      int new_y = to_tiles(slimes[i]->entity->y);

      int rand_dir = rand_int(0, 2);
      if(rand_dir == 0)
      {
        new_x -= 1;
      }
      else if(rand_dir == 1)
      {
        new_x += 1;
      }

      rand_dir = rand_int(0, 2);
      if(rand_dir == 0)
      {
        new_y -= 1;
      }
      else if(rand_dir == 1)
      {
        new_y += 1;
      }

      if(is_traversable_pos(level, new_x, new_y) &&
         !is_player_pos(to_pixels(new_x), to_pixels(new_y)))
      {
        slimes[i]->entity->x = to_pixels(new_x);
        slimes[i]->entity->y = to_pixels(new_y);
      }
    }
    else
    {
      break;
    }
  }
}

void render_slimes()
{
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      SDL_Rect src = {slimes[i]->entity->tile, 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {slimes[i]->entity->x - camera.x, slimes[i]->entity->y - camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(renderer, textures[TEX_MONSTER_SPRITE_SHEET], &src, &dst);
    }
    else
    {
      break;
    }
  }
}

void free_slimes(int32 i)
{
  if(slimes[i])
  {
    if(slimes[i]->entity)
    {
      free(slimes[i]->entity);
    }

    free(slimes[i]);
    slimes[i] = NULL;
  }
}