#include <game.h>
#include <time.h>

// TODO:
//
// All the // NOTE(Rami):'s
//
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows.
// This would also make it easier to generate the interface for multiple game resolutions.
//
// Instead of passing a billion pointers to game_init, have textures be in an array of
// texture pointers, then allocate each texture into it, we can have an enum table
// so we know what texture exactly each element of the array is,
// this also means we only have to pass the array as a pointer to functions
// and then that function can use whatever textures it needs from it.
//
// Implement diagonal controls if we decide to have them

int main(int argc, char **argv)
{
  char *level = malloc(LEVEL_SIZE * LEVEL_SIZE);
  char *fov = malloc(LEVEL_SIZE * LEVEL_SIZE);

  // camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  player_t *player = malloc(sizeof(player_t));
  strcpy(player->name, "Frozii");
  player->level = 0;
  player->money = 0;
  player->hp = 5;
  player->max_hp = 10;
  player->xp = 0;
  player->x = 0;
  player->y = 0;
  player->w = TILE_SIZE;
  player->h = TILE_SIZE;
  player->speed = 1;
  player->fov = 6;
  player->attack = 5;
  player->armor = 6;
  player->turns_taken = 0;
  player->inventory_display = 0;
  player->inventory_item_count = 0;
  player->inventory_hl_index = 0;

  // NOTE(Rami): 
  // print the tile we want based on the number in the level array
  #if 0
    for(int y = 0; y < LEVEL_SIZE; y++)
    {
        for(int x = 0; x < LEVEL_SIZE; x++)
        {
            if(level[y * LEVEL_SIZE + x] == TILE_FLOOR_GRASS)
            {
              printf(".");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_WALL_STONE)
            {
              printf("#");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_DOOR_CLOSED)
            {
              printf("D");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_PATH_UP || level[y * LEVEL_SIZE + x] == TILE_PATH_DOWN)
            {
              printf("S");
            }
        }

        printf("\n");
    }
  #endif

  if(game_init())
  {
    printf("Game failed to initialize\n");
    game_is_running = 0;
  }

  generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2, player);

  // add some items :p
  add_game_item(ID_LESSER_HEALTH_POTION, player->x - 32, player->y);
  add_game_item(ID_IRON_SWORD, player->x + 32, player->y);

  while(game_is_running)
  {
    SDL_RenderClear(renderer);

    update_events();

    update_input(level, player);

    // NOTE(Rami):
    // for (int i = 0; i < INVENTORY_COUNT; i++)
    // {
    //   if (inventory[i].unique_id)
    //   {
    //     printf("[ITEM]\n");
    //     printf("item_id %d\n", inventory[i].item_id);
    //     printf("unique_id %d\n", inventory[i].unique_id);
    //     printf("is_on_ground %d\n", inventory[i].is_on_ground);
    //     printf("equipped %d\n", inventory[i].is_equipped);
    //     printf("x %d\n", inventory[i].x);
    //     printf("y %d\n\n", inventory[i].y);
    //   }
    // }

    // for (int i = 0; i < ITEM_COUNT; i++)
    // {
    //   if (items[i].item_id != ID_NONE)
    //   {
    //     printf("[ITEM]\n");
    //     printf("item_id %d\n", items[i].item_id);
    //     printf("unique_id %d\n", items[i].unique_id);
    //     printf("is_on_ground %d\n", items[i].is_on_ground);
    //     printf("is_equipped %d\n", items[i].is_equipped);
    //     printf("x %d\n", items[i].x);
    //     printf("y %d\n\n", items[i].y);
    //   }
    // }

    // NOTE(Rami): bind the turns to the player entity

    // update_lighting(dungeon, fov, player);

    update_camera(&camera, player);

    render_level(level, fov, &camera);

    render_items(&camera);

    render_player(&camera, player);

    if(player->inventory_display)
    {
      render_inventory(player);
    }

    render_interface(player);

    SDL_RenderPresent(renderer);
  }

  game_exit(level, fov, player);
  return 0;
}