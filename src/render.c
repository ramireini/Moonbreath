#include <render.h>

void render_inventory()
{
  // render inventory background
  SDL_Rect inv_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(renderer, textures[TEX_INVENTORY_WIN], NULL, &inv_rect);

  render_text("Inventory", inv_rect.x + 32, inv_rect.y + 7, TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);

  // item position and the offset
  int item_name_x = inv_rect.x + 10;
  int item_name_y = inv_rect.y + 30;
  int item_name_offset = 25;

  // item window position and the offsets
  int item_win_x = inv_rect.x - 256;
  int item_win_y = inv_rect.y + inv_rect.h - 300;
  int item_win_offset = 10;
  
  // item highlighter position
  int inv_hl_x = inv_rect.x;
  int inv_hl_y = inv_rect.y + 26;

  // reset item amount
  player->inventory_item_count = 0;

  // render inventory items
  for(int i = 0; i < INVENTORY_COUNT; i++)
  {
    if(inventory[i].unique_id)
    {
      // set the current inventory item amount
      player->inventory_item_count++;

      // store this for easier use
      int index = inventory[i].item_id - 1;

      // calculate inventory item index
      char item_name_glyph[2] = {97 + i};

      // render certain things if this item is currently selected in the inventory
      if(player->inventory_item_selected == i)
      {
        // render texture for selected item
        SDL_Rect inv_hl_rect = {inv_hl_x + 4, inv_hl_y + (item_name_offset * i), 392, 22};
        SDL_RenderCopy(renderer, textures[TEX_INVENTORY_ITEM_SELECTED], NULL, &inv_hl_rect);

        // render item index and name in inventory
        render_text(item_name_glyph, item_name_x, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);
        render_text(items_info[index].name, item_name_x + 25, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);

        // render item window
        SDL_Rect inv_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(renderer, textures[TEX_INVENTORY_ITEM_WIN], NULL, &inv_item_rect);

        // render item name in the item window
        render_text(items_info[index].name, item_win_x + item_win_offset, item_win_y + item_win_offset, TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);

        // render item attributes depending on the type of the item
        if(items_info[index].item_type == TYPE_CONSUME)
        {
          render_text(items_info[index].use, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), TEXT_COLOR_GREEN, fonts[FONT_CURSIVE]);
          render_text(items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), TEXT_COLOR_BROWN, fonts[FONT_CURSIVE]);
          render_text("[C]onsume", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);
          render_text("[D]rop", item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);
        }
        else if(items_info[index].item_type == TYPE_EQUIP)
        {
          render_text("%d Damage", item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), TEXT_COLOR_BLUE, fonts[FONT_CURSIVE], items_info[index].damage);
          render_text(items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), TEXT_COLOR_BROWN, fonts[FONT_CURSIVE]);

          // get the unique id of the item we're currently on in the inventory
          int unique_id = inventory[i].unique_id;

          // find the item we're currently on in the inventory
          for(int i = 0; i < ITEM_COUNT; i++)
          {
            if(items[i].unique_id == unique_id)
            {
              if(items[i].is_equipped)
              {
                render_text("[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_YELLOW, fonts[FONT_CURSIVE]);
                render_text("[D]rop", item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);
              }
              else
              {
                render_text("un[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);
                render_text("[D]rop", item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, fonts[FONT_CURSIVE]);
              }

              break;
            }
          }
        }

        // NOTE(Rami): for debugging, REMOVE LATER
        render_text("%d", item_win_x + item_win_offset, item_win_y + (item_win_offset * 25), TEXT_COLOR_YELLOW, fonts[FONT_CURSIVE], inventory[i].unique_id);
      }
      else
      {
        // render item index and name in inventory
        render_text(item_name_glyph, item_name_x, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);
        render_text(items_info[index].name, item_name_x + 25, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);
      }
    }
  }
}

