#include <moonbreath_mountain.h>

void consume_item(entity_t *player, int *inv_hl_index, int *inv_item_count)
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

          add_console_msg("You drink the potion and feel slighty better than before", TEXT_COLOR_STATUS);

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

          add_console_msg("You unequip the item", TEXT_COLOR_WHITE);
        }
        // if it's unequipped
        else
        {
          // equip it
          game_items[i].is_equipped = 1;

          add_console_msg("You equip the item", TEXT_COLOR_WHITE);
        }

        break;
      }
    }
  }
}

void render_text(SDL_Renderer *renderer, font_t *font_struct, int x, int y, char *str, int wrap_width, unsigned int text_color)
{
  // start at the beginning of the string
  char *current_char = str;

  // store the starting x of the text for wrapping
  int initial_x = x;

  // store how many characters we have
  int char_amount = 0;

  // set to 1 if we want to wrap text, set to 0 if we don't want to wrap text
  int force_wrapping = 0;

  while(*current_char != '\0')
  {
    int array_index = *current_char - 38;

    // if we've hit the wrap amount, force wrapping
    if(wrap_width != 0 && char_amount >= wrap_width)
    {
      force_wrapping = 1;
    }

    // if newline, force wrapping
    if((*current_char == '\\' && *(current_char + 1) == 'n') || (*current_char == 'n' && *(current_char - 1) == '\\'))
    {
      force_wrapping = 1;

      // move onto the next byte in the text
      current_char++;
      
      continue;
    }

    if(force_wrapping)
    {
      // move the position of the text to the original x
      x = initial_x;

      // move the position of the text to the next row
      y += 16;

      // reset the character amount
      char_amount = 0;

      force_wrapping = 0;
    }

    // if the character is a space
    if(*current_char == ' ')
    {
      // increment the amount of characters
      char_amount++;

      // move the position of the text
      x += 5;

      // move onto the next byte in the text
      current_char++;

      continue;
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
    char_amount++;

    // move the position of the text
    x += glyph_metrics->advance;

    // move onto the next byte in the text
    current_char++;
  }
}

// NOTE(Rami): returns a MALLOC'd pointer, remember to FREE!
font_t* create_font_atlas(SDL_Renderer *renderer, TTF_Font *font)
{
  // a texture to hold all the glyphs
  SDL_Texture *glyph_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 768);

  SDL_Surface *glyph_surface = NULL;
  SDL_Texture *glyph_texture = NULL;
  
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
    glyph_surface = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_texture = SDL_CreateTextureFromSurface(renderer, glyph_surface);

    // set the glyph atlas as the render target
    SDL_SetRenderTarget(renderer, glyph_atlas);

    // get the advance value of the glyph
    int advance;

    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // set where on the atlas we render
    SDL_Rect atlas_rect = {x, y, glyph_surface->w, glyph_surface->h};

    // store the glyph metrics
    font_struct->metrics[i] = (font_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

    // advance the rendering location
    x += glyph_surface->w;

    // in case the glyphs go over the width of the atlas
    if(x > 1024)
    {
      x = 0;
      
      // glyphs can differ in height so use a constant here to keep spacing consistent
      y += 16;
    }

    // render the glyph on the atlas
    SDL_RenderCopy(renderer, glyph_texture, NULL, &atlas_rect);

    // free the glyph surface and texture
    SDL_FreeSurface(glyph_surface);
    glyph_surface = NULL;
    SDL_DestroyTexture(glyph_texture);
    glyph_texture = NULL;
  }

  // unset the render target
  SDL_SetRenderTarget(renderer, NULL);

  // set the struct pointer to the completed atlas
  font_struct->atlas = glyph_atlas;

  return font_struct;
}

SDL_Color hex_to_rgba_color(unsigned int hex_color)
{
  // shift and mask the rgb out of the hex color
  int r = (hex_color >> 24) & 0xFF;
  int g = (hex_color >> 16) & 0xFF;
  int b = (hex_color >> 8) & 0xFF;
  int a = hex_color & 0xFF;

  SDL_Color rgb_color = {r, g, b, a};

  return rgb_color;
}

