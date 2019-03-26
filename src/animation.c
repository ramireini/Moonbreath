void animation_update(entity_t *entity)
{
  if(game_state.time_elapsed > entity->anim.frame_last_changed + entity->anim.frame_delay)
  {
    entity->anim.frame_num = (entity->anim.frame_num < (entity->anim.frame_count - 1)) ? entity->anim.frame_num + 1 : 0;
    entity->anim.frame_last_changed = game_state.time_elapsed;
  }
}