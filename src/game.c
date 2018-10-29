#include <stdio.h>
#include <string.h>
#include "inc/util_conf.h"
#include "moonbreath_mountain.h"

// TODO:
// Make dropping also unequip the item if it's currently on you.
// 
// Render equipment on top of the player
// 
// Implement diagonal controls?

int main(int argc, char **argv)
{
  conf_t conf;
  if(!conf_load(&conf, "inc/items.cfg"))
  {
    return 0;
  }

  SDL_Window *window = NULL;

  SDL_Renderer *renderer = NULL;

  SDL_Texture *tileset_tex = NULL;
  SDL_Texture *player_tileset_tex = NULL;
  SDL_Texture *tilemap_tex = NULL;
  SDL_Texture *item_tileset_tex = NULL;
  SDL_Texture *inv_tex = NULL;
  SDL_Texture *player_inv_hl_tex = NULL;
  SDL_Texture *inv_item_tex = NULL;
  SDL_Texture *interface_console_tex = NULL;
  SDL_Texture *interface_stats_tex = NULL;

  font_t *font_console = NULL;
  font_t *font_inv = NULL;
  font_t *font_item = NULL;

  // init entities
  for(int i = 0; i < ENTITY_COUNT; i++)
  {
    entities[i] = NULL;
  }

  // init game items
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    game_items[i].item_id = -1;
    game_items[i].unique_id = i;
    game_items[i].is_on_ground = -1;
    game_items[i].is_equipped = -1;
    game_items[i].x = -1;
    game_items[i].y = -1;
  }

  // init inventory
  for(int i = 0; i < INVENTORY_COUNT; i++)
  {
    inventory[i].item_id = -1;
    inventory[i].unique_id = -1;
    inventory[i].is_on_ground = -1;
    inventory[i].is_equipped = -1;
    inventory[i].x = -1;
    inventory[i].y = -1;
  }

  // init console messages
  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    console_messages[i].msg[0] = '.';
    console_messages[i].msg_color = 0;
  }

  player_t *player = new_player();
  player->entity = new_entity("FrozenZerker", 0, 0, 5, 10, 5, 0, 0, 32, 32, 1, 6);

  // the camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 8, player->entity);

  // the game_items_info does not have an ID, but the game_items does have an ID,
  // game_items will get it's id from the items.cfg file, game_items_info uses everything else
  // from the .cfg file, just not the ID.
  game_items_info[0].type = conf.vars[1].conf_var_u.i;
  game_items_info[0].tile = conf.vars[2].conf_var_u.i;
  strcpy(game_items_info[0].name, conf.vars[3].conf_var_u.s);
  strcpy(game_items_info[0].use, conf.vars[4].conf_var_u.s);
  game_items_info[0].damage = conf.vars[5].conf_var_u.i;
  game_items_info[0].armor = conf.vars[6].conf_var_u.i;
  strcpy(game_items_info[0].description, conf.vars[7].conf_var_u.s);

  // Health Potion
  game_items[0].item_id = conf.vars[0].conf_var_u.i;
  game_items[0].is_on_ground = 1;
  game_items[0].is_equipped = 0;
  game_items[0].x = player->entity->x + 32;
  game_items[0].y = player->entity->y;

  game_items_info[1].type = conf.vars[9].conf_var_u.i;
  game_items_info[1].tile = conf.vars[10].conf_var_u.i;
  strcpy(game_items_info[1].name, conf.vars[11].conf_var_u.s);
  strcpy(game_items_info[1].use, conf.vars[12].conf_var_u.s);
  game_items_info[1].damage = conf.vars[13].conf_var_u.i;
  game_items_info[1].armor = conf.vars[14].conf_var_u.i;
  strcpy(game_items_info[1].description, conf.vars[15].conf_var_u.s);

  // Iron Sword
  game_items[1].item_id = conf.vars[8].conf_var_u.i;
  game_items[1].is_on_ground = 1;
  game_items[1].is_equipped = 0;
  game_items[1].x = player->entity->x - 32;
  game_items[1].y = player->entity->y;

  game_items[2].item_id = conf.vars[8].conf_var_u.i;
  game_items[2].is_on_ground = 1;
  game_items[2].is_equipped = 0;
  game_items[2].x = player->entity->x;
  game_items[2].y = player->entity->y - 32;

  conf_free(&conf);

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

  // load media
  if(!initialize(&window, &renderer))
  {
    printf("Failed to initialize\n");
  }
  else
  {
    int game_is_running = 1;
    int display_player_inventory = 0;
    int inv_item_count = 0;
    int inv_hl_index = 0;

    // initialize font atlases    
    TTF_Font *font = NULL;
    font = TTF_OpenFont("data/fonts/classic.ttf", 16);
    font_console = create_font_atlas(renderer, font);
    TTF_CloseFont(font);

    font = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
    font_inv = create_font_atlas(renderer, font);
    TTF_CloseFont(font);

    font = TTF_OpenFont("data/fonts/hello-world.ttf", 13);
    font_item = create_font_atlas(renderer, font);
    TTF_CloseFont(font);
    font = NULL;

    if(!font_console || !font_inv || !font_item)
    {
      game_is_running = 0;
      printf("Could not create font atlases\n");
    }

    //initialize textures
    tileset_tex = load_texture(renderer, "data/images/tileset.png");
    player_tileset_tex = load_texture(renderer, "data/images/player_tileset.png");
    item_tileset_tex = load_texture(renderer, "data/images/item_tileset.png");
    tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
    inv_tex = load_texture(renderer, "data/images/player_inventory.png");
    player_inv_hl_tex = load_texture(renderer, "data/images/player_inventory_highlight.png");
    inv_item_tex = load_texture(renderer, "data/images/player_inventory_item.png");
    interface_console_tex = load_texture(renderer, "data/images/interface_console.png");
    interface_stats_tex = load_texture(renderer, "data/images/interface_statistics.png");

    if(!tileset_tex || !player_tileset_tex || !item_tileset_tex || !tilemap_tex || !inv_tex || !player_inv_hl_tex || !player_inv_hl_tex || !interface_console_tex || !interface_stats_tex)
    {
      game_is_running = 0;
      printf("Could not load textures\n");
    }
    else
    {
      SDL_SetTextureBlendMode(player_inv_hl_tex, SDL_BLENDMODE_BLEND);
      SDL_SetTextureAlphaMod(player_inv_hl_tex, 30);
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
      // for (int i = 0; i < 10; i++)
      // {
      //   if (inventory[i].unique_id != -1)
      //   {
      //     printf("[ITEM]\n");
      //     printf("id %d\n", inventory[i].item_id);
      //     printf("number_id %d\n", inventory[i].unique_id);
      //     printf("is_on_ground %d\n", inventory[i].is_on_ground);
      //     printf("equipped %d\n", inventory[i].is_equipped);
      //     printf("x %d\n", inventory[i].x);
      //     printf("y %d\n\n", inventory[i].y);
      //   }
      // }

      for (int i = 0; i < 3; i++)
      {
        if (game_items[i].unique_id != -1)
        {
          printf("[ITEM]\n");
          printf("id %d\n", game_items[i].item_id);
          printf("number_id %d\n", game_items[i].unique_id);
          printf("is_on_ground %d\n", game_items[i].is_on_ground);
          printf("equipped %d\n", game_items[i].is_equipped);
          printf("x %d\n", game_items[i].x);
          printf("y %d\n\n", game_items[i].y);
        }
      }

      // NOTE(Rami): bind the turns to the player entity

      update_camera(&camera, player->entity);

      //update_lighting(map, fov_map, player->entity);

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
  }

  free_resources(window, renderer, tileset_tex, player_tileset_tex, tilemap_tex, item_tileset_tex, inv_tex, player_inv_hl_tex, inv_item_tex, player, font_console, font_inv, font_item, interface_console_tex, interface_stats_tex);
  return 0;
}