#include <item.h>

void consume_item(player_t *player)
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == inventory[player->inventory_item_selected].unique_id)
    {
      // only proceed if the item is consumable
      if(items_info[items[i].item_id - 1].item_type == TYPE_CONSUME)
      {
        // if the player is already at max hp
        if(player->entity->hp >= player->max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          add_console_msg("You do not feeĺ like drinking this right now", TEXT_COLOR_WHITE);
          break;
        }

        // increase hp amount depending on the potion
        if(items[i].item_id == ID_LESSER_HEALTH_POTION)
        {
          // apply hp increase
          player->entity->hp += items_info[items[i].item_id - 1].hp_healed;

          // if it goes over the players maximum hp then clamp it
          if(player->entity->hp >= player->max_hp)
          {
            player->entity->hp = player->max_hp;
          }

          add_console_msg("You drink the potion and feel slighty better", TEXT_COLOR_BLUE);

          // remove item from inventory
          drop_or_remove_inventory_item(player, 0);

          // remove the item data
          items[i].item_id = ID_NONE;
          items[i].unique_id = 0;
          items[i].is_on_ground = false;
          items[i].is_equipped = false;
          items[i].x = 0;
          items[i].y = 0; 
          break;
        }
        // NOTE(Rami): add other potion types like MEDIUM_HEATH_POTION, GREATER HEALTH_POTION etc.
        // remember the unique drinking message for all of them
      }
    }
  }
}

void equip_or_unequip_item(player_t *player)
{
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == inventory[player->inventory_item_selected].unique_id)
    {
      // only proceed if the item is equippable
      if(items_info[items[i].item_id - 1].item_type == TYPE_EQUIP)
      {
        // if it's equipped
        if(items[i].is_equipped)
        {
          // unequip it
          items[i].is_equipped = false;
          add_console_msg("You unequip the %s", TEXT_COLOR_WHITE, items_info[items[i].item_id - 1].name);
        }
        // if it's unequipped
        else
        {
          // equip it
          items[i].is_equipped = true;
          add_console_msg("You equip the %s", TEXT_COLOR_WHITE, items_info[items[i].item_id - 1].name);
        }

        break;
      }
    }
  }
}

void drop_or_remove_inventory_item(player_t *player, int32 drop)
{
  if(!player->inventory_item_count)
  {
    add_console_msg("You find nothing in your inventory to drop", TEXT_COLOR_WHITE);
    return;
  }

  // the item we want to drop from the inventory
  item_t *item_to_drop = &inventory[player->inventory_item_selected];

  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the correct item from the items array,
    // its .is_on_ground value needs to be zero
    if(item_to_drop->unique_id == items[i].unique_id && !items[i].is_on_ground)
    {
      if(drop)
      {
        // unequip the item when you drop it
        // set the item to be on the ground
        // set the item position to the player
        items[i].is_equipped = false;
        items[i].is_on_ground = true;
        items[i].x = player->entity->x;
        items[i].y = player->entity->y;

        add_console_msg("You drop the %s", TEXT_COLOR_WHITE, items_info[items[i].item_id - 1].name);
      }

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

      printf("Item added\n");
      return;
    }
  }

  printf("No free item slots\n");
}

void add_inventory_item(player_t *player)
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
          // copy the item data into the inventory
          inventory[i] = *item;

          // make the item not exists since it has been picked up
          item->is_on_ground = 0;
          add_console_msg("You pick up the %s", TEXT_COLOR_WHITE, items_info[item->item_id - 1].name);
          return;
        }
      }

      add_console_msg("Your inventory is too full right now", TEXT_COLOR_WHITE);
      return;
    }
  }
  
  add_console_msg("You find nothing nearby to pick up", TEXT_COLOR_WHITE);
}