i32 attack_entity(entity_t *attacker, entity_t *target)
{
  target->hp -= attacker->damage;
  if(target->hp <= 0)
  {
    return 1;
  }

  return 0;
}

void update_input()
{
  SDL_PumpEvents();
  if(game_state.keyboard.is_pressed[SDL_SCANCODE_ESCAPE])
  {
    // NOTE(Rami): For debugging, delete later.
    debug("SDL_SCANCODE_ESCAPE");
    game_state.game_is_running = false;
  }

  /* - IN INVENTORY - */
  else if(player->inventory_is_open)
  {
    if(key_is_pressed(SDL_SCANCODE_K))
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
    }
    else if(key_is_pressed(SDL_SCANCODE_J))
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
    }
    else if(key_is_pressed(SDL_SCANCODE_I))
    {
      player->inventory_is_open = false;
      player->inventory_item_selected = 0;
    }
    else if(key_is_pressed(SDL_SCANCODE_D))
    {
      drop_item();

      // if the bottom item of the inventory got dropped, make the highlighter go up by one
      if(player->inventory_item_selected + 1 == player->inventory_item_count)
      {
        if(player->inventory_item_selected - 1 >= 0)
        {
          player->inventory_item_selected--;
        }
      }
    }
    else if(key_is_pressed(SDL_SCANCODE_E))
    {
      equip_toggle_item();
    }
    else if(key_is_pressed(SDL_SCANCODE_C))
    {
      consume_item();

      // if the bottom item of the inventory got dropped, make the highlighter go up by one
      if(player->inventory_item_selected + 1 == player->inventory_item_count)
      {
        if(player->inventory_item_selected - 1 >= 0)
        {
          player->inventory_item_selected--;
        }
      }
    }
  }

  /* - NOT IN INVENTORY - */
  else if(!player->inventory_is_open)
  {
    if(key_is_pressed(SDL_SCANCODE_K))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x, player->entity->pos.y - 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_J))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x, player->entity->pos.y + 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_H))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x - 1, player->entity->pos.y};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_L))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x + 1, player->entity->pos.y};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_Y))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x - 1, player->entity->pos.y - 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_U))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x + 1, player->entity->pos.y - 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_B))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x - 1, player->entity->pos.y + 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_N))
    {
      player->entity->new_pos = (iv2_t){player->entity->pos.x + 1, player->entity->pos.y + 1};
      game_state.turn_changed = true;
    }
    else if(key_is_pressed(SDL_SCANCODE_T))
    {
      game_state.turn_changed = true;      
    }
    else if(key_is_pressed(SDL_SCANCODE_I))
    {
      player->inventory_is_open = !player->inventory_is_open;
    }
    else if(key_is_pressed(SDL_SCANCODE_COMMA))
    {
      add_inventory_item();
    }
    else if(key_is_pressed(SDL_SCANCODE_D))
    {
      if(tile_is_close(player->entity->pos, tile_path_down))
      {
        // NOTE(Rami): Enable later.
        // add_console_msg("You travel deeper into the mountain..", HEX_COLOR_WHITE);
        // generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2);
        // game_state.turn_changed = true;
      }
    }
    else if(key_is_pressed(SDL_SCANCODE_A))
    {
      if(tile_is_close(player->entity->pos, tile_path_up))
      {
        debug("You flee from the mountain..\n");
        game_state.game_is_running = false;
      }
    }
  }
}

void update_camera()
{
  game_state.camera.x = tile_mul(player->entity->pos.x) - (game_state.camera.w / 2);
  game_state.camera.y = (tile_mul(player->entity->pos.y) + (player->entity->aspect.h / 2)) - (game_state.camera.h / 2);

  if(game_state.camera.x < 0)
  {
    game_state.camera.x = 0;
  }

  if(game_state.camera.y < 0)
  {
    game_state.camera.y = 0;
  }

  if(game_state.camera.x >= LEVEL_WIDTH_IN_PIXELS - game_state.camera.w)
  {
    game_state.camera.x = LEVEL_WIDTH_IN_PIXELS - game_state.camera.w;
  }

  if(game_state.camera.y >= LEVEL_HEIGHT_IN_PIXELS - game_state.camera.h)
  {
    game_state.camera.y = LEVEL_HEIGHT_IN_PIXELS - game_state.camera.h;
  }
}

// void update_lighting()
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

//       //debug("dist_between: %d\n", idist);

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