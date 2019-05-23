enum
{
  anim_min_offset = -5,
  anim_max_offset = 5
};

internal void
update_animation(entity_t *entity)
{
  if(game.time_elapsed > entity->anim.frame_last_changed + entity->anim.frame_delay)
  {
    if(entity->anim.frame_num < (entity->anim.frame_count - 1))
    {
      entity->anim.frame_num++;
    }
    else
    {
      entity->anim.frame_num = 0;
    }

    entity->anim.frame_last_changed = game.time_elapsed;
  }
}