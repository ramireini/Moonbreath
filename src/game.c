#include <game.h>
#include <util_conf.h>
#include <time.h>

int32 game_init()
{
  /* - RANDOM SEED - */

  srand(time(NULL));

  /* - SDL - */

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize: %s\n", SDL_GetError());
    return 0;
  }

  window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!window)
  {
    printf("SDL could not create window: %s\n", SDL_GetError());
    return 0;
  }

  uint32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  renderer = SDL_CreateRenderer(window, -1, renderer_flags);
  if(!renderer)
  {
    printf("SDL could not create a renderer: %s\n", SDL_GetError());
    return 0;
  }

  int32 img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    printf("SLD image library could not initialize: %s\n", IMG_GetError());
    return 0;
  }

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

  for(int32 i = 0; i < FONT_COUNT; i++)
  {
    if(!fonts[i])
    {
      printf("Font atlas %d failed\n", i);
      return 0;
    }
  }

  for(int32 i = 0; i < TEXTURE_COUNT; i++)
  {
    if(!textures[i])
    {
      printf("Texture %d failed\n", i);
      return 0;
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
    messages[i].msg_color = 0;
  }

  /* - CONFIG - */

  conf_t *conf = conf_load("data/items.cfg");
  if(!conf)
  {
    return 0;
  }

  for(int32 i = 0; i < conf->key_value_pair_count / KEY_VALUE_PAIRS_PER_ITEM; i++)
  {
    int32 index = i * KEY_VALUE_PAIRS_PER_ITEM;

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
  printf("Config free'd\n");

  // NOTE(Rami): 
  // so we have something to render without the player pressing at start
  turn_changed = true;
  game_is_running = true;
  return 1;
}

void game_run(char *level, char *fov)
{
  create_player("Frozii", 0, 0, 0, 5, 10, 0, 0, 0, TILE_SIZE, TILE_SIZE, 1, 6, 3, 0);

  generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2);

  add_game_item(ID_LESSER_HEALTH_POTION, player->entity->x, player->entity->y - 32);
  add_game_item(ID_IRON_SWORD, player->entity->x, player->entity->y + 32);

  create_slimes(0, 4, 1, 0, 4, player->entity->x + TILE_SIZE * 2, player->entity->y, TILE_SIZE, TILE_SIZE);

  while(game_is_running)
  {
    // NOTE(Rami): 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

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

    // NOTE(Rami):
    // for (int32 i = 0; i < INVENTORY_COUNT; i++)
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

    // for (int32 i = 0; i < ITEM_COUNT; i++)
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

      update_camera();

      turn_changed = false;
    }

    render_level(level, fov);
    render_items();

    render_slimes();
    render_player();
    
    render_interface();

    if(player->inventory_display)
    {
      render_inventory();
    }

    SDL_RenderPresent(renderer);
  }
}

// NOTE(Rami): set the pointers to null no matter what,
// we don't want dangling pointers in any case
void game_exit(char *level, char *fov)
{
  free_player(player);

  // NOTE(Rami): if there are slimes that aren't killed,
  // this will deallocate them
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