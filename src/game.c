#include <moonbreath_mountain.h>
#include <time.h>

// TODO:
// 
// Should add support for %s, %d etc for the render_text function.
//
// Implement diagonal controls???

int main(int argc, char **argv)
{
  /* -- RANDOM SEED -- */

  srand(time(NULL));
  
  int game_is_running = 1;
  int display_player_inventory = 0;
  int inv_item_count = 0;
  int inv_hl_index = 0;

  // camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  // textures
  SDL_Texture *tileset_tex = NULL;
  SDL_Texture *player_tileset_tex = NULL;
  SDL_Texture *tilemap_tex = NULL;
  SDL_Texture *item_tileset_tex = NULL;
  SDL_Texture *inv_tex = NULL;
  SDL_Texture *player_inv_hl_tex = NULL;
  SDL_Texture *inv_item_tex = NULL;
  SDL_Texture *interface_console_tex = NULL;
  SDL_Texture *interface_stats_tex = NULL;

  // fonts
  font_t *font_console = NULL;
  font_t *font_inv = NULL;
  font_t *font_item = NULL;

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
  player->turns_taken = 0;

  generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2, player);

  // NOTE(Rami): 
  // print the tile we want based on the number in the map array
  #if 0
    for(int y = 0; y < MAP_SIZE; y++)
    {
        for(int x = 0; x < MAP_SIZE; x++)
        {
            if(map[y * MAP_SIZE + x] == TILE_FLOOR_GRASS)
            {
              printf(".");
            }
            else if(map[y * MAP_SIZE + x] == TILE_WALL_STONE)
            {
              printf("#");
            }
            else if(map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
            {
              printf("D");
            }
            else if(map[y * MAP_SIZE + x] == TILE_PATH_UP || map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
            {
              printf("S");
            }
        }

        printf("\n");
    }
  #endif

  if(!game_init(player, &font_console, &font_inv, &font_item, &tileset_tex, &player_tileset_tex, &item_tileset_tex, &tilemap_tex, &inv_tex, &player_inv_hl_tex, &inv_item_tex, &interface_console_tex, &interface_stats_tex))
  {
    printf("Game failed to initialize\n");

    game_is_running = 0;
  }

  while(game_is_running)
  {
    SDL_RenderClear(renderer);

    if(handle_events(&current_key))
    {
      game_is_running = 0;
    }

    handle_input(level, player, &game_is_running, &current_key, &display_player_inventory, &inv_hl_index, &inv_item_count);

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

    // for (int i = 0; i < GAME_ITEMS_COUNT; i++)
    // {
    //   if (game_items[i].item_id != ID_NONE)
    //   {
    //     printf("[ITEM]\n");
    //     printf("item_id %d\n", game_items[i].item_id);
    //     printf("unique_id %d\n", game_items[i].unique_id);
    //     printf("is_on_ground %d\n", game_items[i].is_on_ground);
    //     printf("is_equipped %d\n", game_items[i].is_equipped);
    //     printf("x %d\n", game_items[i].x);
    //     printf("y %d\n\n", game_items[i].y);
    //   }
    // }

    // NOTE(Rami): bind the turns to the player entity

    update_camera(&camera, player);

    // update_lighting(dungeon, fov_map, player);

    render_level(tileset_tex, tilemap_tex, level, fov, &camera);

    render_items(item_tileset_tex, &camera);

    render_player(player_tileset_tex, item_tileset_tex, &camera, player);

    if(display_player_inventory)
    {
      render_inventory(inv_tex, player_inv_hl_tex, inv_item_tex, font_inv, font_item, &inv_hl_index, &inv_item_count);
    }

    render_interface(player, interface_console_tex, interface_stats_tex, font_console);

    SDL_RenderPresent(renderer);
  }

  game_exit(tileset_tex, player_tileset_tex, tilemap_tex, item_tileset_tex, inv_tex, player_inv_hl_tex, inv_item_tex, player, font_console, font_inv, font_item, interface_console_tex, interface_stats_tex);
  return 0;
}