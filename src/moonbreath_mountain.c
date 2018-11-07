#include <moonbreath_mountain.h>
#include <util_conf.h>

void consume_item(player_t *player, int *inv_hl_index, int *inv_item_count)
{
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(game_items[i].unique_id == inventory[*inv_hl_index].unique_id)
    {
      // only proceed if the item is consumable
      if(game_items_info[game_items[i].item_id - 1].item_type == TYPE_CONSUME)
      {
        // if the player is already at max hp
        if(player->hp >= player->max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          add_console_msg("You do not feel injured right now", TEXT_COLOR_WHITE);

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

          add_console_msg("You drink the potion and feel slighty better than before", TEXT_COLOR_BLUE);

          // drop item will remove the items inventory data and organize the inventory
          drop_item(player, inv_hl_index, inv_item_count);

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

void equip_or_unequip_item(int *inv_hl_index)
{
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(game_items[i].unique_id == inventory[*inv_hl_index].unique_id)
    {
      // only proceed if the item is equippable
      if(game_items_info[game_items[i].item_id - 1].item_type == TYPE_EQUIP)
      {
        // if it's equipped
        if(game_items[i].is_equipped)
        {
          // unequip it
          game_items[i].is_equipped = 0;

          char unequip[52];
          sprintf(unequip, "You unequip the %s", game_items_info[game_items[i].item_id - 1].name);
          add_console_msg(unequip, TEXT_COLOR_WHITE);
        }
        // if it's unequipped
        else
        {
          // equip it
          game_items[i].is_equipped = 1;

          char equip[52];
          sprintf(equip, "You equip the %s", game_items_info[game_items[i].item_id - 1].name);
          add_console_msg(equip, TEXT_COLOR_WHITE);
        }

        break;
      }
    }
  }
}

void render_text(font_t *font_struct, int x, int y, char *str, int text_color)
{
  // start at the beginning of the string
  char *current_char = str;

  // store the starting x of the text for wrapping
  int initial_x = x;

  // store how many characters we have
  int char_count = 0;

  // set to 1 if we want to wrap text, set to 0 if we don't want to wrap text
  int force_wrapping = 0;

  // while valid chars
  while(*current_char != '\0')
  {
    int array_index = *current_char - 38;

    // if newline
    if(*current_char == '\\' && *(current_char + 1) == 'n')
    {
      force_wrapping = 1;

      // move to next char in text
      current_char += 2;
      
      continue;
    }
    // if space
    else if(*current_char == ' ')
    {
      // increment the amount of characters
      char_count++;

      // move the position of the text
      x += 5;

      // move onto the next byte in the text
      current_char++;

      continue;
    }

    if(force_wrapping)
    {
      // move the position of the text to the original x
      x = initial_x;

      // move the position of the text to the next line
      y += 16;

      // reset the character amount
      char_count = 0;

      force_wrapping = 0;
    }

    // fetch the glyph metrics for the current character in the text
    font_metrics_t *glyph_metrics = &font_struct->metrics[array_index];

    // the source rectangle to take from the glyph atlas
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};

    // the destination rectangle where to render our glyph
    SDL_Rect dst = {x, y, glyph_metrics->w, glyph_metrics->h};

    // apply color
    SDL_Color color = hex_to_rgba_color(text_color);
    SDL_SetTextureColorMod(font_struct->atlas, color.r, color.g, color.b);

    // render the text
    SDL_RenderCopy(renderer, font_struct->atlas, &src, &dst);

    // increment the amount of characters
    char_count++;

    // move the position of the text
    x += glyph_metrics->advance;

    // move onto the next byte in the text
    current_char++;
  }
}

font_t* create_font_atlas(TTF_Font *font)
{
  // a texture to hold all the glyphs
  SDL_Texture *glyph_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 688, 16);

  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;
  
  // this will hold the atlas and the glyph metrics
  // we will return a pointer to this struct
  font_t *font_struct = malloc(sizeof(font_t));
  font_struct->atlas = NULL;

  // enable alpha blending to remove the black background
  SDL_SetTextureBlendMode(glyph_atlas, SDL_BLENDMODE_BLEND);

  // glyph positions on the glyph atlas
  int x = 0;
  int y = 0;

  for(int i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // store the current character
    char ch = i + 38;

    // render the glyph to a surface
    SDL_Color color = {255, 255, 255, 255};
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(renderer, glyph_surf);

    // set the glyph atlas as the render target
    SDL_SetRenderTarget(renderer, glyph_atlas);

    // get the advance value of the glyph
    int advance;

    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // set where on the atlas we render
    SDL_Rect atlas_rect = {x, y, glyph_surf->w, glyph_surf->h};

    // store the glyph metrics
    font_struct->metrics[i] = (font_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

    // advance the rendering location
    x += glyph_surf->w;

    // render the glyph on the atlas
    SDL_RenderCopy(renderer, glyph_tex, NULL, &atlas_rect);

    // free the glyph surface and texture
    SDL_FreeSurface(glyph_surf);
    glyph_surf = NULL;

    SDL_DestroyTexture(glyph_tex);
    glyph_tex = NULL;
  }

  // unset the render target
  SDL_SetRenderTarget(renderer, NULL);

  // set the struct pointer to the completed atlas
  font_struct->atlas = glyph_atlas;

  return font_struct;
}

SDL_Color hex_to_rgba_color(int hex_color)
{
  // shift and mask the rgb out of the hex color
  int r = (hex_color >> 24) & 0xFF;
  int g = (hex_color >> 16) & 0xFF;
  int b = (hex_color >> 8) & 0xFF;
  int a = hex_color & 0xFF;

  SDL_Color rgb_color = {r, g, b, a};

  return rgb_color;
}

void render_inventory(SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, font_t *font_inv, font_t *font_item, int *inv_hl_index, int *inv_item_count)
{
  // render inventory background
  SDL_Rect inv_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(renderer, inv_tex, NULL, &inv_rect);

  render_text(font_inv, inv_rect.x + 34, inv_rect.y + 5, "Inventory", TEXT_COLOR_WHITE);

  // item position and the offset
  int item_name_x = inv_rect.x + 10;
  int item_name_y = inv_rect.y + 30;
  int item_name_offset = 25;

  // item window position and the offsets
  int item_win_x = inv_rect.x - 256;
  int item_win_y = inv_rect.y + inv_rect.h - 300;
  int item_win_offset = 10;
  
  // item highlighter position
  int inv_hl_x = inv_rect.x;
  int inv_hl_y = inv_rect.y + 26;

  // reset item amount
  *inv_item_count = 0;

  // render inventory items
  for(int i = 0; i < INVENTORY_COUNT; i++)
  {
    if(inventory[i].unique_id)
    {
      // set the current inventory item amount
      (*inv_item_count)++;

      // store this for easier use
      int index = inventory[i].item_id - 1;

      // calculate inventory item index
      char item_name_glyph[] = {97 + i, '\0'};

      // render item index and name in inventory
      render_text(font_inv, item_name_x, item_name_y + (item_name_offset * i), item_name_glyph, TEXT_COLOR_WHITE);
      render_text(font_inv, item_name_x + 25, item_name_y + (item_name_offset * i), game_items_info[index].name, TEXT_COLOR_WHITE);

      // render certain things if this item is currently selected in the inventory
      if(*inv_hl_index == i)
      {
        // render highlighter
        SDL_Rect inv_hl_rect = {inv_hl_x, inv_hl_y + (item_name_offset * i), inv_rect.w, 22};
        SDL_RenderCopy(renderer, inv_hl_tex, NULL, &inv_hl_rect);

        // render item window and item information
        SDL_Rect inv_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(renderer, inv_item_tex, NULL, &inv_item_rect);

        // render item name in the item window
        render_text(font_item, item_win_x + item_win_offset, item_win_y + item_win_offset, game_items_info[index].name, TEXT_COLOR_WHITE);

        // render item attributes depending on the type of the item
        if(game_items_info[index].item_type == TYPE_CONSUME)
        {
          render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), game_items_info[index].use, TEXT_COLOR_GREEN);
          render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), game_items_info[index].description, TEXT_COLOR_ORANGE);
          render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "[C]onsume", TEXT_COLOR_WHITE);
          render_text(font_item, item_win_x + (item_win_offset * 7), item_win_y + (item_win_offset * 27), "[D]rop", TEXT_COLOR_WHITE);
        }
        else if(game_items_info[index].item_type == TYPE_EQUIP)
        {
          char damage[12];
          sprintf(damage, "%d Damage", game_items_info[index].damage);
          render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), damage, TEXT_COLOR_BLUE);

          render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), game_items_info[index].description, TEXT_COLOR_ORANGE);

          // get the unique id of the item we're currently on in the inventory
          int unique_id = inventory[i].unique_id;

          // find the item we're currently on in the inventory
          for(int i = 0; i < GAME_ITEMS_COUNT; i++)
          {
            if(game_items[i].unique_id == unique_id)
            {
              if(game_items[i].is_equipped)
              {
                render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "[E]quipped", TEXT_COLOR_YELLOW);

                render_text(font_item, item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), "[D]rop", TEXT_COLOR_WHITE);
              }
              else
              {
                render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "un[E]quipped", TEXT_COLOR_WHITE);

                render_text(font_item, item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27), "[D]rop", TEXT_COLOR_WHITE);
              }

              break;
            }
          }
        }

        // NOTE(Rami): for debugging, REMOVE LATER
        char temp[24];
        sprintf(temp, "%d", inventory[i].unique_id);
        render_text(font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 25), temp, TEXT_COLOR_YELLOW);
      }
    }
  }

  // if the bottom item of the inventory got dropped, make the highlighter go up by one
  if(*inv_hl_index == *inv_item_count)
  {
    (*inv_hl_index)--;
  }
}

