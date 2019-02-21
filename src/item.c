item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];

void render_inventory()
{
  // render inventory background
  SDL_Rect inv_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_inventory_win], NULL, &inv_rect);

  render_text("Inventory", inv_rect.x + 32, inv_rect.y + 7, RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);

  // item position and the offset
  int32 item_name_x = inv_rect.x + 10;
  int32 item_name_y = inv_rect.y + 30;
  int32 item_name_offset = 25;

  // item window position and the offsets
  int32 item_win_x = inv_rect.x - 256;
  int32 item_win_y = inv_rect.y + inv_rect.h - 300;
  int32 item_win_offset = 10;
  
  // item highlighter position
  int32 inv_hl_x = inv_rect.x;
  int32 inv_hl_y = inv_rect.y + 26;

  // reset item amount
  player->inventory_item_count = 0;

  // render inventory items
  for(int32 i = 0; i < INVENTORY_COUNT; i++)
  {
    if(inventory[i].unique_id != 0)
    {
      // set the current inventory item amount
      player->inventory_item_count++;

      // store this for easier use
      int32 index = inventory[i].item_id;

      // calculate inventory item index
      char item_name_glyph[2] = {97 + i};

      // render certain things if this item is currently selected in the inventory
      if(player->inventory_item_selected == i)
      {
        // render texture for selected item
        SDL_Rect inv_hl_rect = {inv_hl_x + 4, inv_hl_y + (item_name_offset * i), 392, 22};
        SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_inventory_item_selected], NULL, &inv_hl_rect);

        // render item index and name in inventory
        render_text(item_name_glyph, item_name_x, item_name_y + (item_name_offset * i), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);
        render_text(items_info[index].name, item_name_x + 25, item_name_y + (item_name_offset * i), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);

        // render item window
        SDL_Rect inv_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_inventory_item_win], NULL, &inv_item_rect);

        // render item name in the item window
        render_text(items_info[index].name, item_win_x + item_win_offset, item_win_y + item_win_offset, RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);

        // render item attributes depending on the type of the item
        if(items_info[index].item_type == TYPE_CONSUME)
        {
          render_text(items_info[index].use, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), RGBA_COLOR_GREEN_S, global_state.assets.fonts[font_cursive]);
          render_text(items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), RGBA_COLOR_BROWN_S, global_state.assets.fonts[font_cursive]);
          render_text("[C]onsume", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);
          render_text("[D]rop", item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);
        }
        else if(items_info[index].item_type == TYPE_EQUIP)
        {
          render_text("%d Damage", item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), RGBA_COLOR_BLUE_S, global_state.assets.fonts[font_cursive], items_info[index].damage);
          render_text(items_info[index].description, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), RGBA_COLOR_BROWN_S, global_state.assets.fonts[font_cursive]);

          // get the unique id of the item we're currently on in the inventory
          int32 unique_id = inventory[i].unique_id;

          // find the item we're currently on in the inventory
          for(int32 i = 0; i < ITEM_COUNT; i++)
          {
            if(items[i].unique_id == unique_id)
            {
              if(items[i].is_equipped)
              {
                render_text("[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), RGBA_COLOR_YELLOW_S, global_state.assets.fonts[font_cursive]);
                render_text("[D]rop", item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);
              }
              else
              {
                render_text("un[E]quipped", item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);
                render_text("[D]rop", item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_cursive]);
              }

              break;
            }
          }
        }

        // NOTE(Rami): Delete later.
        render_text("%d", item_win_x + item_win_offset, item_win_y + (item_win_offset * 25), RGBA_COLOR_YELLOW_S, global_state.assets.fonts[font_cursive], inventory[i].unique_id);
      }
      else
      {
        // render item index and name in inventory
        render_text(item_name_glyph, item_name_x, item_name_y + (item_name_offset * i), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);
        render_text(items_info[index].name, item_name_x + 25, item_name_y + (item_name_offset * i), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);
      }
    }
  }
}

void render_items()
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // render only items which are on the ground
    if(items[i].is_on_ground)
    {
      SDL_Rect src = {tile_mul(items_info[items[i].item_id].tile), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {items[i].x - global_state.camera.x, items[i].y - global_state.camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_item_tileset], &src, &dst);
    }
  }
}

