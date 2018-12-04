#include <game.h>
#include <util_conf.h>

#include <time.h>

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

int game_init()
{
  /* -- RANDOM SEED -- */

  srand(time(NULL));

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

  fonts[FONT_CLASSIC] = create_bmp_font_atlas("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  fonts[FONT_CURSIVE] = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  for(int i = 0; i < FONT_COUNT; i++)
  {
    if(!fonts[i])
    {
      printf("Could not create font atlases\n");
      return 1;
    }
  }

  /* -- TEXTURES -- */

  // NOTE(Rami): Maybe have some kinda done flag on the checks on FONTS and TEXTURES
  // so you can print which fonts/textures weren't successfully loaded instead of saying
  // that you couldn't load them, just a little more information to be had there.
  // 
  // Also look into a way of automating the texture stuff below like with a lookup table
  // as we have done before.

  textures[0] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
  textures[1] = load_texture("data/images/tileset.png", NULL);
  textures[2] = load_texture("data/images/player_tileset.png", NULL);
  textures[3] = load_texture("data/images/item_tileset.png", NULL);
  textures[4] = load_texture("data/images/inventory.png", NULL);
  textures[5] = load_texture("data/images/inventory_highlight.png", NULL);
  textures[6] = load_texture("data/images/inventory_item.png", NULL);
  textures[7] = load_texture("data/images/interface_console.png", NULL);
  textures[8] = load_texture("data/images/interface_stats.png", NULL);

  for(int i = 0; i < TEXTURE_COUNT; i++)
  {
    if(!textures[i])
    {
      printf("Could not load textures\n");
      return 1;
    }
  }

  /* -- ARRAYS -- */
  
  // init game items
  for(int i = 0; i < ITEM_COUNT; i++)
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

  // all initialization was successful so run the game
  game_is_running = 1;

  return 0;
}

// NOTE(Rami): kinda no point in settings all of these to NULL after freeing
// because the game will close anyway, they won't be dereferenced anymore
void game_exit(char *level, char *fov, player_t *player)
{
  for(int i = 0; i < FONT_COUNT; i++)
  {
    if(fonts[i])
    {
      if(fonts[i]->atlas)
      {
        SDL_DestroyTexture(fonts[i]->atlas);
      }

      free(fonts[i]);
    }
  }

  for(int i = 0; i < TEXTURE_COUNT; i++)
  {
    if(textures[i])
    {
      SDL_DestroyTexture(textures[i]);
    }
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