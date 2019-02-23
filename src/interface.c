console_message_t messages[MESSAGE_COUNT];

void add_console_msg(char *msg, SDL_Color c, ...)
{
  char msg_final[256];

  va_list arg_list;
  va_start(arg_list, c);
  vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);
  va_end(arg_list);

  for(i32 i = 0; i < MESSAGE_COUNT; i++)
  {
    if(messages[i].msg[0] == '.')
    {
      strcpy(messages[i].msg, msg_final);
      messages[i].color = c;
      return;
    }
  }

  messages[0].msg[0] = '.';
  messages[0].color = RGBA_COLOR_NONE_S;

  for(i32 i = 1; i < MESSAGE_COUNT; i++)
  {
    strcpy(messages[i - 1].msg, messages[i].msg);
    messages[i - 1].color = messages[i].color;
  }

  strcpy(messages[MESSAGE_COUNT - 1].msg, msg_final);
  messages[MESSAGE_COUNT - 1].color = c;
  return;
}

void render_interface()
{
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 385, 160};
  SDL_Rect console_rect = {stats_rect.x + stats_rect.w, WINDOW_HEIGHT - 160, WINDOW_WIDTH - console_rect.x, 160};
  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_interface_console_win], NULL, &console_rect);
  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_interface_stats_win], NULL, &stats_rect);

  i32 stats_x = 10;
  i32 stats_y = WINDOW_HEIGHT - 151;
  i32 stats_offset = 8;

  render_text(player->name, stats_x, stats_y, RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic]);

  // NOTE(Rami): could have a function that makes bars like these,
  // it would make this section cleaner and it's valid since
  // we're having more than one of these things exist
  {
    SDL_Rect hp_bar = {stats_x + (stats_offset * 4), stats_y + (stats_offset * 3), player->entity->hp * 20, 16};

    SDL_SetRenderDrawColor(global_state.renderer, RGBA_COLOR_RED_P);
    SDL_RenderFillRect(global_state.renderer, &hp_bar);
    hp_bar.w = 200;
    SDL_SetRenderDrawColor(global_state.renderer, RGBA_COLOR_WHITE_P);
    SDL_RenderDrawRect(global_state.renderer, &hp_bar);

    SDL_Rect xp_bar = {hp_bar.x, hp_bar.y + stats_offset * 2, player->xp * 20, 16};

    SDL_SetRenderDrawColor(global_state.renderer, RGBA_COLOR_YELLOW_P);
    SDL_RenderFillRect(global_state.renderer, &xp_bar);
    xp_bar.w = 200;
    SDL_SetRenderDrawColor(global_state.renderer, RGBA_COLOR_WHITE_P);
    SDL_RenderDrawRect(global_state.renderer, &xp_bar);
  }

  render_text("HP          %d/%d", stats_x, stats_y + (stats_offset * 3), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->entity->hp, player->max_hp);
  render_text("XP            %d", stats_x, stats_y + (stats_offset * 5), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->xp);
  render_text("Level: %d", stats_x, stats_y + (stats_offset * 8), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->level);
  render_text("Damage: %d", stats_x, stats_y + (stats_offset * 10), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->entity->damage);
  render_text("Armor: %d", stats_x, stats_y + (stats_offset * 12), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->entity->armor);
  render_text("Turn: %d", stats_x, stats_y + (stats_offset * 14), RGBA_COLOR_WHITE_S, global_state.assets.fonts[font_classic], player->turn);

  i32 msg_x = console_rect.x + 10;
  i32 msg_y = console_rect.y + 8;
  i32 msg_offset = 16;

  for(i32 i = 0; i < MESSAGE_COUNT; i++)
  {
    if(messages[i].msg[0] != '.')
    {
      render_text(messages[i].msg, msg_x, msg_y + (i * msg_offset), messages[i].color, global_state.assets.fonts[font_classic]);
    }
  }
}