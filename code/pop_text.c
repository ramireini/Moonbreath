
internal void
add_pop_text(char *text, v2u pos, text_type type, ...)
{
    char text_final[8] = {0};
    
    va_list arg_list;
    va_start(arg_list, type);
    vsnprintf(text_final, sizeof(text_final), text, arg_list);
    va_end(arg_list);
    
    for(u32 i = 0; i < array_count(pop_texts); ++i)
    {
        pop_text_t *pop_text = &pop_texts[i];
        
        if(!pop_text->active)
        {
            pop_text->active = true;
            strcpy(pop_text->str, text_final);
            pop_text->pos = pos;
            
            // NOTE(rami): We assume here that the width is 32.
            pop_text->pos_offset = V2u(rand_num(0, 16), rand_num(-8, 8));
            pop_text->direction = rand_num(left, right);
            pop_text->change = 0.0f;
            pop_text->type = type;
            
            if(type == text_normal_attack)
            {
                pop_text->color = color_white;
                pop_text->speed = 20.0f;
                pop_text->duration_time = 1200;
            }
            else if(type == text_critical_attack)
            {
                pop_text->color = color_red;
                pop_text->speed = 15.0f;
                pop_text->duration_time = 1200;
            }
            else if(type == text_heal)
            {
                pop_text->color = color_green;
                pop_text->speed = 20.0f;
                pop_text->duration_time = 1200;
            }
            
            pop_text->start_time = SDL_GetTicks();
            return;
        }
    }
    
    assert(0, "Pop up text array is full");
}

internal void
remove_pop_text(pop_text_t *pop_text)
{
    memset(pop_text, 0, sizeof(pop_text_t));
}

internal void
update_pop_text()
{
    for(u32 i = 0; i < array_count(pop_texts); ++i)
    {
        pop_text_t *pop_text = &pop_texts[i];
        
        if(pop_text->active)
        {
            if(SDL_GetTicks() < pop_text->start_time + pop_text->duration_time)
            {
                pop_text->change -= pop_text->speed * game.dt;
                
                if(pop_text->type == text_normal_attack)
                {
                    pop_text->color.a -= (u32)(300.0f * game.dt);
                }
                
                if(pop_text->color.a > 255)
                {
                    pop_text->color.a = 0;
                }
            }
            else
            {
                remove_pop_text(pop_text);
            }
        }
    }
}

internal void
render_pop_text()
{
    for(u32 i = 0; i < array_count(pop_texts); ++i)
    {
        pop_text_t *pop_text = &pop_texts[i];
        
        if(pop_text->active)
        {
            v2u pos = get_game_position(pop_text->pos);
            pos = V2u_add(pos, pop_text->pos_offset);
            pos.y += (u32)pop_text->change;
            
            render_text(pop_text->str, pos, pop_text->color, fonts[font_classic_outlined]);
        }
    }
}