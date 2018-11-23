#include <render.h>

void render_inventory(player_t *player, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, ttf_font_t *font_inv, ttf_font_t *font_item)
{
  // render inventory background
  SDL_Rect inv_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(renderer, inv_tex, NULL, &inv_rect);

  render_text_ttf("Inventory", inv_rect.x + 34, inv_rect.y + 5, TEXT_COLOR_WHITE, font_inv);

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
      char item_name_glyph[] = {97 + i, '\0'};

      // render item index and name in inventory
      render_text_ttf(item_name_glyph, item_name_x, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, font_inv);
      render_text_ttf(game_items_info[index].name, item_name_x + 25, item_name_y + (item_name_offset * i), TEXT_COLOR_WHITE, font_inv);

      // render certain things if this item is currently selected in the inventory
      if(player->inventory_hl_index == i)
      {
        // render highlighter
        SDL_Rect inv_hl_rect = {inv_hl_x, inv_hl_y + (item_name_offset * i), inv_rect.w, 22};
        SDL_RenderCopy(renderer, inv_hl_tex, NULL, &inv_hl_rect);

        // render item window and item information
        SDL_Rect inv_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(renderer, inv_item_tex, NULL, &inv_item_rect);

        // render item name in the item window
        render_text_ttf(game_items_info[index].name, item_win_x + item_win_offset, item_win_y + item_win_offset, TEXT_COLOR_WHITE, font_item);

        // render item attributes depending on the type of the item
        if(game_items_info[index].item_type == TYPE_CONSUME)
        {
          render_text_ttf(game_items_info[index].use, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), TEXT_COLOR_GREEN, font_item);
          render_text_ttf(game_items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), TEXT_COLOR_ORANGE, font_item);
          render_text_ttf("[C]onsume", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, font_item);
          render_text_ttf("[D]rop", item_win_x + (item_win_offset * 7), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, font_item);
        }
        else if(game_items_info[index].item_type == TYPE_EQUIP)
        {
          render_text_ttf("%d Damage", item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), TEXT_COLOR_BLUE, font_item, game_items_info[index].damage);
          render_text_ttf(game_items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), TEXT_COLOR_ORANGE, font_item);

          // get the unique id of the item we're currently on in the inventory
          int unique_id = inventory[i].unique_id;

          // find the item we're currently on in the inventory
          for(int i = 0; i < GAME_ITEMS_COUNT; i++)
          {
            if(game_items[i].unique_id == unique_id)
            {
              if(game_items[i].is_equipped)
              {
                render_text_ttf("[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_YELLOW, font_item);
                render_text_ttf("[D]rop", item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, font_item);
              }
              else
              {
                render_text_ttf("un[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, font_item);
                render_text_ttf("[D]rop", item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27), TEXT_COLOR_WHITE, font_item);
              }

              break;
            }
          }
        }
        // NOTE(Rami): for debugging, REMOVE LATER
        render_text_ttf("%d", item_win_x + item_win_offset, item_win_y + (item_win_offset * 25), TEXT_COLOR_YELLOW, font_item, inventory[i].unique_id);
      }
    }
  }
}

void render_items(SDL_Texture *item_tileset_tex, SDL_Rect *camera)
{
  SDL_Rect src = {0, 0, TILE_SIZE, TILE_SIZE};
  // NOTE(Rami): might have more rows of items than 1 later
  src.y = 0;

  SDL_Rect dst = {0, 0, TILE_SIZE, TILE_SIZE};

  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // render only items which are on the ground
    if(game_items[i].is_on_ground)
    {
      // set texture x
      src.x = to_pixels(game_items_info[game_items[i].item_id - 1].tile);

      // set item position
      dst.x = game_items[i].x - camera->x;
      dst.y = game_items[i].y - camera->y;

      SDL_RenderCopy(renderer, item_tileset_tex, &src, &dst);
    }
  }
}

