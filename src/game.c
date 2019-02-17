#include <game.h>
#include <util_conf.h>
#include <time.h>

bool32 game_init()
{
  /* - RANDOM SEED - */

  // NOTE(Rami): 
  // srand(time(NULL));
  // printf("SEED: %lu\n", time(NULL));
  srand(1548744253);

  /* - SDL - */

  if(SDL_Init(SDL_INIT_VIDEO))
  {
    printf("SDL could not initialize: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!window)
  {
    printf("SDL could not create window: %s\n", SDL_GetError());
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!renderer)
  {
    printf("SDL could not create a renderer: %s\n", SDL_GetError());
    return false;
  }

  int32 img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    printf("SLD image library could not initialize: %s\n", IMG_GetError());
    return false;
  }

  if(TTF_Init())
  {
    printf("SDL TTF library could not initialize: %s\n", TTF_GetError());
    return false;
  }

  /* - FONTS - */

  fonts[FONT_CLASSIC] = create_bmp_font_atlas("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);

  TTF_Font *temp = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  fonts[FONT_CURSIVE] = create_ttf_font_atlas(temp, 6);
  TTF_CloseFont(temp);

  /* - TEXTURES - */

  textures[TEX_TILEMAP] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH_IN_PIXELS, LEVEL_HEIGHT_IN_PIXELS);
  textures[TEX_GAME_TILESET] = load_texture("data/images/game_tileset.png", NULL);
  textures[TEX_ITEM_TILESET] = load_texture("data/images/item_tileset.png", NULL);
  textures[TEX_PLAYER_SPRITE_SHEET] = load_texture("data/images/player_sprite_sheet.png", NULL);
  textures[TEX_MONSTER_SPRITE_SHEET] = load_texture("data/images/monster_sprite_sheet.png", NULL);
  textures[TEX_INVENTORY_WIN] = load_texture("data/images/inventory_win.png", NULL);
  textures[TEX_INVENTORY_ITEM_WIN] = load_texture("data/images/inventory_item_win.png", NULL);
  textures[TEX_INVENTORY_ITEM_SELECTED] = load_texture("data/images/inventory_item_selected.png", NULL);
  textures[TEX_INTERFACE_CONSOLE_WIN] = load_texture("data/images/interface_console_win.png", NULL);
  textures[TEX_INTERFACE_STATS_WIN] = load_texture("data/images/interface_stats_win.png", NULL);

  for(int32 i = 0; i < FONT_COUNT; i++)
  {
    if(!fonts[i])
    {
      printf("Font atlas %d failed\n", i);
      return false;
    }
  }

  for(int32 i = 0; i < TEXTURE_COUNT; i++)
  {
    if(!textures[i])
    {
      printf("Texture %d failed\n", i);
      return false;
    }
  }

  /* - ARRAYS - */

  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    items[i].item_id = ID_NONE;
    items[i].unique_id = i + 1;
    items[i].is_on_ground = false;
    items[i].is_equipped = false;
    items[i].x = 0;
    items[i].y = 0;
  }

  for(int32 i = 0; i < INVENTORY_COUNT; i++)
  {
    inventory[i].item_id = ID_NONE;
    inventory[i].unique_id = 0;
    inventory[i].is_on_ground = false;
    inventory[i].is_equipped = false;
    inventory[i].x = 0;
    inventory[i].y = 0;
  }

  for(int32 i = 0; i < MESSAGE_COUNT; i++)
  {
    messages[i].msg[0] = '.';
    messages[i].color = RGBA_COLOR_NONE_S;
  }

  /* - CONFIG - */

  conf_t *conf = conf_load("data/items.cfg");
  if(!conf)
  {
    return false;
  }

  for(int32 i = 0; i < conf->key_value_pair_count / KEY_VALUE_PAIRS_PER_ITEM; i++)
  {
    int32 index = i * KEY_VALUE_PAIRS_PER_ITEM;

    if(conf->vars[index].conf_var_u.i < 0 || conf->vars[index].conf_var_u.i > 100) {return false;}
    if(strlen(conf->vars[index + 1].conf_var_u.s) >= 256) {return false;}
    if(conf->vars[index + 2].conf_var_u.i < 0 || conf->vars[index + 2].conf_var_u.i > 100) {return false;}
    if(conf->vars[index + 3].conf_var_u.i < 0 || conf->vars[index + 3].conf_var_u.i > 100) {return false;}
    if(strlen(conf->vars[index + 4].conf_var_u.s) >= 256) {return false;}
    if(conf->vars[index + 5].conf_var_u.i < 0 || conf->vars[index + 5].conf_var_u.i > 100) {return false;}
    if(conf->vars[index + 6].conf_var_u.i < 0 || conf->vars[index + 6].conf_var_u.i > 100) {return false;}
    if(conf->vars[index + 7].conf_var_u.i < 0 || conf->vars[index + 7].conf_var_u.i > 100) {return false;}
    if(strlen(conf->vars[index + 8].conf_var_u.s) >= 256) {return false;}

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
  printf("Config free'd\n");

  // NOTE(Rami): So we have something to render without the player pressing at start.
  turn_changed = true;
  game_is_running = true;
  return true;
}

void game_run()
{
  create_player();

  generate_level();

  // add_game_item(ID_LESSER_HEALTH_POTION, player->entity->x, player->entity->y - 32);
  // add_game_item(ID_IRON_SWORD, player->entity->x, player->entity->y + 32);

  // create_slimes(player->entity->x + 2, player->entity->y);

  // uint32 start, end;
  while(game_is_running)
  {
    time_elapsed = SDL_GetTicks();
    // printf("%d\n", time_elapsed);
    // start = SDL_GetTicks();

    update_events();
    update_input(level);

    // // NOTE(Rami):
    // for(int32 i = 0; i < SLIME_COUNT; i++)
    // {
    //   if(slimes[i])
    //   {
    //     printf("slimes[%d] VALID\n", i);
    //   }
    //   else
    //   {
    //     printf("slimes[%d] NULL\n", i);
    //   }

    //   printf("\n");
    // }

    // for (int32 i = 0; i < INVENTORY_COUNT; i++)
    // {
    //   if(inventory[i].unique_id != 0)
    //   {
    //     printf("%d, [ITEM]\n", i);
    //     printf("item_id %d\n", inventory[i].item_id);
    //     printf("unique_id %d\n", inventory[i].unique_id);
    //     printf("is_on_ground %d\n", inventory[i].is_on_ground);
    //     printf("equipped %d\n", inventory[i].is_equipped);
    //     printf("x %d\n", inventory[i].x);
    //     printf("y %d\n\n", inventory[i].y);
    //   }
    // }

    // for (int32 i = 0; i < ITEM_COUNT; i++)
    // {
    //   if (items[i].item_id != -1)
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

    if(turn_changed)
    {
      update_player();
      // update_slimes();

      update_camera();

      turn_changed = false;
    }

    SDL_SetRenderDrawColor(renderer, RGBA_COLOR_NONE_P);
    SDL_RenderClear(renderer);

    render_level();
    render_items();

    // render_slimes();
    render_player();
    
    render_interface();

    if(player->inventory_display)
    {
      render_inventory();
    }

    SDL_RenderPresent(renderer);

    // end = SDL_GetTicks();
    // printf("%d\n", end - start);
  }
}

// NOTE(Rami): Set the pointers to null no matter what,
// we don't want dangling pointers in any case.
void game_exit()
{
  free_player(player);

  if(level)
  {
    free(level);
  }

  // NOTE(Rami): If there are slimes that aren't killed,
  // this will deallocate them.
  for(int i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(slimes[i]->entity)
      {
        free(slimes[i]->entity);
      }

      free(slimes[i]);
      slimes[i] = NULL;
    }
  }

  for(int32 i = 0; i < FONT_COUNT; i++)
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

  for(int32 i = 0; i < TEXTURE_COUNT; i++)
  {
    if(textures[i])
    {
      SDL_DestroyTexture(textures[i]);
      textures[i] = NULL;
    }
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

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}