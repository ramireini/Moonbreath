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
render_weapon_info(iv2 start_pos, i32 info_index)
{
  iv2 name_pos = start_pos;
  iv2 damage_pos = v2(start_pos.x, start_pos.y + 20);
  iv2 description_pos = v2(start_pos.x, damage_pos.y + 20);

  render_text("%s", name_pos, color_white, font[font_clean], item_info[info_index].name);
  render_text("%d Damage", damage_pos, color_white, font[font_cursive],
              item_info[info_index].damage);
  render_text(item_info[info_index].description, description_pos, color_brown, font[font_cursive]);
}

internal void
render_armor_info(SDL_Rect item_win, i32 info_index)
{

}

// internal void
// render_consumable_info(SDL_Rect item_win, i32 info_index)
// {

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

      i32 info_index = inventory.slot[i].id - 1;

      // Render item
      SDL_Rect src = {tile_mul(item_info[info_index].tile - 1), 0, 32, 32};
      SDL_Rect dest = {first_slot_x + tile_mul(i) + (i * padding), first_slot_y, 32, 32};
      SDL_SetTextureColorMod(texture[tex_item_tileset], 255, 255, 255);
      SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &src, &dest);

      // NOTE(rami): Need to add the items we equip to their own slots in the inventory

      if(i == (inventory.y * INVENTORY_HEIGHT) + inventory.x)
      {
        // Render item window
        SDL_Rect item_win = {inventory_win.x, inventory_win.y, 250, 307};
        item_win.x -= (item_win.w + padding);
        SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_win);

        // Render item info
        iv2 start_pos = v2(item_win.x + 12, item_win.y + 12);

        iv2 name_pos = start_pos;
        render_text("%s", name_pos, color_white, font[font_clean], item_info[info_index].name);

        if(item_info[info_index].category == category_weapon)
        {
          iv2 damage_pos = v2(start_pos.x, start_pos.y + 20);
          iv2 description_pos = v2(start_pos.x, start_pos.y + 40);

          render_text("%d Damage", damage_pos, color_white, font[font_clean],
                      item_info[info_index].damage);
          render_text(item_info[info_index].description, description_pos, color_brown,
                      font[font_cursive]);

          if(inventory.slot[i].equipped)
          {
            iv2 unequip_pos = v2(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_clean]);
          }
          else
          {
            iv2 equip_pos = v2(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_clean]);
          }
        }
        else if(item_info[info_index].category == category_armor)
        {
          iv2 armor_pos = v2(start_pos.x, start_pos.y + 20);
          iv2 description_pos = v2(start_pos.x, start_pos.y + 40);

          render_text("%d Armor", armor_pos, color_white, font[font_clean],
                      item_info[info_index].armor);
          render_text(item_info[info_index].description, description_pos, color_brown,
                      font[font_cursive]);

          if(inventory.slot[i].equipped)
          {
            iv2 unequip_pos = v2(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_clean]);
          }
          else
          {
            iv2 equip_pos = v2(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_clean]);
          }
        }
        else if(item_info[info_index].category == category_consumable)
        {
          iv2 use_pos = v2(start_pos.x, start_pos.y + 20);
          iv2 description_pos = v2(start_pos.x, start_pos.y + 40);
          iv2 consume_pos = v2(start_pos.x, start_pos.y + 250);

          render_text(item_info[info_index].use, use_pos, color_green, font[font_clean]);
          render_text(item_info[info_index].description, description_pos, color_brown,
                      font[font_cursive]);
          render_text("[C]onsume", consume_pos, color_white, font[font_clean]);
        }

        iv2 drop_pos = v2(start_pos.x, start_pos.y + 270);
        render_text("[D]rop", drop_pos, color_white, font[font_clean]);

      #if MOONBREATH_DEBUG
        iv2 debug_pos = v2(start_pos.x, start_pos.y + 230);
        render_text("ID: %d", debug_pos, color_orange, font[font_clean],
                    inventory.slot[i].unique_id);
      #endif
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

  // Normalize current health value (range of 0 - 1),
  // multiply by hp bar width to get a value between 0 and hp bar width
  // with the same ratio as the original value
  i32 hp_bar_inside_w = 0;
  if(player.hp > 0)
  {
    hp_bar_inside_w = ((r32)player.hp / (r32)player.max_hp) * 200.0f;
  }

  SDL_Rect hp_bar_outside = {38, WINDOW_HEIGHT - 134, 204, 24};
  SDL_RenderCopy(game.renderer, texture[tex_health_bar_outside], 0, &hp_bar_outside);

  SDL_Rect hp_bar_inside_src = {0, 0, hp_bar_inside_w, 20};
  SDL_Rect hp_bar_inside_dest = {40, WINDOW_HEIGHT - 132, hp_bar_inside_w, 20};
  SDL_RenderCopy(game.renderer, texture[tex_health_bar_inside], &hp_bar_inside_src, &hp_bar_inside_dest);

  iv2 name_pos = v2(10, WINDOW_HEIGHT - 152);
  iv2 hp_pos = v2(10, WINDOW_HEIGHT - 128);
  iv2 hp_pos_actual = v2(115, WINDOW_HEIGHT - 128);
  iv2 damage_pos = v2(10, WINDOW_HEIGHT - 100);
  iv2 armor_pos = v2(10, WINDOW_HEIGHT - 82);
  iv2 level_pos = v2(10, WINDOW_HEIGHT - 64);
  iv2 turn_pos = v2(10, WINDOW_HEIGHT - 26);

  render_text(player.name, name_pos, color_white, font[font_clean]);
  render_text("HP", hp_pos, color_white, font[font_clean], player.hp, player.max_hp);
  render_text("%d (%d)", hp_pos_actual, color_white, font[font_clean], player.hp, player.max_hp);
  render_text("Damage: %d", damage_pos, color_white, font[font_clean], player.damage);
  render_text("Armor: %d", armor_pos, color_white, font[font_clean], player.armor);
  render_text("Level: %d", level_pos, color_white, font[font_clean], player.level);
  render_text("Turn: %d", turn_pos, color_white, font[font_clean], game.turn);

  iv2 msg_pos = v2(396, WINDOW_HEIGHT - 152);
  i32 msg_offset = 16;

  for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
  {
    if(!str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
    {
      render_text(console_message[i].msg, msg_pos, console_message[i].color, font[font_clean]);
      msg_pos.y += msg_offset;
    }
  }
}