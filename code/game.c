internal void
update_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      game.state = state_quit;
    }
    #if MOONBREATH_DEBUG
    else if(event.type == SDL_KEYDOWN)
    #else
    else if(event.type == SDL_KEYDOWN && !event.key.repeat)
    #endif
    {
      keyboard.keys[event.key.keysym.scancode] = 1;
      player_keypress(event.key.keysym.scancode);
    }
    else if(event.type == SDL_KEYUP)
    {
      keyboard.keys[event.key.keysym.scancode] = 0;
    }
  }
}

internal void
update_camera()
{
  game.camera.x = tile_mul(player.x) - (game.camera.w / 2);
  game.camera.y = (tile_mul(player.y) + (player.h / 2)) - (game.camera.h / 2);

  if(game.camera.x < 0)
  {
    game.camera.x = 0;
  }

  if(game.camera.y < 0)
  {
    game.camera.y = 0;
  }

  if(game.camera.x >= LEVEL_WIDTH_IN_PIXELS - game.camera.w)
  {
    game.camera.x = LEVEL_WIDTH_IN_PIXELS - game.camera.w;
  }

  if(game.camera.y >= LEVEL_HEIGHT_IN_PIXELS - game.camera.h)
  {
    game.camera.y = LEVEL_HEIGHT_IN_PIXELS - game.camera.h;
  }
}

internal i32
init_game()
{
  i32 result = 0;

  if(!SDL_Init(SDL_INIT_VIDEO))
  {
    game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                 WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if(game.window)
    {
      printf("Monitor refresh rate is %d HZ\n", get_window_refresh_rate(game.window));

      u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
      game.renderer = SDL_CreateRenderer(game.window, -1, render_flags);
      if(game.renderer)
      {
        i32 img_flags = IMG_INIT_PNG;
        if(IMG_Init(img_flags) & img_flags)
        {
          if(!TTF_Init())
          {
            b32 font_ok = 1;

            font[font_classic] = create_bmp_font_atlas("../data/fonts/classic16x16.png",
                                                     16, 16, 14, 8, 12);

            font[font_cursive] = create_ttf_font_atlas("../data/fonts/alkhemikal.ttf",
                                                     16, 6);

            for(i32 i = 0; i < font_count; ++i)
            {
              if(!font[i].success)
              {
                font_ok = 0;
                debug("Font atlas %d failed\n", i);
              }
            }

            if(font_ok)
            {
              b32 texture_ok = 1;

              texture[tex_tilemap] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
              texture[tex_game_tileset] = load_texture("../data/images/game_tileset.png", 0);
              texture[tex_item_tileset] = load_texture("../data/images/item_tileset.png", 0);
              texture[tex_sprite_sheet] = load_texture("../data/images/sprite_sheet.png", 0);
              texture[tex_inventory_win] = load_texture("../data/images/inventory_win.png", 0);
              texture[tex_inventory_item_win] = load_texture("../data/images/inventory_item_win.png", 0);
              texture[tex_inventory_item_selected] = load_texture("../data/images/inventory_item_selected.png", 0);
              texture[tex_interface_console_win] = load_texture("../data/images/interface_console_win.png", 0);
              texture[tex_interface_stats_win] = load_texture("../data/images/interface_stats_win.png", 0);

              for(i32 i = 0; i < tex_count; ++i)
              {
                if(!texture[i])
                {
                  texture_ok = 0;
                  debug("Texture %d failed", i);
                }
              }

              if(texture_ok)
              {
                // NOTE(rami):
                // srand(time(0));
                srand(1553293671);
                printf("SEED: %lu\n\n", time(0));

                game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
                game.state = state_running;
                game.turn_changed = 1;
                game.perf_count_frequency = SDL_GetPerformanceFrequency();

                for(i32 i = 0; i < ITEM_COUNT; ++i)
                {
                  item[i].unique_id = i + 1;
                }

                for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
                {
                  strcpy(console_message[i].msg, CONSOLE_MESSAGE_EMPTY);
                  console_message[i].color = color_black;
                }

                b32 conf_ok = 1;

                conf_t *conf = load_conf("../data/item.cfg");
                if(!conf || !conf->success)
                {
                  conf_ok = 0;
                }

                if(conf_ok)
                {
                  for(i32 i = 0; i < conf->length / KEY_VALUE_PAIRS_PER_ITEM; ++i)
                  {
                    i32 index = i * KEY_VALUE_PAIRS_PER_ITEM;

                    if(conf->vars[index].i < 0 || conf->vars[index].i > 100) {return(0);}
                    if(strlen(conf->vars[index + 1].str) >= 256) {return(0);}
                    if(conf->vars[index + 2].i < 0 || conf->vars[index + 2].i > 100) {return(0);}
                    if(conf->vars[index + 3].i < 0 || conf->vars[index + 3].i > 100) {return(0);}
                    if(strlen(conf->vars[index + 4].str) >= 256) {return(0);}
                    if(conf->vars[index + 5].i < 0 || conf->vars[index + 5].i > 100) {return(0);}
                    if(conf->vars[index + 6].i < 0 || conf->vars[index + 6].i > 100) {return(0);}
                    if(conf->vars[index + 7].i < 0 || conf->vars[index + 7].i > 100) {return(0);}
                    if(strlen(conf->vars[index + 8].str) >= 256) {return(0);}

                    item_info[i].id = conf->vars[index].i;
                    strcpy(item_info[i].name, conf->vars[index + 1].str);
                    item_info[i].type = conf->vars[index + 2].i;
                    item_info[i].tile = conf->vars[index + 3].i;
                    strcpy(item_info[i].use, conf->vars[index + 4].str);
                    item_info[i].hp_healed = conf->vars[index + 5].i;
                    item_info[i].damage = conf->vars[index + 6].i;
                    item_info[i].armor = conf->vars[index + 7].i;
                    strcpy(item_info[i].description, conf->vars[index + 8].str);
                  }

                  free_conf(conf);

                  result = 1;
                }
              }
              else
              {
                // NOTE(rami): Texture failed
              }
            }
            else
            {
              // NOTE(rami): Font failed
            }
          }
          else
          {
            debug("SDL TTF library could not initialize: %s\n", SDL_GetError());
          }
        }
        else
        {
          debug("SLD image library could not initialize: %s\n", SDL_GetError());
        }
      }
      else
      {
        debug("SDL could not create a renderer: %s\n", SDL_GetError());
      }
    }
    else
    {
      debug("SDL could not create window: %s\n", SDL_GetError());
    }
  }
  else
  {
    debug("SDL could not initialize: %s\n", SDL_GetError());
  }

  return(result);
}