void render_items(SDL_Texture *item_tileset_tex, SDL_Rect *camera)
{
  SDL_Rect src = {0, 0, TILE_SIZE, TILE_SIZE};
  // NOTE(Rami): might have more rows of items than 1 later
  src.y = 0;

  SDL_Rect dst = {0, 0, TILE_SIZE, TILE_SIZE};

  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // render only items which are on the ground
    if(game_items[i].is_on_ground)
    {
      // set texture x
      src.x = game_items_info[game_items[i].item_id - 1].tile * TILE_SIZE;

      // set item position
      dst.x = game_items[i].x - camera->x;
      dst.y = game_items[i].y - camera->y;

      SDL_RenderCopy(renderer, item_tileset_tex, &src, &dst);
    }
  }
}

void drop_item(player_t *player, int *inv_hl_index, int *inv_item_count)
{
  // the item we want to drop from the inventory
  item_t *item_to_drop = &inventory[*inv_hl_index];

  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // find the correct item from the game_items array,
    // its .is_on_ground value needs to be zero
    if(item_to_drop->unique_id == game_items[i].unique_id && !game_items[i].is_on_ground)
    {
      // unequip the item when you drop it
      // set the item to be on the ground
      // set the item position to the player
      game_items[i].is_equipped = 0;
      game_items[i].is_on_ground = 1;
      game_items[i].x = player->x;
      game_items[i].y = player->y;

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
  int count = INVENTORY_COUNT - *inv_hl_index - 1;

  // if count is over the amount of items we have then clamp it
  if(count > *inv_item_count)
  {
    count = *inv_item_count - *inv_hl_index - 1;
  }

  // move the item data according to the value of count
  for(int i = 0; i != count; i++)
  {
    inventory[*inv_hl_index + i] = inventory[*inv_hl_index + i + 1];
  }

  // after moving the last item remove its original data
  inventory[*inv_hl_index + count].item_id = ID_NONE;
  inventory[*inv_hl_index + count].unique_id = 0;
  inventory[*inv_hl_index + count].is_on_ground = 0;
  inventory[*inv_hl_index + count].is_equipped = 0;
  inventory[*inv_hl_index + count].x = 0;
  inventory[*inv_hl_index + count].y = 0;
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

          char message_string[80];
          sprintf(message_string, "You pick up the %s", game_items_info[item->item_id - 1].name);

          add_console_msg(message_string, TEXT_COLOR_WHITE);

          return;
        }
      }

      add_console_msg("Your inventory is too full right now", TEXT_COLOR_WHITE);

      return;
    }
  }

  add_console_msg("You find nothing nearby to pick up", TEXT_COLOR_WHITE);
}

