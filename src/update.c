void input_update()
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
      keyboard.keys[event.key.keysym.scancode] = true;
      player_keypress(event.key.keysym.scancode);
    }
    else if(event.type == SDL_KEYUP)
    {
      keyboard.keys[event.key.keysym.scancode] = false;
    }
  }
}

void camera_update()
{
  game_state.camera.x = tile_mul(player.entity.pos.x) - (game_state.camera.w / 2);
  game_state.camera.y = (tile_mul(player.entity.pos.y) + (player.entity.aspect.h / 2)) - (game_state.camera.h / 2);

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