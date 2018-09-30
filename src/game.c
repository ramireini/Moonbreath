#include "game.h"

// ideas:
// const char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-=!@#$"
// and then in your atlas creation function, iterate over that
// and render the glyph for each character in that string to the atlas and store the metrics

void render_text(SDL_Renderer *renderer, font_t *font_struct, int text_x, int text_y, char *text, int text_wrap_amount, unsigned int text_hex_color)
{
  // start at the beginning of the string
  char *current_char = text;

  // store the starting x of the text for wrapping
  int initial_x = text_x; 

  // store how many characters we have
  int char_amount = 0;

  // set if we want to wrap text, unset if we do not want to wrap text
  int force_wrapping = 0;

  while (*current_char != '\0')
  {
    char array_index = *current_char - 65;

    if (text_wrap_amount != 0 && char_amount >= text_wrap_amount)
    {
      force_wrapping = 1;
    }

    if (*current_char == '\n')
    {
      force_wrapping = 1;
    }

    if (force_wrapping == 1)
    {
      // move the position of the text to the original x
      text_x = initial_x;

      // move the position of the text to the next row
      text_y += 16;

      // reset the character amount
      char_amount = 0;

      force_wrapping = 0;
    }

    // if the character is a space
    if (*current_char == ' ')
    {
      // increment the amount of characters
      char_amount++;

      // move the position of the text
      text_x += 4;

      // move onto the next byte in the text
      current_char += 1;

      continue;
    }

    // make sure we're indexing the array correctly
    if (array_index > 31)
    {
      array_index = array_index - 6;
    }

    // fetch the glyph metrics for the current character in the text
    font_metrics_t glyph_metrics = font_struct->metrics[(int)array_index];

    // the source rectangle to take from the glyph atlas
    SDL_Rect src = {glyph_metrics.x, glyph_metrics.y, glyph_metrics.w, glyph_metrics.h};

    // the destination rectangle where to render our glyph
    SDL_Rect dest = {text_x, text_y, glyph_metrics.w, glyph_metrics.h};

    // apply color
    SDL_Color text_color = hex_to_rgba_color(text_hex_color);
    SDL_SetTextureColorMod(font_struct->atlas, text_color.r, text_color.g, text_color.b);

    // render the text
    SDL_RenderCopy(renderer, font_struct->atlas, &src, &dest);

    // increment the amount of characters
    char_amount++;

    // move the position of the text
    text_x += glyph_metrics.advance;

    // move onto the next byte in the text
    current_char += 1;
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

  for (int i = 0; i < 52; i++)
  {
    // store the current character
    char ch = 65 + i;

    // skip over unwanted characters
    if (ch > 90)
    {
      // NOTE(Rami): maybe try advance multiplied?
      ch += 6;
    }

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
    font_struct->metrics[i].x = atlas_rect.x;
    font_struct->metrics[i].y = atlas_rect.y;
    font_struct->metrics[i].w = atlas_rect.w;
    font_struct->metrics[i].h = atlas_rect.h;
    font_struct->metrics[i].advance = advance;

    // advance the rendering location
    x += glyph_surface->w;

    // in case the glyphs go over the width of the atlas
    if (x > 1024)
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

void render_inventory(SDL_Renderer *renderer, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, font_t *font_inventory, font_t *font_item, int *player_inventory_highlight_index, int *player_inventory_current_item_amount)
{
  // render inventory background
  SDL_Rect inventory_rect = {600, 50, 400, 500};
  SDL_RenderCopy(renderer, player_inventory_tex, NULL, &inventory_rect);

  render_text(renderer, font_inventory, 634, 55, "Inventory", 0, COLOR_TEXT_WHITE);

  // item position and the offset
  int item_name_pos_x = 610;
  int item_name_pos_y = 80;
  int item_name_pos_offset = 25;

  int item_window_pos_x = 330;
  int item_window_pos_y = 250;
  int item_window_pos_x_offset = 5;
  int item_window_pos_y_offset = 5;

  // reset count
  *player_inventory_current_item_amount = 0;

  for (int i = 0; i < INVENTORY_AMOUNT; i++)
  {
    if (inventory[i].name[0] != '.')
    {
      // set the current inventory item amount
      *player_inventory_current_item_amount += 1;

      // calculate inventory item letter
      char item_name_index[1] = {97 + i};

      // clean whatever might be in the item_name array and join the index with the item name
      char item_name[80];
      item_name[0] = '\0';
      sprintf(item_name, "%s   %s", item_name_index, inventory[i].name);

      if (*player_inventory_highlight_index == i)
      {
        // render highlighter
        SDL_Rect inventory_highlight_rect = {601, 77 + (item_name_pos_offset * i), 398, 22};
        SDL_RenderCopy(renderer, player_inventory_highlight_tex, NULL, &inventory_highlight_rect);

        // render item window and item information
        SDL_Rect inventory_item_rect = {item_window_pos_x, item_window_pos_y, 250, 300};
        SDL_RenderCopy(renderer, player_inventory_item_tex, NULL, &inventory_item_rect);

        render_text(renderer, font_item, item_window_pos_x + item_window_pos_x_offset, item_window_pos_y + item_window_pos_y_offset, inventory[i].name, 0, COLOR_TEXT_WHITE);
        render_text(renderer, font_item, item_window_pos_x + item_window_pos_x_offset, item_window_pos_y + (item_window_pos_y_offset * 5), inventory[i].use, 0, COLOR_TEXT_GREEN);
        render_text(renderer, font_item, item_window_pos_x + item_window_pos_x_offset, item_window_pos_y + (item_window_pos_y_offset * 10), inventory[i].description, 0, COLOR_TEXT_WHITE);
      }

      render_text(renderer, font_inventory, item_name_pos_x, item_name_pos_y + (item_name_pos_offset * i), item_name, 0, COLOR_TEXT_WHITE);
    }
  }
}

void render_items(SDL_Renderer *renderer, SDL_Texture *itemset_tex, SDL_Rect *camera)
{
  SDL_Rect src, dest;

  src.w = TILE_SIZE;
  src.h = TILE_SIZE;

  dest.w = TILE_SIZE;
  dest.h = TILE_SIZE;

  for (int i = 0; i < ITEMS_AMOUNT; i++)
  {
    if (items[i].active)
    {
      src.x = items[i].tile * TILE_SIZE;
      src.y = 0;

      dest.x = items[i].x - camera->x;
      dest.y = items[i].y - camera->y;

      SDL_RenderCopy(renderer, itemset_tex, &src, &dest);
    }
  }
}

void add_item_into_inventory(entity_t *player_entity)
{
  for (int i = 0; i < ITEMS_AMOUNT; i++)
  {
    item_t *item = &items[i];

    // make sure the item exists
    if (!item->active)
    {
      continue;
    }

    // check that the item and the player are in the same location
    if (item->x == player_entity->x && item->y == player_entity->y)
    {
      for (int i = 0; i < INVENTORY_AMOUNT; i++)
      {
        if (inventory[i].name[0] == '.')
        {
          // copy the item information into the inventory
          inventory[i] = item_info[item->id];

          item->active = 0;

          char message_string[80];
          sprintf(message_string, "You pick up a %s", item_info[item->id].name);

          add_console_message(message_string, COLOR_ACTION);

          return;
        }
      }

      add_console_message("Your inventory is full right now", COLOR_ACTION);

      return;
    }
  }

  add_console_message("You find nothing worthy of picking up", COLOR_ACTION);
}

void render_console_messages(SDL_Renderer *renderer, font_t *font_struct)
{
  SDL_Rect background = {0, 608, 1024, 160};
  SDL_Rect console = {384, 618, 634, 140};

  // render the lower background and the console log rectangle
  SDL_RenderFillRect(renderer, &background);
  SDL_SetRenderDrawColor(renderer, 255, 255, 240, 255);
  SDL_RenderDrawRect(renderer, &console);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  // message position and the offset
  int message_pos_x = 390;
  int message_pos_y = 626;
  int message_pos_offset = 12;

  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    if (console_messages[i].message[0] != '.')
    {
      render_text(renderer, font_struct, message_pos_x, message_pos_y + (i * message_pos_offset), console_messages[i].message, 0, console_messages[i].hex_color);
    }
  }
}

void add_console_message(char *message, int message_color)
{
  // fill the initial space of the console log
  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    if (console_messages[i].message[0] == '.')
    {
      strcpy(console_messages[i].message, message);
      console_messages[i].hex_color = message_color;

      return;
    }
  }

  // remove the oldest message
  console_messages[0].message[0] = '.';
  console_messages[0].hex_color = 0;

  // move all messages starting from the second oldest message to create space for the new message
  for (int i = 1; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    strcpy(console_messages[i - 1].message, console_messages[i].message);
    console_messages[i - 1].hex_color = console_messages[i].hex_color;
  }

  // add the new message to the console log
  strcpy(console_messages[CONSOLE_MESSAGE_AMOUNT - 1].message, message);
  console_messages[CONSOLE_MESSAGE_AMOUNT - 1].hex_color = message_color;

  return;
}

void process_input(unsigned char *map, entity_t *player_entity, int *game_is_running, int *current_key, int *display_inventory, int *player_inventory_highlight_index, int *player_inventory_current_item_amount, int *update_logic)
{
  if (*current_key == SDLK_ESCAPE)
  {
    *game_is_running = 0;
  }
  else if (*display_inventory)
  {
    if (*current_key == SDLK_k)
    {
      if (*player_inventory_highlight_index - 1 < 0)
      {
        *player_inventory_highlight_index = *player_inventory_current_item_amount - 1;
      }
      else
      {
        *player_inventory_highlight_index -= 1;
      }

      *current_key = 0;
    }
    else if (*current_key == SDLK_j)
    {
      if (*player_inventory_highlight_index + 1 > *player_inventory_current_item_amount - 1)
      {
        *player_inventory_highlight_index = *player_inventory_current_item_amount = 0;
      }
      else
      {
        *player_inventory_highlight_index += 1;
      }

      *current_key = 0;
    }
    else if (*current_key == SDLK_i)
    {
      *display_inventory = 0;
      *player_inventory_highlight_index = 0;
      *current_key = 0;
    }
  }
  else if (!*display_inventory)
  {
    if (*current_key == SDLK_k)
    {
      entity_move(map, player_entity, 0, -player_entity->speed * TILE_SIZE, &(*game_is_running));
      *current_key = 0;
      *update_logic = 1;
    }
    else if (*current_key == SDLK_j)
    {
      entity_move(map, player_entity, 0, player_entity->speed * TILE_SIZE, &(*game_is_running));
      *current_key = 0;
      *update_logic = 1;
    }
    else if (*current_key == SDLK_h)
    {
      entity_move(map, player_entity, -player_entity->speed * TILE_SIZE, 0, &(*game_is_running));
      *current_key = 0;
      *update_logic = 1;
    }
    else if (*current_key == SDLK_l)
    {
      entity_move(map, player_entity, player_entity->speed * TILE_SIZE, 0, &(*game_is_running));
      *current_key = 0;
      *update_logic = 1;
    }
    else if (*current_key == SDLK_i)
    {
      *display_inventory = 1;
      *current_key = 0;
    }
    else if (*current_key == SDLK_COMMA)
    {
      add_item_into_inventory(&(*player_entity));
      *current_key = 0;
      *update_logic = 1;
    }
    // NOTE(Rami): for debugging the inventory
    else if (*current_key == SDLK_s)
    {
      items[0].active = 1;
      add_console_message("Item Added To Gameworld", COLOR_SPECIAL);

      *current_key = 0;
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

void process_events(int *game_is_running, int *current_key)
{
  // an event struct to hold the current event information
  SDL_Event current_event;

  // handle events as long as there are any
  while (SDL_PollEvent(&current_event))
  {
    if (current_event.type == SDL_QUIT)
    {
      *game_is_running = 0;
    }
    else if (current_event.type == SDL_KEYDOWN)
    {
      // set our current_key to the key that was pressed down
      *current_key = current_event.key.keysym.sym;
    }
  }
}

// void update_lighting(unsigned char *map, unsigned char *fov_map, entity_t *player)
// {
//   // set all elements as not visible
//   for (int y = 0; y < MAP_SIZE; y++)
//   {
//     for (int x = 0; x < MAP_SIZE; x++)
//     {
//       fov_map[y * MAP_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for (int y = (player->y / TILE_SIZE) - player->view_distance; y < (player->y / TILE_SIZE) + player->view_distance; y++)
//   {
//     for (int x = (player->x / TILE_SIZE) - player->view_distance; x < (player->x / TILE_SIZE) + player->view_distance; x++)
//     {
//       fov_map[y * MAP_SIZE + x] = 255;
//     }
//   }
//   #endif

//   // raycasted lighting
//   #if 0
//   for (int angle = 0; angle < 360; angle++)
//   {
//     // calculate the amount for the ray to progress
//     float dx = 0.1 * cos(angle);
//     float dy = 0.1 * sin(angle);

//     // set the ray to begin from the players location
//     float fx = player->x;
//     float fy = player->y;

//     for (;;)
//     {
//       // add to the rays location the amount we calculated
//       fx += dx;
//       fy += dy;

//       float dist = distance(player->x + 16, player->y + 16, fx, fy);
//       //int idist = dist / 32;

//       //printf("dist_between: %d\n", idist);

//       // if the ray is over the players view distance then stop the ray
//       if (dist > (player->view_distance * TILE_SIZE))
//       {
//         break;
//       }

//       // convert to array valid values
//       int ifx = fx / 32;
//       int ify = fy / 32;

//       // make sure the ray isn't going off the level
//       if (ifx >= 0 && ifx <= MAP_SIZE && ify >= 0 && ify <= MAP_SIZE)
//       {
//         //fov_map[ify * MAP_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov_map[ify * MAP_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if (map[ify * MAP_SIZE + ifx] == TILE_WALL_STONE || map[ify * MAP_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov_map[(player->y / TILE_SIZE) * MAP_SIZE + ((player->x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }

void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Rect *camera, entity_t *player)
{
  SDL_Rect player_src = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect player_dest = {player->x - camera->x, player->y - camera->y, player->w, player->h};

  SDL_RenderCopy(renderer, player_tileset_tex, &player_src, &player_dest);
}

void update_camera(SDL_Rect *camera, entity_t *player)
{
  // center camera on player
  camera->x = player->x - (camera->w / 2);
  camera->y = (player->y + (player->h / 2)) - (camera->h / 2);

  if (camera->x < 0)
  {
    camera->x = 0;
  }

  if (camera->y < 0)
  {
    camera->y = 0;
  }

  if (camera->x >= LEVEL_WIDTH - camera->w)
  {
    camera->x = LEVEL_WIDTH - camera->w;
  }

  if (camera->y >= LEVEL_HEIGHT - camera->h)
  {
    camera->y = LEVEL_HEIGHT - camera->h;
  }
}

void render_level(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera)
{
  // set render target to tilemap
  SDL_SetRenderTarget(renderer, tilemap_tex);

  // clear the old contents of the texture
  SDL_RenderClear(renderer);

  int to_y = (camera->y + camera->h) / TILE_SIZE;
  int to_x = (camera->x + camera->w) / TILE_SIZE;

  for (int y = camera->y / TILE_SIZE; y < to_y; y++)
  {
    for (int x = camera->x / TILE_SIZE; x < to_x; x++)
    {
      SDL_Rect src, dest;

      src.w = TILE_SIZE;
      src.h = TILE_SIZE;

      dest.w = TILE_SIZE;
      dest.h = TILE_SIZE;

      if (map[y * MAP_SIZE + x] == TILE_NONE)
      {
        src.x = TILE_NONE * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_FLOOR_GRASS)
      {
        src.x = TILE_FLOOR_GRASS * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_WALL_STONE)
      {
        src.x = TILE_WALL_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_FLOOR_STONE)
      {
        src.x = TILE_FLOOR_STONE * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
      {
        src.x = TILE_DOOR_CLOSED * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_OPEN)
      {
        src.x = TILE_DOOR_OPEN * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_UP)
      {
        src.x = TILE_STAIRS_UP * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
      {
        src.x = TILE_STAIRS_DOWN * TILE_SIZE;
        src.y = 0;
      }

      dest.x = (x * TILE_SIZE);
      dest.y = (y * TILE_SIZE);

      // NOTE(Rami): continue lighting debugging from here

      //if (fov_map[y * MAP_SIZE + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if (fov_map[y * MAP_SIZE + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(renderer, tileset_tex, &src, &dest);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  SDL_Rect dest = {0, 0, camera->w, camera->h};

  // render to the window
  SDL_RenderCopy(renderer, tilemap_tex, camera, &dest);
}

// NOTE(Rami): the return value is for the x-flip, think about if we really want it
int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running)
{
  int entity_map_pos_x = (entity->x + x) / TILE_SIZE;
  int entity_map_pos_y = (entity->y + y) / TILE_SIZE;

  if (entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_FLOOR_STONE)
    //if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_CLOSED)
    {
      add_console_message("You lean forward and push the heavy door open", COLOR_ACTION);
      map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] = TILE_DOOR_OPEN;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }

    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_UP)
    {
      // NOTE(Rami): end game stuff
      *game_is_running = 0;

      return 0;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_DOWN)
    {
      add_console_message("You descend the ladder..", COLOR_ACTION);
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

entity_t* new_entity(int health_points, int x, int y, int w, int h, int speed, int view_distance)
{
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    if (entities[i] == NULL)
    {
      entities[i] = malloc(sizeof(entity_t));

      entities[i]->health_points = health_points;
      entities[i]->x = x;
      entities[i]->y = y;
      entities[i]->w = w;
      entities[i]->h = h;
      entities[i]->speed = speed;
      entities[i]->view_distance = view_distance;

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
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
    success = 0;
  }
  else
  {
    // create the window
    *window = SDL_CreateWindow("Hamxe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      printf("ERROR: SDL could not create window: %s\n", SDL_GetError());
      success = 0;
    }
    else
    {
      // create the renderer for our window
      *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
      if (renderer == NULL)
      {
        printf("ERROR: SDL could not create a renderer: %s\n", SDL_GetError());
        success = 0;
      }
      else
      {
        // initialize PNG loading
        int image_flags = IMG_INIT_PNG;
        if ((IMG_Init(image_flags) & image_flags) == 0)
        {
          printf("ERROR: SLD image library could not initialize: %s\n", IMG_GetError());
          success = 0;
        }
        else
        {
          if (TTF_Init() < 0)
          {
            printf("ERROR: SDL ttf library could not initialize: %s\n", TTF_GetError());
            success = 0;
          }
        }
      }
    }
  }

  return success;
}

SDL_Texture* load_texture(SDL_Renderer *renderer, const char *string)
{
  SDL_Texture *new_texture = NULL;

  SDL_Surface *loaded_surface = IMG_Load(string);
  if (loaded_surface == NULL)
  {
    printf("ERROR: SDL could not load image %s: %s\n", string, IMG_GetError());
  }
  else
  {
    // create a texture from the surface
    new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if (new_texture == NULL)
    {
      printf("ERROR: SDL could not create a texture from surface: %s\n", SDL_GetError());
    }

    // free old surface
    SDL_FreeSurface(loaded_surface);
  }

  return new_texture;
}

void free_resources(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *itemset_tex, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, player_t *player, font_t *font_console, font_t *font_inventory, font_t *font_item)
{
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    if (entities[i] != NULL)
    {
      free(entities[i]);
    }
  }

  SDL_DestroyTexture(font_console->atlas);
  free(font_console);
  font_console = NULL;

  SDL_DestroyTexture(font_inventory->atlas);
  free(font_inventory);
  font_inventory = NULL;

  SDL_DestroyTexture(font_item->atlas);
  free(font_item);
  font_item = NULL;

  free(player);
  player = NULL;

  SDL_DestroyTexture(tileset_tex);
  tileset_tex = NULL;

  SDL_DestroyTexture(player_tileset_tex);
  player_tileset_tex = NULL;

  SDL_DestroyTexture(itemset_tex);
  itemset_tex = NULL;

  SDL_DestroyTexture(tilemap_tex);
  tilemap_tex = NULL;

  SDL_DestroyTexture(player_inventory_tex);
  player_inventory_tex = NULL;

  SDL_DestroyTexture(player_inventory_highlight_tex);
  player_inventory_highlight_tex = NULL;

  SDL_DestroyTexture(player_inventory_item_tex);
  player_inventory_item_tex = NULL;

  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  // quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}