void render_interface(player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex, font_t *font_struct)
{
  // render the interface stats and the console
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 385, 160};
  SDL_Rect console_rect = {stats_rect.x + stats_rect.w, WINDOW_HEIGHT - 160, WINDOW_WIDTH - console_rect.x, 160};
  SDL_RenderCopy(renderer, interface_console_tex, NULL, &console_rect);
  SDL_RenderCopy(renderer, interface_stats_tex, NULL, &stats_rect);

  // statistics position and offset
  int stats_x = 8;
  int stats_y = WINDOW_HEIGHT - 151;
  int stats_offset = 10;

  char name[24];
  sprintf(name, "%s", player->name);
  render_text(font_struct, stats_x, stats_y, name, TEXT_COLOR_WHITE);

  char level[12];
  sprintf(level, "Level: %d", player->level);
  render_text(font_struct, stats_x, stats_y + (stats_offset * 6), level, TEXT_COLOR_WHITE);

  {
    // render player hp bar
    SDL_Rect hp_bar = {stats_x + (stats_offset * 2), stats_y + (stats_offset * 2), player->hp * 20, 14};

    SDL_SetRenderDrawColor(renderer, 77, 23, 23, 255);
    SDL_RenderFillRect(renderer, &hp_bar);
    hp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &hp_bar);

    //render player xp bar
    SDL_Rect xp_bar = {hp_bar.x, hp_bar.y + stats_offset * 2, player->xp * 20, 14};

    SDL_SetRenderDrawColor(renderer, 192, 230, 0, 255);
    SDL_RenderFillRect(renderer, &xp_bar);
    xp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &xp_bar);
  }

  // render hp text
  char hp[32];
  sprintf(hp, "HP                       %d/%d", player->hp, player->max_hp);
  render_text(font_struct, stats_x, stats_y + (stats_offset * 2), hp, TEXT_COLOR_WHITE);

  // NOTE(Rami): implement xp_until_next_level, remember correct xp[] size
  // render xp text
  char xp[54];
  sprintf(xp, "XP                                                %d", player->xp);
  render_text(font_struct, stats_x, stats_y + (stats_offset * 4), xp, TEXT_COLOR_WHITE);

  // render console messages
  int msg_x = console_rect.x + 10;
  int msg_y = console_rect.y + 8;
  int msg_offset = 12;

  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(console_messages[i].msg[0] != '.')
    {
      render_text(font_struct, msg_x, msg_y + (i * msg_offset), console_messages[i].msg, console_messages[i].msg_color);
    }
  }
}

