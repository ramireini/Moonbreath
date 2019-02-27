#include <moonbreath_mountain.h>
#include <util_conf.h>
#include <stdarg.h>

SDL_Color hex_to_rgba_color(int hex_color)
{
  // shift and mask the rgb out of the hex color
  int r = (hex_color >> 24) & 0xFF;
  int g = (hex_color >> 16) & 0xFF;
  int b = (hex_color >> 8) & 0xFF;
  int a = hex_color & 0xFF;

  SDL_Color rgb_color = {r, g, b, a};

  return rgb_color;
}

void add_console_msg(char *msg, int msg_color, ...)
{
  // holds the final message
  char msg_final[256];

  // create an argument list and initialize it
  // to take arguments after the msg_color parameter
  va_list arg_list;
  va_start(arg_list, msg_color);

  // print str to the str_final array and
  // add the format specifiers from arg_list
  vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);

  // close the argument list
  va_end(arg_list);

  // fill the initial space of the console log
  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(console_messages[i].msg[0] == '.')
    {
      strcpy(console_messages[i].msg, msg_final);
      console_messages[i].msg_color = msg_color;

      return;
    }
  }

  // remove the oldest message
  console_messages[0].msg[0] = '.';
  console_messages[0].msg_color = 0;

  // move all messages starting from the second oldest message to create space for the new message
  for(int i = 1; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    strcpy(console_messages[i - 1].msg, console_messages[i].msg);
    console_messages[i - 1].msg_color = console_messages[i].msg_color;
  }

  // add the new message to the console log
  strcpy(console_messages[CONSOLE_MESSAGE_COUNT - 1].msg, msg_final);
  console_messages[CONSOLE_MESSAGE_COUNT - 1].msg_color = msg_color;

  return;
}

void handle_events(int *key_pressed)
{
  // an event struct to hold the current event information
  SDL_Event event;
  SDL_WaitEvent(&event);

  if(event.type == SDL_QUIT)
  {
    // NOTE(Rami): remove later
    printf("SDL_QUIT\n");
    game_is_running = 0;
  }
  else if(event.type == SDL_KEYDOWN)
  {
    // set our key_pressed to the key that was pressed down
    *key_pressed = event.key.keysym.sym;
  }
}

void handle_input(char *level, player_t *player, int *key_pressed)
{
  if(*key_pressed == SDLK_ESCAPE)
  {
    // NOTE(Rami): remove later
    printf("SDLK_ESCAPE\n");
    game_is_running = 0;
  }

  /* -- IN INVENTORY -- */

  else if(player->inventory_display)
  {
    switch(*key_pressed)
    {
      case SDLK_k:
      {
        // if the highlight index can't go any lower
        // meaning that this is the top most item we can be on
        if(player->inventory_hl_index - 1 < 0)
        {
          // then if we have more than one item in the inventory
          if(player->inventory_item_count > 0)
          {
            // set the highlight index to be the last item
            player->inventory_hl_index = player->inventory_item_count - 1;
          }
        }
        // else we can substract because we're not on the top most item
        else
        {
          (player->inventory_hl_index)--;
        }

        *key_pressed = 0;
      } break;

      case SDLK_j:
      {
        // if the highlight index can't go any higher
        // meaning that this is the bottom item we can be on
        if(player->inventory_hl_index + 1 > player->inventory_item_count - 1)
        {
          // set the highlight index to the first item
          player->inventory_hl_index = player->inventory_item_count = 0;
        }
        // else we can add because we're not on the most bottom item
        else
        {
          (player->inventory_hl_index)++;
        }

        *key_pressed = 0;
      } break;

      case SDLK_i:
      {
        // do not display inventory anymore
        // reset highlight index
        player->inventory_display = 0;
        player->inventory_hl_index = 0;

        *key_pressed = 0;
      } break;

      case SDLK_d:
      {
        drop_or_remove_inventory_item(player, 1);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_hl_index + 1 == player->inventory_item_count)
        {
          if(player->inventory_hl_index - 1 >= 0)
          {
            player->inventory_hl_index--;
          }
        }

        *key_pressed = 0;
      } break;

      case SDLK_e:
      {
        equip_or_unequip_item(player);

        *key_pressed = 0;
      } break;

      case SDLK_c:
      {
        consume_item(player);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_hl_index + 1 == player->inventory_item_count)
        {
          if(player->inventory_hl_index - 1 >= 0)
          {
            player->inventory_hl_index--;
          }
        }

        *key_pressed = 0;
      } break;
    }
  }

  /* -- NOT IN INVENTORY -- */

  else if(!player->inventory_display)
  {
    switch(*key_pressed)
    {
      case SDLK_k:
      {
        entity_move(level, player, 0, to_pixels(-player->speed));
        *key_pressed = 0;
      } break;

      case SDLK_j:
      {
        entity_move(level, player, 0, to_pixels(player->speed));

        *key_pressed = 0;
      } break;

      case SDLK_h:
      {
        entity_move(level, player, to_pixels(-player->speed), 0);

        *key_pressed = 0;
      } break;

      case SDLK_l:
      {
        entity_move(level, player, to_pixels(player->speed), 0);

        *key_pressed = 0;
      } break;

      case SDLK_i:
      {
        player->inventory_display = 1;

        *key_pressed = 0;
      } break;

      case SDLK_COMMA:
      {
        add_inventory_item(player);

        *key_pressed = 0;
      } break;

      case SDLK_d:
      {
        int p_x = to_tiles(player->x);
        int p_y = to_tiles(player->y);

        // if path is next to the player horizontally or vertically
        if(level[(p_y * LEVEL_SIZE) + (p_x + 1)] == TILE_PATH_DOWN ||
           level[(p_y * LEVEL_SIZE) + (p_x - 1)] == TILE_PATH_DOWN ||
           level[((p_y + 1) * LEVEL_SIZE) + p_x] == TILE_PATH_DOWN ||
           level[((p_y - 1) * LEVEL_SIZE) + p_x] == TILE_PATH_DOWN)
        {
          add_console_msg("You travel deeper into the mountain..", TEXT_COLOR_WHITE);
          generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2, player);
        }

        *key_pressed = 0;
      } break;

      case SDLK_a:
      {
        int p_x = to_tiles(player->x);
        int p_y = to_tiles(player->y);

        // if path is next to the player horizontally or vertically
        if(level[(p_y * LEVEL_SIZE) + (p_x + 1)] == TILE_PATH_UP ||
           level[(p_y * LEVEL_SIZE) + (p_x - 1)] == TILE_PATH_UP ||
           level[((p_y + 1) * LEVEL_SIZE) + p_x] == TILE_PATH_UP ||
           level[((p_y - 1) * LEVEL_SIZE) + p_x] == TILE_PATH_UP)
        {
          printf("You flee from the mountain..\n");
          game_is_running = 0;
        }

        *key_pressed = 0;
      } break;
    }
  }
}

