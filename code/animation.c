enum
{
  anim_min_offset = -5,
  anim_max_offset = 5
};

internal void
update_animation(render_t *render)
{
  u32 time_elapsed = SDL_GetTicks();

  if(time_elapsed > (render->frame_last_changed + render->frame_duration))
  {
    if(render->current_frame.x < (render->start_frame.x + (render->frame_count - 1)))
    {
      render->current_frame.x++;
    }
    else
    {
      render->current_frame = render->start_frame;
    }

    render->frame_last_changed = time_elapsed;
  }
}