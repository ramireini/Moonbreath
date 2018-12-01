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

int game_init(font_t **font_one, font_t **font_two, player_t *player, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex)
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
  int img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
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

  *font_one = create_bmp_font_atlas("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  *font_two = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  if(!(*font_one) || !(font_two))
  {
    printf("Could not create font atlases\n");
    return 1;
  }

  /* -- TEXTURES -- */

  *tileset_tex = load_texture("data/images/tileset.png", NULL);
  *player_tileset_tex = load_texture("data/images/player_tileset.png", NULL);
  *item_tileset_tex = load_texture("data/images/item_tileset.png", NULL);
  *tilemap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
  *inv_tex = load_texture("data/images/player_inventory.png", NULL);
  *player_inv_hl_tex = load_texture("data/images/player_inventory_highlight.png", NULL);
  *inv_item_tex = load_texture("data/images/player_inventory_item.png", NULL);
  *interface_console_tex = load_texture("data/images/interface_console.png", NULL);
  *interface_stats_tex = load_texture("data/images/interface_statistics.png", NULL);

  if(!(*tileset_tex) || !(*player_tileset_tex) || !(*item_tileset_tex) || !(*tilemap_tex) || !(*inv_tex) || !(*player_inv_hl_tex) || !(*player_inv_hl_tex) || !(*interface_console_tex) || !(*interface_stats_tex))
  {
    printf("Could not load textures\n");
    return 1;
  }

  /* -- ARRAYS -- */
  
  // init game items
  for(int i = 0; i < ITEMS_COUNT; i++)
  {
    items[i].item_id = ID_NONE;
    items[i].unique_id = i + 1;
    items[i].is_on_ground = 0;
    items[i].is_equipped = 0;
    items[i].x = 0;
    items[i].y = 0;
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
  for(int i = 0; i < MESSAGE_COUNT; i++)
  {
    messages[i].msg[0] = '.';
    messages[i].msg_color = 0;
  }

  /* -- CONFIG -- */

  // NOTE(Rami): if the user edits the config file and turns something like key=1 into key = 1
  // then our arrays will get bogus values, we need to somehow make sure that the key value pairs
  // are in the format and ONLY in the format we expect.
  // 
  // load the config
  conf_t conf;
  if(conf_load(&conf, "data/items.cfg"))
  {
    printf("Could not load config\n");
    return 0;
  }

  // assign config data into the game
  for(int i = 0; i < conf.key_value_pair_count / KEY_VALUE_PAIRS_PER_ITEM; i++)
  {
    int index = i * KEY_VALUE_PAIRS_PER_ITEM;

    items_info[i].item_id = conf.vars[index].conf_var_u.i;
    strcpy(items_info[i].name, conf.vars[index + 1].conf_var_u.s);
    items_info[i].item_type = conf.vars[index + 2].conf_var_u.i;
    items_info[i].tile = conf.vars[index + 3].conf_var_u.i;
    strcpy(items_info[i].use, conf.vars[index + 4].conf_var_u.s);
    items_info[i].hp_healed = conf.vars[index + 5].conf_var_u.i;
    items_info[i].damage = conf.vars[index + 6].conf_var_u.i;
    items_info[i].armor = conf.vars[index + 7].conf_var_u.i;
    strcpy(items_info[i].description, conf.vars[index + 8].conf_var_u.s);
  }

  conf_free(&conf);

  // add some items :p
  add_game_item(ID_LESSER_HEALTH_POTION, player->x - 32, player->y);
  add_game_item(ID_IRON_SWORD, player->x + 32, player->y);

  // all initialization was successful so run the game
  game_is_running = 1;

  return 0;
}

// NOTE(Rami): kinda no point in settings all of these to NULL after freeing
// because the game will close anyway, they won't be dereferenced anymore
void game_exit(font_t *font_one, font_t *font_two, char *level, char *fov, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *player_inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex)
{
  if(font_one)
  {
    if(font_one->atlas)
    {
      SDL_DestroyTexture(font_one->atlas);
    }

    free(font_one);
  }

  if(font_two)
  {
    if(font_two->atlas)
    {
      SDL_DestroyTexture(font_two->atlas);
    }

    free(font_two);
  }

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