// void update_lighting(char *level, char *fov, player_t *player)
// {
//   // set all elements as not visible
//   for(int y = 0; y < LEVEL_SIZE; y++)
//   {
//     for(int x = 0; x < LEVEL_SIZE; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for(int y = (player.y / TILE_SIZE) - player.fov; y < (player.y / TILE_SIZE) + player.fov; y++)
//   {
//     for(int x = (player.x / TILE_SIZE) - player.fov; x < (player.x / TILE_SIZE) + player.fov; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 255;
//     }
//   }
//   #endif

//   // raycasted lighting
//   #if 0
//   for(int angle = 0; angle < 360; angle++)
//   {
//     // calculate the amount for the ray to progress
//     float dx = 0.1 * cos(angle);
//     float dy = 0.1 * sin(angle);

//     // set the ray to begin from the players location
//     float fx = player.x;
//     float fy = player.y;

//     for(;;)
//     {
//       // add to the rays location the amount we calculated
//       fx += dx;
//       fy += dy;

//       float dist = distance(player.x + 16, player.y + 16, fx, fy);
//       //int idist = dist / 32;

//       //printf("dist_between: %d\n", idist);

//       // if the ray is over the players view distance then stop the ray
//       if(dist > (player.fov * TILE_SIZE))
//       {
//         break;
//       }

//       // convert to array valid values
//       int ifx = fx / 32;
//       int ify = fy / 32;

//       // make sure the ray isn't going off the level
//       if(ifx >= 0 && ifx <= LEVEL_SIZE && ify >= 0 && ify <= LEVEL_SIZE)
//       {
//         //fov[ify * LEVEL_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov[ify * LEVEL_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if(level[ify * LEVEL_SIZE + ifx] == TILE_WALL_STONE || level[ify * LEVEL_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov[(player.y / TILE_SIZE) * LEVEL_SIZE + ((player.x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }

void update_camera(SDL_Rect *camera, player_t *player)
{
  // center camera on player
  camera->x = player->x - (camera->w / 2);
  camera->y = (player->y + (player->h / 2)) - (camera->h / 2);

  // stop the camera if it goes out of bounds
  if(camera->x < 0)
  {
    camera->x = 0;
  }

  if(camera->y < 0)
  {
    camera->y = 0;
  }

  if(camera->x >= LEVEL_WIDTH - camera->w)
  {
    camera->x = LEVEL_WIDTH - camera->w;
  }

  if(camera->y >= LEVEL_HEIGHT - camera->h)
  {
    camera->y = LEVEL_HEIGHT - camera->h;
  }
}

