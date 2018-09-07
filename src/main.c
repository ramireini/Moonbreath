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

  // init console messages
  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    console_messages[i].message = NULL;
    console_messages[i].r = 0;
    console_messages[i].g = 0;
    console_messages[i].b = 0;
  }

  // init entities
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    entities[i] = NULL;
  }

  player_t *player = new_player(0);
  player->player_entity = new_entity(0, 0, 0, 32, 32, 1, 6);

  // the camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 10, player->player_entity);

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

    // set renderer clear color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    tileset_tex = load_texture(renderer, "data/images/tileset.png");
    player_tileset_tex = load_texture(renderer, "data/images/player_tileset.png");
    tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);

    // main game loop
    while (game_is_running)
    {
      SDL_RenderClear(renderer);

      process_events(&game_is_running, &current_key);

      update_game(map, player->player_entity, &game_is_running, &current_key);

      update_camera(&camera, player->player_entity);

      //update_lighting(map, fov_map, player->player_entity);
      
      render_level(renderer, tileset_tex, tilemap_tex, map, fov_map, &camera);

      render_player(renderer, player_tileset_tex, &camera, player->player_entity);

      render_console_messages(renderer);

      SDL_RenderPresent(renderer);
    }
  }

  cleanup(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, player);
  return 0;
}