#include <slime.h>

slime_t slimes[SLIME_COUNT];

void create_slimes(int tile, int hp, int damage, int armor, int fov, int x, int y, int w, int h)
{
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].state == STATE_UNUSED)
    {
      slimes[i].state = STATE_USED;
      slimes[i].entity.tile = tile;
      slimes[i].entity.hp = hp;
      slimes[i].entity.damage = damage;
      slimes[i].entity.armor = armor;
      slimes[i].entity.fov = fov;
      slimes[i].entity.x = x;
      slimes[i].entity.y = y;
      slimes[i].entity.w = w;
      slimes[i].entity.h = h;

      slimes[i].in_combat = 0;

      return;
    }
  }

  // NOTE(Rami): 
  printf("[Error] Slime array is already full\n");
}

void update_slimes(char *level)
{
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].state == STATE_USED)
    {
      // NOTE(Rami): We will want some kind of combat function,
      // so we won't have to write the same code for attacking the player
      // for every single enemy.

      if(slimes[i].in_combat)
      {
        if(line_of_sight(level, slimes[i].entity.x, slimes[i].entity.y, player->entity->x, player->entity->y))
        {
          int dist = to_tiles(distance(slimes[i].entity.x, slimes[i].entity.y, player->entity->x, player->entity->y));

          if(dist == 1)
          {
            add_console_msg("Slime attacked the player", TEXT_COLOR_RED);
          }
          else if(dist > 1)
          {
            int sx = slimes[i].entity.x < player->entity->x ? TILE_SIZE : -TILE_SIZE;
            int sy = slimes[i].entity.y < player->entity->y ? TILE_SIZE : -TILE_SIZE;

            if(slimes[i].entity.x != player->entity->x)
            {
              slimes[i].entity.x += sx;
            }

            if(slimes[i].entity.y != player->entity->y)
            {
              slimes[i].entity.y += sy;
            }

            add_console_msg("Slime is moving closer", TEXT_COLOR_WHITE);
          }
        }
      }

      // NOTE(Rami): 
      // printf("Slime hp: %d\n", slimes[i].entity.hp);
      // printf("Slime fov: %d\n", slimes[i].entity.fov);
      // printf("Slime in_combat: %d\n\n", slimes[i].in_combat);

    //   int rand_x = rand_int(0, 1);
    //   int rand_y = rand_int(0, 1);

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

    //   int test_x = to_tiles(slimes[i].entity.x + rand_x);
    //   int test_y = to_tiles(slimes[i].entity.y + rand_y);

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
    // else
    // {
    //   break;
    // }
  }
}

void render_slimes()
{
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].state == STATE_USED)
    {
      SDL_Rect src = {slimes[i].entity.tile, 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {slimes[i].entity.x - camera.x, slimes[i].entity.y - camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(renderer, textures[TEX_MONSTER_SPRITE_SHEET], &src, &dst);
    }
    else
    {
      break;
    }
  }
}

void delete_slimes(int i)
{
  slimes[i].state = STATE_UNUSED;
  slimes[i].in_combat = 0;
  // NOTE(Rami): we usually zero things individually,
  // sometime in the future maybe try to apply the below
  // to other things as well.
  slimes[i].entity = (entity_t){0, 0, 0, 0, 0, 0, 0, 0, 0};
}