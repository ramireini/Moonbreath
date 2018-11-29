#include <item.h>

void consume_item(player_t *player)
{
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(game_items[i].unique_id == inventory[player->inventory_hl_index].unique_id)
    {
      // only proceed if the item is consumable
      if(game_items_info[game_items[i].item_id - 1].item_type == TYPE_CONSUME)
      {
        // if the player is already at max hp
        if(player->hp >= player->max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          update_add_console_msg("You do not feeĺ like drinking this right now", TEXT_COLOR_WHITE);

          break;
        }

        // increase hp amount depending on the potion
        if(game_items[i].item_id == ID_LESSER_HEALTH_POTION)
        {
          // apply hp increase
          player->hp += game_items_info[game_items[i].item_id - 1].hp_healed;

          // if it goes over the players maximum hp then clamp it
          if(player->hp >= player->max_hp)
          {
            player->hp = player->max_hp;
          }

          update_add_console_msg("You drink the potion and feel slighty better", TEXT_COLOR_BLUE);

          // remove item from inventory
          drop_or_remove_inventory_item(player, 0);

          // remove the item data
          game_items[i].item_id = ID_NONE;
          game_items[i].unique_id = 0;
          game_items[i].is_on_ground = 0;
          game_items[i].is_equipped = 0;
          game_items[i].x = 0;
          game_items[i].y = 0;

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
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(game_items[i].unique_id == inventory[player->inventory_hl_index].unique_id)
    {
      // only proceed if the item is equippable
      if(game_items_info[game_items[i].item_id - 1].item_type == TYPE_EQUIP)
      {
        // if it's equipped
        if(game_items[i].is_equipped)
        {
          // unequip it
          game_items[i].is_equipped = 0;
          update_add_console_msg("You unequip the %s", TEXT_COLOR_WHITE, game_items_info[game_items[i].item_id - 1].name);
        }
        // if it's unequipped
        else
        {
          // equip it
          game_items[i].is_equipped = 1;
          update_add_console_msg("You equip the %s", TEXT_COLOR_WHITE, game_items_info[game_items[i].item_id - 1].name);
        }

        break;
      }
    }
  }
}

void drop_or_remove_inventory_item(player_t *player, int drop)
{
  if(!player->inventory_item_count)
  {
    update_add_console_msg("You find nothing in your inventory to drop", TEXT_COLOR_WHITE);
    return;
  }

  // the item we want to drop from the inventory
  item_t *item_to_drop = &inventory[player->inventory_hl_index];

  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the correct item from the game_items array,
    // its .is_on_ground value needs to be zero
    if(item_to_drop->unique_id == game_items[i].unique_id && !game_items[i].is_on_ground)
    {
      if(drop)
      {
        // unequip the item when you drop it
        // set the item to be on the ground
        // set the item position to the player
        game_items[i].is_equipped = 0;
        game_items[i].is_on_ground = 1;
        game_items[i].x = player->x;
        game_items[i].y = player->y;

        update_add_console_msg("You drop the %s", TEXT_COLOR_WHITE, game_items_info[game_items[i].item_id - 1].name);
      }

      // remove the item data from inventory
      item_to_drop->item_id = ID_NONE;
      item_to_drop->unique_id = 0;
      item_to_drop->is_on_ground = 0;
      item_to_drop->is_equipped = 0;
      item_to_drop->x = 0;
      item_to_drop->y = 0;
      
      break;
    }
  }

  // count holds how many items we have to move item data
  int count = INVENTORY_COUNT - player->inventory_hl_index - 1;

  // if count is over the amount of items we have then clamp it
  if(count > player->inventory_item_count)
  {
    count = player->inventory_item_count - player->inventory_hl_index - 1;
  }

  // move the item data according to the value of count
  for(int i = 0; i != count; i++)
  {
    inventory[player->inventory_hl_index + i] = inventory[player->inventory_hl_index + i + 1];
  }

  // after moving the last item remove its original data
  inventory[player->inventory_hl_index + count].item_id = ID_NONE;
  inventory[player->inventory_hl_index + count].unique_id = 0;
  inventory[player->inventory_hl_index + count].is_on_ground = 0;
  inventory[player->inventory_hl_index + count].is_equipped = 0;
  inventory[player->inventory_hl_index + count].x = 0;
  inventory[player->inventory_hl_index + count].y = 0;
}

void add_game_item(item_id_e id, int item_x, int item_y)
{
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    if(game_items[i].item_id == ID_NONE)
    {
      game_items[i].item_id = id;
      game_items[i].is_on_ground = 1;
      game_items[i].is_equipped = 0;
      game_items[i].x = item_x;
      game_items[i].y = item_y;

      printf("Item added\n");

      return;
    }
  }

  printf("No free item slots\n");
}

void add_inventory_item(player_t *player)
{
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    item_t *item = &game_items[i];

    // item needs to be on the ground to be added to the inventory
    if(!item->is_on_ground)
    {
      continue;
    }

    // item also needs to be in the same position as the player to be added
    if(item->x == player->x && item->y == player->y)
    {
      for(int i = 0; i < INVENTORY_COUNT; i++)
      {
        // if the element is not taken
        if(inventory[i].item_id == ID_NONE)
        {
          // copy the item data into the inventory
          inventory[i] = *item;

          // make the item not exists since it has been picked up
          item->is_on_ground = 0;
          update_add_console_msg("You pick up the %s", TEXT_COLOR_WHITE, game_items_info[item->item_id - 1].name);
          return;
        }
      }

      update_add_console_msg("Your inventory is too full right now", TEXT_COLOR_WHITE);
      return;
    }
  }
  update_add_console_msg("You find nothing nearby to pick up", TEXT_COLOR_WHITE);
}