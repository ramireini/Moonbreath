enum
{
  anim_min_offset = -5,
  anim_max_offset = 5
};

internal void
update_animation(render_t *render)
{
  if(game.time_elapsed > (render->frame_last_changed + render->frame_delay))
  {
    if(render->frame_current.x < (render->frame_start.x + (render->frame_count - 1)))
    {
      render->frame_current.x++;
    }
    else
    {
      render->frame_current = render->frame_start;
    }

    render->frame_last_changed = game.time_elapsed;
  }
}