void render_items()
{
  for(int i = 0; i < ITEM_COUNT; i++)
  {
    // render only items which are on the ground
    if(items[i].is_on_ground)
    {
      SDL_Rect src = {to_pixels(items_info[items[i].item_id - 1].tile), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {items[i].x - camera.x, items[i].y - camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(renderer, textures[TEX_ITEM_TILESET], &src, &dst);
    }
  }
}

void render_interface()
{
  // render the interface stats and the console
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 385, 160};
  SDL_Rect console_rect = {stats_rect.x + stats_rect.w, WINDOW_HEIGHT - 160, WINDOW_WIDTH - console_rect.x, 160};
  SDL_RenderCopy(renderer, textures[TEX_INTERFACE_CONSOLE_WIN], NULL, &console_rect);
  SDL_RenderCopy(renderer, textures[TEX_INTERFACE_STATS_WIN], NULL, &stats_rect);

  // statistics position and offset
  int stats_x = 10;
  int stats_y = WINDOW_HEIGHT - 151;
  int stats_offset = 8;

  // render name
  render_text(player->name, stats_x, stats_y, TEXT_COLOR_WHITE, fonts[FONT_CLASSIC]);

  // NOTE(Rami): could have a function that makes bars like these,
  // it would make this section cleaner and it's valid since
  // we're having more than one of these things exist
  {
    // render player HP bar
    SDL_Rect hp_bar = {stats_x + (stats_offset * 4), stats_y + (stats_offset * 3), player->entity->hp * 20, 16};

    SDL_SetRenderDrawColor(renderer, 77, 23, 23, 255);
    SDL_RenderFillRect(renderer, &hp_bar);
    hp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &hp_bar);

    //render player XP bar
    SDL_Rect xp_bar = {hp_bar.x, hp_bar.y + stats_offset * 2, player->xp * 20, 16};

    SDL_SetRenderDrawColor(renderer, 192, 230, 0, 255);
    SDL_RenderFillRect(renderer, &xp_bar);
    xp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &xp_bar);
  }

  // render HP
  render_text("HP          %d/%d", stats_x, stats_y + (stats_offset * 3), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->hp, player->max_hp);

  // NOTE(Rami): if we want this..
  // NOTE(Rami): implement xp_until_next_level, remember correct xp[] size
  // render XP
  render_text("XP            %d", stats_x, stats_y + (stats_offset * 5), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->xp);

  // render level
  render_text("Level: %d", stats_x, stats_y + (stats_offset * 8), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->level);

  // render attack
  render_text("Damage: %d", stats_x, stats_y + (stats_offset * 10), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->damage);

  // render armor
  render_text("Armor: %d", stats_x, stats_y + (stats_offset * 12), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->armor);

  // NOTE(Rami): rendering turns
  render_text("Turn: %d", stats_x, stats_y + (stats_offset * 14), TEXT_COLOR_WHITE, fonts[FONT_CLASSIC], player->turn);

  // render console messages
  int msg_x = console_rect.x + 10;
  int msg_y = console_rect.y + 8;
  int msg_offset = 16;

  for(int i = 0; i < MESSAGE_COUNT; i++)
  {
    if(messages[i].msg[0] != '.')
    {
      render_text(messages[i].msg, msg_x, msg_y + (i * msg_offset), messages[i].msg_color, fonts[FONT_CLASSIC]);
    }
  }
}

void render_level(char *level, char *fov)
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

  // set render target to tilemap
  SDL_SetRenderTarget(renderer, textures[TEX_TILEMAP]);

  // clear the old contents of the tilemap
  SDL_RenderClear(renderer);

  int to_y = to_tiles(camera.y + camera.h);
  int to_x = to_tiles(camera.x + camera.w);

  for(int y = to_tiles(camera.y); y < to_y; y++)
  {
    for(int x = to_tiles(camera.x); x < to_x; x++)
    {
      // what part of the tileset to render on the tilemap
      SDL_Rect src;
      src.w = TILE_SIZE;
      src.h = TILE_SIZE;

      // where to render on the tilemap
      SDL_Rect dst = {to_pixels(x), to_pixels(y), TILE_SIZE, TILE_SIZE};

      switch(level[(y * LEVEL_SIZE) + x])
      {
        case TILE_WALL_STONE:
        {
          src.x = to_pixels(TILE_WALL_STONE);
          src.y = 0;
        } break;

        case TILE_FLOOR_GRASS:
        {
          src.x = to_pixels(TILE_FLOOR_GRASS);
          src.y = 0;
        } break;

        case TILE_FLOOR_STONE:
        {
          src.x = to_pixels(TILE_FLOOR_STONE);
          src.y = 0;
        } break;

        case TILE_DOOR_CLOSED:
        {
          src.x = to_pixels(TILE_DOOR_CLOSED);
          src.y = 0;
        } break;

        case TILE_DOOR_OPEN:
        {
          src.x = to_pixels(TILE_DOOR_OPEN);
          src.y = 0;
        } break;

        case TILE_PATH_UP:
        {
          src.x = to_pixels(TILE_PATH_UP);
          src.y = 0;
        } break;

        case TILE_PATH_DOWN:
        {
          src.x = to_pixels(TILE_PATH_DOWN);
          src.y = 0;
        } break;
      }

      // NOTE(Rami): continue lighting debugging from here

      //if(fov[(y * LEVEL_SIZE) + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov[(y * LEVEL_SIZ)E + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      // render to tilemap
      SDL_RenderCopy(renderer, textures[TEX_GAME_TILESET], &src, &dst);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  // render tilemap to window
  SDL_Rect dst = {0, 0, camera.w, camera.h};
  SDL_RenderCopy(renderer, textures[TEX_TILEMAP], &camera, &dst);
}