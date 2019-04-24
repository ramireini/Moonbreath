i32 game_init()
{
  /* - RANDOM SEED - */

  // NOTE(Rami): 
  // srand(time(NULL));
  srand(1553293671);
  debug("SEED: %lu\n", time(NULL));

  /* - SDL - */

  if(SDL_Init(SDL_INIT_VIDEO))
  {
    debug("SDL could not initialize: %s\n", SDL_GetError());
    return 0;
  }

  game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!game.window)
  {
    debug("SDL could not create window: %s\n", SDL_GetError());
    return 0;
  }

  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!game.renderer)
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

  assets.fonts[font_classic] = create_bmp_font_atlas("data/fonts/classic16x16.png", (aspect_t){16, 16}, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  assets.fonts[font_cursive] = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  for(i32 i = 0; i < font_max; i++)
  {
    if(!assets.fonts[i].success)
    {
      debug("Font atlas %d failed", i);
      return 0;
    }
  }

  assets.textures[tex_tilemap] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
  assets.textures[tex_game_tileset] = load_texture("data/images/game_tileset.png", NULL);
  assets.textures[tex_item_tileset] = load_texture("data/images/item_tileset.png", NULL);
  assets.textures[tex_player_sprite_sheet] = load_texture("data/images/player_sprite_sheet.png", NULL);
  assets.textures[tex_monster_sprite_sheet] = load_texture("data/images/monster_sprite_sheet.png", NULL);
  assets.textures[tex_inventory_win] = load_texture("data/images/inventory_win.png", NULL);
  assets.textures[tex_inventory_item_win] = load_texture("data/images/inventory_item_win.png", NULL);
  assets.textures[tex_inventory_item_selected] = load_texture("data/images/inventory_item_selected.png", NULL);
  assets.textures[tex_interface_console_win] = load_texture("data/images/interface_console_win.png", NULL);
  assets.textures[tex_interface_stats_win] = load_texture("data/images/interface_stats_win.png", NULL);

  for(i32 i = 0; i < tex_max; i++)
  {
    if(!assets.textures[i])
    {
      debug("Texture %d failed", i);
      return 0;
    }
  }

  /* - GAME DATA - */

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    items[i].item_id = id_none;
    items[i].unique_id = i + 1;
    items[i].is_on_ground = 0;
    items[i].is_equipped = 0;
    items[i].pos.x = 0;
    items[i].pos.y = 0;
  }

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    strcpy(console_messages[i].msg, CONSOLE_MESSAGE_EMPTY);
    console_messages[i].color = RGBA_COLOR_BLACK_S;
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
    if(strlen(conf->vars[index + 1].conf_var_u.str) >= 256) {return 0;}
    if(conf->vars[index + 2].conf_var_u.i < 0 || conf->vars[index + 2].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 3].conf_var_u.i < 0 || conf->vars[index + 3].conf_var_u.i > 100) {return 0;}
    if(strlen(conf->vars[index + 4].conf_var_u.str) >= 256) {return 0;}
    if(conf->vars[index + 5].conf_var_u.i < 0 || conf->vars[index + 5].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 6].conf_var_u.i < 0 || conf->vars[index + 6].conf_var_u.i > 100) {return 0;}
    if(conf->vars[index + 7].conf_var_u.i < 0 || conf->vars[index + 7].conf_var_u.i > 100) {return 0;}
    if(strlen(conf->vars[index + 8].conf_var_u.str) >= 256) {return 0;}

    items_info[i].item_id = conf->vars[index].conf_var_u.i;
    strcpy(items_info[i].name, conf->vars[index + 1].conf_var_u.str);
    items_info[i].item_type = conf->vars[index + 2].conf_var_u.i;
    items_info[i].tile = conf->vars[index + 3].conf_var_u.i;
    strcpy(items_info[i].use, conf->vars[index + 4].conf_var_u.str);
    items_info[i].hp_healed = conf->vars[index + 5].conf_var_u.i;
    items_info[i].damage = conf->vars[index + 6].conf_var_u.i;
    items_info[i].armor = conf->vars[index + 7].conf_var_u.i;
    strcpy(items_info[i].description, conf->vars[index + 8].conf_var_u.str);
  }

  conf_free(conf);

  game.camera = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};
  game.running = true;
  game.turn_changed = true;

  return 1;
}

