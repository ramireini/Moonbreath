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
    b32 is_initialized;
    
    char name[32];
    u32 x, y, w, h;
    Font *font; // TTF fonts cannot be used currently.
    Color color;
    
    DebugVariable vars[16];
} DebugGroup;

typedef struct
{
    u32 index;
    DebugGroup groups[2];
} DebugState;

internal u32
get_group_index(u32 index)
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
        
        v4u group_name_rect = {group->x, group->y, group->w, group->h};
        if(is_inside_rect(group_name_rect, input->mouse_pos))
        {
            group->color = Color_LightBlue;
            
            if(was_pressed(&input->mouse[Button_Left], input->fkey_active))
            {
                if(state->index == get_group_index(group_index))
                {
                    state->index = 0;
                }
                else
                {
                    state->index = get_group_index(group_index);
                }
            }
        }
        else
        {
            if(state->index == get_group_index(group_index))
            {
                group->color = Color_LightBlue;
            }
            else
            {
                group->color = Color_White;
            }
        }
        
        render_text(game, "%s%s", group->x, group->y, group->font, 0, start_color(group->color), group->name);
        
        if(state->index == get_group_index(group_index))
        {
            u32 var_y = group->y + (group->h * 2);
            
            for(u32 index = 0; index < array_count(group->vars); ++index)
            {
                DebugVariable *var = &group->vars[index];
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
                    
                    render_text(game, text.str, group->x, var_y, group->font, 0);
                    var_y += group->h;
                }
            }
        }
    }
}

internal DebugGroup *
debug_group(DebugState *state, char *name, u32 x, u32 y, Font *font)
{
    for(u32 group_index = 0; group_index < array_count(state->groups); ++group_index)
    {
        DebugGroup *group = &state->groups[group_index];
        if(!group->is_initialized)
        {
            group->is_initialized = true;
            
            strcpy(group->name, name);
            group->x = x;
            group->y = y;
            group->w = strlen(name) * font->shared_advance;
            group->h = font->size;
            group->font = font;
            group->color = Color_White;
            
            return(group);
        }
    }
}

internal void
debug_text(DebugGroup *group, char *text, ...)
{
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted.str, sizeof(formatted.str), text, arg_list);
    va_end(arg_list);
    
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_String;
            strcpy(var->name, formatted.str);
            
            return;
        }
    }
}

internal void
debug_bool32(DebugGroup *group, char *name, b32 *bool32)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
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
debug_uint32(DebugGroup *group, char *name, u32 *uint32)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
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
debug_float32(DebugGroup *group, char *name, f32 *float32)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_Float32;
            strcpy(var->name, name);
            var->float32 = float32;
            
            return;
        }
    }
}