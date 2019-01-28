#include <interface.h>

console_message_t messages[MESSAGE_COUNT];

void add_console_msg(char *msg, uint32 msg_color, ...)
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
  for(int32 i = 0; i < MESSAGE_COUNT; i++)
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
  for(int32 i = 1; i < MESSAGE_COUNT; i++)
  {
    strcpy(messages[i - 1].msg, messages[i].msg);
    messages[i - 1].msg_color = messages[i].msg_color;
  }

  // add the new message to the console log
  strcpy(messages[MESSAGE_COUNT - 1].msg, msg_final);
  messages[MESSAGE_COUNT - 1].msg_color = msg_color;
  return;
}

void render_interface()
{
  // render the interface stats and the console
  SDL_Rect stats_rect = {0, WINDOW_HEIGHT - 160, 385, 160};
  SDL_Rect console_rect = {stats_rect.x + stats_rect.w, WINDOW_HEIGHT - 160, WINDOW_WIDTH - console_rect.x, 160};
  SDL_RenderCopy(renderer, textures[TEX_INTERFACE_CONSOLE_WIN], NULL, &console_rect);
  SDL_RenderCopy(renderer, textures[TEX_INTERFACE_STATS_WIN], NULL, &stats_rect);

  // statistics position and offset
  int32 stats_x = 10;
  int32 stats_y = WINDOW_HEIGHT - 151;
  int32 stats_offset = 8;

  // render name
  render_text(player->name, stats_x, stats_y, HEX_COLOR_WHITE, fonts[FONT_CLASSIC]);

  // NOTE(Rami): could have a function that makes bars like these,
  // it would make this section cleaner and it's valid since
  // we're having more than one of these things exist
  {
    // render player HP bar
    SDL_Rect hp_bar = {stats_x + (stats_offset * 4), stats_y + (stats_offset * 3), player->entity->hp * 20, 16};

    SDL_SetRenderDrawColor(renderer, RGBA_COLOR_RED);
    SDL_RenderFillRect(renderer, &hp_bar);
    hp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, RGBA_COLOR_WHITE);
    SDL_RenderDrawRect(renderer, &hp_bar);

    //render player XP bar
    SDL_Rect xp_bar = {hp_bar.x, hp_bar.y + stats_offset * 2, player->xp * 20, 16};

    SDL_SetRenderDrawColor(renderer, RGBA_COLOR_YELLOW);
    SDL_RenderFillRect(renderer, &xp_bar);
    xp_bar.w = 200;
    SDL_SetRenderDrawColor(renderer, RGBA_COLOR_WHITE);
    SDL_RenderDrawRect(renderer, &xp_bar);
  }

  render_text("HP          %d/%d", stats_x, stats_y + (stats_offset * 3), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->hp, player->max_hp);
  render_text("XP            %d", stats_x, stats_y + (stats_offset * 5), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->xp);
  render_text("Level: %d", stats_x, stats_y + (stats_offset * 8), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->level);
  render_text("Damage: %d", stats_x, stats_y + (stats_offset * 10), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->damage);
  render_text("Armor: %d", stats_x, stats_y + (stats_offset * 12), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->entity->armor);
  render_text("Turn: %d", stats_x, stats_y + (stats_offset * 14), HEX_COLOR_WHITE, fonts[FONT_CLASSIC], player->turn);

  // render console messages
  int32 msg_x = console_rect.x + 10;
  int32 msg_y = console_rect.y + 8;
  int32 msg_offset = 16;

  for(int32 i = 0; i < MESSAGE_COUNT; i++)
  {
    if(messages[i].msg[0] != '.')
    {
      render_text(messages[i].msg, msg_x, msg_y + (i * msg_offset), messages[i].msg_color, fonts[FONT_CLASSIC]);
    }
  }
}