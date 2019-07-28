typedef enum
{
    anim_offset = 5
} sprite_anim_offset;

internal void
update_sprite(sprite_t *sprite)
{
    u32 time_elapsed = SDL_GetTicks();
    if(time_elapsed > (sprite->frame_last_changed + sprite->frame_duration))
    {
        if(sprite->current_frame.x < (sprite->start_frame.x + (sprite->frame_count - 1)))
        {
            ++sprite->current_frame.x;
        }
        else
        {
            sprite->current_frame = sprite->start_frame;
        }
        
        sprite->frame_last_changed = time_elapsed;
    }
}