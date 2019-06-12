internal void
render_items()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(item[i].id && !item[i].in_inventory)
    {
      SDL_Rect src = {tile_mul(item_info[item[i].id - 1].tile), 0,
                      TILE_SIZE, TILE_SIZE};
      SDL_Rect dest = {tile_mul(item[i].x) - game.camera.x,
                       tile_mul(item[i].y) - game.camera.y,
                      TILE_SIZE, TILE_SIZE};

      iv2 item_pos = v2(item[i].x, item[i].y);
      if(is_lit(item_pos))
      {
        iv4 color = get_color_for_lighting_value(item_pos);
        SDL_SetTextureColorMod(assets.texture[tex_item_tileset],
                               color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, assets.texture[tex_item_tileset], &src, &dest);
      }
    }
  }
}

// NOTE(rami): Do we want to pick dropped items in the reverse order?
// Or do we want to give a list of items on that spot so you can choose?
// Or something else?
internal void
drop_item(b32 print_drop)
{
  if(inventory.item_count)
  {
    for(i32 i = 0; i < ITEM_COUNT; i++)
    {
      if(item[i].in_inventory)
      {
        if(item[i].unique_id ==
           inventory.slots[inventory.item_selected - 1].unique_id)
        {
          item[i].in_inventory = false;
          item[i].is_equipped = false;
          item[i].x = player.x;
          item[i].y = player.y;

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
                                item_info[item[i].id - 1].name);
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
remove_item(i32 i)
{
  item[i].id = id_none;
  item[i].in_inventory = false;
  item[i].is_equipped = false;
  item[i].x = 0;
  item[i].y = 0;
}

internal void
consume_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(item[i].in_inventory &&
       item_info[item[i].id - 1].type == type_consume)
    {
      if(item[i].unique_id ==
         inventory.slots[inventory.item_selected - 1].unique_id)
      {
        if(heal_player(item_info[item[i].id - 1].hp_healed))
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
    if(item[i].in_inventory &&
       item_info[item[i].id - 1].type == type_equip)
    {
      if(item[i].unique_id ==
         inventory.slots[inventory.item_selected - 1].unique_id)
      {
        if(item[i].is_equipped &&
           inventory.slots[inventory.item_selected - 1].is_equipped)
        {
          item[i].is_equipped = false;
          inventory.slots[inventory.item_selected - 1].is_equipped = false;
          add_console_message("You unequip the %s", RGBA_COLOR_WHITE_S,
                              item_info[item[i].id - 1].name);
        }
        else
        {
          item[i].is_equipped = true;
          inventory.slots[inventory.item_selected - 1].is_equipped = true;
          add_console_message("You equip the %s", RGBA_COLOR_WHITE_S,
                              item_info[item[i].id - 1].name);
        }

        break;
      }
    }
  }
}

internal void
add_item(item_id item_id, iv2 pos)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(item[i].id == id_none)
    {
      debug("Item added\n");

      item[i].id = item_id;
      item[i].in_inventory = false;
      item[i].is_equipped = false;
      item[i].x = pos.x;
      item[i].y = pos.y;
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
    if(!item[i].in_inventory)
    {
      if(v2_equal(v2(item[i].x, item[i].y), v2(player.x, player.y)))
      {
        for(i32 inventory_i = 0; inventory_i < INVENTORY_SLOT_COUNT; inventory_i++)
        {
          if(!inventory.slots[inventory_i].id)
          {
            item[i].in_inventory = true;
            inventory.slots[inventory_i] = item[i];
            add_console_message("You pickup the %s", RGBA_COLOR_WHITE_S,
                                item_info[item[i].id - 1].name);

            return;
          }
        }

        add_console_message("Your inventory is full right now", RGBA_COLOR_WHITE_S);
      }
    }
  }

  add_console_message("You find nothing to pick up", RGBA_COLOR_WHITE_S);
}