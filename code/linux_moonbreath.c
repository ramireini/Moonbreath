#include "types.h"
#include "moonbreath.c"

internal i32
platform_init_game()
{
  i32 result = 0;

  if(!SDL_Init(SDL_INIT_VIDEO))
  {
    i32 window_flags = 0;
    game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                 WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    if(game.window)
    {
      printf("Monitor refresh rate is %d HZ\n", get_window_refresh_rate(game.window));

      u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
      game.renderer = SDL_CreateRenderer(game.window, -1, renderer_flags);
      if(game.renderer)
      {
        i32 img_flags = IMG_INIT_PNG;
        if(IMG_Init(img_flags) & img_flags)
        {
          if(!TTF_Init())
          {
            b32 font_ok = 1;

            font[font_classic] = create_bmp_font("../data/fonts/classic16x16.png",
                                                 16, 16, 14, 8, 12);
            font[font_cursive] = create_ttf_font("../data/fonts/alkhemikal.ttf", 16, 4);

            for(i32 i = 0; i < font_total; ++i)
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

              texture[tex_tilemap] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_PIXEL_WIDTH, LEVEL_PIXEL_HEIGHT);
              texture[tex_game_tileset] = load_texture("../data/images/game_tileset.png", 0);
              texture[tex_item_tileset] = load_texture("../data/images/item_tileset.png", 0);
              texture[tex_wearable_item_tileset] = load_texture("../data/images/wearable_item_tileset.png", 0);
              texture[tex_sprite_sheet] = load_texture("../data/images/sprite_sheet.png", 0);
              texture[tex_inventory_win] = load_texture("../data/images/inventory_win.png", 0);
              texture[tex_inventory_item_win] = load_texture("../data/images/inventory_item_win.png", 0);
              texture[tex_inventory_selected_item] = load_texture("../data/images/inventory_selected_item.png", 0);
              texture[tex_interface_bottom_win] = load_texture("../data/images/interface_bottom_win.png", 0);
              texture[tex_health_bar_outside] = load_texture("../data/images/health_bar_outside.png", 0);
              texture[tex_health_bar_inside] = load_texture("../data/images/health_bar_inside.png", 0);

              for(i32 i = 0; i < tex_total; ++i)
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

                game.state = state_running;
                game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
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

                // NOTE(rami): Since we know the item we are setting the information for,
                // we could skip all the things that item doesn't care about because
                // the item array is initialized to zero

                item_info[0].id = 1;
                strcpy(item_info[0].name, "Lesser Health Potion");
                item_info[0].category = category_consumable;
                item_info[0].slot = slot_none;
                item_info[0].tile_x = 8;
                item_info[0].tile_y = 0;
                strcpy(item_info[0].use, "Restores 2 health");
                item_info[0].heal_amount = 2;
                item_info[0].damage = 0;
                item_info[0].armor = 0;
                strcpy(item_info[0].description, "A magical red liquid created with \nan unknown formula. Consuming \nthem is said to heal simple cuts \nand even grievous wounds.");

                item_info[1].id = 2;
                strcpy(item_info[1].name, "Iron Sword");
                item_info[1].category = category_weapon;
                item_info[1].slot = slot_first_hand;
                item_info[1].tile_x = 4;
                item_info[1].tile_y = 1;
                item_info[1].use[0] = 0;
                item_info[1].heal_amount = 0;
                item_info[1].damage = 2;
                item_info[1].armor = 0;
                strcpy(item_info[1].description, "A well-built straight sword with a\nsymbol of an unknown blacksmith\ningrained onto it.");

                item_info[2].id = 3;
                strcpy(item_info[2].name, "Rune Helmet");
                item_info[2].category = category_armor;
                item_info[2].slot = slot_head;
                item_info[2].tile_x = 0;
                item_info[2].tile_y = 1;
                item_info[2].use[0] = 0;
                item_info[2].heal_amount = 0;
                item_info[2].damage = 0;
                item_info[2].armor = 1;
                strcpy(item_info[2].description, "A rune helmet.");

                item_info[3].id = 4;
                strcpy(item_info[3].name, "Rune Chestplate");
                item_info[3].category = category_armor;
                item_info[3].slot = slot_body;
                item_info[3].tile_x = 1;
                item_info[3].tile_y = 1;
                item_info[3].use[0] = 0;
                item_info[3].heal_amount = 0;
                item_info[3].damage = 0;
                item_info[3].armor = 1;
                strcpy(item_info[3].description, "A rune chestplate.");

                item_info[4].id = 5;
                strcpy(item_info[4].name, "Rune Platelegs");
                item_info[4].category = category_armor;
                item_info[4].slot = slot_legs;
                item_info[4].tile_x = 2;
                item_info[4].tile_y = 1;
                item_info[4].use[0] = 0;
                item_info[4].heal_amount = 0;
                item_info[4].damage = 0;
                item_info[4].armor = 1;
                strcpy(item_info[4].description, "A pair of rune platelegs.");

                item_info[5].id = 6;
                strcpy(item_info[5].name, "Rune Boots");
                item_info[5].category = category_armor;
                item_info[5].slot = slot_feet;
                item_info[5].tile_x = 3;
                item_info[5].tile_y = 1;
                item_info[5].use[0] = 0;
                item_info[5].heal_amount = 0;
                item_info[5].damage = 0;
                item_info[5].armor = 1;
                strcpy(item_info[5].description, "A pair of rune boots.");

                item_info[6].id = 7;
                strcpy(item_info[6].name, "Rune Shield");
                item_info[6].category = category_armor;
                item_info[6].slot = slot_second_hand;
                item_info[6].tile_x = 5;
                item_info[6].tile_y = 1;
                item_info[6].use[0] = 0;
                item_info[6].heal_amount = 0;
                item_info[6].damage = 0;
                item_info[6].armor = 1;
                strcpy(item_info[6].description, "A rune shield.");

                item_info[7].id = 8;
                strcpy(item_info[7].name, "Rune Amulet");
                item_info[7].category = category_armor;
                item_info[7].slot = slot_amulet;
                item_info[7].tile_x = 6;
                item_info[7].tile_y = 1;
                item_info[7].use[0] = 0;
                item_info[7].heal_amount = 0;
                item_info[7].damage = 0;
                item_info[7].armor = 1;
                strcpy(item_info[7].description, "A rune amulet.");

                item_info[8].id = 9;
                strcpy(item_info[8].name, "Rune Ring");
                item_info[8].category = category_armor;
                item_info[8].slot = slot_ring;
                item_info[8].tile_x = 7;
                item_info[8].tile_y = 1;
                item_info[8].use[0] = 0;
                item_info[8].heal_amount = 0;
                item_info[8].damage = 0;
                item_info[8].armor = 1;
                strcpy(item_info[8].description, "A rune ring.");

                item_info[9].id = 10;
                strcpy(item_info[9].name, "Red Chestplate");
                item_info[9].category = category_armor;
                item_info[9].slot = slot_body;
                item_info[9].tile_x = 1;
                item_info[9].tile_y = 2;
                item_info[9].use[0] = 0;
                item_info[9].heal_amount = 0;
                item_info[9].damage = 0;
                item_info[9].armor = 3;
                strcpy(item_info[9].description, "A red chestplate.");

                item_info[10].id = 11;
                strcpy(item_info[10].name, "Red Sword");
                item_info[10].category = category_weapon;
                item_info[10].slot = slot_first_hand;
                item_info[10].tile_x = 4;
                item_info[10].tile_y = 2;
                item_info[10].use[0] = 0;
                item_info[10].heal_amount = 0;
                item_info[10].damage = 3;
                item_info[10].armor = 0;
                strcpy(item_info[10].description, "A red sword.");

                result = 1;
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
platform_exit_game()
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

i32 main()
{
  if(platform_init_game())
  {
    run_game();
    platform_exit_game();
    return(EXIT_SUCCESS);
  }

  platform_exit_game();
  return(EXIT_FAILURE);
}