void add_console_msg(char *msg, int msg_color)
{
  // fill the initial space of the console log
  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(console_messages[i].msg[0] == '.')
    {
      // copy data
      strcpy(console_messages[i].msg, msg);
      console_messages[i].msg_color = msg_color;

      return;
    }
  }

  // remove the oldest message
  console_messages[0].msg[0] = '.';
  console_messages[0].msg_color = 0;

  // move all messages starting from the second oldest message to create space for the new message
  for(int i = 1; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    strcpy(console_messages[i - 1].msg, console_messages[i].msg);
    console_messages[i - 1].msg_color = console_messages[i].msg_color;
  }

  // add the new message to the console log
  strcpy(console_messages[CONSOLE_MESSAGE_COUNT - 1].msg, msg);
  console_messages[CONSOLE_MESSAGE_COUNT - 1].msg_color = msg_color;

  return;
}

int handle_events(int *current_key)
{
  // an event struct to hold the current event information
  SDL_Event event;
  SDL_WaitEvent(&event);

  int exit_game = 0;

  switch(event.type)
  {
    case SDL_QUIT:
    {
      printf("SDL_QUIT\n");
      exit_game = 1;
    } break;

    case SDL_KEYDOWN:
    {
      // set our current_key to the key that was pressed down
      *current_key = event.key.keysym.sym;
    }
  }

  return exit_game;
}

void handle_input(char *dungeon, player_t *player, int *game_is_running, int *current_key, int *display_inventory, int *inv_hl_index, int *inv_item_count)
{
  if(*current_key == SDLK_ESCAPE)
  {
    printf("SDLK_ESCAPE\n");
    *game_is_running = 0;
  }

  /* -- IN INVENTORY -- */

  else if(*display_inventory)
  {
    switch(*current_key)
    {
      case SDLK_k:
      {
        if(*inv_hl_index - 1 < 0)
        {
          *inv_hl_index = *inv_item_count - 1;
        }
        else
        {
          (*inv_hl_index)--;
        }

        *current_key = 0;
      } break;

      case SDLK_j:
      {
        if(*inv_hl_index + 1 > *inv_item_count - 1)
        {
          *inv_hl_index = *inv_item_count = 0;
        }
        else
        {
          (*inv_hl_index)++;
        }

        *current_key = 0;
      } break;

      case SDLK_i:
      {
        *display_inventory = 0;
        *inv_hl_index = 0;

        *current_key = 0;
      } break;

      case SDLK_d:
      {
        drop_item(player, inv_hl_index, inv_item_count);

        *current_key = 0;
      } break;

      case SDLK_e:
      {
        equip_or_unequip_item(inv_hl_index);

        *current_key = 0;
      } break;

      case SDLK_c:
      {
        consume_item(player, inv_hl_index, inv_item_count);

        *current_key = 0;
      } break;
    }
  }

  /* -- NOT IN INVENTORY -- */

  else if(!*display_inventory)
  {
    switch(*current_key)
    {
      case SDLK_k:
      {
        entity_move(dungeon, player, 0, -player->speed * TILE_SIZE);
        *current_key = 0;
      } break;

      case SDLK_j:
      {
        entity_move(dungeon, player, 0, player->speed * TILE_SIZE);

        *current_key = 0;
      } break;

      case SDLK_h:
      {
        entity_move(dungeon, player, -player->speed * TILE_SIZE, 0);

        *current_key = 0;
      } break;

      case SDLK_l:
      {
        entity_move(dungeon, player, player->speed * TILE_SIZE, 0);

        *current_key = 0;
      } break;

      case SDLK_i:
      {
        *display_inventory = 1;

        *current_key = 0;
      } break;

      case SDLK_COMMA:
      {
        add_inventory_item(player);

        *current_key = 0;
      } break;

      case SDLK_d:
      {
        int fin_x = player->x / TILE_SIZE;
        int fin_y = player->y / TILE_SIZE;

        // if path is next to the player horizontally or vertically
        if(dungeon[(fin_y * LEVEL_SIZE) + (fin_x + 1)] == TILE_PATH_DOWN ||
           dungeon[(fin_y * LEVEL_SIZE) + (fin_x - 1)] == TILE_PATH_DOWN ||
           dungeon[((fin_y + 1) * LEVEL_SIZE) + fin_x] == TILE_PATH_DOWN ||
           dungeon[((fin_y - 1) * LEVEL_SIZE) + fin_x] == TILE_PATH_DOWN)
        {
          add_console_msg("You travel deeper into the mountain..", TEXT_COLOR_WHITE);
          generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2, player);
        }

        *current_key = 0;
      } break;

      case SDLK_a:
      {
        int fin_x = player->x / TILE_SIZE;
        int fin_y = player->y / TILE_SIZE;

        // if path is next to the player horizontally or vertically
        if(dungeon[(fin_y * LEVEL_SIZE) + (fin_x + 1)] == TILE_PATH_UP ||
           dungeon[(fin_y * LEVEL_SIZE) + (fin_x - 1)] == TILE_PATH_UP ||
           dungeon[((fin_y + 1) * LEVEL_SIZE) + fin_x] == TILE_PATH_UP ||
           dungeon[((fin_y - 1) * LEVEL_SIZE) + fin_x] == TILE_PATH_UP)
        {
          printf("You flee from the mountain..\n");
        }

        *game_is_running = 0;
        *current_key = 0;
      } break;
    }
  }
}

