void update_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      game_state.game_is_running = false;
    }
    #if MOONBREATH_DEBUG
    else if(event.type == SDL_KEYDOWN)
    #else
    else if(event.type == SDL_KEYDOWN && !event.key.repeat)
    #endif
    {
      game_state.keyboard.keys[event.key.keysym.scancode] = 1;
      player_keypress(event.key.keysym.scancode);
    }
    else if(event.type == SDL_KEYUP)
    {
      game_state.keyboard.keys[event.key.keysym.scancode] = 0;
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