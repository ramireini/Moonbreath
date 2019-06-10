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

  debug("Refresh rate is %d HZ\n", SDl_GetWindowRefreshRate(game.window));

  u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  game.renderer = SDL_CreateRenderer(game.window, -1, render_flags);
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
  assets.textures[sprite_sheet_tex] = load_texture("../data/images/sprite_sheet.png", NULL);
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
    items[i].unique_id = i + 1;
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

    if(conf->vars[index].conf_var.i < 0 || conf->vars[index].conf_var.i > 100) {return 0;}
    if(strlen(conf->vars[index + 1].conf_var.str) >= 256) {return 0;}
    if(conf->vars[index + 2].conf_var.i < 0 || conf->vars[index + 2].conf_var.i > 100) {return 0;}
    if(conf->vars[index + 3].conf_var.i < 0 || conf->vars[index + 3].conf_var.i > 100) {return 0;}
    if(strlen(conf->vars[index + 4].conf_var.str) >= 256) {return 0;}
    if(conf->vars[index + 5].conf_var.i < 0 || conf->vars[index + 5].conf_var.i > 100) {return 0;}
    if(conf->vars[index + 6].conf_var.i < 0 || conf->vars[index + 6].conf_var.i > 100) {return 0;}
    if(conf->vars[index + 7].conf_var.i < 0 || conf->vars[index + 7].conf_var.i > 100) {return 0;}
    if(strlen(conf->vars[index + 8].conf_var.str) >= 256) {return 0;}

    items_info[i].id = conf->vars[index].conf_var.i;
    strcpy(items_info[i].name, conf->vars[index + 1].conf_var.str);
    items_info[i].type = conf->vars[index + 2].conf_var.i;
    items_info[i].tile = conf->vars[index + 3].conf_var.i;
    strcpy(items_info[i].use, conf->vars[index + 4].conf_var.str);
    items_info[i].hp_healed = conf->vars[index + 5].conf_var.i;
    items_info[i].damage = conf->vars[index + 6].conf_var.i;
    items_info[i].armor = conf->vars[index + 7].conf_var.i;
    strcpy(items_info[i].description, conf->vars[index + 8].conf_var.str);
  }

  free_conf(conf);

  game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
  game.state = state_running;
  game.turn_changed = 1;

  return 1;
}