// NOTE(Rami): the return value is for the x-flip, think about if we really want it
void entity_move(char *level, player_t *entity, int x, int y)
{
  int entity_x = (entity->x + x) / TILE_SIZE;
  int entity_y = (entity->y + y) / TILE_SIZE;

  if(entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    // NOTE(Rami): 
    // if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_FLOOR_STONE)
    if(level[(entity_y * LEVEL_SIZE) + entity_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the door open", TEXT_COLOR_WHITE);
      level[(entity_y * LEVEL_SIZE) + entity_x] = TILE_DOOR_OPEN;
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_PATH_UP)
    {
      add_console_msg("A path that leads back to the surface.. [A]scend and flee the mountain?", TEXT_COLOR_WHITE);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_PATH_DOWN)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", TEXT_COLOR_WHITE);
    }
  }
}

// // NOTE(Rami): !!!
// // entity_t* new_entity(char *name, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov)
// // {
// //   for(int i = 0; i < ENTITY_COUNT; i++)
// //   {
// //     if(!entities[i])
// //     {
// //       entities[i] = malloc(sizeof(entity_t));

// //       strcpy(entities[i]->name, name);
// //       entities[i]->level = level;
// //       entities[i]->money = money;
// //       entities[i]->hp = hp;
// //       entities[i]->max_hp = max_hp;
// //       entities[i]->xp = xp;
// //       entities[i]->x = x;
// //       entities[i]->y = y;
// //       entities[i]->w = w;
// //       entities[i]->h = h;
// //       entities[i]->speed = speed;
// //       entities[i]->fov = fov;

// //       return entities[i];
// //     }
// //   }

// //   return NULL;
// // }

int game_init(player_t *player, font_t **font_console, font_t **font_inv, font_t **font_item, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex)
{
  /* -- SDL-- */

  // initialize SDL video subsystem
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize: %s\n", SDL_GetError());

    return 0;
  }

  // create window
  window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!window)
  {
    printf("SDL could not create window: %s\n", SDL_GetError());

    return 0;
  }

  // create renderer for window
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer)
  {
    printf("SDL could not create a renderer: %s\n", SDL_GetError());

    return 0;
  }

  // initialize PNG loading
  int image_flags = IMG_INIT_PNG;
  if(!(IMG_Init(image_flags) & image_flags))
  {
    printf("SLD image library could not initialize: %s\n", IMG_GetError());

    return 0;
  }

  // initialize TTF library
  if(TTF_Init())
  {
    printf("SDL TTF library could not initialize: %s\n", TTF_GetError());

    return 0;
  }

  /* -- FONTS -- */

  TTF_Font *font = TTF_OpenFont("data/fonts/classic.ttf", 16);
  *font_console = create_font_atlas(font);
  TTF_CloseFont(font);

  font = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  *font_inv = create_font_atlas(font);
  TTF_CloseFont(font);

  font = TTF_OpenFont("data/fonts/hello-world.ttf", 13);
  *font_item = create_font_atlas(font);
  TTF_CloseFont(font);
  font = NULL;

  if(!(*font_console) || !(*font_inv) || !(*font_item))
  {
    printf("Could not create font atlases\n");

    return 0;
  }

  /* -- TEXTURES -- */

  *tileset_tex = load_texture("data/images/tileset.png");
  *player_tileset_tex = load_texture("data/images/player_tileset.png");
  *item_tileset_tex = load_texture("data/images/item_tileset.png");
  *tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
  *inv_tex = load_texture("data/images/player_inventory.png");
  *player_inv_hl_tex = load_texture("data/images/player_inventory_highlight.png");
  *inv_item_tex = load_texture("data/images/player_inventory_item.png");
  *interface_console_tex = load_texture("data/images/interface_console.png");
  *interface_stats_tex = load_texture("data/images/interface_statistics.png");

  if(!(*tileset_tex) || !(*player_tileset_tex) || !(*item_tileset_tex) || !(*tilemap_tex) || !(*inv_tex) || !(*player_inv_hl_tex) || !(*player_inv_hl_tex) || !(*interface_console_tex) || !(*interface_stats_tex))
  {
    printf("Could not load textures\n");

    return 0;
  }
  else
  {
    // set texture opacity
    SDL_SetTextureBlendMode(*player_inv_hl_tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(*player_inv_hl_tex, 30);
  }

  /* -- ARRAYS -- */
  
  // init game items
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    game_items[i].item_id = ID_NONE;
    game_items[i].unique_id = i + 1;
    game_items[i].is_on_ground = 0;
    game_items[i].is_equipped = 0;
    game_items[i].x = 0;
    game_items[i].y = 0;
  }

  // init inventory
  for(int i = 0; i < INVENTORY_COUNT; i++)
  {
    inventory[i].item_id = ID_NONE;
    inventory[i].unique_id = 0;
    inventory[i].is_on_ground = 0;
    inventory[i].is_equipped = 0;
    inventory[i].x = 0;
    inventory[i].y = 0;
  }

  // init console messages
  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    console_messages[i].msg[0] = '.';
    console_messages[i].msg_color = 0;
  }

  /* -- CONFIG -- */

  // NOTE(Rami): if the user edits the config file and turns something like key=1 into key = 1
  // then our arrays will get bogus values, we need to somehow make sure that the key value pairs
  // are in the format and ONLY in the format we expect.
  // 
  // load the config
  conf_t conf;
  if(!conf_load(&conf, "data/items.cfg"))
  {
    printf("Could not load config\n");

    return 0;
  }

  // assign config data into the game
  for(int i = 0; i < conf.key_value_pair_count / KEY_VALUE_PAIRS_PER_ITEM; i++)
  {
    int index = i * KEY_VALUE_PAIRS_PER_ITEM;

    game_items_info[i].item_id = conf.vars[index].conf_var_u.i;
    strcpy(game_items_info[i].name, conf.vars[index + 1].conf_var_u.s);
    game_items_info[i].item_type = conf.vars[index + 2].conf_var_u.i;
    game_items_info[i].tile = conf.vars[index + 3].conf_var_u.i;
    strcpy(game_items_info[i].use, conf.vars[index + 4].conf_var_u.s);
    game_items_info[i].hp_healed = conf.vars[index + 5].conf_var_u.i;
    game_items_info[i].damage = conf.vars[index + 6].conf_var_u.i;
    game_items_info[i].armor = conf.vars[index + 7].conf_var_u.i;
    strcpy(game_items_info[i].description, conf.vars[index + 8].conf_var_u.s);
  }

  conf_free(&conf);

  // add some items :p
  add_game_item(ID_LESSER_HEALTH_POTION, player->x - 32, player->y);
  add_game_item(ID_IRON_SWORD, player->x + 32, player->y);

  // all initialization was successful so run the game
  game_is_running = 1;

  return 1;
}