void render_inventory(SDL_Renderer *renderer, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, font_t *font_inv, font_t *font_item, int *inv_hl_index, int *inv_item_count)
{
  // render inventory background
  SDL_Rect inventory_rect = {WINDOW_WIDTH - 424, WINDOW_HEIGHT - 718, 400, 500};
  SDL_RenderCopy(renderer, inv_tex, NULL, &inventory_rect);

  render_text(renderer, font_inv, WINDOW_WIDTH - 390, WINDOW_HEIGHT - 713, "Inventory", 0, TEXT_COLOR_WHITE);

  // item position and the offset
  int item_name_x = WINDOW_WIDTH - 414;
  int item_name_y = WINDOW_HEIGHT - 688;
  int item_name_offset = 25;

  // item window position and the offsets
  int item_win_x = WINDOW_WIDTH - 694;
  int item_win_y = WINDOW_HEIGHT - 518;
  int item_win_offset = 10;
  
  // item highlighter position
  int inv_hl_x = WINDOW_WIDTH - 423;
  int inv_hl_y = WINDOW_HEIGHT - 691;

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
      char item_name_index[] = {97 + i, '\0'};

      // render item index and name in inventory
      render_text(renderer, font_inv, item_name_x, item_name_y + (item_name_offset * i), item_name_index, 0, TEXT_COLOR_WHITE);
      render_text(renderer, font_inv, item_name_x + 25, item_name_y + (item_name_offset * i), game_items_info[index].name, 0, TEXT_COLOR_WHITE);

      // render certain things if this item is currently selected in the inventory
      if(*inv_hl_index == i)
      {
        // render highlighter
        SDL_Rect inventory_highlight_rect = {inv_hl_x, inv_hl_y + (item_name_offset * i), 398, 22};
        SDL_RenderCopy(renderer, inv_hl_tex, NULL, &inventory_highlight_rect);

        // render item window and item information
        SDL_Rect inventory_item_rect = {item_win_x, item_win_y, 250, 300};
        SDL_RenderCopy(renderer, inv_item_tex, NULL, &inventory_item_rect);

        // render item name in the item window
        render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + item_win_offset, game_items_info[index].name, 0, TEXT_COLOR_WHITE);

        // render item attributes depending on the type of the item
        if(game_items_info[index].item_type == TYPE_CONSUME)
        {
          render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), game_items_info[index].use, 0, TEXT_COLOR_GREEN);
          render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), game_items_info[index].description, 0, TEXT_COLOR_ORANGE);
          render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "[C]onsume", 0, TEXT_COLOR_YELLOW);
          render_text(renderer, font_item, item_win_x + (item_win_offset * 7), item_win_y + (item_win_offset * 27), "[D]rop", 0, TEXT_COLOR_YELLOW);
        }
        else if(game_items_info[index].item_type == TYPE_EQUIP)
        {
          char damage[12];
          sprintf(damage, "%d Damage", game_items_info[index].damage);
          render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 3), damage, 0, TEXT_COLOR_BLUE);

          render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 5), game_items_info[index].description, 0, TEXT_COLOR_ORANGE);

          // get the unique id of the item we're currently on in the inventory
          int unique_id = inventory[i].unique_id;

          // find the item we're currently on in the inventory
          for(int i = 0; i < GAME_ITEMS_COUNT; i++)
          {
            if(game_items[i].unique_id == unique_id)
            {
              if(game_items[i].is_equipped)
              {
                render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "[E]quipped", 0, TEXT_COLOR_YELLOW);

                render_text(renderer, font_item, item_win_x + (item_win_offset * 8), item_win_y + (item_win_offset * 27), "[D]rop", 0, TEXT_COLOR_YELLOW);
              }
              else
              {
                render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 27), "Not [E]quipped", 0, TEXT_COLOR_WHITE);

                render_text(renderer, font_item, item_win_x + (item_win_offset * 10), item_win_y + (item_win_offset * 27), "[D]rop", 0, TEXT_COLOR_YELLOW);
              }

              break;
            }
          }
        }

        // NOTE(Rami): for debugging, REMOVE LATER
        char temp[24];
        sprintf(temp, "%d", inventory[i].unique_id);
        render_text(renderer, font_item, item_win_x + item_win_offset, item_win_y + (item_win_offset * 25), temp, 0, TEXT_COLOR_YELLOW);
      }
    }
  }

  // if the bottom item of the inventory got dropped, make the highlighter go up by one
  if(*inv_hl_index == *inv_item_count)
  {
    (*inv_hl_index)--;
  }
}

