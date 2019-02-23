// larger
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

// medium
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// smaller
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

#define CONSOLE_WIDTH WINDOW_WIDTH
#define CONSOLE_HEIGHT 160

// extern game_state_t global_state;

i32 game_init()
{
  // /* - RANDOM SEED - */

  // // NOTE(Rami): 
  srand(time(NULL));
  // srand(1548744253);
  // // debug("SEED: %lu\n", time(NULL));

  /* - SDL - */

  if(SDL_Init(SDL_INIT_VIDEO))
  {
    debug("SDL could not initialize: %s\n", SDL_GetError());
    return 0;
  }

  global_state.window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!global_state.window)
  {
    debug("SDL could not create window: %s\n", SDL_GetError());
    return 0;
  }

  global_state.renderer = SDL_CreateRenderer(global_state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!global_state.renderer)
  {
    debug("SDL could not create a renderer: %s\n", SDL_GetError());
    return 0;
  }

  i32 img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    debug("SLD image library could not initialize: %s\n", SDL_GetError());
    return 0;
  }

  if(TTF_Init())
  {
    debug("SDL TTF library could not initialize: %s\n", SDL_GetError());
    return 0;
  }

  /* - ASSETS - */

  global_state.assets.fonts[font_classic] = create_bmp_font_atlas("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  global_state.assets.fonts[font_cursive] = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  for(i32 i = 0; i < font_max; i++)
  {
    if(!global_state.assets.fonts[i])
    {
      debug("Font atlas %d failed\n", i);
      return 0;
    }
  }

  global_state.assets.textures[tex_tilemap] = SDL_CreateTexture(global_state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
  global_state.assets.textures[tex_game_tileset] = load_texture("data/images/game_tileset.png", NULL);
  global_state.assets.textures[tex_item_tileset] = load_texture("data/images/item_tileset.png", NULL);
  global_state.assets.textures[tex_player_sprite_sheet] = load_texture("data/images/player_sprite_sheet.png", NULL);
  global_state.assets.textures[tex_monster_sprite_sheet] = load_texture("data/images/monster_sprite_sheet.png", NULL);
  global_state.assets.textures[tex_inventory_win] = load_texture("data/images/inventory_win.png", NULL);
  global_state.assets.textures[tex_inventory_item_win] = load_texture("data/images/inventory_item_win.png", NULL);
  global_state.assets.textures[tex_inventory_item_selected] = load_texture("data/images/inventory_item_selected.png", NULL);
  global_state.assets.textures[tex_interface_console_win] = load_texture("data/images/interface_console_win.png", NULL);
  global_state.assets.textures[tex_interface_stats_win] = load_texture("data/images/interface_stats_win.png", NULL);

  for(i32 i = 0; i < tex_max; i++)
  {
    if(!global_state.assets.textures[i])
    {
      debug("Texture %d failed\n", i);
      return 0;
    }
  }

  /* - GAME DATA - */

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    items[i].item_id = ID_NONE;
    items[i].unique_id = i + 1;
    items[i].is_on_ground = 0;
    items[i].is_equipped = 0;
    items[i].x = 0;
    items[i].y = 0;
  }

  for(i32 i = 0; i < INVENTORY_COUNT; i++)
  {
    inventory[i].item_id = ID_NONE;
    inventory[i].unique_id = 0;
    inventory[i].is_on_ground = 0;
    inventory[i].is_equipped = 0;
    inventory[i].x = 0;
    inventory[i].y = 0;
  }

  for(i32 i = 0; i < MESSAGE_COUNT; i++)
  {
    messages[i].msg[0] = '.';
    messages[i].color = RGBA_COLOR_NONE_S;
  }

  /* - CONFIG - */

  conf_t *conf = conf_load("data/items.cfg");
  if(!conf)
  {
    return 0;
  }

  for(i32 i = 0; i < conf->key_value_pair_count / KEY_VALUE_PAIRS_PER_ITEM; i++)
  {
    i32 index = i * KEY_VALUE_PAIRS_PER_ITEM;

    if(conf->vars[index].conf_var_u.i < 0 || conf->vars[index].conf_var_u.i > 100) {return 0;}
    if(strlen(conf->vars[index + 1].conf_var_u.s) >= 256) {return 0;}
    if(conf->vars[index + 2].conf_var_u.i < 0 || conf->vars[index + 2].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 3].conf_var_u.i < 0 || conf->vars[index + 3].conf_var_u.i > 100) {return 0;}
    if(strlen(conf->vars[index + 4].conf_var_u.s) >= 256) {return 0;}
    if(conf->vars[index + 5].conf_var_u.i < 0 || conf->vars[index + 5].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 6].conf_var_u.i < 0 || conf->vars[index + 6].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 7].conf_var_u.i < 0 || conf->vars[index + 7].conf_var_u.i > 100) {return 0;}
    if(strlen(conf->vars[index + 8].conf_var_u.s) >= 256) {return 0;}

    items_info[i].item_id = conf->vars[index].conf_var_u.i;
    strcpy(items_info[i].name, conf->vars[index + 1].conf_var_u.s);
    items_info[i].item_type = conf->vars[index + 2].conf_var_u.i;
    items_info[i].tile = conf->vars[index + 3].conf_var_u.i;
    strcpy(items_info[i].use, conf->vars[index + 4].conf_var_u.s);
    items_info[i].hp_healed = conf->vars[index + 5].conf_var_u.i;
    items_info[i].damage = conf->vars[index + 6].conf_var_u.i;
    items_info[i].armor = conf->vars[index + 7].conf_var_u.i;
    strcpy(items_info[i].description, conf->vars[index + 8].conf_var_u.s);
  }

  conf_free(conf);

  global_state.camera = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};
  global_state.turn_changed = true;
  global_state.game_is_running = true;

  return 1;
}