void render_interface(player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex, ttf_font_t *font_struct)
{
  // render the interface stats and the console
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 385, 160};
  SDL_Rect console_rect = {stats_rect.x + stats_rect.w, WINDOW_HEIGHT - 160, WINDOW_WIDTH - console_rect.x, 160};
  SDL_RenderCopy(renderer, interface_console_tex, NULL, &console_rect);
  SDL_RenderCopy(renderer, interface_stats_tex, NULL, &stats_rect);

  // statistics position and offset
  int stats_x = 8;
  int stats_y = WINDOW_HEIGHT - 151;
  int stats_offset = 10;

  // render name
  render_text_ttf(player->name, stats_x, stats_y, TEXT_COLOR_WHITE, font_struct);

  // render level
  render_text_ttf("Level: %d", stats_x, stats_y + (stats_offset * 6), TEXT_COLOR_WHITE, font_struct, player->level);

  {
    // render player HP bar
    SDL_Rect hp_bar = {stats_x + (stats_offset * 2), stats_y + (stats_offset * 2), player->hp * 20, 14};

    SDL_SetRenderDrawColor(renderer, 77, 23, 23, 255);
    SDL_RenderFillRect(renderer, &hp_bar);
    hp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &hp_bar);

    //render player XP bar
    SDL_Rect xp_bar = {hp_bar.x, hp_bar.y + stats_offset * 2, player->xp * 20, 14};

    SDL_SetRenderDrawColor(renderer, 192, 230, 0, 255);
    SDL_RenderFillRect(renderer, &xp_bar);
    xp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &xp_bar);
  }

  // render HP text
  render_text_ttf("HP                   %d/%d", stats_x, stats_y + (stats_offset * 2), TEXT_COLOR_WHITE, font_struct, player->hp, player->max_hp);

  // NOTE(Rami): implement xp_until_next_level, remember correct xp[] size
  // render XP text
  render_text_ttf("XP                                                %d", stats_x, stats_y + (stats_offset * 4), TEXT_COLOR_WHITE, font_struct, player->xp);

  // render attack text
  render_text_ttf("Attack: %d", stats_x, stats_y + (stats_offset * 7), TEXT_COLOR_WHITE, font_struct, player->attack);

  // render armor text
  render_text_ttf("Armor: %d", stats_x, stats_y + (stats_offset * 8), TEXT_COLOR_WHITE, font_struct, player->armor);

  // render console messages
  int msg_x = console_rect.x + 10;
  int msg_y = console_rect.y + 8;
  int msg_offset = 12;

  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(console_messages[i].msg[0] != '.')
    {
      render_text_ttf(console_messages[i].msg, msg_x, msg_y + (i * msg_offset), console_messages[i].msg_color, font_struct);
    }
  }
}

void render_player(SDL_Texture *player_tileset_tex, SDL_Texture *item_tileset_tex, SDL_Rect *camera, player_t *player)
{
  // calculate player texture source and destination
  SDL_Rect player_src = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect player_dst = {player->x - camera->x, player->y - camera->y, player->w, player->h};

  // render player
  SDL_RenderCopy(renderer, player_tileset_tex, &player_src, &player_dst);

  // sword one
  int sword_one = 0;
  SDL_Rect sword_one_dst = {player->x - camera->x + 0, player->y - camera->y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  int sword_two = 0;
  SDL_Rect sword_two_dst = {player->x - camera->x + 11, player->y - camera->y - 3, player->w, player->h};

  // NOTE(Rami): fix this later, issue with the sword dual wield

  // source for the item texture
  SDL_Rect item_src;
  item_src.y = 0;
  item_src.w = TILE_SIZE;
  item_src.h = TILE_SIZE;
  
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // if equipped
    if(game_items[i].is_equipped)
    {
      // if an iron sword
      if(game_items[i].item_id == ID_IRON_SWORD)
      {
        // if hasn't been rendered before
        if(!sword_one)
        {
          sword_one = 1;

          // get the correct x-axis position for the item tile
          item_src.x = to_pixels(game_items_info[game_items[i].item_id - 1].tile);

          // render it
          SDL_RenderCopy(renderer, item_tileset_tex, &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = to_pixels(game_items_info[game_items[i].item_id - 1].tile);

          SDL_RenderCopy(renderer, item_tileset_tex, &item_src, &sword_two_dst);
        }
      }
    }
  }
}

void render_level(SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, char *level, char *fov, SDL_Rect *camera)
{
  // set render target to tilemap
  SDL_SetRenderTarget(renderer, tilemap_tex);

  // clear the old contents of the tilemap
  SDL_RenderClear(renderer);

  int to_y = (camera->y + camera->h) / TILE_SIZE;
  int to_x = (camera->x + camera->w) / TILE_SIZE;

  for(int y = camera->y / TILE_SIZE; y < to_y; y++)
  {
    for(int x = camera->x / TILE_SIZE; x < to_x; x++)
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
      SDL_RenderCopy(renderer, tileset_tex, &src, &dst);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  // render tilemap to window
  SDL_Rect dst = {0, 0, camera->w, camera->h};
  SDL_RenderCopy(renderer, tilemap_tex, camera, &dst);
}