#include <moonbreath_mountain.h>

// TODO:
// 
// Work on the [A]scend, [D]escend functionality
// 
// Render equipment on top of the player
// 
// Implement diagonal controls???

int main(int argc, char **argv)
{
  int game_is_running = 1;
  int display_player_inventory = 0;
  int inv_item_count = 0;
  int inv_hl_index = 0;

  // camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  // window
  SDL_Window *window = NULL;

  // renderer
  SDL_Renderer *renderer = NULL;

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

  player_t *player = new_player();
  player->entity = new_entity("FrozenZerker", // name
                              0,              // level
                              0,              // money
                              5,              // hp
                              10,             // max_hp
                              5,              // xp
                              0,              // x
                              0,              // y
                              TILE_SIZE,      // w
                              TILE_SIZE,      // h
                              1,              // speed
                              6);             // fov


  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 8, player->entity);

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
            else if(map[y * MAP_SIZE + x] == TILE_STAIRS_UP || map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
            {
              printf("S");
            }
        }

        printf("\n");
    }
  #endif

  if(!game_init(&window, &renderer, player, &font_console, &font_inv, &font_item, &tileset_tex, &player_tileset_tex, &item_tileset_tex, &tilemap_tex, &inv_tex, &player_inv_hl_tex, &inv_item_tex, &interface_console_tex, &interface_stats_tex))
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

    handle_input(map, player->entity, &game_is_running, &current_key, &display_player_inventory, &inv_hl_index, &inv_item_count);

    // // NOTE(Rami):
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

    update_camera(&camera, player->entity);

    // update_lighting(map, fov_map, player->entity);

    render_level(renderer, tileset_tex, tilemap_tex, map, fov_map, &camera);

    render_items(renderer, item_tileset_tex, &camera);

    render_player(renderer, player_tileset_tex, item_tileset_tex, &camera, player->entity);

    if(display_player_inventory)
    {
      render_inventory(renderer, inv_tex, player_inv_hl_tex, inv_item_tex, font_inv, font_item, &inv_hl_index, &inv_item_count);
    }

    render_interface(renderer, player->entity, interface_console_tex, interface_stats_tex, font_console);

    SDL_RenderPresent(renderer);
  }

  game_exit(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, item_tileset_tex, inv_tex, player_inv_hl_tex, inv_item_tex, player, font_console, font_inv, font_item, interface_console_tex, interface_stats_tex);
  return 0;
}