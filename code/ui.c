internal void
add_console_message(char *msg, iv4 color, ...)
{
  char msg_final[256];

  va_list arg_list;
  va_start(arg_list, color);
  vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);
  va_end(arg_list);

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
  {
    if(str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
    {
      strcpy(console_message[i].msg, msg_final);
      console_message[i].color = color;
      return;
    }
  }

  strcpy(console_message[0].msg, CONSOLE_MESSAGE_EMPTY);
  console_message[0].color = color_black;

  for(i32 i = 1; i < CONSOLE_MESSAGE_COUNT; ++i)
  {
    strcpy(console_message[i - 1].msg, console_message[i].msg);
    console_message[i - 1].color = console_message[i].color;
  }

  strcpy(console_message[CONSOLE_MESSAGE_COUNT - 1].msg, msg_final);
  console_message[CONSOLE_MESSAGE_COUNT - 1].color = color;
}

internal void
render_inventory_item_window(SDL_Rect item_window, i32 info_index, i32 item_index)
{
<<<<<<< HEAD
  SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_window);
=======
  SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], NULL, &item_window);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

  if(item_info[info_index].type == type_consume)
  {
    iv2 use_pos = v2(item_window.x + 10, item_window.y + 10);
    render_text(item_info[info_index].use, use_pos, color_green, font[font_cursive]);

    iv2 description_pos = v2(item_window.x + 10, item_window.y + 30);
    render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

    iv2 consume_pos = v2(item_window.x + 10, item_window.y + 255);
    render_text("[C]onsume", consume_pos, color_white, font[font_cursive]);
  }
  else if(item_info[info_index].type == type_equip)
  {
    iv2 damage_pos = v2(item_window.x + 10, item_window.y + 10);
    render_text("%d Damage", damage_pos, color_blue, font[font_cursive], item_info[info_index].damage);

    iv2 description_pos = v2(item_window.x + 10, item_window.y + 30);
    render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

    if(inventory.slot[item_index].is_equipped)
    {
      iv2 equipped_pos = v2(item_window.x + 10, item_window.y + 255);
      render_text("[E]quipped", equipped_pos, color_yellow, font[font_cursive]);
    }
    else
    {
      iv2 unequipped_pos = v2(item_window.x + 10, item_window.y + 255);
      render_text("un[E]quipped", unequipped_pos, color_white, font[font_cursive]);
    }
  }

  iv2 drop_pos = v2(item_window.x + 10, item_window.y + 275);
  render_text("[D]rop", drop_pos, color_white, font[font_cursive]);
}

internal void
render_inventory()
{
  SDL_Rect inventory_window = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
<<<<<<< HEAD
  SDL_RenderCopy(game.renderer, texture[tex_inventory_win], 0, &inventory_window);
=======
  SDL_RenderCopy(game.renderer, texture[tex_inventory_win], NULL, &inventory_window);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

  iv2 header = v2(inventory_window.x + 38, inventory_window.y + 8);
  render_text("Inventory", header, color_white, font[font_classic]);

  iv2 item_name_start = v2(inventory_window.x + 10, inventory_window.y + 30);
  i32 item_count = 0;
  i32 item_name_offset = 25;

  for(i32 item_index = 0; item_index < INVENTORY_SLOT_COUNT; ++item_index)
  {
    if(inventory.slot[item_index].id)
    {
      ++item_count;

      i32 info_index = inventory.slot[item_index].id - 1;
      char item_name_glyph[2] = {LOWERCASE_ALPHABET_START + item_index};

      if(inventory.item_selected == (item_index + 1))
      {
        SDL_Rect selected_item_background = {item_name_start.x - 6, (item_name_start.y - 4) + (item_name_offset * item_index), 392, 22};
<<<<<<< HEAD
        SDL_RenderCopy(game.renderer, texture[tex_inventory_item_selected], 0, &selected_item_background);
=======
        SDL_RenderCopy(game.renderer, texture[tex_inventory_item_selected], NULL, &selected_item_background);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

        SDL_Rect item_window = {inventory_window.x - 256, inventory_window.y + inventory_window.h - 300, 250, 300};
        render_inventory_item_window(item_window, info_index, item_index);

        #if MOONBREATH_DEBUG
        iv2 debug_pos = v2(item_window.x + 200, item_window.y + 275);
        render_text("id: %d", debug_pos, color_yellow, font[font_cursive], inventory.slot[item_index].unique_id);
        #endif
      }

      iv2 item_name_pos = v2(item_name_start.x, item_name_start.y + (item_name_offset * item_index));
      render_text("%s  %s", item_name_pos, color_white, font[font_classic], item_name_glyph, item_info[info_index].name);
    }
  }

  inventory.item_count = item_count;
}

internal void
render_ui()
{
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 386, 160};
<<<<<<< HEAD
  SDL_RenderCopy(game.renderer, texture[tex_interface_stats_win], 0, &stats_rect);

  SDL_Rect console_rect = {386, WINDOW_HEIGHT - 160, WINDOW_WIDTH - 386, 160};
  SDL_RenderCopy(game.renderer, texture[tex_interface_console_win], 0, &console_rect);
=======
  SDL_RenderCopy(game.renderer, texture[tex_interface_stats_win], NULL, &stats_rect);

  SDL_Rect console_rect = {386, WINDOW_HEIGHT - 160, WINDOW_WIDTH - 386, 160};
  SDL_RenderCopy(game.renderer, texture[tex_interface_console_win], NULL, &console_rect);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

  // NOTE(rami): Replace the bars with pixel art versions
  SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
  SDL_Rect hp_bar_inside = {40, WINDOW_HEIGHT - 132, player.hp * 20, 20};
  SDL_RenderFillRect(game.renderer, &hp_bar_inside);

  SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 255);
  SDL_Rect hp_bar_outline = {40, WINDOW_HEIGHT - 132, 200, 20};
  SDL_RenderDrawRect(game.renderer, &hp_bar_outline);

  iv2 name_pos = v2(10, WINDOW_HEIGHT - 152);
  iv2 hp_pos = v2(10, WINDOW_HEIGHT - 130);
  iv2 damage_pos = v2(10, WINDOW_HEIGHT - 110);
  iv2 armor_pos = v2(10, WINDOW_HEIGHT - 92);
  iv2 level_pos = v2(10, WINDOW_HEIGHT - 74);
  iv2 turn_pos = v2(10, WINDOW_HEIGHT - 38);

  render_text(player.name, name_pos, color_white, font[font_classic]);
  render_text("HP          %d/%d", hp_pos, color_white, font[font_classic], player.hp, player.max_hp);
  render_text("Damage: %d", damage_pos, color_white, font[font_classic], player.damage);
  render_text("Armor: %d", armor_pos, color_white, font[font_classic], player.armor);
  render_text("Level: %d", level_pos, color_white, font[font_classic], player.level);
  render_text("Turn: %d", turn_pos, color_white, font[font_classic], game.turn);

  iv2 msg_pos = v2(console_rect.x + 10, console_rect.y + 8);
  i32 msg_offset = 16;

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
  {
    if(!str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
    {
      render_text(console_message[i].msg, msg_pos, console_message[i].color, font[font_classic]);
      msg_pos.y += msg_offset;
    }
  }
}