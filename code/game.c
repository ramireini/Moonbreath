internal i32
init_game()
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

  assets.fonts[classic_font] = create_bmp_font_atlas("../data/fonts/classic16x16.png",
                                                     16, 16, 14, 8, 12);
  assets.fonts[cursive_font] = create_ttf_font_atlas("../data/fonts/alkhemikal.ttf", 16, 6);

  for(i32 i = 0; i < font_count; i++)
  {
    if(!assets.fonts[i].success)
    {
      debug("Font atlas %d failed", i);
      return 0;
    }
  }

  assets.textures[tilemap_tex] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
  assets.textures[game_tileset_tex] = load_texture("../data/images/game_tileset.png", NULL);
  assets.textures[item_tileset_tex] = load_texture("../data/images/item_tileset.png", NULL);
  assets.textures[player_sprite_sheet_tex] = load_texture("../data/images/player_sprite_sheet.png", NULL);
  assets.textures[monster_sprite_sheet_tex] = load_texture("../data/images/monster_sprite_sheet.png", NULL);
  assets.textures[inventory_win_tex] = load_texture("../data/images/inventory_win.png", NULL);
  assets.textures[inventory_item_win_tex] = load_texture("../data/images/inventory_item_win.png", NULL);
  assets.textures[inventory_item_selected_tex] = load_texture("../data/images/inventory_item_selected.png", NULL);
  assets.textures[interface_console_win_tex] = load_texture("../data/images/interface_console_win.png", NULL);
  assets.textures[interface_stats_win_tex] = load_texture("../data/images/interface_stats_win.png", NULL);

  for(i32 i = 0; i < tex_count; i++)
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
    items[i].in_inventory = 0;
    items[i].is_equipped = 0;
    items[i].x = 0;
    items[i].y = 0;
  }

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    strcpy(console_messages[i].msg, CONSOLE_MESSAGE_EMPTY);
    console_messages[i].color = RGBA_COLOR_BLACK_S;
  }

  /* - CONFIG - */

  conf_t *conf = load_conf("../data/items.cfg");
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

  free_conf(conf);

  game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
  game.running = true;
  game.turn_changed = true;

  return 1;
}

internal void
run_game()
{
  create_player();

  generate_level();

  add_game_item(id_iron_sword, v2(16, 56));
  add_game_item(id_lesser_health_potion, v2(16, 57));
  // add_game_item(id_iron_sword, v2(16, 58));

  // create_slimes(v2(16, 54));
  // create_slimes(v2(16, 55));
  // create_slimes(v2(16, 56));
  // create_slimes(v2(16, 57));
  // create_slimes(v2(16, 58));
  // create_slimes(v2(16, 59));

  // ui32 start, end;
  while(game.running)
  {
    game.time_elapsed = SDL_GetTicks();
    // debug("%d\n", time_elapsed);
    // start = SDL_GetTicks();

    update_input();

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

    // for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; i--)
    // {
    //   if(player.inventory.slots[i].item_id)
    //   {
    //     printf("\nInventory, %d, [ITEM]\n", i);
    //     printf("item_id %d\n", player.inventory.slots[i].item_id);
    //     printf("unique_id %d\n", player.inventory.slots[i].unique_id);
    //     printf("in_inventory %d\n", player.inventory.slots[i].in_inventory);
    //     printf("equipped %d\n", player.inventory.slots[i].is_equipped);
    //     printf("x %d\n", player.inventory.slots[i].x);
    //     printf("y %d\n\n", player.inventory.slots[i].y);
    //   }
    // }

    // for(i32 i = ITEM_COUNT - 1; i > -1; i--)
    // {
    //   if(items[i].item_id)
    //   {
    //     printf("\nGame, %d, [ITEM]\n", i);
    //     printf("item_id %d\n", items[i].item_id);
    //     printf("unique_id %d\n", items[i].unique_id);
    //     printf("in_inventory %d\n", items[i].in_inventory);
    //     printf("is_equipped %d\n", items[i].is_equipped);
    //     printf("x %d\n", items[i].x);
    //     printf("y %d\n", items[i].y);
    //   }
    // }

    if(game.turn_changed)
    {
      update_player();
      update_slimes();
      update_camera();
      update_lighting(player.entity);
      // update_lighting(test_entity);

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

      // printf("is the 2nd slime position occupied: %d\n", is_occupied(v2(16, 54)));

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
    // add_light(v2(21, 58), 2);

    render_tilemap();
    render_items();

    render_slimes();
    render_player();
    
    render_equipped_items();

    render_ui();

    if(player.inventory.is_open)
    {
      render_inventory();
    }

    SDL_RenderPresent(game.renderer);

    // end = SDL_GetTicks();
    // debug("%d\n", end - start);
  }
}

// NOTE(Rami): Set the pointers to null no matter what,
// we don't want dangling pointers in any case.
internal void
exit_game()
{
  free_assets();

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