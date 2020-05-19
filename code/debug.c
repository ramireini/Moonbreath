typedef enum
{
    debug_var_type_none,
    
    debug_var_type_text,
    debug_var_type_bool32,
    debug_var_type_uint32,
    debug_var_type_float32
} debug_var_type;

typedef struct
{
    debug_var_type type;
    char name[32];
    
    union
    {
        b32 *bool32;
        u32 *uint32;
        f32 *float32;
    };
} debug_var_t;

typedef struct
{
    b32 is_created;
    
    char name[32];
    u32 x, y, w, h;
    
    // NOTE(rami): TTF fonts cannot be used with the debug code currently.
    font_t *font;
    v4u color;
    
    debug_var_t vars[16];
} debug_group_t;

typedef struct
{
    u32 selected_group_index;
    debug_group_t groups[2];
} debug_state_t;

internal u32
correct_group_index(u32 index)
{
    u32 result = index + 1;
    return(result);
}

internal void
update_and_render_debug_state(game_state_t *game, game_input_t *input, debug_state_t *state)
{
    for(u32 group_index = 0;
        group_index < array_count(state->groups);
        ++group_index)
    {
        debug_group_t *group = &state->groups[group_index];
        
        if(is_inside_rectangle(input->mouse_pos,
                               V4u(group->x, group->y, group->w, group->h)))
        {
            group->color = color_light_blue;
            
            if(is_input_valid(&input->mouse[button_left]))
            {
                if(state->selected_group_index == correct_group_index(group_index))
                {
                    state->selected_group_index = 0;
                }
                else
                {
                    state->selected_group_index = correct_group_index(group_index);
                }
            }
        }
        else
        {
            if(state->selected_group_index == correct_group_index(group_index))
            {
                group->color = color_light_blue;
            }
            else
            {
                group->color = color_white;
            }
        }
        
        render_text(game, group->name, group->x, group->y, group->font, group->color);
        
        // Render Debug Variables
        if(state->selected_group_index == correct_group_index(group_index))
        {
            u32 var_y = group->y + (group->h * 2);
            
            for(u32 var_index = 0;
                var_index < array_count(group->vars);
                ++var_index)
            {
                debug_var_t *var = &group->vars[var_index];
                if(var->type)
                {
                    char text[128] = {0};
                    
                    switch(var->type)
                    {
                        case debug_var_type_text:
                        {
                            snprintf(text, sizeof(text), "%s", var->name);
                        } break;
                        
                        case debug_var_type_bool32:
                        {
                            snprintf(text, sizeof(text), "%s: %s", var->name, (*var->bool32 == true) ? "true" : "false");
                        } break;
                        
                        case debug_var_type_uint32:
                        {
                            snprintf(text, sizeof(text), "%s: %u", var->name, *var->uint32);
                        } break;
                        
                        case debug_var_type_float32:
                        {
                            snprintf(text, sizeof(text), "%s: %.02f", var->name, *var->float32);
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    render_text(game, text, group->x, var_y, group->font, color_white);
                    var_y += group->h;
                }
            }
        }
    }
}

internal debug_group_t *
add_debug_group(debug_state_t *state, char *name, u32 x, u32 y, font_t *font)
{
    for(u32 group_index = 0;
        group_index < array_count(state->groups);
        ++group_index)
    {
        debug_group_t *group = &state->groups[group_index];
        if(!group->is_created)
        {
            group->is_created = true;
            
            strcpy(group->name, name);
            group->x = x;
            group->y = y;
            group->w = strlen(name) * font->shared_glyph_advance;
            group->h = font->size;
            group->font = font;
            group->color = color_white;
            
            return(group);
        }
    }
}

internal void
add_debug_text(debug_group_t *group, char *text)
{
    for(u32 var_index = 0;
        var_index < array_count(group->vars);
        ++var_index)
    {
        debug_var_t *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = debug_var_type_text;
            strcpy(var->name, text);
            
            return;
        }
    }
}

internal void
add_debug_bool32(debug_group_t *group, char *name, b32 *bool32)
{
    for(u32 var_index = 0;
        var_index < array_count(group->vars);
        ++var_index)
    {
        debug_var_t *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = debug_var_type_bool32;
            strcpy(var->name, name);
            var->bool32 = bool32;
            
            return;
        }
    }
}

internal void
add_debug_uint32(debug_group_t *group, char *name, u32 *uint32)
{
    for(u32 var_index = 0;
        var_index < array_count(group->vars);
        ++var_index)
    {
        debug_var_t *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = debug_var_type_uint32;
            strcpy(var->name, name);
            var->uint32 = uint32;
            
            return;
        }
    }
}

internal void
add_debug_float32(debug_group_t *group, char *name, f32 *float32)
{
    for(u32 var_index = 0;
        var_index < array_count(group->vars);
        ++var_index)
    {
        debug_var_t *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = debug_var_type_float32;
            strcpy(var->name, name);
            var->float32 = float32;
            
            return;
        }
    }
}