internal void
run_game()
{
  add_player();

  generate_level();

  add_monster(monster_slime, player.x + 2, player.y);
  add_monster(monster_slime, player.x + 3, player.y);

  add_item(id_iron_sword, 16, 56);
  add_item(id_lesser_health_potion, 16, 57);
  add_item(id_iron_sword, 16, 58);

  u32 frames_per_second = 60;
  r32 target_seconds_per_frame = 1.0f / (r32)frames_per_second;
  u64 counter_old = SDL_GetPerformanceCounter();
  r32 old_dt = SDL_GetPerformanceCounter();

  while(game.state)
  {
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    r32 new_dt = SDL_GetPerformanceCounter();
    game.dt = (r32)(new_dt - old_dt) / (r32)game.perf_count_frequency;
    old_dt = new_dt;
    // printf("\ndt: %f\n", game.dt);

    // NOTE(rami): Pop up text
    #if 0
    for(i32 i = POP_UP_TEXT_COUNT - 1; i > -1; i--)
    {
      if(pop_up_text[i].active)
      {
        printf("\npop_up_text[%d]\n", i);
        printf("str: %s\n", pop_up_text[i].str);
        printf("x: %.02f, y: %.02f\n", pop_up_text[i].x, pop_up_text[i].y);
        printf("change: %.02f\n", pop_up_text[i].change);
        printf("speed: %.02f\n", pop_up_text[i].speed);
        printf("duration_time: %dms\n", pop_up_text[i].duration_time);
        printf("start_time: %dms\n", pop_up_text[i].start_time);
      }
    }
    #endif

    // NOTE(rami): Inventory
    #if 0
    for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; i--)
    {
      if(inventory.slots[i].id)
      {
        printf("\nInventory.slots[%d]\n", i);
        printf("id %d\n", inventory.slots[i].id);
        printf("unique_id %d\n", inventory.slots[i].unique_id);
        printf("in_inventory %d\n", inventory.slots[i].in_inventory);
        printf("equipped %d\n", inventory.slots[i].is_equipped);
        printf("x %d\n", inventory.slots[i].x);
        printf("y %d\n", inventory.slots[i].y);
      }
    }
    #endif

    // NOTE(rami): Item
    #if 0
    for(i32 i = ITEM_COUNT - 1; i > -1; i--)
    {
      if(items[i].id)
      {
        printf("\nitems[%d]\n", i);
        printf("id %d\n", items[i].id);
        printf("unique_id %d\n", items[i].unique_id);
        printf("in_inventory %d\n", items[i].in_inventory);
        printf("is_equipped %d\n", items[i].is_equipped);
        printf("x %d\n", items[i].x);
        printf("y %d\n", items[i].y);
      }
    }
    #endif

    // NOTE(rami): Player
    #if 0
    printf("\nPlayer\n");
    printf("frame_start.x, y: %d, %d\n", player.sprite.frame_start.x,
                                         player.sprite.frame_start.y);
    printf("frame_current.x, y: %d, %d\n", player.sprite.frame_current.x,
                                           player.sprite.frame_current.y);
    printf("frame_count: %d\n", player.sprite.frame_count);
    printf("frame_delay: %d\n", player.sprite.frame_delay);
    printf("frame_last_changed: %d\n", player.sprite.frame_last_changed);
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

    // NOTE(rami): Monster
    #if 0
    for(i32 i = MONSTER_COUNT - 1; i > -1; i--)
    {
      if(monster[i].type)
      {
        printf("\nmonster[%d]\n", i);
        printf("type: %d\n", monster[i].type);
        printf("ai: %d\n", monster[i].ai);

        printf("frame_start.x, y: %d, %d\n", monster[i].sprite.frame_start.x,
                                             monster[i].sprite.frame_start.y);
        printf("frame_current.x, y: %d, %d\n", monster[i].sprite.frame_current.x,
                                               monster[i].sprite.frame_current.y);
        printf("frame_count : %d\n", monster[i].sprite.frame_count);
        printf("frame_delay: %d\n", monster[i].sprite.frame_delay);
        printf("frame_last_changed: %d\n", monster[i].sprite.frame_last_changed);

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

    update_pop_up_text();

    render_tilemap();
    render_items();
    render_player();
    render_monster();
    render_ui();
    render_pop_up_text();

    if(inventory.open)
    {
      render_inventory();
    }

    u64 work_counter_elapsed = SDL_GetPerformanceCounter() - counter_old;
    r32 ms_for_work = (1000.0f * (r32)work_counter_elapsed) / (r32)game.perf_count_frequency;
    // printf("ms_for_work: %.02f\n", ms_for_work);

    if(get_seconds_elapsed(counter_old, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
    {
      u32 time_to_delay = ((target_seconds_per_frame - get_seconds_elapsed(counter_old,
                          SDL_GetPerformanceCounter())) * 1000) - 1;
      if(time_to_delay > 0)
      {
        SDL_Delay(time_to_delay);
      }

      while(get_seconds_elapsed(counter_old, SDL_GetPerformanceCounter())
            < target_seconds_per_frame)
      {
      }
    }
    else
    {
      // NOTE(rami): We're right on the schedule or late
      // NOTE(rami): Valgrind will trigger this!
      // return;
    }

    u64 counter_new = SDL_GetPerformanceCounter();
    u64 counter_elapsed = counter_new - counter_old;
    SDL_RenderPresent(game.renderer);

    r32 ms_per_frame = (1000.0f * (r32)counter_elapsed) / (r32)game.perf_count_frequency;
    r32 frames_per_second = (r32)game.perf_count_frequency / (r32)counter_elapsed;
    counter_old = counter_new;
    // printf("ms_per_frame: %.02f\n", ms_per_frame);
    // printf("frames_per_second: %.02f\n", frames_per_second);
    // printf("time_elapsed: %d\n", game.time_elapsed);
  }
}

internal void
exit_game()
{
  free_assets();

  if(game.renderer)
  {
    SDL_DestroyRenderer(game.renderer);
    game.renderer = 0;
  }

  if(game.window)
  {
    SDL_DestroyWindow(game.window);
    game.window = 0;
  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}