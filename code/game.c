internal void
update_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      game.state = state_quit;
    }
  #if MOONBREATH_DEBUG
    else if(event.type == SDL_KEYDOWN)
  #else
    else if(event.type == SDL_KEYDOWN && !event.key.repeat)
  #endif
    {
      keyboard.keys[event.key.keysym.scancode] = 1;
      player_keypress(event.key.keysym.scancode);
    }
    else if(event.type == SDL_KEYUP)
    {
      keyboard.keys[event.key.keysym.scancode] = 0;
    }
  }
}

internal void
update_camera()
{
  game.camera.x = tile_mul(player.x) - (game.camera.w / 2);
  game.camera.y = (tile_mul(player.y) + (player.h / 2)) - (game.camera.h / 2);

  if(game.camera.x < 0)
  {
    game.camera.x = 0;
  }

  if(game.camera.y < 0)
  {
    game.camera.y = 0;
  }

  if(game.camera.x >= LEVEL_PIXEL_WIDTH - game.camera.w)
  {
    game.camera.x = LEVEL_PIXEL_WIDTH - game.camera.w;
  }

  if(game.camera.y >= LEVEL_PIXEL_HEIGHT - game.camera.h)
  {
    game.camera.y = LEVEL_PIXEL_HEIGHT - game.camera.h;
  }
}