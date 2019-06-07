internal void
render_items()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id && !items[i].in_inventory)
    {
      SDL_Rect src = {tile_mul(items_info[items[i].id - 1].tile), 0,
                      TILE_SIZE, TILE_SIZE};
      SDL_Rect dest = {tile_mul(items[i].x) - game.camera.x,
                       tile_mul(items[i].y) - game.camera.y,
                      TILE_SIZE, TILE_SIZE};

      v2_t item_pos = v2(items[i].x, items[i].y);
      if(is_lit(item_pos))
      {
        v4_t color = get_color_for_lighting_value(item_pos);
        SDL_SetTextureColorMod(assets.textures[item_tileset_tex],
                               color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, assets.textures[item_tileset_tex], &src, &dest);
      }
    }
  }
}

// NOTE(Rami): Do we want to pick dropped items in the reverse order?
// Or do we want to give a list of items on that spot so you can choose?
// Or something else?
internal void
drop_item(b32 print_drop)
{
  if(inventory.item_count)
  {
    for(i32 i = 0; i < ITEM_COUNT; i++)
    {
      if(items[i].in_inventory)
      {
        if(items[i].unique_id ==
           inventory.slots[inventory.item_selected - 1].unique_id)
        {
          items[i].in_inventory = false;
          items[i].is_equipped = false;
          items[i].x = player.x;
          items[i].y = player.y;

          inventory.slots[inventory.item_selected - 1] =
          (item_t){id_none, 0, false, false, 0, 0};

          for(i32 i = 1; i < INVENTORY_SLOT_COUNT; i++)
          {
            if(inventory.slots[i].id &&
              !inventory.slots[i - 1].id)
            {
              inventory.slots[i - 1] = inventory.slots[i];
              inventory.slots[i] = (item_t){id_none, 0, false, false, 0, 0};
            }
          }

          inventory.item_count--;
          if(inventory.item_selected > inventory.item_count)
          {
            inventory.item_selected--;
          }

          if(print_drop)
          {
            add_console_message("You drop the %s", RGBA_COLOR_WHITE_S,
                                items_info[items[i].id - 1].name);
          }

          break;
        }
      }
    }
  }
  else
  {
    add_console_message("You have nothing to drop", RGBA_COLOR_WHITE_S);
  }
}

internal void
remove_item(i32 item_index)
{
  items[item_index].id = id_none;
  items[item_index].in_inventory = false;
  items[item_index].is_equipped = false;
  items[item_index].x = 0;
  items[item_index].y = 0;
}

internal void
consume_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].in_inventory &&
       items_info[items[i].id - 1].type == type_consume)
    {
      if(items[i].unique_id ==
         inventory.slots[inventory.item_selected - 1].unique_id)
      {
        if(heal_player(items_info[items[i].id - 1].hp_healed))
        {
          add_console_message("You drink the potion and feel slightly better", RGBA_COLOR_GREEN_S);
          drop_item(0);
          remove_item(i);
        }
        else
        {
          add_console_message("You do not feel the need to drink this", RGBA_COLOR_WHITE_S);
        }

        break;
      }
    }
  }
}

internal void
toggle_equipped_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].in_inventory &&
       items_info[items[i].id - 1].type == type_equip)
    {
      if(items[i].unique_id ==
         inventory.slots[inventory.item_selected - 1].unique_id)
      {
        if(items[i].is_equipped &&
           inventory.slots[inventory.item_selected - 1].is_equipped)
        {
          items[i].is_equipped = false;
          inventory.slots[inventory.item_selected - 1].is_equipped = false;
          add_console_message("You unequip the %s", RGBA_COLOR_WHITE_S,
                              items_info[items[i].id - 1].name);
        }
        else
        {
          items[i].is_equipped = true;
          inventory.slots[inventory.item_selected - 1].is_equipped = true;
          add_console_message("You equip the %s", RGBA_COLOR_WHITE_S,
                              items_info[items[i].id - 1].name);
        }

        break;
      }
    }
  }
}

internal void
add_item(item_id item_id, v2_t pos)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id == id_none)
    {
      debug("Item added\n");

      items[i].id = item_id;
      items[i].in_inventory = false;
      items[i].is_equipped = false;
      items[i].x = pos.x;
      items[i].y = pos.y;
      return;
    }
  }

  debug("No free item slots\n");
}

internal void
pickup_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(!items[i].in_inventory)
    {
      if(v2_equal(v2(items[i].x, items[i].y), v2(player.x, player.y)))
      {
        for(i32 inventory_i = 0; inventory_i < INVENTORY_SLOT_COUNT; inventory_i++)
        {
          if(!inventory.slots[inventory_i].id)
          {
            items[i].in_inventory = true;
            inventory.slots[inventory_i] = items[i];
            add_console_message("You pickup the %s", RGBA_COLOR_WHITE_S,
                                items_info[items[i].id - 1].name);

            return;
          }
        }

        add_console_message("Your inventory is full right now", RGBA_COLOR_WHITE_S);
      }
    }
  }

  add_console_message("You find nothing to pick up", RGBA_COLOR_WHITE_S);
}