void render_items(SDL_Renderer *renderer, SDL_Texture *item_tileset_tex, SDL_Rect *camera)
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

void drop_item(entity_t *player, int *inv_hl_index, int *inv_item_count)
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

void add_inventory_item(entity_t *player)
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

          add_console_msg(message_string, TEXT_COLOR_ACTION);

          return;
        }
      }

      add_console_msg("Your inventory is full right now", TEXT_COLOR_ACTION);

      return;
    }
  }

  add_console_msg("You find nothing worthy of picking up", TEXT_COLOR_ACTION);
}

void render_interface(SDL_Renderer *renderer, entity_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex, font_t *font_struct)
{
  SDL_Rect console = {WINDOW_WIDTH - 639, WINDOW_HEIGHT - 160, 639, 160};
  SDL_Rect stats = {0, WINDOW_HEIGHT - 160, 385, 160};

  // render the interface statistics and the interface console
  SDL_RenderCopy(renderer, interface_console_tex, NULL, &console);
  SDL_RenderCopy(renderer, interface_stats_tex, NULL, &stats);

  // statistics position and offset
  int stats_x = 8;
  int stats_y = WINDOW_HEIGHT - 151;
  int stats_offset = 10;

  char name[24];
  sprintf(name, "%s", player->name);
  render_text(renderer, font_struct, stats_x, stats_y, name, 0, TEXT_COLOR_WHITE);

  char level[12];
  sprintf(level, "Level: %d", player->level);
  render_text(renderer, font_struct, stats_x, stats_y + (stats_offset * 6), level, 0, TEXT_COLOR_WHITE);

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
  render_text(renderer, font_struct, stats_x, stats_y + (stats_offset * 2), hp, 0, TEXT_COLOR_WHITE);

  // NOTE(Rami): implement xp_until_next_level, remember correct xp[] size
  // render xp text
  char xp[54];
  sprintf(xp, "XP                                                %d", player->xp);
  render_text(renderer, font_struct, stats_x, stats_y + (stats_offset * 4), xp, 0, TEXT_COLOR_WHITE);

  // render console messages
  int msg_x = WINDOW_WIDTH - 626;
  int msg_y = WINDOW_HEIGHT - 154;
  int msg_offset = 12;

  for(int i = 0; i < CONSOLE_MESSAGE_COUNT; i++)
  {
    if(console_messages[i].msg[0] != '.')
    {
      render_text(renderer, font_struct, msg_x, msg_y + (i * msg_offset), console_messages[i].msg, 0, console_messages[i].msg_color);
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

void handle_input(char *map, entity_t *player, int *game_is_running, int *current_key, int *display_inventory, int *inv_hl_index, int *inv_item_count)
{
  if(*current_key == SDLK_ESCAPE)
  {
    printf("SDLK_ESCAPE\n");
    *game_is_running = 0;
  }
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
  else if(!*display_inventory)
  {
    switch(*current_key)
    {
      case SDLK_k:
      {
        entity_move(map, player, 0, -player->speed * TILE_SIZE, game_is_running);

        *current_key = 0;
      } break;

      case SDLK_j:
      {
        entity_move(map, player, 0, player->speed * TILE_SIZE, game_is_running);

        *current_key = 0;
      } break;

      case SDLK_h:
      {
        entity_move(map, player, -player->speed * TILE_SIZE, 0, game_is_running);

        *current_key = 0;
      } break;

      case SDLK_l:
      {
        entity_move(map, player, player->speed * TILE_SIZE, 0, game_is_running);

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

      case SDLK_s:
      {
        // NOTE(Rami): this is useless now, we'd have to get a new slot for this added item and then display it
        game_items[0].is_on_ground = 1;
        add_console_msg("Item(s) Added To Gameworld", TEXT_COLOR_SPECIAL);

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

// void update_lighting(char *map, char *fov_map, entity_t *player)
// {
//   // set all elements as not visible
//   for(int y = 0; y < MAP_SIZE; y++)
//   {
//     for(int x = 0; x < MAP_SIZE; x++)
//     {
//       fov_map[y * MAP_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for(int y = (player->y / TILE_SIZE) - player->fov; y < (player->y / TILE_SIZE) + player->fov; y++)
//   {
//     for(int x = (player->x / TILE_SIZE) - player->fov; x < (player->x / TILE_SIZE) + player->fov; x++)
//     {
//       fov_map[y * MAP_SIZE + x] = 255;
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
//     float fx = player->x;
//     float fy = player->y;

//     for(;;)
//     {
//       // add to the rays location the amount we calculated
//       fx += dx;
//       fy += dy;

//       float dist = distance(player->x + 16, player->y + 16, fx, fy);
//       //int idist = dist / 32;

//       //printf("dist_between: %d\n", idist);

//       // if the ray is over the players view distance then stop the ray
//       if(dist > (player->fov * TILE_SIZE))
//       {
//         break;
//       }

//       // convert to array valid values
//       int ifx = fx / 32;
//       int ify = fy / 32;

//       // make sure the ray isn't going off the level
//       if(ifx >= 0 && ifx <= MAP_SIZE && ify >= 0 && ify <= MAP_SIZE)
//       {
//         //fov_map[ify * MAP_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov_map[ify * MAP_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if(map[ify * MAP_SIZE + ifx] == TILE_WALL_STONE || map[ify * MAP_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov_map[(player->y / TILE_SIZE) * MAP_SIZE + ((player->x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }

void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Texture *item_tileset_tex, SDL_Rect *camera, entity_t *player)
{
  // calc player source and destination
  SDL_Rect player_src = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect player_dst = {player->x - camera->x, player->y - camera->y, player->w, player->h};

  // render player
  SDL_RenderCopy(renderer, player_tileset_tex, &player_src, &player_dst);

  // sword one
  int sword_one = 0;
  SDL_Rect sword_one_dst = {player->x - camera->x + 2, player->y - camera->y - 8, TILE_SIZE, TILE_SIZE};

  // sword two
  int sword_two = 0;
  SDL_Rect sword_two_dst = {player->x - camera->x + 13, player->y - camera->y - 8, player->w, player->h};

  // NOTE(Rami): fix this later, issue with the sword dual wield

  // source for the item asset
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

void update_camera(SDL_Rect *camera, entity_t *player)
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

void render_level(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, char *map, char *fov_map, SDL_Rect *camera)
{
  // set render target to tilemap
  SDL_SetRenderTarget(renderer, tilemap_tex);

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

      if(map[y * MAP_SIZE + x] == TILE_NONE)
      {
        src.x = TILE_NONE * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_FLOOR_GRASS)
      {
        src.x = TILE_FLOOR_GRASS * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_WALL_STONE)
      {
        src.x = TILE_WALL_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_FLOOR_STONE)
      {
        src.x = TILE_FLOOR_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
      {
        src.x = TILE_DOOR_CLOSED * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_DOOR_OPEN)
      {
        src.x = TILE_DOOR_OPEN * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_STAIRS_UP)
      {
        src.x = TILE_STAIRS_UP * TILE_SIZE;
        src.y = 0;
      }
      else if(map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
      {
        src.x = TILE_STAIRS_DOWN * TILE_SIZE;
        src.y = 0;
      }

      dst.x = (x * TILE_SIZE);
      dst.y = (y * TILE_SIZE);

      // NOTE(Rami): continue lighting debugging from here

      //if(fov_map[y * MAP_SIZE + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov_map[y * MAP_SIZE + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(renderer, tileset_tex, &src, &dst);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  SDL_Rect dst = {0, 0, camera->w, camera->h};

  // render to the window
  SDL_RenderCopy(renderer, tilemap_tex, camera, &dst);
}

// NOTE(Rami): the return value is for the x-flip, think about if we really want it
int entity_move(char *map, entity_t *entity, int x, int y, int *game_is_running)
{
  int entity_map_x = (entity->x + x) / TILE_SIZE;
  int entity_map_y = (entity->y + y) / TILE_SIZE;

  if(entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    // NOTE(Rami): 
    //if(map[entity_map_y * MAP_SIZE + entity_map_x] == TILE_FLOOR_STONE)
    if(map[entity_map_y * MAP_SIZE + entity_map_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }
    else if(map[entity_map_y * MAP_SIZE + entity_map_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the heavy door open", TEXT_COLOR_ACTION);
      map[entity_map_y * MAP_SIZE + entity_map_x] = TILE_DOOR_OPEN;
    }
    else if(map[entity_map_y * MAP_SIZE + entity_map_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }

    else if(map[entity_map_y * MAP_SIZE + entity_map_x] == TILE_STAIRS_UP)
    {
      // NOTE(Rami): end game stuff
      *game_is_running = 0;

      return 0;
    }
    else if(map[entity_map_y * MAP_SIZE + entity_map_x] == TILE_STAIRS_DOWN)
    {
      // NOTE(Rami): when the player moves on top of a down/up ladder we want to
      // give them a message that says There is a ladder here, [D]escend/[A]scend?
      add_console_msg("You descend the ladder..", TEXT_COLOR_ACTION);
      generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 4, entity);

      return 1;
    }
  }

  return 1;
}

player_t* new_player()
{
  player_t *p = malloc(sizeof(player_t));

  return p;
}

entity_t* new_entity(char *name, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov)
{
  for(int i = 0; i < ENTITY_COUNT; i++)
  {
    if(!entities[i])
    {
      entities[i] = malloc(sizeof(entity_t));

      entities[i]->name = name;
      entities[i]->level = level;
      entities[i]->money = money;
      entities[i]->hp = hp;
      entities[i]->max_hp = max_hp;
      entities[i]->xp = xp;
      entities[i]->x = x;
      entities[i]->y = y;
      entities[i]->w = w;
      entities[i]->h = h;
      entities[i]->speed = speed;
      entities[i]->fov = fov;

      return entities[i];
    }
  }

  return NULL;
}

int initialize(SDL_Window **window, SDL_Renderer **renderer)
{
  // success flag
  int success = 1;

  // initialize SDL video subsystem
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize: %s\n", SDL_GetError());
    success = 0;
  }
  else
  {
    // create the window
    *window = SDL_CreateWindow("Moonbreath Mountain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
      printf("SDL could not create window: %s\n", SDL_GetError());
      success = 0;
    }
    else
    {
      // create the renderer for our window
      *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
      if(renderer == NULL)
      {
        printf("SDL could not create a renderer: %s\n", SDL_GetError());
        success = 0;
      }
      else
      {
        // initialize PNG loading
        int image_flags = IMG_INIT_PNG;
        if(!(IMG_Init(image_flags) & image_flags))
        {
          printf("SLD image library could not initialize: %s\n", IMG_GetError());
          success = 0;
        }
        else
        {
          if(TTF_Init() < 0)
          {
            printf("SDL ttf library could not initialize: %s\n", TTF_GetError());
            success = 0;
          }
        }
      }
    }
  }

  return success;
}

SDL_Texture* load_texture(SDL_Renderer *renderer, const char *str)
{
  SDL_Texture *new_texture = NULL;

  SDL_Surface *loaded_surface = IMG_Load(str);
  if(!loaded_surface)
  {
    printf("SDL could not load image %s: %s\n", str, IMG_GetError());
  }
  else
  {
    // create a texture from the surface
    new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if(!new_texture)
    {
      printf("SDL could not create a texture from surface: %s\n", SDL_GetError());
    }

    // free old surface
    SDL_FreeSurface(loaded_surface);
  }

  return new_texture;
}

void free_resources(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *player_inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, font_t *font_console, font_t *font_inv, font_t *font_item, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex)
{
  for(int i = 0; i < ENTITY_COUNT; i++)
  {
    if(entities[i])
    {
      free(entities[i]);
    }
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

  if(player)
  {
    free(player);
    player = NULL;
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

  if(tilemap_tex)
  {
    SDL_DestroyTexture(tilemap_tex);
    tilemap_tex = NULL;
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