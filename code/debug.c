typedef enum
{
    DebugVariableType_Newline,
    DebugVariableType_Group,
    DebugVariableType_Text,
    
    DebugVariableType_U32,
    DebugVariableType_B32,
    DebugVariableType_F32,
    DebugVariableType_V2U
    } DebugVariableType;

typedef struct DebugVariable DebugVariable;

typedef struct
{
    b32 is_expanded;
    
    DebugVariable *first_child;
    DebugVariable *last_child;
} DebugVariableGroup;

struct DebugVariable
{
    DebugVariableType type;
    char name[32];
    Color color;
    
    DebugVariable *next;
    DebugVariable *parent;
    
    union
    {
        b32 *b32;
        u32 *u32;
        f32 *f32;
        v2u *v2u;
        DebugVariableGroup group;
    };
};

typedef struct
{
    v2u pos;
    Font *font;
    
    u32 y_offset;
    u32 x_offset;
    
    MemoryArena *arena;
    DebugVariable *root_group;
    DebugVariable *active_var;
    } DebugState;

#define add_debug_variable(name, value, type) add_debug_variable_(&debug_state, name, &value, 0, type)
#define add_debug_text(text, color) add_debug_text_(&debug_state, text, color)

internal DebugVariable *
add_debug_variable_(DebugState *state, char *name, void *value, Color color, DebugVariableType type)
{
    DebugVariable *var = push_memory_struct(state->arena, DebugVariable);
    var->type = type;
    strcpy(var->name, name);
    var->color = color;
    
    var->next = 0;
    
    switch(var->type)
    {
        case DebugVariableType_Newline: break;
        case DebugVariableType_Group: break;
        case DebugVariableType_Text: break;
        
        case DebugVariableType_U32: var->u32 = (u32 *)value; break;
        case DebugVariableType_B32: var->b32 = (b32 *)value; break;
        case DebugVariableType_F32: var->f32 = (f32 *)value; break;
        case DebugVariableType_V2U: var->v2u = (v2u *)value; break;
        
        invalid_default_case;
    }
    
    DebugVariable *root_group = state->root_group;
    var->parent = root_group;
    
    if(root_group)
    {
        if(root_group->group.last_child)
        {
            // Add to the end of list
            root_group->group.last_child = root_group->group.last_child->next = var;
            }
        else
        {
            // Set the first entry in the list
            root_group->group.last_child = root_group->group.first_child = var;
        }
    }
    
    return(var);
}

internal void
add_debug_text_(DebugState *state, char *text, Color color, ...)
{
    String128 text_final = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(text_final.str, sizeof(text_final), text, arg_list);
    va_end(arg_list);
    
    add_debug_variable_(state, text_final.str, 0, color, DebugVariableType_Text);
}

internal void
add_debug_newline(DebugState *state)
{
    add_debug_variable_(state, "", 0, 0, DebugVariableType_Newline);
}

internal DebugVariable *
begin_debug_group(DebugState *state, char *name)
{
    DebugVariable *group = add_debug_variable_(state, name, 0, 0, DebugVariableType_Group);
    
    group->group.is_expanded = false;
    group->group.first_child = group->group.last_child = 0;
    
    state->root_group = group;
    return(group);
}

internal void
end_debug_group(DebugState *state)
{
    assert(state->root_group);
    
    state->root_group = state->root_group->parent;
}

internal void
process_debug_state(Game *game, Input *input, DebugState *state)
{
    assert(state->root_group);
    
    state->active_var = 0;
    
    u32 var_count = 0;
    u32 group_depth = 0;
    
    DebugVariable *var = state->root_group->group.first_child;
    while(var)
    {
        // Set text
        String128 text = {0};
        
        switch(var->type)
        {
            case DebugVariableType_Newline: break;
            case DebugVariableType_Group: snprintf(text.str, sizeof(text), "%s %s", var->group.is_expanded ? "-" : "+", var->name); break;
            case DebugVariableType_Text: snprintf(text.str, sizeof(text), "%s", var->name); break;
            
            case DebugVariableType_U32: snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->u32); break;
            case DebugVariableType_B32: snprintf(text.str, sizeof(text), "%s: %s", var->name, (*var->b32 == true) ? "true" : "false"); break;
            case DebugVariableType_F32: snprintf(text.str, sizeof(text), "%s: %.02f", var->name, *var->f32); break;
            case DebugVariableType_V2U: snprintf(text.str, sizeof(text), "%s: %u, %u", var->name, var->v2u->x, var->v2u->y); break;
            
            invalid_default_case;
        }
        
        // Render text
        v4u text_rect =
        {
            state->pos.x + (group_depth * state->x_offset),
            state->pos.y + (var_count * state->y_offset),
        };
        
        Font *font = state->font;
        assert(font->type == FontType_TTF);
        text_rect.w += get_text_width(font, text.str);
        text_rect.h = font->size;
        
        Color text_color = Color_White;
        if(is_pos_inside_rect(text_rect, input->mouse_pos))
        {
            state->active_var = var;
            text_color = Color_LightBlue;
        }
        
        // If a variable has a color value, it will be used
        // If instead it's 0, we use defaults
        if(var->color)
        {
            text_color = var->color;
        }
        
        render_text(game, "%s%s", text_rect.x, text_rect.y, font, 0, start_color(text_color), text.str);
        
        // Get next variable
        if(var->type == DebugVariableType_Group && var->group.is_expanded)
        {
            var = var->group.first_child;
            ++group_depth;
            }
        else
        {
            while(var)
            {
                if(var->next)
                {
                    var = var->next;
                    break;
                }
                else
                {
                    var = var->parent;
                    --group_depth;
                }
            }
        }
        
        ++var_count;
    }
    
    // Process interaction with active debug variable
        if(was_pressed(&input->Button_Left))
    {
        DebugVariable *active_var = state->active_var;
        if(active_var)
        {
            switch(active_var->type)
            {
                case DebugVariableType_Newline: break;
                case DebugVariableType_Group: active_var->group.is_expanded = !active_var->group.is_expanded; break;
                case DebugVariableType_Text: break;
                
                case DebugVariableType_U32: break;
                case DebugVariableType_B32: break;
                case DebugVariableType_F32: break;
                case DebugVariableType_V2U: break;
                
                invalid_default_case;
            }
        }
    }
}