// NOTE(Rami): kinda no point in settings all of these to NULL after freeing
// because the game will close anyway, they won't be dereferenced anymore
void game_exit(SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *player_inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, font_t *font_console, font_t *font_inv, font_t *font_item, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex)
{
  if(player)
  {
    free(player);
    player = NULL;
  }

  if(font_console)
  {
    if(font_console->atlas)
    {
      SDL_DestroyTexture(font_console->atlas);
    }

    free(font_console);
    font_console = NULL;
  }

  if(font_inv)
  {
    if(font_inv->atlas)
    {
      SDL_DestroyTexture(font_inv->atlas); 
    }

    free(font_inv);
    font_inv = NULL;
  }

  if(font_item)
  {
    if(font_item->atlas)
    {
      SDL_DestroyTexture(font_item->atlas);
    }

    free(font_item);
    font_item = NULL;
  }

  if(tileset_tex)
  {
    SDL_DestroyTexture(tileset_tex);
    tileset_tex = NULL; 
  }

  if(player_tileset_tex)
  {
    SDL_DestroyTexture(player_tileset_tex);
    player_tileset_tex = NULL;
  }

  if(item_tileset_tex)
  {
    SDL_DestroyTexture(item_tileset_tex);
    item_tileset_tex = NULL;
  }

  if(tilemap_tex)
  {
    SDL_DestroyTexture(tilemap_tex);
    tilemap_tex = NULL;
  }

  if(inv_tex)
  {
    SDL_DestroyTexture(inv_tex);
    inv_tex = NULL;
  }

  if(player_inv_hl_tex)
  {
    SDL_DestroyTexture(player_inv_hl_tex);
    player_inv_hl_tex = NULL; 
  }

  if(inv_item_tex)
  {
    SDL_DestroyTexture(inv_item_tex);
    inv_item_tex = NULL;
  }

  if(interface_console_tex)
  {
    SDL_DestroyTexture(interface_console_tex);
    interface_console_tex = NULL;
  }

  if(interface_stats_tex)
  {
    SDL_DestroyTexture(interface_stats_tex);
    interface_stats_tex = NULL;
  }

  if(renderer)
  {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
  }

  if(window)
  {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  // quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}