double distance(double x1, double y1, double x2, double y2)
{
  double difference_x = (x2 - x1) * (x2 - x1);
  double difference_y = (y2 - y1) * (y2 - y1);
  double sum = difference_x + difference_y;
  double value = sqrt(sum);

  return value;
}

// void update_lighting(char *dungeon, char *fov_dungeon, player_t *player)
// {
//   // set all elements as not visible
//   for(int y = 0; y < DUNGEON_SIZE; y++)
//   {
//     for(int x = 0; x < DUNGEON_SIZE; x++)
//     {
//       fov_dungeon[y * DUNGEON_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for(int y = (player.y / TILE_SIZE) - player.fov; y < (player.y / TILE_SIZE) + player.fov; y++)
//   {
//     for(int x = (player.x / TILE_SIZE) - player.fov; x < (player.x / TILE_SIZE) + player.fov; x++)
//     {
//       fov_dungeon[y * DUNGEON_SIZE + x] = 255;
//     }
//   }
//   #endif

//   // raycasted lighting
//   #if 0
//   for(int angle = 0; angle < 360; angle++)
//   {
//     // calculate the amount for the ray to progress
//     float dx = 0.1 * cos(angle);
//     float dy = 0.1 * sin(angle);

//     // set the ray to begin from the players location
//     float fx = player.x;
//     float fy = player.y;

//     for(;;)
//     {
//       // add to the rays location the amount we calculated
//       fx += dx;
//       fy += dy;

//       float dist = distance(player.x + 16, player.y + 16, fx, fy);
//       //int idist = dist / 32;

//       //printf("dist_between: %d\n", idist);

//       // if the ray is over the players view distance then stop the ray
//       if(dist > (player.fov * TILE_SIZE))
//       {
//         break;
//       }

//       // convert to array valid values
//       int ifx = fx / 32;
//       int ify = fy / 32;

//       // make sure the ray isn't going off the level
//       if(ifx >= 0 && ifx <= DUNGEON_SIZE && ify >= 0 && ify <= DUNGEON_SIZE)
//       {
//         //fov_dungeon[ify * DUNGEON_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov_dungeon[ify * DUNGEON_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if(dungeon[ify * DUNGEON_SIZE + ifx] == TILE_WALL_STONE || dungeon[ify * DUNGEON_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov_dungeon[(player.y / TILE_SIZE) * DUNGEON_SIZE + ((player.x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }

