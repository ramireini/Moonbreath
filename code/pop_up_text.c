
internal void
add_pop_up_text(char *str, v2u pos, text_type type, ...)
{
    char str_final[8] = {0};
    
    va_list arg_list;
    va_start(arg_list, type);
    vsnprintf(str_final, sizeof(str_final), str, arg_list);
    va_end(arg_list);
    
    for(u32 i = 0; i < array_count(pop_up_texts); ++i)
    {
        if(!pop_up_texts[i].active)
        {
            pop_up_texts[i].active = true;
            strcpy(pop_up_texts[i].str, str_final);
            pop_up_texts[i].pos = pos;
            
            // NOTE(rami): We assume here that the width is 32.
            pop_up_texts[i].offset = V2u(rand_num(0, 16), -8);
            pop_up_texts[i].change = 0.0f;
            pop_up_texts[i].type = type;
            
            if(type == text_normal_attack)
            {
                pop_up_texts[i].color = color_white;
                pop_up_texts[i].speed = 20.0f;
                pop_up_texts[i].duration_time = 1200;
            }
            else if(type == text_critical_attack)
            {
                pop_up_texts[i].color = color_red;
                pop_up_texts[i].speed = 15.0f;
                pop_up_texts[i].duration_time = 1200;
            }
            else if(type == text_heal)
            {
                pop_up_texts[i].color = color_green;
                pop_up_texts[i].speed = 20.0f;
                pop_up_texts[i].duration_time = 1200;
            }
            
            pop_up_texts[i].start_time = SDL_GetTicks();
            return;
        }
    }
    
    assert(0, "Pop up text array is full");
}

internal void
remove_pop_up_text(pop_up_text_t *pop_up_text)
{
    memset(pop_up_text, 0, sizeof(pop_up_text_t));
}

internal void
update_pop_up_text()
{
    for(u32 i = 0; i < array_count(pop_up_texts); ++i)
    {
        if(pop_up_texts[i].active)
        {
            if(SDL_GetTicks() < pop_up_texts[i].start_time + pop_up_texts[i].duration_time)
            {
                pop_up_texts[i].change -= pop_up_texts[i].speed * game.dt;
                
                if(pop_up_texts[i].type == text_normal_attack)
                {
                    pop_up_texts[i].color.a -= (u32)(300.0f * game.dt);
                }
                else if(pop_up_texts[i].type == text_critical_attack)
                {
                    pop_up_texts[i].color.a -= (u32)(100.0f * game.dt);
                }
                
                if(pop_up_texts[i].color.a > 255)
                {
                    pop_up_texts[i].color.a = 0;
                }
            }
            else
            {
                remove_pop_up_text(&pop_up_texts[i]);
            }
        }
    }
}

internal void
render_pop_up_text()
{
    for(u32 i = 0; i < array_count(pop_up_texts); ++i)
    {
        if(pop_up_texts[i].active)
        {
            v2u pos = get_game_position(pop_up_texts[i].pos);
            pos = V2u_add(pos, pop_up_texts[i].offset);
            
            render_text(pop_up_texts[i].str, V2u(pos.x, pos.y + (u32)pop_up_texts[i].change),
                        pop_up_texts[i].color, fonts[font_classic]);
        }
    }
}