void drop_or_remove_item(int32 action)
{
  if(!player->inventory_item_count)
  {
    add_console_msg("You find nothing in your inventory to drop", RGBA_COLOR_WHITE_S);
    return;
  }

  // the item we want to drop from the inventory
  item_t *item_to_drop = &inventory[player->inventory_item_selected];

  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the correct item from the items array,
    // its .is_on_ground value needs to be zero
    if(item_to_drop->unique_id == items[i].unique_id &&
      !items[i].is_on_ground)
    {
      if(!action)
      {
        // unequip the item when you drop it
        // set the item to be on the ground
        // set the item position to the player
        items[i].is_equipped = false;
        items[i].is_on_ground = true;
        items[i].x = player->entity->x;
        items[i].y = player->entity->y;

        add_console_msg("You drop the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
        break;
      }
      else
      {
        // remove the item data from inventory
        item_to_drop->item_id = ID_NONE;
        item_to_drop->unique_id = 0;
        item_to_drop->is_on_ground = false;
        item_to_drop->is_equipped = false;
        item_to_drop->x = 0;
        item_to_drop->y = 0;
        break;
      }
    }
  }

  // count holds how many items we have to move item data
  int32 count = INVENTORY_COUNT - player->inventory_item_selected - 1;

  // if count is over the amount of items we have then clamp it
  if(count > player->inventory_item_count)
  {
    count = player->inventory_item_count - player->inventory_item_selected - 1;
  }

  // move the item data according to the value of count
  for(int32 i = 0; i != count; i++)
  {
    inventory[player->inventory_item_selected + i] = inventory[player->inventory_item_selected + i + 1];
  }

  // after moving the last item remove its original data
  inventory[player->inventory_item_selected + count].item_id = ID_NONE;
  inventory[player->inventory_item_selected + count].unique_id = 0;
  inventory[player->inventory_item_selected + count].is_on_ground = false;
  inventory[player->inventory_item_selected + count].is_equipped = false;
  inventory[player->inventory_item_selected + count].x = 0;
  inventory[player->inventory_item_selected + count].y = 0;
}

void consume_item()
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == inventory[player->inventory_item_selected].unique_id)
    {
      // only proceed if the item is consumable
      if(items_info[items[i].item_id].item_type == TYPE_CONSUME)
      {
        // if the player is already at max hp
        if(player->entity->hp >= player->max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          add_console_msg("You do not feeĺ like drinking this right now", RGBA_COLOR_WHITE_S);
          break;
        }

        // increase hp amount depending on the potion
        if(items[i].item_id == ID_LESSER_HEALTH_POTION)
        {
          // apply hp increase
          player->entity->hp += items_info[items[i].item_id].hp_healed;

          // if it goes over the players maximum hp then clamp it
          if(player->entity->hp >= player->max_hp)
          {
            player->entity->hp = player->max_hp;
          }

          add_console_msg("You drink the potion and feel slighty better", RGBA_COLOR_BLUE_S);

          // remove item from inventory
          drop_or_remove_item(1);
          break;
        }
        // NOTE(Rami): add other potion types like MEDIUM_HEATH_POTION, GREATER HEALTH_POTION etc.
        // remember the unique drinking message for all of them
      }
    }
  }
}

void equip_or_unequip_item()
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == inventory[player->inventory_item_selected].unique_id)
    {
      // only proceed if the item is equippable
      if(items_info[items[i].item_id].item_type == TYPE_EQUIP)
      {
        // if it's equipped
        if(items[i].is_equipped)
        {
          // unequip it
          items[i].is_equipped = false;
          add_console_msg("You unequip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
        }
        // if it's unequipped
        else
        {
          // equip it
          items[i].is_equipped = true;
          add_console_msg("You equip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
        }

        break;
      }
    }
  }
}

void add_game_item(item_id_e id, int32 item_x, int32 item_y)
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].item_id == ID_NONE)
    {
      items[i].item_id = id;
      items[i].is_on_ground = true;
      items[i].is_equipped = false;
      items[i].x = item_x;
      items[i].y = item_y;

      debug("Item added\n");
      return;
    }
  }

  // NOTE(Rami): Delete later.
  debug("No free item slots\n");
}

void add_inventory_item()
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    item_t *item = &items[i];

    // item needs to be on the ground to be added to the inventory
    if(!item->is_on_ground)
    {
      continue;
    }

    // item also needs to be in the same position as the player to be added
    if(item->x == player->entity->x && item->y == player->entity->y)
    {
      for(int32 i = 0; i < INVENTORY_COUNT; i++)
      {
        // if the element is not taken
        if(inventory[i].item_id == ID_NONE)
        {
          // make the item not exists since it has been picked up
          item->is_on_ground = false;

          // copy the item data into the inventory
          inventory[i] = *item;
          
          add_console_msg("You pick up the %s", RGBA_COLOR_WHITE_S, items_info[item->item_id].name);
          return;
        }
      }

      add_console_msg("Your inventory is too full right now", RGBA_COLOR_WHITE_S);
      return;
    }
  }
  
  add_console_msg("You find nothing to pick up", RGBA_COLOR_WHITE_S);
}