void render_player(SDL_Texture *player_tileset_tex, SDL_Texture *item_tileset_tex, SDL_Rect *camera, player_t *player)
{
  // calculate player texture source and destination
  SDL_Rect player_src = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect player_dst = {player->x - camera->x, player->y - camera->y, player->w, player->h};

  // render player
  SDL_RenderCopy(renderer, player_tileset_tex, &player_src, &player_dst);

  // sword one
  int sword_one = 0;
  SDL_Rect sword_one_dst = {player->x - camera->x + 0, player->y - camera->y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  int sword_two = 0;
  SDL_Rect sword_two_dst = {player->x - camera->x + 11, player->y - camera->y - 3, player->w, player->h};

  // NOTE(Rami): fix this later, issue with the sword dual wield

  // source for the item texture
  SDL_Rect item_src;
  item_src.y = 0;
  item_src.w = TILE_SIZE;
  item_src.h = TILE_SIZE;
  
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    // if equipped
    if(game_items[i].is_equipped)
    {
      // if an iron sword
      if(game_items[i].item_id == ID_IRON_SWORD)
      {
        // if hasn't been rendered before
        if(!sword_one)
        {
          sword_one = 1;

          // get the correct x-axis position for the item tile
          item_src.x = game_items_info[game_items[i].item_id - 1].tile * TILE_SIZE;

          // render it
          SDL_RenderCopy(renderer, item_tileset_tex, &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = game_items_info[game_items[i].item_id - 1].tile * TILE_SIZE;

          SDL_RenderCopy(renderer, item_tileset_tex, &item_src, &sword_two_dst);
        }
      }
    }
  }
}

void update_camera(SDL_Rect *camera, player_t *player)
{
  // center camera on player
  camera->x = player->x - (camera->w / 2);
  camera->y = (player->y + (player->h / 2)) - (camera->h / 2);

  if(camera->x < 0)
  {
    camera->x = 0;
  }

  if(camera->y < 0)
  {
    camera->y = 0;
  }

  if(camera->x >= LEVEL_WIDTH - camera->w)
  {
    camera->x = LEVEL_WIDTH - camera->w;
  }

  if(camera->y >= LEVEL_HEIGHT - camera->h)
  {
    camera->y = LEVEL_HEIGHT - camera->h;
  }
}

void render_level(SDL_Texture *tileset_tex, SDL_Texture *tile_tex, char *dungeon, char *fov, SDL_Rect *camera)
{
  // set render target to tiledungeon
  SDL_SetRenderTarget(renderer, tile_tex);

  // clear the old contents of the texture
  SDL_RenderClear(renderer);

  int to_y = (camera->y + camera->h) / TILE_SIZE;
  int to_x = (camera->x + camera->w) / TILE_SIZE;

  for(int y = camera->y / TILE_SIZE; y < to_y; y++)
  {
    for(int x = camera->x / TILE_SIZE; x < to_x; x++)
    {
      SDL_Rect src, dst;

      src.w = TILE_SIZE;
      src.h = TILE_SIZE;

      dst.w = TILE_SIZE;
      dst.h = TILE_SIZE;

      if(dungeon[y * LEVEL_SIZE + x] == TILE_FLOOR_GRASS)
      {
        src.x = TILE_FLOOR_GRASS * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_WALL_STONE)
      {
        src.x = TILE_WALL_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_FLOOR_STONE)
      {
        src.x = TILE_FLOOR_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_DOOR_CLOSED)
      {
        src.x = TILE_DOOR_CLOSED * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_DOOR_OPEN)
      {
        src.x = TILE_DOOR_OPEN * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_PATH_UP)
      {
        src.x = TILE_PATH_UP * TILE_SIZE;
        src.y = 0;
      }
      else if(dungeon[y * LEVEL_SIZE + x] == TILE_PATH_DOWN)
      {
        src.x = TILE_PATH_DOWN * TILE_SIZE;
        src.y = 0;
      }

      dst.x = (x * TILE_SIZE);
      dst.y = (y * TILE_SIZE);

      // NOTE(Rami): continue lighting debugging from here

      //if(fov_dungeon[y * LEVEL_SIZE + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov_dungeon[y * LEVEL_SIZE + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(renderer, tileset_tex, &src, &dst);
    }
  }

  // unset render target from tiledungeon
  SDL_SetRenderTarget(renderer, NULL);

  SDL_Rect dst = {0, 0, camera->w, camera->h};

  // render to the window
  SDL_RenderCopy(renderer, tile_tex, camera, &dst);
}

// NOTE(Rami): the return value is for the x-flip, think about if we really want it
void entity_move(char *dungeon, player_t *entity, int x, int y)
{
  int entity_dungeon_x = (entity->x + x) / TILE_SIZE;
  int entity_dungeon_y = (entity->y + y) / TILE_SIZE;

  if(entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    // NOTE(Rami): 
    if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] == TILE_FLOOR_STONE)
    // if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the door open", TEXT_COLOR_WHITE);
      dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] = TILE_DOOR_OPEN;
    }
    else if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] == TILE_PATH_UP)
    {
      add_console_msg("A path that leads back to the surface.. [A]scend and flee the mountain?", TEXT_COLOR_WHITE);
    }
    else if(dungeon[(entity_dungeon_y * LEVEL_SIZE) + entity_dungeon_x] == TILE_PATH_DOWN)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", TEXT_COLOR_WHITE);
    }
  }
}

// NOTE(Rami): !!!
// entity_t* new_entity(char *name, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov)
// {
//   for(int i = 0; i < ENTITY_COUNT; i++)
//   {
//     if(!entities[i])
//     {
//       entities[i] = malloc(sizeof(entity_t));

//       strcpy(entities[i]->name, name);
//       entities[i]->level = level;
//       entities[i]->money = money;
//       entities[i]->hp = hp;
//       entities[i]->max_hp = max_hp;
//       entities[i]->xp = xp;
//       entities[i]->x = x;
//       entities[i]->y = y;
//       entities[i]->w = w;
//       entities[i]->h = h;
//       entities[i]->speed = speed;
//       entities[i]->fov = fov;

