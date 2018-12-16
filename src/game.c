#include <game.h>
#include <util_conf.h>
#include <time.h>

int game_init()
{
  /* - RANDOM SEED - */

  srand(time(NULL));

  /* - SDL - */

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

  /* - FONTS - */

  fonts[FONT_CLASSIC] = create_bmp_font_atlas("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  fonts[FONT_CURSIVE] = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  /* - TEXTURES - */

  // NOTE(Rami): Right now the sprite sheets are just tilesets,
  // that will be changed later at some point.
  textures[TEX_TILEMAP] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
  textures[TEX_GAME_TILESET] = load_texture("data/images/game_tileset.png", NULL);
  textures[TEX_ITEM_TILESET] = load_texture("data/images/item_tileset.png", NULL);
  textures[TEX_PLAYER_SPRITE_SHEET] = load_texture("data/images/player_sprite_sheet.png", NULL);
  textures[TEX_MONSTER_SPRITE_SHEET] = load_texture("data/images/monster_sprite_sheet.png", NULL);
  textures[TEX_INVENTORY_WIN] = load_texture("data/images/inventory_win.png", NULL);
  textures[TEX_INVENTORY_ITEM_WIN] = load_texture("data/images/inventory_item_win.png", NULL);
  textures[TEX_INVENTORY_ITEM_SELECTED] = load_texture("data/images/inventory_item_selected.png", NULL);
  textures[TEX_INTERFACE_CONSOLE_WIN] = load_texture("data/images/interface_console_win.png", NULL);
  textures[TEX_INTERFACE_STATS_WIN] = load_texture("data/images/interface_stats_win.png", NULL);

  // check if assets failed
  int done = 1;
  for(int i = 0; i < FONT_COUNT; i++)
  {
    if(!fonts[i])
    {
      printf("Font atlas %d failed\n", i);
      done = 0;
    }
  }

  for(int i = 0; i < TEXTURE_COUNT; i++)
  {
    if(!textures[i])
    {
      printf("Texture %d failed\n", i);
      done = 0;
    }
  }

  if(!done)
  {
    return 1;
  }

  /* - ARRAYS - */

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

  /* - CONFIG - */

  // NOTE(Rami): if the user edits the config file and turns something like key=1 into key = 1
  // then our arrays will get bogus values, we need to somehow make sure that the key value pairs
  // are in the format and ONLY in the format we expect.
  // 
  // load the config
  conf_t conf;
  if(!conf_load(&conf, "data/items.cfg"))
  {
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
  printf("Config free'd\n");

  // NOTE(Rami): 
  // so we have something to render without the player pressing at start
  turn_changed = 1;

  // all initialization was successful so run the game
  game_is_running = 1;

  return 1;
}

void game_run(char *level, char *fov, SDL_Rect *camera)
{
  create_player("Frozii", 0, 0, 0, 5, 10, 0, 0, 0, TILE_SIZE, TILE_SIZE, 1, 6, 3, 0);

  generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2);

  add_game_item(ID_LESSER_HEALTH_POTION, player->entity->x, player->entity->y - 32);
  add_game_item(ID_IRON_SWORD, player->entity->x, player->entity->y + 32);

  create_slimes(0, player->entity->x + 32, player->entity->y, TILE_SIZE, TILE_SIZE, 4);
  // create_slimes(0, player->entity->x - 32, player->entity->y, TILE_SIZE, TILE_SIZE, 4);

  while(game_is_running)
  {
    SDL_RenderClear(renderer);

    update_events();

    update_input(level);

    // NOTE(Rami):
    // for(int i = 0; i < SLIME_COUNT; i++)
    // {
    //   if(slimes[i].state == STATE_USED)
    //   {
    //     printf("slimes[%d] used\n", i);
    //   }
    //   else if(slimes[i].state == STATE_UNUSED)
    //   {
    //     printf("slimes[%d] unused\n", i);
    //   }
    // }

    // NOTE(Rami):
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

    // for (int i = 0; i < ITEM_COUNT; i++)
    // {
    //   if (items[i].item_id != ID_NONE)
    //   {
    //     printf("[ITEM]\n");
    //     printf("item_id %d\n", items[i].item_id);
    //     printf("unique_id %d\n", items[i].unique_id);
    //     printf("is_on_ground %d\n", items[i].is_on_ground);
    //     printf("is_equipped %d\n", items[i].is_equipped);
    //     printf("x %d\n", items[i].x);
    //     printf("y %d\n\n", items[i].y);
    //   }
    // }

    // update_lighting(dungeon, fov, player);

    if(turn_changed)
    {
      update_player(level);
      update_slimes(level);

      update_camera(camera);

      turn_changed = 0;
    }

    render_level(level, fov, camera);
    render_items(camera);
    render_interface();

    render_slimes(camera);
    render_player(camera);

    if(player->inventory_display)
    {
      render_inventory();
    }

    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // if(plot_line(level, player->entity->x - camera->x, player->entity->y - camera->y, slimes[0].entity.x - camera->x, slimes[0].entity.y - camera->y))
    // {
    //   printf("Can see\n");
    // }
    // else
    // {
    //   printf("Cannot see\n");
    // }

    // plot_line(level, player->entity->x, player->entity->y, slimes[0].entity.x, slimes[0].entity.y);
    if(!plot_line(camera, level, to_tiles(player->entity->x), to_tiles(player->entity->y), to_tiles(slimes[0].entity.x), to_tiles(slimes[0].entity.y)))
    {
      printf("no line of sight\n");
    }
    else
    {
      printf("there is a line of sight\n");
    }

    // int x = to_tiles(slimes[0].entity.x);
    // int y = to_tiles(slimes[0].entity.y);

    // level[(y * LEVEL_SIZE) + x] = TILE_DOOR_CLOSED;

    SDL_RenderPresent(renderer);
  }
}

// NOTE(Rami): set the pointers to null no matter what,
// we don't want dangling pointers in any case
void game_exit(char *level, char *fov)
{
  free_player(player);
  player = NULL;

  for(int i = 0; i < FONT_COUNT; i++)
  {
    if(fonts[i])
    {
      if(fonts[i]->atlas)
      {
        SDL_DestroyTexture(fonts[i]->atlas);
      }

      free(fonts[i]);
      fonts[i] = NULL;
    }
  }

  for(int i = 0; i < TEXTURE_COUNT; i++)
  {
    if(textures[i])
    {
      SDL_DestroyTexture(textures[i]);
      textures[i] = NULL;
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