internal iv2
get_item_offsets_from_item_slot(item_slot slot)
{
  iv2 result = {0};

  // NOTE(rami): Switch?
  if(player.flip == SDL_FLIP_NONE)
  {  
    if(slot == slot_head)
    {
      result.x = -1;
      result.y = -12;
    }
    else if(slot == slot_body)
    {
      result.x = -1;
      result.y = -1;
    }
    else if(slot == slot_legs)
    {
      result.y = 8;
    }
    else if(slot == slot_feet)
    {
      
    }
    else if(slot == slot_first_hand)
    {
      result.x = 3;
      result.y = -3;
    }
    else if(slot == slot_second_hand)
    {
      result.x = 7;
      result.y = 2;
    }
    else if(slot == slot_amulet)
    {
      
    }
    else if(slot == slot_ring)
    {
      result.x = -3;
      result.y = 3;
    }
  }
  else if(player.flip == SDL_FLIP_HORIZONTAL)
  {
    if(slot == slot_head)
    {

    }
    else if(slot == slot_body)
    {
      result.x = 1;
      result.y = -1;
    }
    else if(slot == slot_legs)
    {
      result.y = 8;
    }
    else if(slot == slot_feet)
    {
      
    }
    else if(slot == slot_first_hand)
    {
      result.x = -3;
      result.y = -3;
    }
    else if(slot == slot_second_hand)
    {
      result.x = -7;
      result.y = 2;
    }
    else if(slot == slot_amulet)
    {
      
    }
    else if(slot == slot_ring)
    {
      // result.x = -3;
      // result.y = 3;
    }
  }

  return(result);
}

internal void
render_item()
{
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].id && !item[i].in_inventory)
    {
      SDL_Rect src = {tile_mul(item_info[item[i].id - 1].tile_x),
                      tile_mul(item_info[item[i].id - 1].tile_y),
                      32, 32};

      iv2 pos = get_real_position(item[i].x, item[i].y);
      SDL_Rect dest = {pos.x, pos.y, 32, 32};

      iv2 item_pos = v2(item[i].x, item[i].y);
      if(is_lit(item_pos))
      {
        iv4 color = get_color_for_lighting_value(item_pos);
        SDL_SetTextureColorMod(texture[tex_item_tileset], color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &src, &dest);
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
    for(i32 i = 0; i < ITEM_COUNT; ++i)
    {
      if(item[i].in_inventory)
      {
        if(item[i].unique_id ==
           inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].unique_id)
        {
          item[i].in_inventory = 0;
          item[i].equipped = 0;
          item[i].x = player.x;
          item[i].y = player.y;

          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].id = 0;
          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].unique_id = 0;
          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].x = 0;
          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].y = 0;
          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].in_inventory = 0;
          inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].equipped = 0;

          if(print_drop)
          {
            add_console_message("You drop the %s", color_white,
                                item_info[item[i].id - 1].name);
          }

          --inventory.item_count;
          break;
        }
      }
    }
  }
  else
  {
    add_console_message("You have nothing to drop", color_white);
  }
}

internal void
remove_item(i32 i)
{
  item[i].id = id_none;
  item[i].in_inventory = 0;
  item[i].equipped = 0;
  item[i].x = 0;
  item[i].y = 0;
}

internal void
consume_item()
{
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].in_inventory &&
       item_info[item[i].id - 1].category == category_consumable)
    {
      if(item[i].unique_id ==
         inventory.slot[(inventory.y * INVENTORY_WIDTH) + inventory.x].unique_id)
      {
        if(heal_player(item_info[item[i].id - 1].heal_amount))
        {
          add_console_message("You drink the potion and feel slightly better", color_green);
          drop_item(0);
          remove_item(i);
        }
        else
        {
          add_console_message("You do not feel the need to drink this", color_white);
        }

        break;
      }
    }
  }
}

internal void
add_item_stats(i32 item_info_index)
{
  if(item_info[item_info_index].category == category_weapon)
  {
    player.damage += item_info[item_info_index].damage;
  }
  else if(item_info[item_info_index].category == category_armor)
  {
    player.armor += item_info[item_info_index].armor;
  }
}

internal void
remove_item_stats(i32 item_info_index)
{
  if(item_info[item_info_index].category == category_weapon)
  {
    player.damage -= item_info[item_info_index].damage;
  }
  else if(item_info[item_info_index].category == category_armor)
  {
    player.armor -= item_info[item_info_index].armor;
  }
}

internal return_data_t
get_item_index_from_unique_id(i32 unique_id)
{
  return_data_t data = {0};

  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].unique_id == unique_id)
    {
      data.success = 1;
      data.value = i;
      break;
    }
  }

  return(data);
}

internal void
toggle_equipped_item()
{
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].in_inventory &&
       (item_info[item[i].id - 1].category == category_weapon ||
       item_info[item[i].id - 1].category == category_armor))
    {
      i32 inventory_index = (inventory.y * INVENTORY_WIDTH) + inventory.x;

      if(item[i].unique_id ==
         inventory.slot[inventory_index].unique_id)
      {
        if(item[i].equipped &&
           inventory.slot[inventory_index].equipped)
        {
          item[i].equipped = 0;
          inventory.slot[inventory_index].equipped = 0;

          remove_item_stats(item[i].id - 1);
          add_console_message("You unequip the %s", color_white,
                              item_info[item[i].id - 1].name);
        }
        else
        {
          // If the item slot already has something in it,
          // unequip whatever item is there to make space for the new item
          item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
          if(slot.occupied)
          {
            return_data_t ret = get_item_index_from_unique_id(inventory.slot[slot.index].unique_id);
            item[ret.value].equipped = 0;
            inventory.slot[slot.index].equipped = 0;

            remove_item_stats(inventory.slot[slot.index].id - 1);
          }

          item[i].equipped = 1;
          inventory.slot[inventory_index].equipped = 1;

          add_item_stats(item[i].id - 1);
          add_console_message("You equip the %s", color_white,
                              item_info[item[i].id - 1].name);
        }

        break;
      }
    }
  }
}

internal void
add_item(item_id item_id, i32 x, i32 y)
{
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].id == id_none)
    {
      printf("Item added\n");

      item[i].id = item_id;
      item[i].in_inventory = 0;
      item[i].equipped = 0;
      item[i].x = x;
      item[i].y = y;
      return;
    }
  }

  assert(0, "Item array is full");
}

internal void
pick_up_item()
{
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(!item[i].in_inventory)
    {
      if(v2_equal(v2(item[i].x, item[i].y), v2(player.x, player.y)))
      {
        for(i32 inventory_i = 0; inventory_i < INVENTORY_SLOT_COUNT; ++inventory_i)
        {
          if(!inventory.slot[inventory_i].id)
          {
            item[i].in_inventory = 1;
            inventory.slot[inventory_i] = item[i];
            add_console_message("You pick up the %s", color_white,
                                item_info[item[i].id - 1].name);

            return;
          }
        }

        add_console_message("Your inventory is full right now", color_white);
      }
    }
  }

  add_console_message("You find nothing to pick up", color_white);
}