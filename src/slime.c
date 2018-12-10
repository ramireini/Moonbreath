#include <slime.h>

slime_t slimes[SLIME_COUNT];

void create_slime(int tile, int x, int y, int w, int h)
{
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].state == STATE_UNUSED)
    {
      slimes[i].state = STATE_USED;
      slimes[i].entity.tile = tile;
      slimes[i].entity.x = x;
      slimes[i].entity.y = y;
      slimes[i].entity.new_x = 0;
      slimes[i].entity.new_y = 0;
      slimes[i].entity.w = w;
      slimes[i].entity.h = h;

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
      int rand_x = rand_int(0, 1);
      int rand_y = rand_int(0, 1);

      if(rand_x == 0)
      {
        rand_x = -TILE_SIZE;
      }
      else
      {
        rand_x = TILE_SIZE;
      }

      if(rand_y == 0)
      {
        rand_y = -TILE_SIZE;
        
      }
      else
      {
        rand_y = +TILE_SIZE;
      }

      slimes[i].entity.x += rand_x;
      slimes[i].entity.y += rand_y;

    //   // assume we can move
    //   int can_move = 1;

    //   // turn into units we can use for arrays
    //   int test_x = to_tiles(slimes[i].entity.x) + to_pixels(rand_x);
    //   int test_y = to_tiles(slimes[i].entity.y) + to_pixels(rand_y);

    //   if(level[(test_y * LEVEL_SIZE) + test_x] == TILE_WALL_STONE ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_DOOR_CLOSED ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_PATH_UP ||
    //      level[(test_y * LEVEL_SIZE) + test_x] == TILE_PATH_DOWN)
    //   {
    //     can_move = 0;
    //   }

    //     if(can_move)
    //     {
    //       slimes[i].entity.x += to_pixels(test_x);
    //       slimes[i].entity.y += to_pixels(test_y);
    //     }
    // }
    // else
    // {
    //   break;
    }
  }
}

void render_slimes(SDL_Rect *camera)
{
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].state == STATE_USED)
    {
      SDL_Rect src = {slimes[i].entity.tile, 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {slimes[i].entity.x - camera->x, slimes[i].entity.y - camera->y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(renderer, textures[TEX_MONSTER_SPRITE_SHEET], &src, &dst);
    }
    else
    {
      break;
    }
  }
}