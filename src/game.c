#include <game.h>
#include <util_conf.h>

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

int game_init(bmp_font_t **bmp_test_font, player_t *player, ttf_font_t **font_console, ttf_font_t **font_inv, ttf_font_t **font_item, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex)
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

  // NOTE(Rami): remove classic.ttf if no longer needed

  /* -- FONTS -- */

  // NOTE(Rami): add checks for null pointers because
  // otherwise the entire program segfaults into oblivion
  // you have to make the atlas tex dimensions are enough for the font if you make it bigger :p
  // download fonts and test them out, make sure to do the above ^
  // TTF_Font *font = TTF_OpenFont("data/fonts/classic.ttf", 16);
  // TTF_Font *font = TTF_OpenFont("data/fonts/MorePerfectDOSVGA.ttf", 16);
  // *font_console = create_ttf_font_atlas(font);
  // TTF_CloseFont(font);

  *bmp_test_font = create_bmp_font_atlas("data/fonts/basic16x16.png", 16, 16, 14);

  TTF_Font *font = TTF_OpenFont("data/fonts/alkhemikal.ttf", 18);
  *font_inv = create_ttf_font_atlas(font);
  TTF_CloseFont(font);

  font = TTF_OpenFont("data/fonts/hello-world.ttf", 13);
  *font_item = create_ttf_font_atlas(font);
  TTF_CloseFont(font);
  font = NULL;

  // NOTE(Rami): 
  // if(!(*font_console) || !(*font_inv) || !(*font_item))
  if(!(*bmp_test_font) || !(*font_inv) || !(*font_item))
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
void game_exit(char *level, char *fov, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *player_inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, ttf_font_t *font_console, ttf_font_t *font_inv, ttf_font_t *font_item, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex)
{
  if(level)
  {
    free(level);
    level = NULL;
  }

  if(fov)
  {
    free(fov);
    fov = NULL;
  }

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