typedef enum
{
    DebugVariable_None,
    
    DebugVariableType_NewLine,
    DebugVariableType_String,
    DebugVariableType_Bool32,
    DebugVariableType_Uint32,
    DebugVariableType_Float32,
    DebugVariableType_UintVector2
} DebugVariableType;

typedef struct
{
    DebugVariableType type;
    char name[32];
    
    union
    {
        b32 *b32;
        u32 *u32;
        f32 *f32;
        v2u *v2u;
    };
} DebugVariable;

typedef struct
{
    b32 is_initialized;
    
    char name[32];
    u32 x, y, w, h;
    Font *font;
    Color color;
    
    DebugVariable vars[16];
} DebugGroup;

typedef struct
{
    u32 current_index;
    DebugGroup groups[8];
} DebugState;

internal u32
get_group_index(u32 index)
{
    u32 result = index + 1;
    return(result);
}

internal void
update_and_render_debug_state(Game *game, DebugState *debug, Input *input)
{
    for(u32 index = 0; index < array_count(debug->groups); ++index)
    {
        DebugGroup *group = &debug->groups[index];
        if(group->is_initialized)
        {
            v4u group_name_rect = {group->x, group->y, group->w, group->h};
            if(is_inside_rect(group_name_rect, input->mouse_pos))
            {
                group->color = Color_LightBlue;
                
                if(was_pressed(&input->Button_Left))
                {
                    if(debug->current_index == get_group_index(index))
                    {
                        debug->current_index = 0;
                    }
                    else
                    {
                        debug->current_index = get_group_index(index);
                    }
                }
            }
            else
            {
                if(debug->current_index == get_group_index(index))
                {
                    group->color = Color_LightBlue;
                }
                else
                {
                    group->color = Color_White;
                }
            }
            
            render_text(game, "%s%s", group->x, group->y, group->font, 0, start_color(group->color), group->name);
            
            if(debug->current_index == get_group_index(index))
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
                            case DebugVariableType_NewLine: break;
                            
                            case DebugVariableType_String:
                            {
                                snprintf(text.str, sizeof(text), "%s", var->name);
                            } break;
                            
                            case DebugVariableType_Bool32:
                            {
                                snprintf(text.str, sizeof(text), "%s: %s", var->name, (*var->b32 == true) ? "true" : "false");
                            } break;
                            
                            case DebugVariableType_Uint32:
                            {
                                snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->u32);
                            } break;
                            
                            case DebugVariableType_Float32:
                            {
                                snprintf(text.str, sizeof(text), "%s: %.02f", var->name, *var->f32);
                            } break;
                            
                            case DebugVariableType_UintVector2:
                            {
                                snprintf(text.str, sizeof(text), "%s: %u, %u", var->name, var->v2u->x, var->v2u->y);
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
}

internal DebugGroup *
add_debug_group(DebugState *debug, char *name, u32 x, u32 y, Font *font)
{
    for(u32 group_index = 0; group_index < array_count(debug->groups); ++group_index)
    {
        DebugGroup *group = &debug->groups[group_index];
        if(!group->is_initialized)
        {
            group->is_initialized = true;
            
            strcpy(group->name, name);
            group->x = x;
            group->y = y;
            
            switch(font->type)
            {
                case FontType_BMP:
                {
                    group->w = strlen(name) * font->bmp_advance;
                } break;
                
                case FontType_TTF:
                {
                    group->w += get_text_width(font, name);
                } break;
                
                invalid_default_case;
            }
            
            group->h = font->size;
            group->font = font;
            group->color = Color_White;
            
            return(group);
        }
    }
}

internal void
add_debug_newline(DebugGroup *group)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_NewLine;
            
            return;
        }
    }
}

internal void
add_debug_text(DebugGroup *group, char *text, ...)
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
add_debug_v2u(DebugGroup *group, char *name, v2u *value)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_UintVector2;
            strcpy(var->name, name);
            var->v2u = value;
            
            return;
        }
    }
}

internal void
add_debug_bool32(DebugGroup *group, char *name, b32 *value)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_Bool32;
            strcpy(var->name, name);
            var->b32 = value;
            
            return;
        }
    }
}

internal void
add_debug_uint32(DebugGroup *group, char *name, u32 *value)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_Uint32;
            strcpy(var->name, name);
            var->u32 = value;
            
            return;
        }
    }
}

internal void
add_debug_float32(DebugGroup *group, char *name, f32 *value)
{
    for(u32 index = 0; index < array_count(group->vars); ++index)
    {
        DebugVariable *var = &group->vars[index];
        if(!var->type)
        {
            var->type = DebugVariableType_Float32;
            strcpy(var->name, name);
            var->f32 = value;
            
            return;
        }
    }
}