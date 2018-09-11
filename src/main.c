#include <stdio.h>
#include <string.h>

#include "game.h"

int main()
{
  SDL_Window *window = NULL;

  SDL_Renderer *renderer = NULL;

  SDL_Texture *tileset_tex = NULL;
  SDL_Texture *player_tileset_tex = NULL;
  SDL_Texture *tilemap_tex = NULL;
  SDL_Texture *itemset_tex = NULL;

  // init entities
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    entities[i] = NULL;
  }

  // init items
  for (int i = 0; i < 10; i++)
  {
    items[i].id = 0;
    items[i].active = 0;
    items[i].x = 0;
    items[i].y = 0;
    items[i].tile = 0;
  }

  // init console messages
  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    console_messages[i].message = NULL;
    console_messages[i].r = 0;
    console_messages[i].g = 0;
    console_messages[i].b = 0;
  }

  // init inventory
  for (int i = 0; i < 10; i++)
  {
    inventory[i] = NULL;
  }

  player_t *player = new_player();
  player->entity = new_entity(10, 0, 0, 32, 32, 1, 6);

  // the camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 10, player->entity);

  item_info[ITEM_HEALTH_POTION] = (item_info_t){"Health Potion"};

  items[0].id = ITEM_HEALTH_POTION;
  items[0].active = 1;
  items[0].x = player->entity->x + 32;
  items[0].y = player->entity->y;
  items[0].tile = ITEM_HEALTH_POTION;

  // print the tile we want based on the number in the map array
  #if 0
    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
        {
            if (map[y * MAP_SIZE + x] == TILE_FLOOR_GRASS)
            {
              printf(".");
            }
            else if (map[y * MAP_SIZE + x] == TILE_WALL_STONE)
            {
              printf("#");
            }
            else if (map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
            {
              printf("D");
            }
            else if (map[y * MAP_SIZE + x] == TILE_STAIRS_UP || map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
            {
              printf("S");
            }
        }

        printf("\n");
    }
  #endif

  // load media
  if (!initialize(&window, &renderer))
  {
    printf("ERROR: Failed to initialize\n");
  }
  else
  {
    // gameloop flag
    int game_is_running = 1;

    int display_inventory = 0;

    // set renderer clear color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    tileset_tex = load_texture(renderer, "data/images/tileset.png");
    player_tileset_tex = load_texture(renderer, "data/images/player_tileset.png");
    itemset_tex = load_texture(renderer, "data/images/itemset.png");
    tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);

    // main game loop
    while (game_is_running)
    {
      SDL_RenderClear(renderer);

      process_events(&game_is_running, &current_key);

      update_game(map, player->entity, &game_is_running, &current_key, &display_inventory);

      update_camera(&camera, player->entity);

      //update_lighting(map, fov_map, player->player_entity);
      
      render_level(renderer, tileset_tex, tilemap_tex, map, fov_map, &camera);

      render_items(renderer, itemset_tex, &camera);

      render_player(renderer, player_tileset_tex, &camera, player->entity);

      if (display_inventory)
      {
        render_inventory(renderer);
      }

      render_console_messages(renderer);

      SDL_RenderPresent(renderer);
    }
  }

  cleanup(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, itemset_tex, player);
  return 0;
}