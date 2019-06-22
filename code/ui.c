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

// internal void
// render_item_window(SDL_Rect item_window, i32 info_index, i32 item_index)
// {
//   SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_window);

//   if(item_info[info_index].category == category_consumable)
//   {
//     iv2 use_pos = v2(item_window.x + 10, item_window.y + 10);
//     render_text(item_info[info_index].use, use_pos, color_green, font[font_classic]);

//     iv2 description_pos = v2(item_window.x + 10, item_window.y + 30);
//     render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

//     iv2 consume_pos = v2(item_window.x + 10, item_window.y + 255);
//     render_text("[C]onsume", consume_pos, color_white, font[font_cursive]);
//   }
//   else if(item_info[info_index].category == category_weapon ||
//           item_info[info_index].category == category_armor)
//   {
//     if(item_info[info_index].category == category_weapon)
//     {
//       iv2 damage_pos = v2(item_window.x + 10, item_window.y + 10);
//       render_text("%d Damage", damage_pos, color_white, font[font_classic], item_info[info_index].damage);
//     }
//     else
//     {
//       iv2 armor_pos = v2(item_window.x + 10, item_window.y + 10);
//       render_text("%d Armor", armor_pos, color_white, font[font_classic], item_info[info_index].armor);
//     }

//     iv2 description_pos = v2(item_window.x + 10, item_window.y + 30);
//     render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

//     if(inventory.slot[item_index].equipped)
//     {
//       iv2 equipped_pos = v2(item_window.x + 10, item_window.y + 255);
//       render_text("[E]quipped", equipped_pos, color_yellow, font[font_cursive]);
//     }
//     else
//     {
//       iv2 unequipped_pos = v2(item_window.x + 10, item_window.y + 255);
//       render_text("un[E]quipped", unequipped_pos, color_white, font[font_cursive]);
//     }
//   }

//   iv2 drop_pos = v2(item_window.x + 10, item_window.y + 275);
//   render_text("[D]rop", drop_pos, color_white, font[font_cursive]);
// }

