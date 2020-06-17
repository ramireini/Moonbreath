typedef enum
{
    DebugVariableType_None,
    
    DebugVariableType_String,
    DebugVariableType_Bool32,
    DebugVariableType_Uint32,
    DebugVariableType_Float32
} DebugVarType;

typedef struct
{
    DebugVarType type;
    char name[32];
    
    union
    {
        b32 *bool32;
        u32 *uint32;
        f32 *float32;
    };
} DebugVariable;

typedef struct
{
    b32 is_created;
    
    char name[32];
    u32 x, y, w, h;
    
    // TTF fonts cannot be used with the debug code currently.
    Font *font;
    Color color;
    
    DebugVariable vars[16];
} DebugGroup;

typedef struct
{
    u32 selected_group_index;
    DebugGroup groups[2];
} DebugState;

internal u32
correct_group_index(u32 index)
{
    u32 result = index + 1;
    return(result);
}

internal void
update_and_render_debug_state(GameState *game, DebugState *state, GameInput *input)
{
    for(u32 group_index = 0; group_index < array_count(state->groups); ++group_index)
    {
        DebugGroup *group = &state->groups[group_index];
        
        if(is_inside_rectangle(input->mouse_pos, V4u(group->x, group->y, group->w, group->h)))
        {
            group->color = Color_LightBlue;
            
            if(is_input_valid(&input->Button_Left))
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
                group->color = Color_LightBlue;
            }
            else
            {
                group->color = Color_White;
            }
        }
        
        render_text(game, group->name, group->x, group->y, group->font, group->color);
        
        // Render Debug Variables
        if(state->selected_group_index == correct_group_index(group_index))
        {
            u32 var_y = group->y + (group->h * 2);
            
            for(u32 var_index = 0; var_index < array_count(group->vars); ++var_index)
            {
                DebugVariable *var = &group->vars[var_index];
                if(var->type)
                {
                    String128 text = {0};
                    
                    switch(var->type)
                    {
                        case DebugVariableType_String:
                        {
                            snprintf(text.str, sizeof(text), "%s", var->name);
                        } break;
                        
                        case DebugVariableType_Bool32:
                        {
                            snprintf(text.str, sizeof(text), "%s: %s", var->name, (*var->bool32 == true) ? "true" : "false");
                        } break;
                        
                        case DebugVariableType_Uint32:
                        {
                            snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->uint32);
                        } break;
                        
                        case DebugVariableType_Float32:
                        {
                            snprintf(text.str, sizeof(text), "%s: %.02f", var->name, *var->float32);
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    render_text(game, text.str, group->x, var_y, group->font, Color_White);
                    var_y += group->h;
                }
            }
        }
    }
}

internal DebugGroup *
add_debug_group(DebugState *state, char *name, u32 x, u32 y, Font *font)
{
    for(u32 group_index = 0; group_index < array_count(state->groups); ++group_index)
    {
        DebugGroup *group = &state->groups[group_index];
        if(!group->is_created)
        {
            group->is_created = true;
            
            strcpy(group->name, name);
            group->x = x;
            group->y = y;
            group->w = strlen(name) * font->shared_glyph_advance;
            group->h = font->size;
            group->font = font;
            group->color = Color_White;
            
            return(group);
        }
    }
}

internal void
add_debug_text(DebugGroup *group, char *text, ...)
{
    char formatted_text[128] = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted_text, sizeof(formatted_text), text, arg_list);
    va_end(arg_list);
    
    for(u32 var_index = 0; var_index < array_count(group->vars); ++var_index)
    {
        DebugVariable *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = DebugVariableType_String;
            strcpy(var->name, formatted_text);
            
            return;
        }
    }
}

internal void
add_debug_bool32(DebugGroup *group, char *name, b32 *bool32)
{
    for(u32 var_index = 0; var_index < array_count(group->vars); ++var_index)
    {
        DebugVariable *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = DebugVariableType_Bool32;
            strcpy(var->name, name);
            var->bool32 = bool32;
            
            return;
        }
    }
}

internal void
add_debug_uint32(DebugGroup *group, char *name, u32 *uint32)
{
    for(u32 var_index = 0; var_index < array_count(group->vars); ++var_index)
    {
        DebugVariable *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = DebugVariableType_Uint32;
            strcpy(var->name, name);
            var->uint32 = uint32;
            
            return;
        }
    }
}

internal void
add_debug_float32(DebugGroup *group, char *name, f32 *float32)
{
    for(u32 var_index = 0; var_index < array_count(group->vars); ++var_index)
    {
        DebugVariable *var = &group->vars[var_index];
        if(!var->type)
        {
            var->type = DebugVariableType_Float32;
            strcpy(var->name, name);
            var->float32 = float32;
            
            return;
        }
    }
}