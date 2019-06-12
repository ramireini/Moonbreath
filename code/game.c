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
      debug("Monitor refresh rate is %d HZ\n", SDL_GetWindowRefreshRate(game.window));

      u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
      game.renderer = SDL_CreateRenderer(game.window, -1, render_flags);
      if(game.renderer)
      {
        i32 img_flags = IMG_INIT_PNG;
        if(IMG_Init(img_flags) & img_flags)
        {
          if(!TTF_Init())
          {
            b32 assets_ok = 1;

            assets.font[font_classic] = create_bmp_font_atlas("../data/fonts/classic16x16.png",
                                                     16, 16, 14, 8, 12);
            assets.font[font_cursive] = create_ttf_font_atlas("../data/fonts/alkhemikal.ttf",
                                                     16, 6);

            for(i32 i = 0; i < font_count; i++)
            {
              if(!assets.font[i].success)
              {
                assets_ok = 0;
                debug("Font atlas %d failed", i);
              }
            }

            assets.texture[tex_tilemap] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
            assets.texture[tex_game_tileset] = load_texture("../data/images/game_tileset.png", NULL);
            assets.texture[tex_item_tileset] = load_texture("../data/images/item_tileset.png", NULL);
            assets.texture[tex_sprite_sheet] = load_texture("../data/images/sprite_sheet.png", NULL);
            assets.texture[tex_inventory_win] = load_texture("../data/images/inventory_win.png", NULL);
            assets.texture[tex_inventory_item_win] = load_texture("../data/images/inventory_item_win.png", NULL);
            assets.texture[tex_inventory_item_selected] = load_texture("../data/images/inventory_item_selected.png", NULL);
            assets.texture[tex_interface_console_win] = load_texture("../data/images/interface_console_win.png", NULL);
            assets.texture[tex_interface_stats_win] = load_texture("../data/images/interface_stats_win.png", NULL);

            for(i32 i = 0; i < tex_count; i++)
            {
              if(!assets.texture[i])
              {
                assets_ok = 0;
                debug("Texture %d failed", i);
              }
            }

            if(assets_ok)
            {
              // NOTE(rami):
              // srand(time(NULL));
              srand(1553293671);
              debug("SEED: %lu\n", time(NULL));

              game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
              game.state = state_running;
              game.turn_changed = 1;

              for(i32 i = 0; i < ITEM_COUNT; i++)
              {
                item[i].unique_id = i + 1;
              }

              for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
              {
                strcpy(console_message[i].msg, CONSOLE_MESSAGE_EMPTY);
                console_message[i].color = RGBA_COLOR_BLACK_S;
              }

              b32 conf_ok = 1;

              conf_t *conf = load_conf("../data/item.cfg");
              if(!conf || !conf->success)
              {
                conf_ok = 0;
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

                item_info[i].id = conf->vars[index].conf_var.i;
                strcpy(item_info[i].name, conf->vars[index + 1].conf_var.str);
                item_info[i].type = conf->vars[index + 2].conf_var.i;
                item_info[i].tile = conf->vars[index + 3].conf_var.i;
                strcpy(item_info[i].use, conf->vars[index + 4].conf_var.str);
                item_info[i].hp_healed = conf->vars[index + 5].conf_var.i;
                item_info[i].damage = conf->vars[index + 6].conf_var.i;
                item_info[i].armor = conf->vars[index + 7].conf_var.i;
                strcpy(item_info[i].description, conf->vars[index + 8].conf_var.str);
              }

              free_conf(conf);

              if(conf_ok)
              {
                result = 1;
              }
              else
              {
                // NOTE(rami): Conf failed
              }
            }
            else
            {
              // NOTE(rami): Assets failed
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

  return result;
}

internal void
run_game()
{
  add_player();

  generate_level();

  add_monster(monster_slime, v2(player.x + 2, player.y));
  add_monster(monster_slime, v2(player.x + 3, player.y));

  add_item(id_iron_sword, v2(16, 56));
  add_item(id_lesser_health_potion, v2(16, 57));
  add_item(id_iron_sword, v2(16, 58));

  u64 performance_frequency = SDL_GetPerformanceFrequency();
  u32 frames_per_second = 60;
  r32 target_seconds_per_frame = 1.0f / (r32)frames_per_second;
  u64 counter_old = SDL_GetPerformanceCounter();
  r32 dt_old = SDL_GetPerformanceCounter();

  while(game.state)
  {
    game.time_elapsed = SDL_GetTicks();

    SDL_SetRenderDrawColor(game.renderer, RGBA_COLOR_BLACK_P);
    SDL_RenderClear(game.renderer);

    r32 dt_new = SDL_GetPerformanceCounter();
    game.dt = (r32)(dt_new - dt_old) / (r32)performance_frequency;
    dt_old = dt_new;
    // printf("\ndt: %f\n", game.dt);

    // // NOTE(rami): Pop up text
    // #if 0
    // for(i32 i = POP_UP_TEXT_COUNT - 1; i > -1; i--)
    // {
    //   if(pop_up_text[i].active)
    //   {
    //     printf("\npop_up_text[%d]\n", i);
    //     printf("str: %s\n", pop_up_text[i].str);
    //     printf("x: %.02f, y: %.02f\n", pop_up_text[i].x, pop_up_text[i].y);
    //     printf("speed: %.02f\n", pop_up_text[i].speed);
    //     printf("direction: %d\n", pop_up_text[i].dir);
    //     printf("duration_time: %dms\n", pop_up_text[i].duration_time);
    //     printf("start_time: %dms\n\n", pop_up_text[i].start_time);
    //   }
    // }
    // #endif

    // // NOTE(rami): Inventory
    // #if 0
    // for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; i--)
    // {
    //   if(inventory.slots[i].id)
    //   {
    //     printf("\nInventory.slots[%d]\n", i);
    //     printf("id %d\n", inventory.slots[i].id);
    //     printf("unique_id %d\n", inventory.slots[i].unique_id);
    //     printf("in_inventory %d\n", inventory.slots[i].in_inventory);
    //     printf("equipped %d\n", inventory.slots[i].is_equipped);
    //     printf("x %d\n", inventory.slots[i].x);
    //     printf("y %d\n\n", inventory.slots[i].y);
    //   }
    // }
    // #endif

    // // NOTE(rami): Item
    // #if 0
    // for(i32 i = ITEM_COUNT - 1; i > -1; i--)
    // {
    //   if(items[i].id)
    //   {
    //     printf("\nitems[%d]\n", i);
    //     printf("id %d\n", items[i].id);
    //     printf("unique_id %d\n", items[i].unique_id);
    //     printf("in_inventory %d\n", items[i].in_inventory);
    //     printf("is_equipped %d\n", items[i].is_equipped);
    //     printf("x %d\n", items[i].x);
    //     printf("y %d\n", items[i].y);
    //   }
    // }
    // #endif

    // // NOTE(rami): Player
    // #if 0
    // printf("\nPlayer\n");
    // printf("frame_start.x, y: %d, %d\n", player.render.frame_start.x,
    //                                      player.render.frame_start.y);
    // printf("frame_current.x, y: %d, %d\n", player.render.frame_current.x,
    //                                        player.render.frame_current.y);
    // printf("frame_count: %d\n", player.render.frame_count);
    // printf("frame_delay: %d\n", player.render.frame_delay);
    // printf("frame_last_changed: %d\n", player.render.frame_last_changed);
    // printf("new_x, new_y: %d, %d\n", player.new_x, player.new_y);
    // printf("x, y: %d, %d\n", player.x, player.y);
    // printf("w, h: %d, %d\n", player.w, player.h);
    // printf("name: %s\n", player.name);
    // printf("max_hp: %d\n", player.max_hp);
    // printf("hp: %d\n", player.hp);
    // printf("damage: %d\n", player.damage);
    // printf("armor: %d\n", player.armor);
    // printf("speed: %d\n", player.speed);
    // printf("level: %d\n", player.level);
    // printf("money: %d\n", player.money);
    // printf("fov: %d\n", player.fov);
    // printf("brightness: %d\n", player.brightness);

    // #endif

    // // NOTE(rami): Monster
    // #if 0
    // for(i32 i = MONSTER_COUNT - 1; i > -1; i--)
    // {
    //   if(monster[i].type)
    //   {
    //     printf("\nmonster[%d]\n", i);
    //     printf("type: %d\n", monster[i].type);
    //     printf("ai: %d\n", monster[i].ai);

    //     printf("frame_start.x, y: %d, %d\n", monster[i].render.frame_start.x,
    //                                          monster[i].render.frame_start.y);
    //     printf("frame_current.x, y: %d, %d\n", monster[i].render.frame_current.x,
    //                                            monster[i].render.frame_current.y);
    //     printf("frame_count : %d\n", monster[i].render.frame_count);
    //     printf("frame_delay: %d\n", monster[i].render.frame_delay);
    //     printf("frame_last_changed: %d\n", monster[i].render.frame_last_changed);

    //     printf("x, y: %d, %d\n", monster[i].x, monster[i].y);
    //     printf("w, h: %d, %d\n", monster[i].w, monster[i].h);
    //     printf("in_combat: %d\n", monster[i].in_combat);
    //     printf("max_hp: %d\n", monster[i].max_hp);
    //     printf("hp: %d\n", monster[i].hp);
    //     printf("damage: %d\n", monster[i].damage);
    //     printf("armor: %d\n", monster[i].armor);
    //     printf("speed: %d\n", monster[i].speed);
    //     printf("level: %d\n", monster[i].level);
    //   }
    // }
    // #endif

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

    printf("\nx: %d, y: %d\n", player.x, player.y);
    printf("new_x: %d, new_y: %d\n\n", player.new_x, player.new_y);

    render_tilemap();
    render_items();
    render_player();
    render_monster();
    render_ui();
    render_pop_up_text();

    if(inventory.is_open)
    {
      render_inventory();
    }

    u64 work_counter_elapsed = SDL_GetPerformanceCounter() - counter_old;
    r32 ms_for_work = (1000.0f * (r32)work_counter_elapsed) / (r32)performance_frequency;
    // printf("ms_for_work: %.02f\n", ms_for_work);

    if(SDL_GetSecondsElapsed(counter_old, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
    {
      u32 time_to_delay = ((target_seconds_per_frame - SDL_GetSecondsElapsed(counter_old,
                          SDL_GetPerformanceCounter())) * 1000) - 1;
      if(time_to_delay > 0)
      {
        SDL_Delay(time_to_delay);
      }

      while(SDL_GetSecondsElapsed(counter_old, SDL_GetPerformanceCounter())
            < target_seconds_per_frame)
      {
      }
    }
    else
    {
      // NOTE(rami): We're right on the schedule or late
      // return;
    }

    u64 counter_new = SDL_GetPerformanceCounter();
    u64 counter_elapsed = counter_new - counter_old;
    SDL_RenderPresent(game.renderer);

    r32 ms_per_frame = (1000.0f * (r32)counter_elapsed) / (r32)performance_frequency;
    r32 frames_per_second = (r32)performance_frequency / (r32)counter_elapsed;
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