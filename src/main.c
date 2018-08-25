#include <stdio.h>
#include <string.h>

#include "game.h"

SDL_Rect tiles[NUMBER_OF_TILES_ON_TILESHEET];
unsigned char map[MAP_SIZE * MAP_SIZE];
unsigned char fov_map[MAP_SIZE * MAP_SIZE];
//char *console_log[20];

int main()
{
  SDL_Window *window = NULL;

  SDL_Renderer *renderer = NULL;

  SDL_Texture *tileset_tex = NULL;
  SDL_Texture *player_tileset_tex = NULL;
  SDL_Texture *tilemap_tex = NULL;
  
  // init entities
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    entities[i] = NULL;
  }

  player_t *player = player_new(0);
  player->player_entity = entity_new(0, 0, 0, 32, 32, 1, 6);

  // the camera, which is the whole window
  //SDL_Rect camera = {0, 0, GAME_WIDTH, GAME_HEIGHT};
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 160};

  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 5, player->player_entity);

  // print the tile we want based on the number in the map array
  #if 1
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

      process_events(map, player->player_entity, &game_is_running);

      update_camera(&camera, player->player_entity);

      //update_lighting(map, fov_map, player->player_entity);
      
      render_background_texture(renderer, tileset_tex, tilemap_tex, map, fov_map, &camera);

      render_player(renderer, player_tileset_tex, &camera, player->player_entity);

      SDL_RenderPresent(renderer);
    }
  }

  cleanup(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, player);
  return 0;
}