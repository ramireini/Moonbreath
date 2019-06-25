internal void
add_pop_up_text(char *str, i32 x, i32 y, i32 x_offset, i32 y_offset, text_type type, iv4 color, i32 speed, u32 duration_time, ...)
{
  char str_final[256] = {0};

  va_list arg_list;
  va_start(arg_list, duration_time);
  vsnprintf(str_final, sizeof(str_final), str, arg_list);
  va_end(arg_list);

  for(i32 i = 0; i < POP_UP_TEXT_COUNT; ++i)
  {
    if(!pop_up_text[i].active)
    {
      pop_up_text[i].active = 1;
      strcpy(pop_up_text[i].str, str_final);
      pop_up_text[i].x = x;
      pop_up_text[i].y = y;
      pop_up_text[i].x_offset = x_offset;
      pop_up_text[i].y_offset = y_offset;
      pop_up_text[i].change = 0;
      pop_up_text[i].type = type;
      pop_up_text[i].color = color;
      pop_up_text[i].speed = speed;
      pop_up_text[i].duration_time = duration_time;
      pop_up_text[i].start_time = SDL_GetTicks();
      break;
    }
  }
}

internal void
remove_pop_up_text(i32 i)
{
  memset(&pop_up_text[i], 0, sizeof(pop_up_text_t));
}

internal void
update_pop_up_text()
{
  for(i32 i = 0; i < POP_UP_TEXT_COUNT; ++i)
  {
    if(pop_up_text[i].active)
    {
      if(SDL_GetTicks() < pop_up_text[i].start_time + pop_up_text[i].duration_time)
      {
        pop_up_text[i].change -= pop_up_text[i].speed * game.dt;

        if(pop_up_text[i].type == type_fading)
        {
          pop_up_text[i].color.a -= 400 * game.dt;
        }
      }
      else
      {
        remove_pop_up_text(i);
      }
    }
  }
}

internal void
render_pop_up_text()
{
  for(i32 i = 0; i < POP_UP_TEXT_COUNT; ++i)
  {
    if(pop_up_text[i].active)
    {
      iv2 pos = get_real_position(pop_up_text[i].x, pop_up_text[i].y);
      pos.x += pop_up_text[i].x_offset;
      pos.y -= pop_up_text[i].y_offset;

      render_text(pop_up_text[i].str, v2(pos.x, pos.y + pop_up_text[i].change),
                  pop_up_text[i].color, font[font_clean]);
    }
  }
}