internal void
run_game()
{
  add_player();

  generate_level();

  add_monster(monster_slime, player.x + 2, player.y);
  add_monster(monster_skeleton, player.x + 3, player.y);

  add_item(id_iron_sword, v2(16, 56));
  add_item(id_lesser_health_potion, v2(16, 57));
  add_item(id_iron_sword, v2(16, 58));

  u32 game_update_hz = 60;
  r32 target_seconds_per_frame = 1.0f / (r32)game_update_hz;
  u64 last_counter = SDL_GetPerformanceCounter();

  // v2_t pos = v2(400, WINDOW_HEIGHT - 100);
  while(game.state)
  {
    game.time_elapsed = SDL_GetTicks();

    SDL_SetRenderDrawColor(game.renderer, RGBA_COLOR_BLACK_P);
    SDL_RenderClear(game.renderer);

    // NOTE(Rami): Inventory
    #if 0
    for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; i--)
    {
      if(inventory.slots[i].item_id)
      {
        printf("\nInventory, %d, [ITEM]\n", i);
        printf("item_id %d\n", inventory.slots[i].item_id);
        printf("unique_id %d\n", inventory.slots[i].unique_id);
        printf("in_inventory %d\n", inventory.slots[i].in_inventory);
        printf("equipped %d\n", inventory.slots[i].is_equipped);
        printf("x %d\n", inventory.slots[i].x);
        printf("y %d\n\n", inventory.slots[i].y);
      }
    }
    #endif

    // NOTE(Rami): Item
    #if 0
    for(i32 i = ITEM_COUNT - 1; i > -1; i--)
    {
      if(items[i].item_id)
      {
        printf("\nGame, %d, [ITEM]\n", i);
        printf("item_id %d\n", items[i].item_id);
        printf("unique_id %d\n", items[i].unique_id);
        printf("in_inventory %d\n", items[i].in_inventory);
        printf("is_equipped %d\n", items[i].is_equipped);
        printf("x %d\n", items[i].x);
        printf("y %d\n", items[i].y);
      }
    }
    #endif

    // NOTE(Rami): Player
    #if 0
    printf("\nPlayer\n");
    printf("frame_start.x, y: %d, %d\n", player.render.frame_start.x,
                                         player.render.frame_start.y);
    printf("frame_current.x, y: %d, %d\n", player.render.frame_current.x,
                                           player.render.frame_current.y);
    printf("frame_count: %d\n", player.render.frame_count);
    printf("frame_delay: %d\n", player.render.frame_delay);
    printf("frame_last_changed: %d\n", player.render.frame_last_changed);
    printf("new_x, new_y: %d, %d\n", player.new_x, player.new_y);
    printf("x, y: %d, %d\n", player.x, player.y);
    printf("w, h: %d, %d\n", player.w, player.h);
    printf("name: %s\n", player.name);
    printf("max_hp: %d\n", player.max_hp);
    printf("hp: %d\n", player.hp);
    printf("damage: %d\n", player.damage);
    printf("armor: %d\n", player.armor);
    printf("speed: %d\n", player.speed);
    printf("level: %d\n", player.level);
    printf("money: %d\n", player.money);
    printf("fov: %d\n", player.fov);
    printf("brightness: %d\n", player.brightness);

    #endif

    // NOTE(Rami): Monster
    #if 0
    for(i32 i = MONSTER_COUNT - 1; i > -1; i--)
    {
      if(monster[i].type)
      {
        printf("\nmonster[%d]\n", i);
        printf("type: %d\n", monster[i].type);
        printf("ai: %d\n", monster[i].ai);

        printf("frame_start.x, y: %d, %d\n", monster[i].render.frame_start.x,
                                             monster[i].render.frame_start.y);
        printf("frame_current.x, y: %d, %d\n", monster[i].render.frame_current.x,
                                               monster[i].render.frame_current.y);
        printf("frame_count : %d\n", monster[i].render.frame_count);
        printf("frame_delay: %d\n", monster[i].render.frame_delay);
        printf("frame_last_changed: %d\n", monster[i].render.frame_last_changed);

        printf("x, y: %d, %d\n", monster[i].x, monster[i].y);
        printf("w, h: %d, %d\n", monster[i].w, monster[i].h);
        printf("in_combat: %d\n", monster[i].in_combat);
        printf("max_hp: %d\n", monster[i].max_hp);
        printf("hp: %d\n", monster[i].hp);
        printf("damage: %d\n", monster[i].damage);
        printf("armor: %d\n", monster[i].armor);
        printf("speed: %d\n", monster[i].speed);
        printf("level: %d\n", monster[i].level);
      }
    }
    #endif

    update_input();

    if(game.turn_changed)
    {
      update_player();
      update_monster();
      update_lighting();
      update_camera();

      game.turn_changed = 0;
    }

    render_tilemap();
    render_items();
    render_player();
    render_monster();
    render_ui();

    if(inventory.is_open)
    {
      render_inventory();
    }

    // NOTE(Rami):
    // render_text("testing", pos, RGBA_COLOR_WHITE_S, assets.fonts[classic_font]);
    // pos.y--;

    u64 work_counter_elapsed = SDL_GetPerformanceCounter() - last_counter;
    r32 ms_for_work = (1000.0f * (r32)work_counter_elapsed) / (r32)SDL_GetPerformanceFrequency();
    // printf("ms_for_work: %.02f\n", ms_for_work);

    if(SDL_GetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
    {
      u32 time_to_delay = ((target_seconds_per_frame - SDL_GetSecondsElapsed(last_counter,
                          SDL_GetPerformanceCounter())) * 1000) - 1;
      if(time_to_delay > 0)
      {
        SDL_Delay(time_to_delay);
      }

      while(SDL_GetSecondsElapsed(last_counter, SDL_GetPerformanceCounter())
            < target_seconds_per_frame)
      {
      }
    }

    u64 end_counter = SDL_GetPerformanceCounter();
    u64 counter_elapsed = end_counter - last_counter;
    SDL_RenderPresent(game.renderer);

    u64 performance_frequency = SDL_GetPerformanceFrequency();
    r32 ms_per_frame = (1000.0f * (r32)counter_elapsed) / (r32)performance_frequency;
    r32 frames_per_second = (r32)performance_frequency / (r32)counter_elapsed;
    // printf("ms_per_frame: %.02f\n", ms_per_frame);
    // printf("frames_per_second: %.02f\n", frames_per_second);
    last_counter = end_counter;
  }
}

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