//       return entities[i];
//     }
//   }

//   return NULL;
// }

int game_init(player_t *player, font_t **font_console, font_t **font_inv, font_t **font_item, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **dungeon_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex)
{
  /* -- SDL-- */

  // initialize SDL video subsystem
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize: %s\n", SDL_GetError());

    return 0;
  }

  // create window
  window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(!window)
  {
    printf("SDL could not create window: %s\n", SDL_GetError());

    return 0;
  }

  // create renderer for window
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer)
  {
    printf("SDL could not create a renderer: %s\n", SDL_GetError());

    return 0;
  }

  // initialize PNG loading
  int image_flags = IMG_INIT_PNG;
  if(!(IMG_Init(image_flags) & image_flags))
  {
    printf("SLD image library could not initialize: %s\n", IMG_GetError());

    return 0;
  }

  // initialize TTF library
  if(TTF_Init())
  {
    printf("SDL TTF library could not initialize: %s\n", TTF_GetError());

    return 0;
  }

  /* -- FONTS -- */

  TTF_Font *font = TTF_OpenFont("data/fonts/classic.ttf", 16);
  *font_console = create_font_atlas(font);
  TTF_CloseFont(font);

  font = TTF_OpenFont("data/fonts/alkhemikal.ttf", 16);
  *font_inv = create_font_atlas(font);
  TTF_CloseFont(font);

  font = TTF_OpenFont("data/fonts/hello-world.ttf", 13);
  *font_item = create_font_atlas(font);
  TTF_CloseFont(font);
  font = NULL;

  if(!(*font_console) || !(*font_inv) || !(*font_item))
  {
    printf("Could not create font atlases\n");

    return 0;
  }

  /* -- TEXTURES -- */

  *tileset_tex = load_texture("data/images/tileset.png");
  *player_tileset_tex = load_texture("data/images/player_tileset.png");
  *item_tileset_tex = load_texture("data/images/item_tileset.png");
  *dungeon_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);
  *inv_tex = load_texture("data/images/player_inventory.png");
  *player_inv_hl_tex = load_texture("data/images/player_inventory_highlight.png");
  *inv_item_tex = load_texture("data/images/player_inventory_item.png");
  *interface_console_tex = load_texture("data/images/interface_console.png");
  *interface_stats_tex = load_texture("data/images/interface_statistics.png");

  if(!(*tileset_tex) || !(*player_tileset_tex) || !(*item_tileset_tex) || !(*dungeon_tex) || !(*inv_tex) || !(*player_inv_hl_tex) || !(*player_inv_hl_tex) || !(*interface_console_tex) || !(*interface_stats_tex))
  {
    printf("Could not load textures\n");

    return 0;
  }
  else
  {
    // set texture opacity
    SDL_SetTextureBlendMode(*player_inv_hl_tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(*player_inv_hl_tex, 30);
  }

  /* -- ARRAYS -- */
  
  // init game items
  for(int i = 0; i < GAME_ITEMS_COUNT; i++)
  {
    game_items[i].item_id = ID_NONE;
    game_items[i].unique_id = i + 1;
    game_items[i].is_on_ground = 0;
    game_items[i].is_equipped = 0;
    game_items[i].x = 0;
    game_items[i].y = 0;
  }

  // init inventory
  for(int i = 0; i < INVENTORY_COUNT; i++)
  {
    inventory[i].item_id = ID_NONE;
    inventory[i].unique_id = 0;
    inventory[i].is_on_ground = 0;
    inventory[i].is_equipped = 0;
    inventory[i].x = 0;
    inventory[i].y = 0;
  }

  // init console messages
  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    console_messages[i].msg[0] = '.';
    console_messages[i].msg_color = 0;
  }

  /* -- CONFIG -- */

  // NOTE(Rami): if the user edits the config file and turns something like key=1 into key = 1
  // then our arrays will get bogus values, we need to somehow make sure that the key value pairs
  // are in the format and ONLY in the format we expect.
  {
    conf_t conf;
    if(!conf_load(&conf, "data/items.cfg"))
    {
      return 0;
    }

    printf("key_value_pair_count: %d\n", conf.key_value_pair_count);

    // loop for as many items we have (conf.key_value_pair_count / 9)
    // 9 being the amount of k_v_pairs each item has
    for(int i = 0; i < conf.key_value_pair_count / 9; i++)
    {
      int index = i * 9;

      // game_items will get it's ID from the items.cfg file, game_items_info uses everything else
      // from the config file, just not the ID.
      strcpy(game_items_info[i].name, conf.vars[index + 1].conf_var_u.s);
      game_items_info[i].item_type = conf.vars[index + 2].conf_var_u.i;
      game_items_info[i].tile = conf.vars[index + 3].conf_var_u.i;
      strcpy(game_items_info[i].use, conf.vars[index + 4].conf_var_u.s);
      game_items_info[i].hp_healed = conf.vars[index + 5].conf_var_u.i;
      game_items_info[i].damage = conf.vars[index + 6].conf_var_u.i;
      game_items_info[i].armor = conf.vars[index + 7].conf_var_u.i;
      strcpy(game_items_info[i].description, conf.vars[index + 8].conf_var_u.s);
    }

    // NOTE(Rami): replace these with functions like add_game_item()
    // so we can just add items to the game via functions

    // Health Potion
    game_items[0].item_id = conf.vars[0].conf_var_u.i;
    game_items[0].is_on_ground = 1;
    game_items[0].is_equipped = 0;
    game_items[0].x = player->x;
    game_items[0].y = player->y - 32;

    // Health Potion
    game_items[1].item_id = conf.vars[0].conf_var_u.i;
    game_items[1].is_on_ground = 1;
    game_items[1].is_equipped = 0;
    game_items[1].x = player->x + 32;
    game_items[1].y = player->y;

    // Health Potion
    game_items[2].item_id = conf.vars[0].conf_var_u.i;
    game_items[2].is_on_ground = 1;
    game_items[2].is_equipped = 0;
    game_items[2].x = player->x;
    game_items[2].y = player->y + 32;

    // Health Potion
    game_items[3].item_id = conf.vars[0].conf_var_u.i;
    game_items[3].is_on_ground = 1;
    game_items[3].is_equipped = 0;
    game_items[3].x = player->x - 32;
    game_items[3].y = player->y;

    // Iron Sword
    game_items[4].item_id = conf.vars[9].conf_var_u.i;
    game_items[4].is_on_ground = 1;
    game_items[4].is_equipped = 0;
    game_items[4].x = player->x + 32;
    game_items[4].y = player->y + 32;

    // Iron Sword
    game_items[5].item_id = conf.vars[9].conf_var_u.i;
    game_items[5].is_on_ground = 1;
    game_items[5].is_equipped = 0;
    game_items[5].x = player->x + 32;
    game_items[5].y = player->y - 32;

    conf_free(&conf);
  }

  return 1;
}

