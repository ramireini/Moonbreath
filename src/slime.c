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

      slimes[i]->entity = (entity_t){tile, hp , damage, armor, fov, x, y, w, h};
      slimes[i]->in_combat = false;
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
      if(slimes[i]->entity.hp <= 0)
      {
        delete_slimes(i);
        continue;
      }

      if(slimes[i]->in_combat)
      {
        if(line_of_sight(level, slimes[i]->entity.x, slimes[i]->entity.y, player->entity->x, player->entity->y))
        {
          int32 dist = to_tiles(distance(slimes[i]->entity.x, slimes[i]->entity.y, player->entity->x, player->entity->y));
          if(dist == 1)
          {
            attack(&slimes[i]->entity, player->entity);
            add_console_msg("Slime attacks you for %d damage", TEXT_COLOR_RED, slimes[i]->entity.damage);
          }
          else
          {
            int32 sx = slimes[i]->entity.x < player->entity->x ? TILE_SIZE : -TILE_SIZE;
            int32 sy = slimes[i]->entity.y < player->entity->y ? TILE_SIZE : -TILE_SIZE;

            if(slimes[i]->entity.x != player->entity->x)
            {
              slimes[i]->entity.x += sx;
            }

            if(slimes[i]->entity.y != player->entity->y)
            {
              slimes[i]->entity.y += sy;
            }
          }
        }
      }

      // NOTE(Rami): 
      // printf("Slime hp: %d\n", slimes[i].entity.hp);
      // printf("Slime fov: %d\n", slimes[i].entity.fov);
      // printf("Slime in_combat: %d\n\n", slimes[i].in_combat);

    //   int32 rand_x = rand_int(0, 1);
    //   int32 rand_y = rand_int(0, 1);

    //   if(rand_x == 0)
    //   {
    //     rand_x = -TILE_SIZE;
    //   }
    //   else
    //   {
    //     rand_x = TILE_SIZE;
    //   }

    //   if(rand_y == 0)
    //   {
    //     rand_y = -TILE_SIZE;
    //   }
    //   else
    //   {
    //     rand_y = TILE_SIZE;
    //   }

    //   int32 test_x = to_tiles(slimes[i].entity.x + rand_x);
    //   int32 test_y = to_tiles(slimes[i].entity.y + rand_y);

        // NOTE(Rami): use traversable func
    //   if(level[(test_y * LEVEL_SIZE) + test_x] == TILE_WALL_STONE ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_DOOR_CLOSED ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_PATH_UP ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_PATH_DOWN)
    //   {
    //     continue;
    //   }

    //   slimes[i].entity.x += rand_x;
    //   slimes[i].entity.y += rand_y;
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
      SDL_Rect src = {slimes[i]->entity.tile, 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {slimes[i]->entity.x - camera.x, slimes[i]->entity.y - camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(renderer, textures[TEX_MONSTER_SPRITE_SHEET], &src, &dst);
    }
    else
    {
      break;
    }
  }
}

void delete_slimes(int32 i)
{
  free(slimes[i]);
  slimes[i] = NULL;
}