void game_run()
{
  player_create();

  gen_level();

  // add_game_item(id_lesser_health_potion, (iv2_t){player.entity.new_pos.x + 1, player.entity.new_pos.y});
  // add_game_item(id_iron_sword, (iv2_t){player.entity.new_pos.x + 2, player.entity.new_pos.y});
  // add_game_item(id_iron_sword, (iv2_t){player.entity.new_pos.x + 3, player.entity.new_pos.y});

  // slime_create((iv2_t){19, 56});
  // slime_create((iv2_t){16, 54});
  // slime_create((iv2_t){16, 55});
  // create_slime((iv2_t){16, 56});
  // create_slime((iv2_t){16, 57});
  // create_slime((iv2_t){16, 58});
  // create_slime((iv2_t){16, 59});

  // ui32 start, end;
  while(game.running)
  {
    game.time_elapsed = SDL_GetTicks();
    // debug("%d\n", time_elapsed);
    // start = SDL_GetTicks();

    input_update();

    // if(player.active)
    // {
    //   printf("PLAYER USED\n");
    // }
    // else
    // {
    //   printf("PLAYER NOT USED\n");
    // }

    // for(i32 i = 0; i < SLIME_COUNT; i++)
    // {
    //   if(!slimes[i].active)
    //   {
    //     printf("%d UNUSED\n", i);
    //   }
    //   else
    //   {
    //     printf("%d USED\n", i);
    //   }
    // }
    // printf("\n");

    // for (i32 i = 0; i < INVENTORY_SLOT_COUNT; i++)
    // {
    //   debug("%d, [ITEM]\n", i);
    //   debug("item_id %d\n", player.inventory.slots[i].item_id);
    //   debug("unique_id %d\n", player.inventory.slots[i].unique_id);
    //   debug("is_on_ground %d\n", player.inventory.slots[i].is_on_ground);
    //   debug("equipped %d\n", player.inventory.slots[i].is_equipped);
    //   debug("x %d\n", player.inventory.slots[i].pos.x);
    //   debug("y %d\n\n", player.inventory.slots[i].pos.y);
    // }

    // for(i32 i = 0; i < ITEM_COUNT; i++)
    // {
    //   debug("[ITEM]\n");
    //   debug("item_id %d\n", items[i].item_id);
    //   debug("unique_id %d\n", items[i].unique_id);
    //   debug("is_on_ground %d\n", items[i].is_on_ground);
    //   debug("is_equipped %d\n", items[i].is_equipped);
    //   debug("x %d\n", items[i].pos.x);
    //   debug("y %d\n\n", items[i].pos.y);
    // }

    if(game.turn_changed)
    {
      player_update();
      slime_update();
      camera_update();
      lighting_update(player.entity);
      // lighting_update(test_entity);

      // for(i32 x = 0; x < LEVEL_WIDTH_IN_TILES; x++)
      // {
      //   for(i32 y = 0; y < LEVEL_HEIGHT_IN_TILES; y++)
      //   {
      //     if(level.is_occupied[(y * LEVEL_WIDTH_IN_TILES) + x])
      //     {
      //       printf("x: %d, y: %d is occupied\n", x, y);
      //     }
      //   }
      // }
      // printf("\n");

      // printf("is the 2nd slime position occupied: %d\n", occupied((iv2_t){16, 54}));

      // for(i32 i = 0; i < SLIME_COUNT; i++)
      // {
      //   printf("slime %d occupied: %d\n", i, occupied(slimes[i].entity.pos));
      //   // printf("slime %d active: %d\n", i, slimes[i].active);
      // }
      // printf("\n");

      game.turn_changed = false;
    }

    SDL_SetRenderDrawColor(game.renderer, RGBA_COLOR_BLACK_P);
    SDL_RenderClear(game.renderer);

    // NOTE(Rami): 
    // add_light((iv2_t){21, 58}, 2);

    tilemap_render();
    item_render();

    slime_render();
    player_render();
    
    interface_render();

    if(player.inventory.is_open)
    {
      inventory_render();
    }

    SDL_RenderPresent(game.renderer);

    // end = SDL_GetTicks();
    // debug("%d\n", end - start);
  }
}

// NOTE(Rami): Set the pointers to null no matter what,
// we don't want dangling pointers in any case.
void game_exit()
{
  assets_free();

  if(game.renderer)
  {
    SDL_DestroyRenderer(game.renderer);
    game.renderer = NULL;
  }

  if(game.window)
  {
    SDL_DestroyWindow(game.window);
    game.window = NULL;
  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}