void game_run()
{
  create_player();

  generate_level();

  // add_game_item(ID_LESSER_HEALTH_POTION, player->entity->x, player->entity->y - 32);
  // add_game_item(ID_IRON_SWORD, player->entity->x, player->entity->y + 32);

  // create_slimes(player->entity->x + 2, player->entity->y);

  // ui32 start, end;
  while(global_state.game_is_running)
  {
    global_state.time_elapsed = SDL_GetTicks();
    // debug("%d\n", time_elapsed);
    // start = SDL_GetTicks();

    update_events();
    update_input(level);

    // // NOTE(Rami):
    // for(i32 i = 0; i < SLIME_COUNT; i++)
    // {
    //   if(slimes[i])
    //   {
    //     debug("slimes[%d] VALID\n", i);
    //   }
    //   else
    //   {
    //     debug("slimes[%d] NULL\n", i);
    //   }

    //   debug("\n");
    // }

    // for (i32 i = 0; i < INVENTORY_COUNT; i++)
    // {
    //   if(inventory[i].unique_id != 0)
    //   {
    //     debug("%d, [ITEM]\n", i);
    //     debug("item_id %d\n", inventory[i].item_id);
    //     debug("unique_id %d\n", inventory[i].unique_id);
    //     debug("is_on_ground %d\n", inventory[i].is_on_ground);
    //     debug("equipped %d\n", inventory[i].is_equipped);
    //     debug("x %d\n", inventory[i].x);
    //     debug("y %d\n\n", inventory[i].y);
    //   }
    // }

    // for (i32 i = 0; i < ITEM_COUNT; i++)
    // {
    //   if (items[i].item_id != -1)
    //   {
    //     debug("[ITEM]\n");
    //     debug("item_id %d\n", items[i].item_id);
    //     debug("unique_id %d\n", items[i].unique_id);
    //     debug("is_on_ground %d\n", items[i].is_on_ground);
    //     debug("is_equipped %d\n", items[i].is_equipped);
    //     debug("x %d\n", items[i].x);
    //     debug("y %d\n\n", items[i].y);
    //   }
    // }

    if(global_state.turn_changed)
    {
      update_player();
      // update_slimes();

      update_camera();

      global_state.turn_changed = false;
    }

    SDL_SetRenderDrawColor(global_state.renderer, RGBA_COLOR_NONE_P);
    SDL_RenderClear(global_state.renderer);

    render_level();
    render_items();

    // render_slimes();
    render_player();
    
    render_interface();

    if(player->inventory_display)
    {
      render_inventory();
    }

    SDL_RenderPresent(global_state.renderer);

    // end = SDL_GetTicks();
    // debug("%d\n", end - start);
  }
}

// NOTE(Rami): Set the pointers to null no matter what,
// we don't want dangling pointers in any case.
void game_exit()
{
  free_player(player);
  free_slimes(0, SLIME_COUNT);
  free_assets();

  if(global_state.renderer)
  {
    SDL_DestroyRenderer(global_state.renderer);
    global_state.renderer = NULL;
  }

  if(global_state.window)
  {
    SDL_DestroyWindow(global_state.window);
    global_state.window = NULL;
  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}