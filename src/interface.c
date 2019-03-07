void add_console_msg(char *msg, SDL_Color color, ...)
{
  char msg_final[256];

  va_list arg_list;
  va_start(arg_list, color);
  vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);
  va_end(arg_list);

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(str_cmp(console_messages[i].msg, CONSOLE_MESSAGE_EMPTY))
    {
      strcpy(console_messages[i].msg, msg_final);
      console_messages[i].color = color;
      return;
    }
  }

  strcpy(console_messages[0].msg, CONSOLE_MESSAGE_EMPTY);
  console_messages[0].color = RGBA_COLOR_BLACK_S;

  for(i32 i = 1; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    strcpy(console_messages[i - 1].msg, console_messages[i].msg);
    console_messages[i - 1].color = console_messages[i].color;
  }

  strcpy(console_messages[CONSOLE_MESSAGE_COUNT - 1].msg, msg_final);
  console_messages[CONSOLE_MESSAGE_COUNT - 1].color = color;
  return;
}

// NOTE(Rami): Make sense of this monstrosity
void render_inventory()
{
  // render inventory background
  SDL_Rect inv_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(game_state.renderer, assets.textures[tex_inventory_win], NULL, &inv_rect);

  render_text("Inventory", (iv2_t){inv_rect.x + 32, inv_rect.y + 7}, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);

  // item position and the offset
  i32 item_name_x = inv_rect.x + 10;
  i32 item_name_y = inv_rect.y + 30;
  i32 item_name_offset = 25;

  // item window position and the offsets
  i32 item_win_x = inv_rect.x - 256;
  i32 item_win_y = inv_rect.y + inv_rect.h - 300;
  i32 item_win_offset = 10;
  
  // item highlighter position
  i32 inv_hl_x = inv_rect.x;
  i32 inv_hl_y = inv_rect.y + 26;

  // reset item count
  player.inventory.item_count = 0;

  // render inventory items
  for(i32 i = 0; i < INVENTORY_SLOT_COUNT; i++)
  {
    if(player.inventory.slots[i].unique_id)
    {
      // set the current inventory item amount
      player.inventory.item_count++;

      // store this for easier use
      i32 index = player.inventory.slots[i].item_id;

      // calculate inventory item index
      char item_name_glyph[2] = {97 + i};

      // render certain things if this item is currently selected in the inventory
      if(player.inventory.item_selected == i)
      {
        // render texture for selected item
        SDL_Rect inv_hl_rect = {inv_hl_x + 4, inv_hl_y + (item_name_offset * i), 392, 22};
        SDL_RenderCopy(game_state.renderer, assets.textures[tex_inventory_item_selected], NULL, &inv_hl_rect);

        // render item index and name in inventory
        render_text(item_name_glyph, (iv2_t){item_name_x, item_name_y + (item_name_offset * i)}, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);
        render_text(items_info[index].name, (iv2_t){item_name_x + 25, item_name_y + (item_name_offset * i)}, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);

        // render item window
        SDL_Rect inv_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(game_state.renderer, assets.textures[tex_inventory_item_win], NULL, &inv_item_rect);

        // render item name in the item window
        render_text(items_info[index].name, (iv2_t){item_win_x + item_win_offset, item_win_y + item_win_offset}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);

        // render item attributes depending on the type of the item
        if(items_info[index].item_type == type_consume)
        {
          render_text(items_info[index].use, (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 3)}, RGBA_COLOR_GREEN_S, assets.fonts[font_cursive]);
          render_text(items_info[index].description, (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 5)}, RGBA_COLOR_BROWN_S, assets.fonts[font_cursive]);
          render_text("[C]onsume", (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 27)}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);
          render_text("[D]rop", (iv2_t){item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27)}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);
        }
        else if(items_info[index].item_type == type_equip)
        {
          render_text("%d Damage", (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 3)}, RGBA_COLOR_BLUE_S, assets.fonts[font_cursive], items_info[index].damage);
          render_text(items_info[index].description, (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 5)}, RGBA_COLOR_BROWN_S, assets.fonts[font_cursive]);

          // get the unique id of the item we're currently on in the inventory
          i32 unique_id = player.inventory.slots[i].unique_id;

          // find the item we're currently on in the inventory
          for(i32 i = 0; i < ITEM_COUNT; i++)
          {
            if(items[i].unique_id == unique_id)
            {
              if(items[i].is_equipped)
              {
                render_text("[E]quipped", (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 27)}, RGBA_COLOR_YELLOW_S, assets.fonts[font_cursive]);
                render_text("[D]rop", (iv2_t){item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27)}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);
              }
              else
              {
                render_text("un[E]quipped", (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 27)}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);
                render_text("[D]rop", (iv2_t){item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27)}, RGBA_COLOR_WHITE_S, assets.fonts[font_cursive]);
              }

              break;
            }
          }
        }

        // NOTE(Rami): For debugging, delete later.
        render_text("%d", (iv2_t){item_win_x + item_win_offset, item_win_y + (item_win_offset * 25)}, RGBA_COLOR_YELLOW_S, assets.fonts[font_cursive], player.inventory.slots[i].unique_id);
      }
      else
      {
        // render item index and name in inventory
        render_text(item_name_glyph, (iv2_t){item_name_x, item_name_y + (item_name_offset * i)}, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);
        render_text(items_info[index].name, (iv2_t){item_name_x + 25, item_name_y + (item_name_offset * i)}, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);
      }
    }
  }
}

