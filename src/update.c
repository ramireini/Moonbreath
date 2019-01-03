#include <update.h>

int32 attack_entity(entity_t *attacker, entity_t *target)
{
  target->hp -= attacker->damage;
  if(target->hp <= 0)
  {
    return 1;
  }

  return 0;
}

void update_input(char *level)
{
  if(key_pressed == SDLK_ESCAPE)
  {
    // NOTE(Rami): Delete later.
    printf("SDLK_ESCAPE\n");
    game_is_running = false;
  }

  /* - IN INVENTORY - */

  else if(player->inventory_display)
  {
    switch(key_pressed)
    {
      case SDLK_k:
      {
        // if the highlight index can't go any lower
        // meaning that this is the top most item we can be on
        if(player->inventory_item_selected - 1 < 0)
        {
          // then if we have more than one item in the inventory
          if(player->inventory_item_count > 0)
          {
            // set the highlight index to be the last item
            player->inventory_item_selected = player->inventory_item_count - 1;
          }
        }
        // else we can substract because we're not on the top most item
        else
        {
          (player->inventory_item_selected)--;
        }
      } break;

      case SDLK_j:
      {
        // if the highlight index can't go any higher
        // meaning that this is the bottom item we can be on
        if(player->inventory_item_selected + 1 > player->inventory_item_count - 1)
        {
          // set the highlight index to the first item
          player->inventory_item_selected = player->inventory_item_count = 0;
        }
        // else we can add because we're not on the most bottom item
        else
        {
          (player->inventory_item_selected)++;
        }
      } break;

      case SDLK_i:
      {
        // do not display inventory anymore
        // reset highlight index
        player->inventory_display = false;
        player->inventory_item_selected = 0;
      } break;

      case SDLK_d:
      {
        drop_or_remove_item(0);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_item_selected + 1 == player->inventory_item_count)
        {
          if(player->inventory_item_selected - 1 >= 0)
          {
            player->inventory_item_selected--;
          }
        }
      } break;

      case SDLK_e:
      {
        equip_or_unequip_item(player);
      } break;

      case SDLK_c:
      {
        consume_item(player);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_item_selected + 1 == player->inventory_item_count)
        {
          if(player->inventory_item_selected - 1 >= 0)
          {
            player->inventory_item_selected--;
          }
        }
      } break;
    }
  }

  /* - NOT IN INVENTORY - */

  else if(!player->inventory_display)
  {
    switch(key_pressed)
    {
      // Up
      case SDLK_k:
      {
        player->new_x = player->entity->x;
        player->new_y = player->entity->y - TILE_SIZE;
        turn_changed = true;
      } break;

      // Down
      case SDLK_j:
      {
        player->new_x = player->entity->x;
        player->new_y = player->entity->y + TILE_SIZE;
        turn_changed = true;
      } break;

      // Left
      case SDLK_h:
      {
        player->new_x = player->entity->x - TILE_SIZE;
        player->new_y = player->entity->y;
        turn_changed = true;
      } break;

      // Right
      case SDLK_l:
      {
        player->new_x = player->entity->x + TILE_SIZE;
        player->new_y = player->entity->y;
        turn_changed = true;
      } break;

      // Left Up
      case SDLK_y:
      {
        player->new_x = player->entity->x - TILE_SIZE;
        player->new_y = player->entity->y - TILE_SIZE;
        turn_changed = true;
      } break;

      // Right Up
      case SDLK_u:
      {
        player->new_x = player->entity->x + TILE_SIZE;
        player->new_y = player->entity->y - TILE_SIZE;
        turn_changed = true;
      } break;

      // Left Down
      case SDLK_b:
      {
        player->new_x = player->entity->x - TILE_SIZE;
        player->new_y = player->entity->y + TILE_SIZE;
        turn_changed = true;
      } break;

      // Right Down
      case SDLK_n:
      {
        player->new_x = player->entity->x + TILE_SIZE;
        player->new_y = player->entity->y + TILE_SIZE;
        turn_changed = true;
      } break;

      // NOTE(Rami): advance one turn, for testing
      case SDLK_t:
      {
        turn_changed = true;
      } break;

      case SDLK_i:
      {
        player->inventory_display = true;
      } break;

      case SDLK_COMMA:
      {
        add_inventory_item(player);
      } break;

      case SDLK_d:
      {
        if(is_tile_close(level, player->entity->x, player->entity->y, TILE_PATH_DOWN))
        {
          add_console_msg("You travel deeper into the mountain..", HEX_COLOR_WHITE);
          generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2);
          turn_changed = true;
        }
      } break;

      case SDLK_a:
      {
        if(is_tile_close(level, player->entity->x, player->entity->y, TILE_PATH_UP))
        {
          printf("You flee from the mountain..\n");
          game_is_running = false;
        }
      } break;
    }
  }

  key_pressed = 0;
}

void update_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      // NOTE(Rami):
      printf("SDL_QUIT\n");
      game_is_running = false;
    }
    else if(event.type == SDL_KEYDOWN && !event.key.repeat)
    {
      key_pressed = event.key.keysym.sym;
    }
  }
}

void update_camera()
{
  // Center camera on player
  camera.x = player->entity->x - (camera.w / 2);
  camera.y = (player->entity->y + (player->entity->h / 2)) - (camera.h / 2);

  // Stop the camera if it goes outside the level
  if(camera.x < 0)
  {
    camera.x = 0;
  }

  if(camera.y < 0)
  {
    camera.y = 0;
  }

  if(camera.x >= LEVEL_WIDTH - camera.w)
  {
    camera.x = LEVEL_WIDTH - camera.w;
  }

  if(camera.y >= LEVEL_HEIGHT - camera.h)
  {
    camera.y = LEVEL_HEIGHT - camera.h;
  }
}

// void update_lighting(char *level, char *fov)
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
//       if(dist > to_pixels(player->fov))
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