SDL_Texture* load_texture(char *path)
{
  SDL_Texture *new_tex = NULL;

  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    printf("SDL could not load image %s: %s\n", path, IMG_GetError());
  }
  else
  {
    // create texture from surface
    new_tex = SDL_CreateTextureFromSurface(renderer, loaded_surf);
    if(!new_tex)
    {
      printf("SDL could not create a texture from surface: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(loaded_surf);
  }

  return new_tex;
}

void game_exit(SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *dungeon_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *player_inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, font_t *font_console, font_t *font_inv, font_t *font_item, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex)
{
  if(player)
  {
    free(player);
    player = NULL;
  }

  if(font_console)
  {
    if(font_console->atlas)
    {
      SDL_DestroyTexture(font_console->atlas);
    }

    free(font_console);
    font_console = NULL;
  }

  if(font_inv)
  {
    if(font_inv->atlas)
    {
      SDL_DestroyTexture(font_inv->atlas); 
    }

    free(font_inv);
    font_inv = NULL;
  }

  if(font_item)
  {
    if(font_item->atlas)
    {
      SDL_DestroyTexture(font_item->atlas);
    }

    free(font_item);
    font_item = NULL;
  }

  if(tileset_tex)
  {
    SDL_DestroyTexture(tileset_tex);
    tileset_tex = NULL; 
  }

  if(player_tileset_tex)
  {
    SDL_DestroyTexture(player_tileset_tex);
    player_tileset_tex = NULL;
  }

  if(item_tileset_tex)
  {
    SDL_DestroyTexture(item_tileset_tex);
    item_tileset_tex = NULL;
  }

  if(dungeon_tex)
  {
    SDL_DestroyTexture(dungeon_tex);
    dungeon_tex = NULL;
  }

  if(inv_tex)
  {
    SDL_DestroyTexture(inv_tex);
    inv_tex = NULL;
  }

  if(player_inv_hl_tex)
  {
    SDL_DestroyTexture(player_inv_hl_tex);
    player_inv_hl_tex = NULL; 
  }

  if(inv_item_tex)
  {
    SDL_DestroyTexture(inv_item_tex);
    inv_item_tex = NULL;
  }

  if(interface_console_tex)
  {
    SDL_DestroyTexture(interface_console_tex);
    interface_console_tex = NULL;
  }

  if(interface_stats_tex)
  {
    SDL_DestroyTexture(interface_stats_tex);
    interface_stats_tex = NULL;
  }

  if(renderer)
  {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
  }

  if(window)
  {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  // quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}