internal void
render_inventory()
{
  // Render inventory window
  SDL_Rect inventory_win = {WINDOW_WIDTH - 324, WINDOW_HEIGHT - 500, 298, 307};
  SDL_RenderCopy(game.renderer, texture[tex_inventory_win], 0, &inventory_win);

  i32 padding = 4;
  i32 first_slot_x = inventory_win.x + 7;
  i32 first_slot_y = inventory_win.y + 160;

  // Render selected item texture
  i32 selected_x_offset = tile_mul(inventory.x) + ((inventory.x) * padding);
  i32 selected_y_offset = tile_mul(inventory.y) + ((inventory.y) * padding);
  SDL_Rect selected = {first_slot_x + selected_x_offset, first_slot_y + selected_y_offset, 32, 32};
  SDL_RenderCopy(game.renderer, texture[tex_inventory_selected_item], 0, &selected);

  i32 new_item_count = 0;

  for(i32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
  {
    if(inventory.slot[i].id)
    {
      ++new_item_count;

      // NOTE(rami): If we don't need this more than once or something then remove this
      i32 info_index = inventory.slot[i].id - 1;

      // Render item
      SDL_Rect src = {tile_mul(item_info[info_index].tile - 1), 0, 32, 32};
      SDL_Rect dest = {first_slot_x + tile_mul(i) + (i * padding), first_slot_y, 32, 32};

      SDL_SetTextureColorMod(texture[tex_item_tileset], 255, 255, 255);
      SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &src, &dest);

      // NOTE(rami): Investigate the best way to have text positions and/or see if
      // you can wrap some of the + whatever's into variables

      // NOTE(rami): Idea: Perhaps something like first_stat, second_stat etc. structs that you can use as you see fit,
      // and they have hardcoded positions so you just put certain stats where they need to be for whatever item
      if(i  == (inventory.y * INVENTORY_HEIGHT) + inventory.x)
      {
        // Render item window
        SDL_Rect item_win = {inventory_win.x, inventory_win.y, 250, 307};
        item_win.x -= (item_win.w + padding);
        SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_win);

        // Render item information
        iv2 name_pos = v2(item_win.x + 8, item_win.y + 8);
        render_text("%s", name_pos, color_white, font[font_cursive], item_info[info_index].name);

        if(item_info[info_index].category == category_consumable)
        {
          iv2 use_pos = v2(item_win.x + 8, item_win.y + 28);
          render_text(item_info[info_index].use, use_pos, color_green, font[font_cursive]);

          iv2 description_pos = v2(item_win.x + 8, item_win.y + 48);
          render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

          iv2 consume_pos = v2(item_win.x + 48, item_win.y + item_win.h - 44);
          render_text("[C]onsume", consume_pos, color_white, font[font_cursive]);
        }
        else if(item_info[info_index].category == category_weapon ||
                item_info[info_index].category == category_armor)
        {
          if(item_info[info_index].category == category_weapon)
          {
            iv2 damage_pos = v2(item_win.x + 8, item_win.y + 28);
            render_text("%d Damage", damage_pos, color_white, font[font_cursive], item_info[info_index].damage);
          }
          else
          {
            iv2 armor_pos = v2(item_win.x + 8, item_win.y + 28);
            render_text("%d Armor", armor_pos, color_white, font[font_cursive], item_info[info_index].armor);
          }

          iv2 description_pos = v2(item_win.x + 8, item_win.y + 48);
          render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);

          if(inventory.slot[i].equipped)
          {
            iv2 equipped_pos = v2(item_win.x + 48, item_win.y + item_win.h - 44);
            render_text("[E]quipped", equipped_pos, color_yellow, font[font_cursive]);
          }
          else
          {
            iv2 unequipped_pos = v2(item_win.x + 48, item_win.y + item_win.h - 44);
            render_text("un[E]quipped", unequipped_pos, color_white, font[font_cursive]);
          }
        }

        iv2 drop_pos = v2(item_win.x + 8, item_win.y + item_win.h - 44);
        render_text("[D]rop", drop_pos, color_white, font[font_cursive]);

        iv2 debug_pos = v2(item_win.x + 8, item_win.y + item_win.h - 24);
        render_text("id: %d", debug_pos, color_yellow, font[font_cursive], inventory.slot[i].unique_id);
      }
    }
  }

  inventory.item_count = new_item_count;
}

internal void
render_ui()
{
  SDL_Rect bottom_rect = {0, WINDOW_HEIGHT - 160, 1280, 160};
  SDL_RenderCopy(game.renderer, texture[tex_interface_bottom_win], 0, &bottom_rect);

  iv4 color = color_red;
  SDL_SetRenderDrawColor(game.renderer, color.r, color.g, color.b, color.a);
  SDL_Rect hp_bar_inside = {40, WINDOW_HEIGHT - 132, player.hp * 20, 20};
  SDL_RenderFillRect(game.renderer, &hp_bar_inside);

  SDL_Rect hp_bar = {40, WINDOW_HEIGHT - 132, 200, 20};
  SDL_RenderCopy(game.renderer, texture[tex_health_bar], 0, &hp_bar);

  iv2 name_pos = v2(10, WINDOW_HEIGHT - 152);
  iv2 hp_pos = v2(10, WINDOW_HEIGHT - 130);
  iv2 damage_pos = v2(10, WINDOW_HEIGHT - 110);
  iv2 armor_pos = v2(10, WINDOW_HEIGHT - 92);
  iv2 level_pos = v2(10, WINDOW_HEIGHT - 74);
  iv2 turn_pos = v2(10, WINDOW_HEIGHT - 38);

  render_text(player.name, name_pos, color_white, font[font_classic]);
  render_text("HP         %d/%d", hp_pos, color_white, font[font_classic], player.hp, player.max_hp);
  render_text("Damage: %d", damage_pos, color_white, font[font_classic], player.damage);
  render_text("Armor: %d", armor_pos, color_white, font[font_classic], player.armor);
  render_text("Level: %d", level_pos, color_white, font[font_classic], player.level);
  render_text("Turn: %d", turn_pos, color_white, font[font_classic], game.turn);

  iv2 msg_pos = v2(396, WINDOW_HEIGHT - 152);
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