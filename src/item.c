void item_render()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].is_on_ground && !items[i].is_equipped)
    {
      SDL_Rect src = {tile_mul(items_info[items[i].item_id].tile), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(items[i].pos.x) - game_state.camera.x, tile_mul(items[i].pos.y) - game_state.camera.y, TILE_SIZE, TILE_SIZE};

      SDL_RenderCopy(game_state.renderer, assets.textures[tex_item_tileset], &src, &dst);
    }
  }
}

// NOTE(Rami): When you drop multiple items they need to be picked up in the reverse order.
void item_drop()
{
  if(!player.inventory.item_count)
  {
    console_message_add("You find nothing in your inventory to drop", RGBA_COLOR_WHITE_S);
    return;
  }

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the correct item from the items array,
    // its .is_on_ground value needs to be zero
    if(player.inventory.slots[player.inventory.item_selected].unique_id == items[i].unique_id && !items[i].is_on_ground)
    {
      // unequip the item when you drop it
      // set the item to be on the ground
      // set the item position to the player
      items[i].is_equipped = false;
      items[i].is_on_ground = true;
      items[i].pos = player.entity.pos;

      console_message_add("You drop the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
      break;
    }
  }

  // count holds how many items we have to move item data
  i32 count = INVENTORY_SLOT_COUNT - player.inventory.item_selected - 1;

  // if count is over the amount of items we have then clamp it
  if(count > player.inventory.item_count)
  {
    count = player.inventory.item_count - player.inventory.item_selected - 1;
  }

  // move the item data according to the value of count
  for(i32 i = 0; i != count; i++)
  {
    player.inventory.slots[player.inventory.item_selected + i] = player.inventory.slots[player.inventory.item_selected + i + 1];
  }

  // after moving the last item remove the data from the slot you moved it from
  player.inventory.slots[player.inventory.item_selected + count].item_id = id_none;
  player.inventory.slots[player.inventory.item_selected + count].unique_id = 0;
  player.inventory.slots[player.inventory.item_selected + count].is_on_ground = false;
  player.inventory.slots[player.inventory.item_selected + count].is_equipped = false;
  player.inventory.slots[player.inventory.item_selected + count].pos.x = 0;
  player.inventory.slots[player.inventory.item_selected + count].pos.y = 0;
}

void item_remove()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the correct item from the items array,
    // its .is_on_ground value needs to be zero
    if(player.inventory.slots[player.inventory.item_selected].unique_id == items[i].unique_id && !items[i].is_on_ground)
    {
      // remove the item data from inventory
      player.inventory.slots[player.inventory.item_selected].item_id = id_none;
      player.inventory.slots[player.inventory.item_selected].unique_id = 0;
      player.inventory.slots[player.inventory.item_selected].is_on_ground = false;
      player.inventory.slots[player.inventory.item_selected].is_equipped = false;
      player.inventory.slots[player.inventory.item_selected].pos.x = 0;
      player.inventory.slots[player.inventory.item_selected].pos.y = 0;
      break;
    }
  }

  // count holds how many items we have to move item data
  i32 count = INVENTORY_SLOT_COUNT - player.inventory.item_selected - 1;

  // if count is over the amount of items we have then clamp it
  if(count > player.inventory.item_count)
  {
    count = player.inventory.item_count - player.inventory.item_selected - 1;
  }

  // move the item data according to the value of count
  for(i32 i = 0; i != count; i++)
  {
    player.inventory.slots[player.inventory.item_selected + i] = player.inventory.slots[player.inventory.item_selected + i + 1];
  }

  // after moving the last item remove the data from the slot you moved it from
  player.inventory.slots[player.inventory.item_selected + count].item_id = id_none;
  player.inventory.slots[player.inventory.item_selected + count].unique_id = 0;
  player.inventory.slots[player.inventory.item_selected + count].is_on_ground = false;
  player.inventory.slots[player.inventory.item_selected + count].is_equipped = false;
  player.inventory.slots[player.inventory.item_selected + count].pos.x = 0;
  player.inventory.slots[player.inventory.item_selected + count].pos.y = 0;
}

void item_consume()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == player.inventory.slots[player.inventory.item_selected].unique_id)
    {
      // only proceed if the item is consumable
      if(items_info[items[i].item_id].item_type == type_consume)
      {
        // if the player is already at max hp
        if(player.entity.hp >= player.max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          console_message_add("You do not feel like drinking this right now", RGBA_COLOR_WHITE_S);
          break;
        }

        // increase hp amount depending on the potion
        if(items[i].item_id == id_lesser_health_potion)
        {
          // apply hp increase
          player.entity.hp += items_info[items[i].item_id].hp_healed;

          // if it goes over the players maximum hp then clamp it
          if(player.entity.hp >= player.max_hp)
          {
            player.entity.hp = player.max_hp;
          }

          console_message_add("You drink the potion and feel slighty better", RGBA_COLOR_BLUE_S);

          item_remove();
          break;
        }
        // NOTE(Rami): add other potion types like MEDIUM_HEATH_POTION, GREATER HEALTH_POTION etc.
        // remember the unique drinking message for all of them
      }
    }
  }
}

void item_toggle_equip()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == player.inventory.slots[player.inventory.item_selected].unique_id)
    {
      // only proceed if the item is equippable
      if(items_info[items[i].item_id].item_type == type_equip)
      {
        // if equipped
        if(items[i].is_equipped)
        {
          // unequip it
          items[i].is_equipped = false;
          console_message_add("You unequip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
        }
        // if unequipped
        else
        {
          // equip it
          items[i].is_equipped = true;
          console_message_add("You equip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id].name);
        }
        break;
      }
    }
  }
}

void item_add_game(item_id_e id, iv2_t pos)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].item_id == id_none)
    {
      items[i] = (item_t){id, items[i].unique_id, true, false, pos};
      debug("Item added");
      return;
    }
  }
  debug("No free item slots");
}

void item_pickup()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].is_on_ground)
    {
      if(iv2_equal(items[i].pos, player.entity.pos))
      {
        for(i32 j = 0; j < INVENTORY_SLOT_COUNT; j++)
        {
          if(player.inventory.slots[j].item_id == id_none)
          {
            items[i].is_on_ground = false;
            player.inventory.slots[j] = items[i];
            return;
          }
        }
        console_message_add("Your inventory is full right now", RGBA_COLOR_WHITE_S);   
      }
    }
  }
  console_message_add("You find nothing to pick up", RGBA_COLOR_WHITE_S);
}