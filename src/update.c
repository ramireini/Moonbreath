#include <update.h>

void update_add_console_msg(char *msg, int msg_color, ...)
{
  // holds the final message
  char msg_final[256];

  // create an argument list and initialize it
  // to take arguments after the msg_color parameter
  va_list arg_list;
  va_start(arg_list, msg_color);

  // print str to the str_final array and
  // add the format specifiers from arg_list
  vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);

  // close the argument list
  va_end(arg_list);

  // fill the initial space of the console log
  for(int i = 0; i < MESSAGE_COUNT; i++)
  {
    if(messages[i].msg[0] == '.')
    {
      strcpy(messages[i].msg, msg_final);
      messages[i].msg_color = msg_color;

      return;
    }
  }

  // remove the oldest message
  messages[0].msg[0] = '.';
  messages[0].msg_color = 0;

  // move all messages starting from the second oldest message to create space for the new message
  for(int i = 1; i < MESSAGE_COUNT; i++)
  {
    strcpy(messages[i - 1].msg, messages[i].msg);
    messages[i - 1].msg_color = messages[i].msg_color;
  }

  // add the new message to the console log
  strcpy(messages[MESSAGE_COUNT - 1].msg, msg_final);
  messages[MESSAGE_COUNT - 1].msg_color = msg_color;

  return;
}

void update_input(char *level, player_t *player)
{
  if(key_pressed == SDLK_ESCAPE)
  {
    // NOTE(Rami): remove later
    printf("SDLK_ESCAPE\n");
    game_is_running = 0;
  }

  /* -- IN INVENTORY -- */

  else if(player->inventory_display)
  {
    switch(key_pressed)
    {
      case SDLK_k:
      {
        // if the highlight index can't go any lower
        // meaning that this is the top most item we can be on
        if(player->inventory_hl_index - 1 < 0)
        {
          // then if we have more than one item in the inventory
          if(player->inventory_item_count > 0)
          {
            // set the highlight index to be the last item
            player->inventory_hl_index = player->inventory_item_count - 1;
          }
        }
        // else we can substract because we're not on the top most item
        else
        {
          (player->inventory_hl_index)--;
        }

        key_pressed = 0;
      } break;

      case SDLK_j:
      {
        // if the highlight index can't go any higher
        // meaning that this is the bottom item we can be on
        if(player->inventory_hl_index + 1 > player->inventory_item_count - 1)
        {
          // set the highlight index to the first item
          player->inventory_hl_index = player->inventory_item_count = 0;
        }
        // else we can add because we're not on the most bottom item
        else
        {
          (player->inventory_hl_index)++;
        }

        key_pressed = 0;
      } break;

      case SDLK_i:
      {
        // do not display inventory anymore
        // reset highlight index
        player->inventory_display = 0;
        player->inventory_hl_index = 0;

        key_pressed = 0;
      } break;

      case SDLK_d:
      {
        drop_or_remove_inventory_item(player, 1);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_hl_index + 1 == player->inventory_item_count)
        {
          if(player->inventory_hl_index - 1 >= 0)
          {
            player->inventory_hl_index--;
          }
        }

        key_pressed = 0;
      } break;

      case SDLK_e:
      {
        equip_or_unequip_item(player);

        key_pressed = 0;
      } break;

      case SDLK_c:
      {
        consume_item(player);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_hl_index + 1 == player->inventory_item_count)
        {
          if(player->inventory_hl_index - 1 >= 0)
          {
            player->inventory_hl_index--;
          }
        }

        key_pressed = 0;
      } break;
    }
  }

  /* -- NOT IN INVENTORY -- */

  else if(!player->inventory_display)
  {
    switch(key_pressed)
    {
      case SDLK_k:
      {
        update_entity_move(level, player, 0, to_pixels(-player->speed));
        key_pressed = 0;
      } break;

      case SDLK_j:
      {
        update_entity_move(level, player, 0, to_pixels(player->speed));

        key_pressed = 0;
      } break;

      case SDLK_h:
      {
        update_entity_move(level, player, to_pixels(-player->speed), 0);

        key_pressed = 0;
      } break;

      case SDLK_l:
      {
        update_entity_move(level, player, to_pixels(player->speed), 0);

        key_pressed = 0;
      } break;

      case SDLK_i:
      {
        player->inventory_display = 1;

        key_pressed = 0;
      } break;

      case SDLK_COMMA:
      {
        add_inventory_item(player);

        key_pressed = 0;
      } break;

      case SDLK_d:
      {
        int p_x = to_tiles(player->x);
        int p_y = to_tiles(player->y);

        // if path is next to the player horizontally or vertically
        if(level[(p_y * LEVEL_SIZE) + (p_x + 1)] == TILE_PATH_DOWN ||
           level[(p_y * LEVEL_SIZE) + (p_x - 1)] == TILE_PATH_DOWN ||
           level[((p_y + 1) * LEVEL_SIZE) + p_x] == TILE_PATH_DOWN ||
           level[((p_y - 1) * LEVEL_SIZE) + p_x] == TILE_PATH_DOWN)
        {
          update_add_console_msg("You travel deeper into the mountain..", TEXT_COLOR_WHITE);
          generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2, player);
        }

        key_pressed = 0;
      } break;

      case SDLK_a:
      {
        int p_x = to_tiles(player->x);
        int p_y = to_tiles(player->y);

        // if path is next to the player horizontally or vertically
        if(level[(p_y * LEVEL_SIZE) + (p_x + 1)] == TILE_PATH_UP ||
           level[(p_y * LEVEL_SIZE) + (p_x - 1)] == TILE_PATH_UP ||
           level[((p_y + 1) * LEVEL_SIZE) + p_x] == TILE_PATH_UP ||
           level[((p_y - 1) * LEVEL_SIZE) + p_x] == TILE_PATH_UP)
        {
          printf("You flee from the mountain..\n");
          game_is_running = 0;
        }

        key_pressed = 0;
      } break;
    }
  }
}