void render_interface()
{
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 386, 160};
  SDL_RenderCopy(game_state.renderer, assets.textures[tex_interface_stats_win], NULL, &stats_rect);

  SDL_Rect console_rect = {386, WINDOW_HEIGHT - 160, WINDOW_WIDTH - 386, 160};
  SDL_RenderCopy(game_state.renderer, assets.textures[tex_interface_console_win], NULL, &console_rect);

  // NOTE(Rami): Replace the bars with pixel art versions.
  SDL_Rect hp_bar_inside = {40, WINDOW_HEIGHT - 132, player.entity.hp * 20, 20};
  SDL_Rect hp_bar_outline = {40, WINDOW_HEIGHT - 132, 200, 20};

  SDL_SetRenderDrawColor(game_state.renderer, RGBA_COLOR_RED_P);
  SDL_RenderFillRect(game_state.renderer, &hp_bar_inside);
  SDL_SetRenderDrawColor(game_state.renderer, RGBA_COLOR_WHITE_P);
  SDL_RenderDrawRect(game_state.renderer, &hp_bar_outline);

  iv2_t name_pos = {10, WINDOW_HEIGHT - 152};
  iv2_t hp_pos = {10, WINDOW_HEIGHT - 130};
  iv2_t damage_pos = {10, WINDOW_HEIGHT - 110};
  iv2_t armor_pos = {10, WINDOW_HEIGHT - 92};
  iv2_t level_pos = {10, WINDOW_HEIGHT - 74};
  iv2_t turn_pos = {10, WINDOW_HEIGHT - 38};

  render_text(player.name, name_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic]);
  render_text("HP          %d/%d", hp_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic], player.entity.hp, player.max_hp);
  render_text("Damage: %d", damage_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic], player.entity.damage);
  render_text("Armor: %d", armor_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic], player.entity.armor);
  render_text("Level: %d", level_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic], player.level);
  render_text("Turn: %d", turn_pos, RGBA_COLOR_WHITE_S, assets.fonts[font_classic], player.turn);

  iv2_t msg_pos = {console_rect.x + 10, console_rect.y + 8};
  i32 msg_offset = 16;

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(!str_cmp(console_messages[i].msg, CONSOLE_MESSAGE_EMPTY))
    {
      render_text(console_messages[i].msg, msg_pos, console_messages[i].color, assets.fonts[font_classic]);
      msg_pos.y += msg_offset;
    }
  }
}