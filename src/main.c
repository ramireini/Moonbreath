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
  SDL_Texture *player_inventory_tex = NULL;
  SDL_Texture *player_inventory_highlight_tex = NULL;
  SDL_Texture *player_inventory_item_tex = NULL;
  SDL_Texture *glyph_atlas = NULL;


  TTF_Font *font_console = NULL;
  TTF_Font *font_inventory = NULL;
  TTF_Font *font_item = NULL;

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
    console_messages[i].message[0] = '.';
    console_messages[i].r = 0;
    console_messages[i].g = 0;
    console_messages[i].b = 0;
  }

  // init inventory
  for (int i = 0; i < 10; i++)
  {
    inventory[i].name[0] = '.';
  }

  player_t *player = new_player();
  player->entity = new_entity(10, 0, 0, 32, 32, 1, 6);

  // the camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 2, player->entity);

  // NOTE(Rami): we could have all the item information in some file like items.cfg etc and just load that
  item_info[ITEM_HEALTH_POTION] = (item_info_t){"Health Potion", "Restores a partial amount of health", "A magical red liquid created with an unknown formula. Consuming it is said to heal simple cuts and even grievous wounds"};

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
    // NOTE(Rami): make sure they're not NULL
    // initialize fonts
    font_console = TTF_OpenFont("data/fonts/classic.ttf", 16);
    font_inventory = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
    font_item = TTF_OpenFont("data/fonts/arialnb.ttf", 16);

    // NOTE(Rami): make sure they're not NULL
    // initialize textures
    tileset_tex = load_texture(renderer, "data/images/tileset.png");
    player_tileset_tex = load_texture(renderer, "data/images/player_tileset.png");
    itemset_tex = load_texture(renderer, "data/images/itemset.png");
    tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
    player_inventory_tex = load_texture(renderer, "data/images/player_inventory.png");
    player_inventory_highlight_tex = load_texture(renderer, "data/images/player_inventory_highlight.png");
    player_inventory_item_tex = load_texture(renderer, "data/images/player_inventory_item.png");
    glyph_atlas = create_texture_atlas(renderer, font_console);
    SDL_SetTextureBlendMode(player_inventory_highlight_tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(player_inventory_highlight_tex, 30);

    // gameloop flag
    int game_is_running = 1;

    int display_player_inventory = 0;
    int player_inventory_current_item_amount = 0;
    int player_inventory_highlight_index = 0;

    int update_logic = 1;

    int turns_taken = 0;

    // set renderer clear color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // main game loop
    while (game_is_running)
    {
      SDL_RenderClear(renderer);

      process_events(&game_is_running, &current_key);

      process_input(map, player->entity, &game_is_running, &current_key, &display_player_inventory, &player_inventory_highlight_index, &player_inventory_current_item_amount, &update_logic);

      if (update_logic)
      {
        turns_taken += 1;
        //printf("turns taken = %d\n\n", turns_taken - 1);

        update_camera(&camera, player->entity);

        //update_lighting(map, fov_map, player->entity);
        
        // set to false
        update_logic = !update_logic;
      }

      render_level(renderer, tileset_tex, tilemap_tex, map, fov_map, &camera);

      render_items(renderer, itemset_tex, &camera);

      render_player(renderer, player_tileset_tex, &camera, player->entity);

      if (display_player_inventory)
      {
        //render_inventory(renderer, player_inventory_tex, player_inventory_highlight_tex, player_inventory_item_tex, font_inventory, font_item, &player_inventory_highlight_index, &player_inventory_current_item_amount);
      }

      //render_console_messages(renderer, font_console);

      SDL_Rect r;
      r.x = 0;
      r.y = 0;
      r.w = 1024;
      r.h = 1024;

      SDL_RenderCopy(renderer, glyph_atlas, NULL, &r);

      SDL_RenderPresent(renderer);
    }
  }

  free_resources(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, itemset_tex, player_inventory_tex, player_inventory_highlight_tex, player_inventory_item_tex, player, glyph_atlas, font_console, font_inventory, font_item);
  return 0;
}