void update_events()
{
  // event struct to hold current event information
  SDL_Event event;
  SDL_WaitEvent(&event);

  if(event.type == SDL_QUIT)
  {
    // NOTE(Rami): remove later
    printf("SDL_QUIT\n");
    game_is_running = 0;
  }
  else if(event.type == SDL_KEYDOWN && event.key.repeat == 0)
  {
    // set our key_pressed to the key that was pressed down
    key_pressed = event.key.keysym.sym;
  }
}

void update_camera(SDL_Rect *camera, player_t *player)
{
  // center camera on player
  camera->x = player->x - (camera->w / 2);
  camera->y = (player->y + (player->h / 2)) - (camera->h / 2);

  // stop the camera if it goes out of bounds
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

// NOTE(Rami): the return value is for the x-flip, think about if we really want it
void update_entity_move(char *level, player_t *entity, int x, int y)
{
  int entity_x = (entity->x + x) / TILE_SIZE;
  int entity_y = (entity->y + y) / TILE_SIZE;

  if(entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    // NOTE(Rami): 
    // if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_FLOOR_STONE)
    if(level[(entity_y * LEVEL_SIZE) + entity_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_DOOR_CLOSED)
    {
      update_add_console_msg("You lean forward and push the door open", TEXT_COLOR_WHITE);
      level[(entity_y * LEVEL_SIZE) + entity_x] = TILE_DOOR_OPEN;
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_PATH_UP)
    {
      update_add_console_msg("A path that leads back to the surface.. [A]scend and flee the mountain?", TEXT_COLOR_WHITE);
    }
    else if(level[(entity_y * LEVEL_SIZE) + entity_x] == TILE_PATH_DOWN)
    {
      update_add_console_msg("A path that leads further downwards.. [D]escend?", TEXT_COLOR_WHITE);
    }
  }
}

// void update_lighting(char *level, char *fov, player_t *player)
// {
//   // set all elements as not visible
//   for(int y = 0; y < LEVEL_SIZE; y++)
//   {
//     for(int x = 0; x < LEVEL_SIZE; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for(int y = (player.y / TILE_SIZE) - player.fov; y < (player.y / TILE_SIZE) + player.fov; y++)
//   {
//     for(int x = (player.x / TILE_SIZE) - player.fov; x < (player.x / TILE_SIZE) + player.fov; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 255;
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
//       if(ifx >= 0 && ifx <= LEVEL_SIZE && ify >= 0 && ify <= LEVEL_SIZE)
//       {
//         //fov[ify * LEVEL_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov[ify * LEVEL_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if(level[ify * LEVEL_SIZE + ifx] == TILE_WALL_STONE || level[ify * LEVEL_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov[(player.y / TILE_SIZE) * LEVEL_SIZE + ((player.x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }