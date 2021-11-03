#if MOONBREATH_SLOW
internal void
ui_print_view(char *name, View view)
{
    printf("%s - count: %u\n", name, view.count);
    printf("%s - start: %u\n", name, view.start);
    printf("%s - end: %u\n\n", name, view.end);
}

internal void
print_owners(Owner *owners)
{
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        if(owner->type)
        {
            printf("owners[%u]\n", i);
            printf("Parent Type: %u\n", owner->type);
            printf("Parent Name: %s\n", owner->item->name.s);
            printf("Owner Letter: %c\n\n", owner->c);
        }
    }
    
    printf("\n");
}
#endif

internal void
init_mark(Mark *mark, u32 view_end)
{
    assert(mark);
    
    zero_struct(*mark);
    mark->cursor_blink_duration = 800;
    mark->view.end = view_end;
}

internal void
render_entity_health_regen_string(SDL_Renderer *renderer, Entity *entity, v2u *pos, UI *ui, b32 is_defer)
{
    assert(renderer);
    assert(is_entity_valid(entity));
    assert(pos);
    assert(ui);
    
    if(is_set(entity->flags, EntityFlag_HealthRegeneration))
    {
        EntityRegen *regen = &entity->regen;
        assert(is_entity_health_regen_valid(regen));
        
        // Figure out how many turns it takes to regen
        u32 regen_turn_count = 0;
        u32 temp_max = regen->max;
        u32 temp_advance = regen->advance;
        while(!is_zero(temp_max))
        {
            ++regen_turn_count;
            temp_max -= temp_advance;
        }
        
        // Set turn string
        char *regen_turn_string = "turn";
        if(regen_turn_count > 1)
        {
            regen_turn_string = "turns";
        }
        
        // Render regen string
        if(is_defer)
        {
            defer_string("Regeneration: %u health / %u %s", pos, 0, 1, ui, entity->regen.percent, regen_turn_count, regen_turn_string);
        }
        else
        {
            render_string_and_move(renderer, "Regeneration: %u health / %u %s", pos, 0, 1, ui->font, entity->regen.percent, regen_turn_count, regen_turn_string);
        }
    }
    else
    {
        if(is_defer)
        {
            defer_string("Regeneration: None", pos, 0, 1, ui);
        }
        else
        {
            render_string_and_move(renderer, "Regeneration: None", pos, 0, 1, ui->font);
        }
    }
}

internal b32
update_mark_input(Input *input, Mark *mark)
{
    assert(input);
    assert(mark);
    
    b32 result = false;
    
    if(mark->is_active)
    {
        mark->view.count = get_string_length(mark->array);
        
        if(was_pressed(&input->Key_Del))
        {
            result = true;
            
            // Don't do this if we are at the end of the buffer
            if((mark->cursor < mark->view.count) && mark->view.count)
            {
                // Remove the character at mark->cursor and move the buffer
                for(u32 index = mark->cursor; index < MAX_MARK_SIZE; ++index)
                {
                    mark->array[index] = mark->array[index + 1];
                    mark->array[index + 1] = 0;
                }
                
                force_render_mark_cursor(mark);
            }
        }
        else if(was_pressed_core(&input->Key_Backspace))
        {
            result = true;
            
            if(input->Key_Control.is_down)
            {
                while(mark->cursor &&
                      mark->array[mark->cursor - 1] == ' ')
                {
                    remove_mark_char(mark);
                }
                
                while(mark->cursor &&
                      mark->array[mark->cursor - 1] != ' ')
                {
                    remove_mark_char(mark);
                }
            }
            else
            {
                remove_mark_char(mark);
            }
        }
        else if(was_pressed(&input->Key_ArrowLeft))
        {
            result = true;
            
            if(mark->cursor)
            {
                if(input->Key_Control.is_down)
                {
                    while(mark->cursor &&
                          mark->array[mark->cursor - 1] == ' ')
                    {
                        move_mark_cursor_left(mark);
                    }
                    
                    while(mark->cursor &&
                          mark->array[mark->cursor - 1] != ' ')
                    {
                        move_mark_cursor_left(mark);
                    }
                }
                else
                {
                    move_mark_cursor_left(mark);
                }
            }
        }
        else if(was_pressed(&input->Key_ArrowRight))
        {
            result = true;
            
            if(input->Key_Control.is_down)
            {
                while(mark->cursor + 1 <= mark->view.count &&
                      mark->array[mark->cursor] == ' ')
                {
                    move_mark_cursor_right(mark);
                }
                
                while(mark->cursor + 1 <= mark->view.count &&
                      mark->array[mark->cursor] != ' ')
                {
                    move_mark_cursor_right(mark);
                }
            }
            else
            {
                move_mark_cursor_right(mark);
            }
        }
        else if(was_pressed(&input->Key_Home))
        {
            result = true;
            
            set_mark_at_start(mark);
            force_render_mark_cursor(mark);
        }
        else if(was_pressed(&input->Key_End))
        {
            result = true;
            
            set_mark_at_end(mark);
            force_render_mark_cursor(mark);
        }
        else
        {
            char pressed = get_pressed_keyboard_char(input, KeyboardCharType_Printable);
            if(pressed)
            {
                result = true;
                add_mark_character(mark, pressed);
            }
        }
        
#if 0
        printf("mark->cursor: %u\n", mark->cursor);
        ui_print_view("Mark View", mark->view);
#endif
        
        return(result);
    }
}

internal void
set_mark_at_start(Mark *mark)
{
    assert(mark);
    
    set_view_at_start(&mark->view);
    mark->cursor = 0;
}

internal void
set_mark_at_end(Mark *mark)
{
    assert(mark);
    
    mark->cursor = mark->view.count;
    if(is_view_scrollable_with_count(mark->view, mark->view.count + 1))
    {
        set_view_at_end(&mark->view);
    }
}

internal b32
is_mark_array_valid(char *mark_array)
{
    assert(mark_array);
    
    b32 result = false;
    
    for(u32 index = 0; index < MAX_MARK_SIZE; ++index)
    {
        if(mark_array[index] &&
           mark_array[index] != ' ')
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal void
deselect_mark(Mark *mark)
{
    assert(mark);
    
    mark->is_active = false;
    mark->render_cursor = false;
    mark->cursor_blink_start = 0;
}

internal String32
get_text_padding_from_max_length(char *text, u32 max_length)
{
    String32 result = {0};
    
    u32 padding_length = max_length - get_string_length(text);
    for(u32 index = 0; index < padding_length; ++index)
    {
        result.s[index] = ' ';
    }
    
    return(result);
}

internal v2u
get_tile_pos(v2u pos, u32 view_move_offset)
{
    assert(!is_v2u_zero(pos));
    
    v2u result = {pos.x + 8, pos.y + view_move_offset};
    return(result);
}

internal b32
render_mouse_highlight(SDL_Renderer *renderer, u32 pos_y, u32 header_x, v2u mouse_pos, v4u clip_rect, u32 entry_size, UI *ui)
{
    assert(renderer);
    assert(header_x);
    assert(!is_v4u_zero(clip_rect));
    assert(entry_size);
    
    b32 result = false;
    
    v4u highlight_rect =
    {
        header_x,
        pos_y,
        clip_rect.w,
        entry_size
    };
    
    // This is tested against mouse pos so that only one thing can be highlighted at a time.
    v4u test_highlight_rect =
    {
        highlight_rect.x,
        highlight_rect.y + 1,
        highlight_rect.w,
        highlight_rect.h - 1
    };
    
    if(is_pos_inside_rect(test_highlight_rect, mouse_pos) &&
       is_pos_inside_rect(clip_rect, mouse_pos))
    {
        render_fill_rect(renderer, highlight_rect, Color_MouseHighlightBackground, true);
        result = true;
    }
    
    //render_outline_rect(renderer, highlight_rect, Color_Yellow); // Debug
    return(result);
}

internal v2u
render_tile_and_mouse_highlight(SDL_Renderer *renderer,
                                View *view,
                                u32 header_x,
                                v2u window_pos,
                                v2u mouse_pos,
                                void *parent,
                                OwnerType owner_type,
                                SDL_Texture *texture,
                                UI *ui)
{
    assert(renderer);
    assert(view);
    assert(!is_v2u_zero(window_pos));
    assert(owner_type);
    assert(parent);
    assert(texture);
    assert(ui);
    
    v2u result = get_tile_pos(window_pos, view->move.offset);
    
    if(render_mouse_highlight(renderer, result.y, header_x, mouse_pos, view->clip_rect, view->entry_size, ui))
    {
        ui->mouse_highlight.type = owner_type;
        switch(owner_type)
        {
            case OwnerType_Item: ui->mouse_highlight.item = (Item *)parent; break;
            case OwnerType_Trap: ui->mouse_highlight.trap = (DungeonTrap *)parent; break;
            case OwnerType_Entity: ui->mouse_highlight.entity = (Entity *)parent; break;
            
            invalid_default_case;
        }
    }
    
    switch(owner_type)
    {
        case OwnerType_Item: render_tile(renderer, texture, ((Item *)parent)->tile_src, result); break;
        case OwnerType_Trap: render_tile(renderer, texture, ((DungeonTrap *)parent)->tile_src, result); break;
        case OwnerType_Entity: render_tile(renderer, texture, ((Entity *)parent)->tile_src, result); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
render_tile(SDL_Renderer *renderer, SDL_Texture *texture, v4u tile_src, v2u pos)
{
    assert(renderer);
    assert(texture);
    assert(!is_v4u_zero(tile_src));
    assert(!is_v2u_zero(pos));
    
    v4u tile_dest = {pos.x, pos.y, TILE_SIZE, TILE_SIZE};
    render_texture(renderer, texture, &tile_src, &tile_dest, false, false);
}

internal u32
get_centering_offset(u32 total_size, u32 part_size)
{
    assert(total_size);
    assert(part_size);
    
    u32 result = (total_size - part_size) / 2;
    return(result);
}

internal b32
can_fit_view_entry(u32 pos_y, u32 entry_size, u32 window_scroll_start_y)
{
    assert(pos_y);
    
    b32 result = (pos_y + entry_size) < window_scroll_start_y;
    return(result);
}

internal void
render_header_text_separator(SDL_Renderer *renderer, char *text, u32 x, u32 y, Font *font)
{
    assert(renderer);
    assert(text);
    assert(x);
    assert(y);
    assert(font);
    
    v4u separator_rect = {0};
    separator_rect.x = x;
    separator_rect.y = y;
    separator_rect.w = get_text_width(text, font, false);
    separator_rect.h = 1;
    
    render_fill_rect(renderer, separator_rect, Color_WindowShadow, false);
};

internal void
render_wide_separator(Game *game, v2u pos, v4u rect, s32 fill_rect_y)
{
    assert(game);
    assert(!is_v2u_zero(pos));
    assert(!is_v4u_zero(rect));
    
    v4u separator_rect = {0};
    separator_rect.x = pos.x;
    separator_rect.y = fill_rect_y;
    separator_rect.w = rect.w - ((pos.x - rect.x) * 2);
    separator_rect.h = 1;
    
    render_fill_rect(game->renderer, separator_rect, Color_WindowShadow, false);
};

internal void
next_defer_window_entry(v2u *pos, u32 entry_size)
{
    assert(pos);
    assert(entry_size);
    
    pos->y += entry_size;
}

internal v2u
get_examine_type_header_pos(v2u pos, View *view)
{
    assert(!is_v2u_zero(pos));
    assert(view);
    
    v2u result =
    {
        pos.x,
        (pos.y + (view->entry_size / 2)) + view->move.offset
    };
    
    return(result);
}

internal void
render_examine_type_header(Game *game,
                           char *string,
                           v4u rect,
                           v2u *window_pos,
                           v2u *trap_header_pos,
                           u32 view_entry_size,
                           UI *ui)
{
    assert(game);
    assert(string);
    assert(!is_v4u_zero(rect));
    assert(window_pos);
    assert(trap_header_pos);
    assert(view_entry_size);
    assert(ui);
    
    render_string(game->renderer, string, trap_header_pos, ui->font);
    render_wide_separator(game, *window_pos, rect, trap_header_pos->y - 4);
    next_defer_window_entry(window_pos, view_entry_size);
}

internal b32
can_render_not_inventory_item(Item *item, ItemType type, v2u pos)
{
    assert(type);
    assert(!is_v2u_zero(pos));
    
    b32 result = (is_valid_not_inventory_item_on_pos(item, pos) && item->type == type);
    return(result);
}

internal b32
can_render_inventory_item(Item *item, ItemType type)
{
    assert(type);
    
    b32 result = (is_valid_inventory_item(item) && item->type == type);
    return(result);
}

internal b32
can_render_inventory_item_and_fits_interact_type(Item *item,
                                                 ItemType item_type,
                                                 ItemInteractType interact_type)
{
    assert(item_type);
    assert(interact_type);
    
    // Check the item since it could be from inventory
    b32 result = (item &&
                  can_render_inventory_item(item, item_type) &&
                  can_interact_type_be_used_on_item(item, interact_type));
    
    return(result);
}

internal void
add_items_to_view_count(ItemState *item_state, v2u required_item_pos, u32 *view_count)
{
    assert(item_state);
    assert(view_count);
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &item_state->array[index];
            
            b32 is_valid = false;
            if(is_v2u_zero(required_item_pos) && can_render_inventory_item(item, item_type))
            {
                is_valid = true;
            }
            else if(!is_v2u_zero(required_item_pos) && can_render_not_inventory_item(item, item_type, required_item_pos))
            {
                is_valid = true;
            }
            
            if(is_valid)
            {
                if(render_item_type)
                {
                    render_item_type = false;
                    *view_count += 1;
                }
                
                *view_count += 1;
            }
        }
    }
}

internal v2u
get_window_entry_name_pos(v2u pos, UI *ui)
{
    assert(ui);
    
    v2u result =
    {
        pos.x + (get_font_newline(ui->font->size) * 3),
        pos.y + (ui->font->size / 2)
    };
    
    return(result);
}

internal void
render_window_items(Game *game,
                    ItemState *item_state,
                    UI *ui,
                    View *view,
                    v4u rect,
                    v2u required_item_pos,
                    v2u *window_pos,
                    v2u mouse_pos,
                    ItemInteractType interact_type,
                    SDL_Texture *texture)
{
    assert(game);
    assert(item_state);
    assert(view);
    assert(!is_v4u_zero(rect));
    assert(window_pos);
    assert(texture);
    assert(ui);
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        // This gets set and wants to stay set until we go to the next item type, it's used
        // by the mouse highlight rendering.
        v2u item_header_pos = {0};
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &item_state->array[index];
            
            b32 is_valid = false;
            
            if(is_v2u_zero(required_item_pos))
            {
                if(can_render_inventory_item(item, item_type))
                {
                    if(interact_type)
                    {
                        if(can_interact_type_be_used_on_item(item, interact_type))
                        {
                            is_valid = true;
                        }
                    }
                    else
                    {
                        is_valid = true;
                    }
                }
            }
            else if(can_render_not_inventory_item(item, item_type, required_item_pos))
            {
                is_valid = true;
            }
            
            if(is_valid)
            {
                if(render_item_type)
                {
                    render_item_type = false;
                    
                    char *item_type_text = 0;
                    switch(item_type)
                    {
                        case ItemType_Weapon: item_type_text = "Weapons"; break;
                        case ItemType_Armor: item_type_text = "Armors"; break;
                        case ItemType_Potion: item_type_text = "Potions"; break;
                        case ItemType_Scroll: item_type_text = "Scrolls"; break;
                        case ItemType_Ration: item_type_text = "Rations"; break;
                        
                        invalid_default_case;
                    }
                    
                    item_header_pos = get_examine_type_header_pos(*window_pos, view);
                    render_examine_type_header(game, item_type_text, rect, window_pos, &item_header_pos, view->entry_size, ui);
                }
                
                if(!is_v2u_zero(required_item_pos) && !item->select_letter)
                {
                    item->select_letter = add_new_char_to_owners(ui->temp_owners, item, OwnerType_Item);
                }
                
                v2u item_tile_pos = render_tile_and_mouse_highlight(game->renderer, view, item_header_pos.x, *window_pos, mouse_pos, item, OwnerType_Item, texture, ui);
                v2u item_name_pos = get_window_entry_name_pos(item_tile_pos, ui);
                String8 item_letter = get_item_letter_string(item);
                String128 item_mark = get_item_mark_string(item->flags, item->mark.array);
                
                if(is_set(item->flags, ItemFlag_Consumable))
                {
                    if(is_set(item->flags, ItemFlag_Identified))
                    {
                        render_string(game->renderer, "%s%s%s", &item_name_pos, ui->font,
                                      item_letter.s,
                                      get_full_item_name(item).s,
                                      item_mark.s);
                    }
                    else
                    {
                        render_string(game->renderer, "%s%s%s", &item_name_pos, ui->font,
                                      item_letter.s,
                                      get_full_item_name(item).s,
                                      item_mark.s);
                    }
                }
                else
                {
                    if(is_set(item->flags, ItemFlag_Identified))
                    {
                        String32 equipped_string = {0};
                        
                        if(is_set(item->flags, ItemFlag_Equipped))
                        {
                            strcpy(equipped_string.s, " (equipped)");
                        }
                        
                        render_string(game->renderer, "%s%s%s%s%s", &item_name_pos, ui->font,
                                      get_item_status_color(item->flags, item->rarity),
                                      item_letter.s,
                                      get_full_item_name(item).s,
                                      equipped_string.s,
                                      item_mark.s);
                    }
                    else
                    {
                        render_string(game->renderer, "%s%s%s", &item_name_pos, ui->font,
                                      item_letter.s,
                                      get_item_id_string(item->id, false),
                                      item_mark.s);
                    }
                }
                
                next_defer_window_entry(window_pos, view->entry_size);
            }
        }
    }
    
    
}

internal b32
update_window_test_pos(View *view, v2u *test_pos, u32 window_scroll_start_y, u32 view_end_add)
{
    assert(test_pos);
    assert(view);
    assert(view->entry_size);
    assert(window_scroll_start_y);
    
    b32 should_break = false;
    
    if(can_fit_view_entry(test_pos->y, view->entry_size, window_scroll_start_y))
    {
        // The reason we add a variable amount is because dungeon traps and entities need to have their
        // header and content added to the view which is + 2, while items add their header first and then
        // the content so 1 + 1. This of course assumes we won't have more than one dungeon trap or
        // entity on a dungeon pos ever.
        view->end += view_end_add;
        next_defer_window_entry(test_pos, view->entry_size);
    }
    else
    {
        should_break = true;
    }
    
    return(should_break);
}

internal b32
set_view_end_from_inventory_items(Inventory *inventory,
                                  View *view,
                                  v2u pos,
                                  u32 window_scroll_start_y,
                                  b32 came_from_interact)
{
    assert(inventory);
    assert(view);
    assert(window_scroll_start_y);
    
    view->end = 0;
    v2u test_pos = pos;
    
    b32 result = false;
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            b32 is_valid = false;
            Item *item = inventory->slots[index];
            
            if(came_from_interact
               && can_render_inventory_item_and_fits_interact_type(item, item_type, inventory->interact_type))
            {
                is_valid = true;
            }
            else if(can_render_inventory_item(item, item_type))
            {
                is_valid = true;
            }
            
            if(is_valid)
            {
                if(render_item_type)
                {
                    render_item_type = false;
                    if(update_window_test_pos(view, &test_pos, window_scroll_start_y, 1)) return(true);
                }
                
                if(update_window_test_pos(view, &test_pos, window_scroll_start_y, 1)) return(true);
            }
        }
    }
    
    return(result);
}

internal void
enable_window_view_clip_rect(SDL_Renderer *renderer, View *view, v4u window_rect, v2u pos)
{
    assert(renderer);
    assert(view);
    assert(!is_v4u_zero(window_rect));
    assert(!is_v2u_zero(pos));
    
    // Update view content rect
    view->content_rect.x = view->clip_rect.x;
    view->content_rect.y = view->move.offset;
    view->content_rect.w = view->clip_rect.w;
    view->content_rect.h = view->entry_size * view->count;
    
    u32 dist_from_rect = pos.x - window_rect.x;
    u32 width = window_rect.w - (dist_from_rect * 2);
    u32 horizontal_offset = 8;
    
    view->clip_rect.x = pos.x - horizontal_offset;
    view->clip_rect.y = pos.y;
    view->clip_rect.w = width + (horizontal_offset * 2);
    view->clip_rect.h = view->entry_size * view->end;
    set_render_clip_rect(renderer, &view->clip_rect);
}

internal b32
set_view_end_from_items(ItemState *item_state,
                        View *view,
                        v2u *test_pos,
                        v2u examine_pos,
                        u32 window_scroll_start_y)
{
    assert(item_state);
    assert(view);
    assert(test_pos);
    assert(!is_v2u_zero(examine_pos));
    assert(window_scroll_start_y);
    
    b32 result = false;
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &item_state->array[index];
            
            if(can_render_not_inventory_item(item, item_type, examine_pos))
            {
                if(render_item_type)
                {
                    render_item_type = false;
                    if(update_window_test_pos(view, test_pos, window_scroll_start_y, 1)) return(true);
                }
                
                if(update_window_test_pos(view, test_pos, window_scroll_start_y, 1)) return(true);
            }
        }
    }
    
    return(result);
}

internal v2u
get_window_content_start_pos(v4u rect, u32 window_offset)
{
    assert(!is_v4u_zero(rect));
    assert(window_offset);
    
    u32 offset = window_offset * 2;
    v2u result =
    {
        rect.x + offset,
        rect.y + offset
    };
    
    return(result);
}

internal u32
get_rect_x_centering_offset(u32 window_width, u32 rect_width)
{
    assert(window_width);
    assert(rect_width);
    
    u32 result = (window_width / 2) - (rect_width / 2);
    return(result);
}

internal v4u
init_rect_window(Game *game, ViewMove *view_move, u32 window_width, u32 window_scroll_start_y)
{
    assert(game);
    assert(window_width);
    assert(window_scroll_start_y);
    
    update_view_move(view_move);
    
    v4u result = {0};
    result.w = window_width;
    result.h = window_scroll_start_y - 8;
    result.x = get_rect_x_centering_offset(game->window_size.w, result.w);
    result.y = get_centering_offset(window_scroll_start_y, result.h);
    
    render_window(game, result, 2);
    return(result);
}

internal u32
get_view_range(View view)
{
    u32 result = view.start + view.end;
    return(result);
}

internal b32
can_view_go_up(View view)
{
    b32 result = (view.start > 1);
    return(result);
}

internal b32
can_view_go_down(View view)
{
    b32 result = (get_view_range(view) <= view.count);
    return(result);
}

internal b32
are_view_params_valid(ViewMoveParams params)
{
    b32 result = (params.move_count && is_direction_vertical(params.direction) && params.step_multiplier);
    return(result);
}

internal void
extend_view_move(ViewMove *move, ViewMoveParams params)
{
    assert(move && move->is_moving);
    assert(are_view_params_valid);
    
    f32 total_view_move_size = move->size * params.move_count;
    if(params.direction == Direction_Up)
    {
        move->moving_offset_target += total_view_move_size;
    }
    else
    {
        move->moving_offset_target -= total_view_move_size;
    }
    
    move->count += params.move_count;
    move->step = lerp(0.0f, move->size * move->count, params.step_multiplier);
}

internal void
start_view_move(ViewMove *move, ViewMoveParams params)
{
    assert(move && !move->is_moving);
    assert(are_view_params_valid);
    
    move->is_moving = true;
    move->can_extend = params.can_extend;
    move->direction = params.direction;
    
    move->moving_offset = (f32)move->offset;
    move->moving_offset_target = move->moving_offset;
    
    assert(!move->count);
    extend_view_move(move, params);
}

internal void
update_view_move(ViewMove *move)
{
    assert(move);
    
    if(move->is_moving)
    {
        assert(move->step);
        b32 end_move = false;
        
        if(move->direction == Direction_Up)
        {
            move->moving_offset += move->step;
            if(move->moving_offset > move->moving_offset_target) end_move = true;
        }
        else
        {
            move->moving_offset -= move->step;
            if(move->moving_offset < move->moving_offset_target) end_move = true;
        }
        
        if(end_move)
        {
            move->is_moving = false;
            move->can_extend = false;
            move->count = 0;
            
            move->moving_offset = move->moving_offset_target;
            move->offset = (s32)move->moving_offset;
            
#if MOONBREATH_SLOW
            if(move->print_info)
            {
                printf("\nmove->step: %.02f\n", move->step);
                printf("move->offset: %d\n", move->offset);
                printf("move->moving_offset_target: %.02f\n\n", move->moving_offset_target);
                
                printf("\n\n\n");
            }
#endif
            
        }
        else
        {
            move->offset = (s32)move->moving_offset;
            
#if MOONBREATH_SLOW
            if(move->print_info)
            {
                printf("move->moving_offset: %.02f\n", move->moving_offset);
            }
#endif
        }
    }
}

internal void
move_view_in_direction(View *view, ViewMoveParams params)
{
    assert(view);
    assert(are_view_params_valid(params));
    
    switch(params.direction)
    {
        case Direction_Up: view->start -= params.move_count; break;
        case Direction_Down: view->start += params.move_count; break;
        
        invalid_default_case;
    }
}

internal void
pos_newline(v2u *pos, u32 font_size, u32 count)
{
    assert(pos);
    assert(font_size);
    
    pos->y += (get_font_newline(font_size) * count);
}

internal void
update_defer_window_rect(char *text, v2u *pos, u32 lines_after, UI *ui)
{
    assert(text);
    assert(pos);
    assert(ui);
    
    u32 new_width = pos->x + get_text_width(text, ui->font, true);
    if(new_width > ui->defer_window_rect.w)
    {
        ui->defer_window_rect.w = new_width;
        
        // Going over custom width means we're no longer custom
        if(ui->defer_window_has_custom_width) ui->defer_window_has_custom_width = false;
    }
    
    if(lines_after) pos_newline(pos, ui->font->size, lines_after);
}

internal void
defer_string(char *string, v2u *pos, u32 lines_before, u32 lines_after, UI *ui, ...)
{
    assert(string);
    assert(pos);
    assert(ui);
    
    String256 string_format = {0};
    va_list arg_list;
    va_start(arg_list, ui);
    vsnprintf(string_format.s, sizeof(string_format), string, arg_list);
    va_end(arg_list);
    
    if(lines_before) pos_newline(pos, ui->font->size, lines_before);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Text;
            strcpy(defer->text.s, string_format.s);
            defer->pos = *pos;
            
            update_defer_window_rect(string_format.s, pos, lines_after, ui);
            return;
        }
    }
    
    assert(0);
}

internal void
defer_tile(v2u *pos, v4u tile_src, UI *ui, b32 move_pos_by_tile_height)
{
    assert(pos);
    assert(!is_v4u_zero(tile_src));
    assert(ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Tile;
            defer->pos = *pos;
            defer->tile_src = tile_src;
            
            if(move_pos_by_tile_height) pos->y += tile_src.h;
            return;
        }
    }
    
    assert(0);
}

internal void
defer_rect(u32 x, u32 y, u32 w, u32 h, Color color, UI *ui, b32 is_fill_rect)
{
    assert(color);
    assert(ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            if(is_fill_rect)
            {
                defer->type = DeferType_FillRect;
            }
            else
            {
                defer->type = DeferType_OutlineRect;
            }
            
            defer->rect = make_v4u(x, y, w, h);
            defer->rect_color = color;
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_header_text_separator(char *text, u32 x, u32 y, UI *ui)
{
    assert(text);
    assert(ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Separator;
            defer->rect.x = x;
            defer->rect.y = y;
            defer->rect.w = get_text_width(text, ui->font, false);
            defer->rect.h = 1;
            
            return;
        }
    }
    
    assert(0);
}

internal void
process_defers(Game *game, Texture tileset, UI *ui)
{
    assert(game);
    assert(ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        v2u real_defer_pos =
        {
            defer->pos.x + ui->defer_window_rect.x,
            defer->pos.y + ui->defer_window_rect.y
        };
        
        v4u real_defer_rect =
        {
            defer->rect.x + ui->defer_window_rect.x,
            defer->rect.y + ui->defer_window_rect.y,
            defer->rect.w,
            defer->rect.h
        };
        
        if(defer->type == DeferType_Text)
        {
            render_string(game->renderer, "%s", &real_defer_pos, ui->font, defer->text.s);
        }
        else if(defer->type == DeferType_Tile)
        {
            render_tile(game->renderer, tileset.tex, defer->tile_src, real_defer_pos);
        }
        else if(defer->type == DeferType_FillRect)
        {
            render_fill_rect(game->renderer, real_defer_rect, defer->rect_color, false);
        }
        else if(defer->type == DeferType_OutlineRect)
        {
            render_outline_rect(game->renderer, real_defer_rect, defer->rect_color);
        }
        else if(defer->type == DeferType_Separator)
        {
            render_fill_rect(game->renderer, real_defer_rect, Color_WindowShadow, false);
        }
    }
    
    zero_struct(ui->defer_window_rect);
    zero_array(ui->defers, MAX_DEFER_COUNT);
}

internal void
render_header_string_and_separator(char *header, Game *game, v2u *pos, UI *ui, u32 lines_before, b32 is_defer)
{
    assert(header);
    assert(game);
    assert(pos);
    assert(ui);
    
    if(is_defer)
    {
        defer_string(header, pos, lines_before, 1, ui);
        defer_header_text_separator(header, pos->x, pos->y, ui);
    }
    else
    {
        render_string_and_move(game->renderer, header, pos, lines_before, 1, ui->font);
        render_header_text_separator(game->renderer, header, pos->x, pos->y + 4, ui->font);
    }
    
    pos_newline(pos, ui->font->size, 1);
}

internal void
render_entity_resistances(Game *game, EntityResist *resists, v2u *pos, UI *ui, b32 is_defer)
{
    assert(game);
    assert(resists);
    assert(pos);
    assert(ui);
    
    render_header_string_and_separator("Resistances", game, pos, ui, 1, is_defer);
    
    u32 max_damage_type_length = get_max_enum_string_length(EntityDamageType_None, EntityDamageType_Count, get_entity_damage_type_string);
    for(EntityDamageType damage_type = EntityDamageType_None + 1; damage_type < EntityDamageType_Count; ++damage_type)
    {
        EntityResist resist = resists[damage_type];
        u32 absolute_resist = get_absolute(resist.value);
        u32 resistance_percent = absolute_resist * (ENTITY_RESISTANCE_PER_POINT * 10);
        
        char *damage_type_string = get_entity_damage_type_string(damage_type);
        String32 padding = get_text_padding_from_max_length(damage_type_string, max_damage_type_length);
        
        char sign = get_sign(resist.value);
        char *format_string = "%s: %s%c%u (%u%%)";
        if(is_defer)
        {
            defer_string(format_string, pos, 0, 1, ui,
                         damage_type_string, padding.s, sign,
                         absolute_resist, resistance_percent);
        }
        else
        {
            render_string_and_move(game->renderer, format_string, pos, 0, 1, ui->font,
                                   damage_type_string, padding.s, sign,
                                   absolute_resist, resistance_percent);
        }
    }
}

internal void
render_entity_status_effects(Game *game, EntityStatus *statuses, v2u *pos, UI *ui, u32 header_lines_before, b32 is_defer)
{
    assert(game);
    assert(statuses);
    assert(pos);
    assert(ui);
    
    render_header_string_and_separator("Status Effects", game, pos, ui, header_lines_before, is_defer);
    
    if(is_entity_under_any_status(statuses))
    {
        for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
        {
            EntityStatus *status = &statuses[index];
            if(status->type)
            {
                if(is_defer)
                {
                    assert(status->type != EntityStatusType_Stat);
                    defer_string("%s", pos, 0, 1, ui, get_status_type_string(status->type));
                }
                else
                {
                    if(status->type == EntityStatusType_Stat)
                    {
                        render_string_and_move(game->renderer, "%s", pos, 0, 1, ui->font, status->name.s);
                    }
                    else
                    {
                        render_string_and_move(game->renderer, "%s", pos, 0, 1, ui->font, get_status_type_string(status->type));
                    }
                }
            }
        }
    }
    else
    {
        if(is_defer)
        {
            defer_string("None", pos, 0, 1, ui);
        }
        else
        {
            render_string_and_move(game->renderer, "None", pos, 0, 1, ui->font);
        }
    }
}

internal void
center_rect_to_game_window(v2u game_window_size, v4u *rect, u32 window_scroll_start_y)
{
    assert(!is_v2u_zero(game_window_size));
    assert(rect);
    assert(window_scroll_start_y);
    
    if(rect->w && rect->w)
    {
        rect->x = get_rect_x_centering_offset(game_window_size.w, rect->w);
        rect->y = get_centering_offset(window_scroll_start_y, rect->h);
        
#if MOONBREATH_SLOW
        if(rect->y > game_window_size.h) rect->y = 0; // Clamp if wrong
#endif
        
    }
}

internal void
render_centered_rect(Game *game, v4u *rect, u32 window_scroll_start_y)
{
    assert(game);
    assert(rect);
    assert(window_scroll_start_y);
    
    center_rect_to_game_window(game->window_size, rect, window_scroll_start_y);
    render_window(game, *rect, 2);
}

internal v2u *
init_defer_window(DeferWindow *window, u32 x_multiplier, u32 y_multiplier, UI *ui)
{
    window->view.count = 0;
    zero_struct(ui->defer_window_rect);
    
    ui->defer_window_padding.x = ui->window_offset * x_multiplier;
    ui->defer_window_padding.y = ui->window_offset * y_multiplier;
    
    window->pos.x = ui->defer_window_rect.x + ui->defer_window_padding.x;
    window->pos.y = ui->defer_window_rect.y + ui->defer_window_padding.y;
    
    return(&window->pos);
}

internal void
end_defer_window(Game *game, DeferWindow *window, Assets *assets, UI *ui)
{
    assert(game);
    assert(window);
    assert(assets);
    assert(ui);
    
    ui->defer_window_rect.h = window->pos.y;
    
    // Width padding
    if(!ui->defer_window_has_custom_width &&
       ui->defer_window_padding.x)
    {
        ui->defer_window_rect.w += ui->defer_window_padding.x;
    }
    
    // Height padding
    if(ui->defer_window_padding.y)
    {
        ui->defer_window_rect.h += ui->defer_window_padding.y;
    }
    
#if 0
    printf("ui->defer_window_rect.x: %u\n", ui->defer_window_rect.x);
    printf("ui->defer_window_rect.y: %u\n", ui->defer_window_rect.y);
    printf("ui->defer_window_rect.w: %u\n", ui->defer_window_rect.w);
    printf("ui->defer_window_rect.h: %u\n\n", ui->defer_window_rect.h);
#endif
    
    render_centered_rect(game, &ui->defer_window_rect, ui->window_scroll_start_y);
    render_scrollbar(game, ui->defer_window_rect, &window->view, ui);
    
    process_defers(game, assets->tileset, ui);
}

internal void
defer_window_separator(v4u rect, v2u pos, UI *ui)
{
    assert(!is_v4u_zero(rect));
    assert(!is_v2u_zero(pos));
    assert(ui);
    
    defer_rect(pos.x, pos.y - 4, rect.w - (pos.x * 2), 1, Color_WindowShadow, ui, true);
}

internal String8
get_letter_string(char letter)
{
    String8 result = {0};
    if(letter) sprintf(result.s, "%c - ", letter);
    
    return(result);
}

internal String8
get_item_letter_string(Item *item)
{
    assert(item);
    
    String8 result = {0};
    
    if(item->select_letter)
    {
        if(is_set(item->flags, ItemFlag_Selected))
        {
            sprintf(result.s, "%c + ", item->select_letter);
        }
        else
        {
            result = get_letter_string(item->select_letter);
        }
    }
    else
    {
        result = get_letter_string(item->inventory_letter);
    }
    
    return(result);
}

internal void
reset_all_owner_select_letters(Owner *owners)
{
    assert(owners);
    
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        if(owner->type)
        {
            assert(owner->c);
            
            switch(owner->type)
            {
                case OwnerType_Item:
                {
                    assert(owner->item->select_letter);
                    
                    owner->item->select_letter = 0;
                } break;
                
                case OwnerType_Spell:
                {
                    assert(owner->spell->select_letter);
                    
                    owner->spell->select_letter = 0;
                } break;
                
                case OwnerType_Entity:
                {
                    assert(owner->entity->select_letter);
                    
                    owner->entity->select_letter = 0;
                } break;
                
                case OwnerType_Trap:
                {
                    assert(owner->trap->select_letter);
                    
                    owner->trap->select_letter = 0;
                } break;
                
                invalid_default_case;
            }
            
            owner->type = OwnerType_None;
        }
    }
}

internal void
init_all_owner_letters(Owner *owners)
{
    assert(owners);
    
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        assert(!owner->type);
        assert(!owner->c);
        
        // Set the letter for all owner elements based on index.
        u32 alphabet_size = 26;
        if(i < alphabet_size)
        {
            owner->c = 97 + i;
        }
        else
        {
            owner->c = 65 + (i - alphabet_size);
        }
        
        assert(is_alpha(owner->c));
    }
}

internal void
clear_owner_from_character(Owner *owners, char *character)
{
    assert(owners);
    assert(is_alpha(*character));
    
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        if(owner->type && owner->c == *character)
        {
            owner->type = OwnerType_None;
            *character = 0; // Clear so the caller doesn't have to.
            
            return;
        }
    }
    
    assert(0);
}

internal Owner *
get_owner_from_letter(Owner *owners, char character)
{
    assert(owners);
    assert(character);
    
    Owner *result = 0;
    
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        if(owner->c == character)
        {
            result = owner;
            break;
        }
    }
    
    return(result);
}

internal char
set_owner_src(Owner *owner, void *parent, OwnerType type)
{
    assert(owner);
    assert(!owner->type);
    assert(owner->c);
    assert(parent);
    assert(type);
    
    owner->type = type;
    
    switch(owner->type)
    {
        case OwnerType_Item: owner->item = parent; break;
        case OwnerType_Spell: owner->spell = parent; break;
        case OwnerType_Entity: owner->entity = parent; break;
        case OwnerType_Trap: owner->trap = parent; break;
        
        invalid_default_case;
    }
    
    return(owner->c);
}

internal char
add_new_char_to_owners(Owner *owners, void *parent, OwnerType type)
{
    assert(owners);
    assert(parent);
    assert(type);
    
    for(u32 i = 0; i < MAX_OWNER_COUNT; ++i)
    {
        Owner *owner = &owners[i];
        
        if(!owner->type)
        {
            char result = set_owner_src(owner, parent, type);
            return(result);
        }
    }
    
    assert(0);
}

internal void
log_add_okay(UI *ui)
{
    assert(ui);
    
    log_add("%sOkay.", ui, start_color(Color_LightYellow));
}

internal void
defer_window_option(char *text, v2u *pos, UI *ui)
{
    assert(text);
    assert(pos);
    assert(ui);
    
    defer_string(text, pos, 0, 0, ui);
    pos->x += (get_text_width(text, ui->font, false) + 10);
}

internal b32
is_view_scrollable_with_count(View view, u32 view_count)
{
    b32 result = view.end && (view_count > view.end);
    return(result);
}

internal b32
is_view_scrollable(View view)
{
    b32 result = is_view_scrollable_with_count(view, view.count);
    return(result);
}

internal void
reset_view_move(ViewMove *move)
{
    assert(move);
    assert(move->size);
    
    u32 size = move->size;
    zero_struct(*move);
    move->size = size;
}

internal void
set_view_at_start(View *view)
{
    assert(view);
    view->start = 1;
}

internal void
set_view_at_start_and_reset_move(View *view)
{
    assert(view);
    
    set_view_at_start(view);
    reset_view_move(&view->move);
}

internal u32
get_view_end(View view)
{
    assert(is_view_scrollable(view));
    
    u32 result = (view.count - view.end) + 1;
    return(result);
}

internal s32
get_view_offset_from_start(u32 view_start, u32 view_move_size)
{
    assert(view_start);
    
    s32 result = -((view_start - 1) * view_move_size);
    return(result);
}

internal void
set_view_at_end(View *view)
{
    assert(view);
    
    view->start = get_view_end(*view);
    view->move.offset = get_view_offset_from_start(view->start, view->move.size);
}

internal v2u
get_window_header_pos(v2u pos, UI *ui)
{
    assert(!is_v2u_zero(pos));
    assert(ui);
    
    v2u result =
    {
        pos.x + (ui->window_offset * 4),
        pos.y + (ui->font->size / 2)
    };
    
    return(result);
}

internal void
start_view_scrolling(Input *input, View *view)
{
    assert(input);
    assert(view);
    assert(view->move.size);
    assert(view->shared_step_multiplier);
    
    if(is_view_scrollable(*view))
    {
        ViewMoveParams params = {0};
        params.step_multiplier = view->shared_step_multiplier;
        
        if(was_pressed(&input->Button_ScrollUp) && can_view_go_up(*view))
        {
            params.can_extend = true;
            params.move_count = 1;
            params.direction = Direction_Up;
        }
        else if(was_pressed(&input->Button_ScrollDown) && can_view_go_down(*view))
        {
            params.can_extend = true;
            params.move_count = 1;
            params.direction = Direction_Down;
        }
        else if(was_pressed(&input->Key_Home) && can_view_go_up(*view))
        {
            params.move_count = view->start - 1;
            params.direction = Direction_Up;
        }
        else if(was_pressed(&input->Key_End) && can_view_go_down(*view))
        {
            params.move_count = get_view_end(*view) - view->start;
            params.direction = Direction_Down;
        }
        
#if 0
        printf("params.move_count: %u\n", params.move_count);
        printf("params.direction is vertical: %u\n", is_direction_vertical(params.direction));
        printf("params.step_multiplier: %.02f\n\n", params.step_multiplier);
#endif
        
        if(are_view_params_valid(params))
        {
            if(params.can_extend &&
               view->move.direction == params.direction &&
               view->move.is_moving)
            {
                assert(params.move_count == 1);
                move_view_in_direction(view, params);
                extend_view_move(&view->move, params);
            }
            else if(!view->move.is_moving)
            {
                //view->move.print_info = true;
                //view->scrollbar_move.print_info = true;
                
                move_view_in_direction(view, params);
                start_view_move(&view->move, params);
            }
        }
    }
}

internal void
render_scrollbar(Game *game, v4u window_rect, View *view, UI *ui)
{
    assert(game);
    assert(!is_v4u_zero(window_rect));
    assert(view);
    assert(ui);
    
    //printf("is_view_scrollable(*view): %u\n", is_view_scrollable(*view));
    //ui_print_view("test", *view);
    
    if(is_view_scrollable(*view))
    {
        // Render gutter
        v4u gutter = {0};
        gutter.x = get_rect_width(window_rect) - 12;
        gutter.y = view->clip_rect.y;
        gutter.w = 2;
        gutter.h = view->clip_rect.h;
        render_fill_rect(game->renderer, gutter, Color_WindowShadow, false);
        
        // Render scrollbar
        assert(!is_v4s_zero(view->content_rect));
        
        f32 gutter_y_scale = (f32)get_absolute(view->content_rect.y) / (f32)view->content_rect.h;
        f32 gutter_h_scale = (f32)view->clip_rect.h / (f32)view->content_rect.h;
        
        v4u scrollbar = {0};
        scrollbar.x = gutter.x;
        scrollbar.y = gutter.y + (gutter.h * gutter_y_scale);
        scrollbar.w = gutter.w;
        scrollbar.h = (gutter.h * gutter_h_scale) + 1; // + 1 so we're not off by one at the end of the gutter
        render_fill_rect(game->renderer, scrollbar, Color_WindowBorder, false);
        
#if 0
        gutter.x -= 2;
        render_fill_rect(game->renderer, gutter, Color_WindowShadow, false);
        
        print_v4u(view->clip_rect);
        print_v4u(corrected_view_content_rect);
        print_v4s(view->content_rect);
        print_v4u(gutter);
        print_v4u(scrollbar);
#endif
        
    }
}

internal void
disable_window_view_clip_rect_and_render_scrollbar(Game *game, View *view, v4u window_rect, UI *ui)
{
    assert(game);
    assert(view);
    assert(!is_v4u_zero(window_rect));
    assert(ui);
    
    //render_outline_rect(game->renderer, view->clip_rect, Color_Yellow); // Render clip rect
    SDL_RenderSetClipRect(game->renderer, 0);
    render_scrollbar(game, window_rect, view, ui);
}

internal v4u
get_border_adjusted_rect(v4u rect, u32 border_size)
{
    assert(border_size);
    
    v4u result =
    {
        rect.x + border_size,
        rect.y + border_size,
        rect.w - (border_size * 2),
        rect.h - (border_size * 2)
    };
    
    return(result);
}

internal void
render_entity_examine_window(Game *game, Entity *entity, Assets *assets, UI *ui)
{
    assert(game);
    assert(is_enemy_entity_valid(entity));
    assert(assets);
    assert(ui);
    
    DeferWindow window = {0};
    v2u *pos = init_defer_window(&window, 2, 2, ui);
    
    v2u name_pos = get_window_header_pos(*pos, ui);
    if(get_entity_status(entity->statuses, EntityStatusType_Invisible))
    {
        defer_string("Something", &name_pos, 0, 1, ui);
        defer_tile(pos, get_dungeon_tileset_rect(DungeonTileID_EntityInvisible), ui, true);
        
        defer_string("There's something here that you can't see.", pos, 1, 1, ui);
    }
    else
    {
        defer_string("%s%s", &name_pos, 0, 1, ui, get_letter_string(entity->select_letter).s, entity->name.s);
        defer_tile(pos, entity->tile_src, ui, true);
        
        v2u spells_pos = {pos->x + 352, pos->y};
        v2u resistances_pos = {spells_pos.x + 192, spells_pos.y};
        
        // Render entity stats
        render_header_string_and_separator("Stats", game, pos, ui, 1, true);
        
#if MOONBREATH_SLOW
        defer_string("Health: %u/%u", pos, 0, 1, ui, entity->hp, entity->max_hp);
#else
        defer_string("Max Health: %u", pos, 0, 1, ui, entity->max_hp);
#endif
        
        if(is_entity_damage_valid(entity->e.damage)) defer_string("Damage: %u-%u", pos, 0, 1, ui, entity->e.damage.min, entity->e.damage.max);
        defer_string("Defence: %u", pos, 0, 1, ui, entity->stats.def);
        
        assert(is_entity_evasion_valid(entity->stats.ev));
        char evasion_text[32] = {0};
        
        if(!entity->stats.ev)
        {
            strcpy(evasion_text, "None");
        }
        else if(entity->stats.ev <= 3)
        {
            strcpy(evasion_text, "Very Low");
        }
        else if(entity->stats.ev <= 7)
        {
            strcpy(evasion_text, "Low");
        }
        else if(entity->stats.ev <= 13)
        {
            strcpy(evasion_text, "Average");
        }
        else if(entity->stats.ev <= 17)
        {
            strcpy(evasion_text, "High");
        }
        else if(entity->stats.ev <= 21)
        {
            strcpy(evasion_text, "Very High");
        }
        
        defer_string("Evasion: %s (%u)", pos, 0, 1, ui, evasion_text, entity->stats.ev);
        defer_string("View Range: %u", pos, 0, 1, ui, entity->stats.fov);
        defer_string("Action Time: %.01f", pos, 0, 1, ui, entity->action_time);
        render_entity_health_regen_string(game->renderer, entity, pos, ui, true);
        
        { // Render spells
            render_header_string_and_separator("Spells", game, &spells_pos, ui, 1, true);
            
            if(entity->e.spell_count)
            {
                for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
                {
                    Spell *spell = &entity->e.spells[index];
                    if(spell->status_type)
                    {
                        if(!spell->select_letter) spell->select_letter = add_new_char_to_owners(ui->temp_owners, spell, OwnerType_Spell);
                        defer_string("%s%s", &spells_pos, 0, 1, ui, get_letter_string(spell->select_letter).s, spell->name.s);
                    }
                }
            }
            else
            {
                defer_string("None", &spells_pos, 0, 1, ui);
            }
        }
        
        v2u hit_status_effects_pos = {pos->x + 192, pos->y};
        v2u spells_affected_by_pos = {hit_status_effects_pos.x + 192, hit_status_effects_pos.y};
        
        render_entity_resistances(game, entity->resists, &resistances_pos, ui, true);
        render_entity_status_effects(game, entity->statuses, pos, ui, 6, true);
        
        { // Render hit status effects
            render_header_string_and_separator("Hit Status Effects", game, &hit_status_effects_pos, ui, 6, true);
            
            if(entity->e.on_hit_status.type)
            {
                defer_string(get_status_type_string(entity->e.on_hit_status.type), &hit_status_effects_pos, 0, 1, ui);
            }
            else
            {
                defer_string("None", &hit_status_effects_pos, 0, 1, ui);
            }
        }
        
        { // Render spells affected by
            render_header_string_and_separator("Spells Affected By", game, &spells_affected_by_pos, ui, 6, true);
            
            b32 affected_by_any_spell = false;
            for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
            {
                EntityStatus *status = &entity->statuses[index];
                if(status->type && status->spell)
                {
                    affected_by_any_spell = true;
                    
                    Spell *spell = status->spell;
                    if(!spell->select_letter)
                    {
                        spell->select_letter = add_new_char_to_owners(ui->temp_owners, status->spell, OwnerType_Spell);
                    }
                    
                    defer_string("%s%s", &spells_affected_by_pos, 0, 1, ui, get_letter_string(spell->select_letter).s, status->name.s);
                }
            }
            
            if(!affected_by_any_spell) defer_string("None", &spells_affected_by_pos, 0, 1, ui);
        }
        
        if(entity->e.spell_count) defer_string("Press the key next to a spell to view it.", pos, 2, 1, ui);
    }
    
    end_defer_window(game, &window, assets, ui);
}

internal void
render_spell_examine_window(Game *game,
                            Assets *assets,
                            UI *ui,
                            Spell *spell,
                            v2u attacker_pos,
                            v2u defender_pos)
{
    assert(game);
    assert(assets);
    assert(ui);
    assert(is_entity_spell_valid(spell));
    assert(!is_v2u_zero(attacker_pos));
    assert(!is_v2u_zero(defender_pos));
    
    DeferWindow window = {0};
    v2u *pos = init_defer_window(&window, 2, 2, ui);
    
    defer_string(spell->name.s, pos, 0, 1, ui);
    defer_string(spell->description.s, pos, 1, 1, ui);
    
    char spell_range_text[32] = "Range: View Range";
    if(spell->range)
    {
        sprintf(spell_range_text, "Range: %u", spell->range);
    }
    
    if(!spell->summon_type)
    {
        char *in_spell_range_text = " (You are not in range)";
        if(is_pos_in_spell_range(attacker_pos, defender_pos, spell->range))
        {
            in_spell_range_text = " (You are in range)";
        }
        
        defer_string("%s%s", pos, 0, 1, ui, spell_range_text, in_spell_range_text);
    }
    
    end_defer_window(game, &window, assets, ui);
}

internal void
render_tile_examine_window(Game *game, DungeonTileID tile_id, Assets *assets, UI *ui)
{
    assert(game);
    assert(tile_id);
    assert(assets);
    assert(ui);
    
    DeferWindow window = {0};
    v2u *pos = init_defer_window(&window, 2, 2, ui);
    
    // Name
    v2u header_pos = get_window_header_pos(*pos, ui);
    defer_string("%s", &header_pos, 0, 0, ui, get_dungeon_tile_name(tile_id));
    
    // Tile
    defer_tile(pos, get_dungeon_tileset_rect(tile_id), ui, true);
    
    // Description
    char *description = get_dungeon_tile_description(tile_id);
    if(description)
    {
        defer_string(description, pos, 1, 1, ui);
    }
    
    end_defer_window(game, &window, assets, ui);
}

internal void
render_trap_examine_window(Game *game, DungeonTrap *trap, Assets *assets, UI *ui)
{
    assert(trap);
    
    DeferWindow window = {0};
    v2u *pos = init_defer_window(&window, 2, 2, ui);
    
    defer_tile(pos, trap->tile_src, ui, false);
    
    v2u header_pos = get_window_header_pos(*pos, ui);
    defer_string("%s%s", &header_pos, 0, 1, ui, get_letter_string(trap->select_letter).s, trap->name.s);
    defer_string(trap->description.s, pos, 3, 1, ui);
    
    end_defer_window(game, &window, assets, ui);
}

internal void
render_item_examine_window(Game *game, Item *item, Assets *assets, UI *ui, b32 show_inventory_options)
{
    assert(game);
    assert(item);
    assert(assets);
    assert(ui);
    
    DeferWindow window = {0};
    v2u *pos = init_defer_window(&window, 2, 2, ui);
    
    // Render item picture and name
    defer_tile(pos, item->tile_src, ui, false);
    
    v2u header_pos = get_window_header_pos(*pos, ui);
    defer_string("%s%s%s%s%s", &header_pos, 0, 1, ui,
                 get_item_status_color(item->flags, item->rarity),
                 get_item_letter_string(item).s,
                 get_full_item_name(item).s,
                 end_color(),
                 get_item_mark_string(item->flags, item->mark.array).s);
    
    if(item->type == ItemType_Weapon)
    {
        if(is_set(item->flags, ItemFlag_Identified))
        {
            defer_string("Damage: %u-%u", pos, 3, 0, ui, item->w.damage.min, item->w.damage.max + item->enchant_level);
            defer_string("Accuracy: %d", pos, 1, 0, ui, item->w.accuracy + item->enchant_level);
            defer_string("Attack Speed: %.1f", pos, 1, 0, ui, item->w.speed);
        }
        else
        {
            defer_string("Base Damage: %u-%u", pos, 3, 0, ui, item->w.damage.min, item->w.damage.max);
            defer_string("Base Accuracy: %d", pos, 1, 0, ui, item->w.accuracy);
            defer_string("Base Attack Speed: %.1f", pos, 1, 0, ui, item->w.speed);
        }
    }
    else if(item->type == ItemType_Armor)
    {
        if(is_set(item->flags, ItemFlag_Identified))
        {
            defer_string("Defence: %d", pos, 3, 0, ui, item->a.defence + item->enchant_level);
            defer_string("Weight: %d", pos, 1, 0, ui, item->a.weight);
        }
        else
        {
            defer_string("Base Defence: %d", pos, 3, 0, ui, item->a.defence);
            defer_string("Base Weight: %d", pos, 1, 0, ui, item->a.weight);
        }
    }
    else if(is_set(item->flags, ItemFlag_Consumable))
    {
        if(is_set(item->flags, ItemFlag_Identified))
        {
            defer_string("%s", pos, 3, 0, ui, item->description.s);
        }
        else if(item->type == ItemType_Potion)
        {
            defer_string("Consuming potions will bestow you with different effects.", pos, 3, 0, ui);
            defer_string("Some of these effects will be helpful, while others harmful.", pos, 1, 0, ui);
        }
        else if(item->type == ItemType_Scroll)
        {
            defer_string("Reading scrolls will bring out different magical effects.", pos, 3, 0, ui);
        }
    }
    
    // Render item stats
    if(is_set(item->flags, ItemFlag_Identified))
    {
        u32 stat_lines_before = 2;
        
        for(ItemStatType stat_type = ItemStatType_None; stat_type < MAX_ITEM_STAT_COUNT; ++stat_type)
        {
            ItemStat *stat = &item->stats[stat_type];
            if(stat->type)
            {
                defer_string("%s", pos, stat_lines_before, 0, ui, stat->description.s);
                stat_lines_before = 1;
            }
        }
    }
    
    if(is_set(item->flags, ItemFlag_CanEquip))
    {
        pos_newline(pos, ui->font->size, 2);
        
        b32 is_identified_and_cursed = is_item_identified_and_cursed(item->flags);
        if(is_identified_and_cursed) defer_string("It is a cursed item.", pos, 0, 0, ui);
        
        // We move one line down before deferring this if is_identified_and_cursed is set.
        defer_string("It is of %s rarity.", pos, is_identified_and_cursed, 0, ui, get_item_rarity_string(item->rarity));
        if(item->type == ItemType_Weapon) defer_string("It is a %s weapon.", pos, 1, 0, ui, get_item_handedness_string(item->w.handedness));
    }
    
    if(show_inventory_options)
    {
        pos_newline(pos, ui->font->size, 2);
        defer_window_option("(a)djust", pos, ui);
        
        if(is_set(item->flags, ItemFlag_CanEquip))
        {
            if(is_set(item->flags, ItemFlag_Equipped))
            {
                defer_window_option("(u)nequip", pos, ui);
            }
            else
            {
                defer_window_option("(e)quip", pos, ui);
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            defer_window_option("(c)onsume", pos, ui);
        }
        else if(item->type == ItemType_Scroll)
        {
            defer_window_option("(r)ead", pos, ui);
        }
        
        defer_window_option("(d)rop", pos, ui);
        defer_window_option("(m)ark", pos, ui);
    }
    
    pos_newline(pos, ui->font->size, 1);
    end_defer_window(game, &window, assets, ui);
}

internal u32
get_font_newline(u32 font_size)
{
    assert(font_size);
    
    u32 result = font_size * 1.15f;
    return(result);
}

internal void
update_log_view(View *view, u32 index)
{
    assert(view);
    
    u32 message_count = (view->end - 1);
    if(index > message_count)
    {
        view->start = (index - message_count);
    }
}

internal void
log_add(char *text, UI *ui, ...)
{
    assert(text);
    assert(ui);
    
    String128 text_format = {0};
    
    va_list arg_list;
    va_start(arg_list, ui);
    vsnprintf(text_format.s, sizeof(text_format), text, arg_list);
    va_end(arg_list);
    
    // Set the new log message to an unused index if possible
    for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        update_log_view(&ui->full_log.view, index);
        update_log_view(&ui->short_log_view, index);
        
        LogMessage *message = &ui->log_messages[index];
        if(!message->is_set)
        {
            message->is_set = true;
            strcpy(message->string.s, text_format.s);
            
            return;
        }
    }
    
    // Move all log messages up by one
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        LogMessage *current_message = &ui->log_messages[index];
        LogMessage *last_message = &ui->log_messages[index - 1];
        
        assert(current_message->is_set);
        assert(last_message->is_set);
        
        strcpy(last_message->string.s, current_message->string.s);
    }
    
    // Set the new log message to the bottom
    LogMessage *new_message = &ui->log_messages[MAX_LOG_MESSAGE_COUNT - 1];
    strcpy(new_message->string.s, text_format.s);
}

internal void
render_player_status_window(Game *game, Entity *player, Assets *assets, UI *ui)
{
    assert(game);
    assert(is_entity_valid(player));
    assert(assets);
    assert(ui);
    
    v4u rect = {0};
    rect.w = 576;
    rect.h = 512;
    center_rect_to_game_window(game->window_size, &rect, ui->window_scroll_start_y);
    render_window(game, rect, 2);
    v2u pos = get_window_content_start_pos(rect, ui->window_offset);
    
    v4u clip_rect = get_border_adjusted_rect(rect, 2);
    set_render_clip_rect(game->renderer, &clip_rect);
    
    render_header_string_and_separator("Status", game, &pos, ui, 0, false);
    render_string_and_move(game->renderer, "%s", &pos, 0, 1, ui->font, player->name.s);
    render_string_and_move(game->renderer, "Session Time: %s:%s:%s", &pos, 0, 1, ui->font,
                           get_game_session_time_string(game->timer.hours).s,
                           get_game_session_time_string(game->timer.minutes).s,
                           get_game_session_time_string(game->timer.seconds).s);
    
    render_entity_health_regen_string(game->renderer, player, &pos, ui, false);
    
    v2u equipment_pos = {pos.x + 224, pos.y};
    render_entity_resistances(game, player->resists, &pos, ui, false);
    
    { // Render equipment
        render_header_string_and_separator("Equipment", game, &equipment_pos, ui, 1, false);
        
        u32 max_item_slot_length = get_max_enum_string_length(ItemSlot_None, ItemSlot_Count, get_item_slot_string);
        for(ItemSlot item_slot = ItemSlot_None + 1; item_slot < ItemSlot_Count; ++item_slot)
        {
            char *item_slot_string = get_item_slot_string(item_slot);
            String32 padding = get_text_padding_from_max_length(item_slot_string, max_item_slot_length);
            
            Item *item = player->p.equipment[item_slot];
            if(item)
            {
                render_string_and_move(game->renderer, "%s: %s%s%s%s", &equipment_pos, 0, 1, ui->font,
                                       item_slot_string,
                                       padding.s,
                                       get_item_status_color(item->flags, item->rarity),
                                       get_full_item_name(item).s,
                                       get_item_mark_string(item->flags, item->mark.array).s);
            }
            else
            {
                render_string_and_move(game->renderer, "%s: %sNone", &equipment_pos, 0, 1, ui->font,
                                       item_slot_string,
                                       padding.s);
            }
        }
    }
    
    render_entity_status_effects(game, player->statuses, &pos, ui, 2, false);
    SDL_RenderSetClipRect(game->renderer, 0);
}

internal void
render_multiple_pickup_window(Game *game,
                              ItemState *item_state,
                              Inventory *inventory,
                              Assets *assets,
                              UI *ui,
                              v2u mouse_pos,
                              v2u player_pos)
{
    assert(game);
    assert(item_state);
    assert(inventory);
    assert(assets);
    assert(ui);
    assert(!is_v2u_zero(player_pos));
    
    View *view = &item_state->pickup_window.view;
    assert(view->start);
    
    u32 items_selected_count = 0;
    
    { // Set view count
        view->count = 0;
        
        for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
        {
            b32 render_item_type = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &item_state->array[index];
                
                if(can_render_not_inventory_item(item, item_type, player_pos))
                {
                    if(render_item_type)
                    {
                        render_item_type = false;
                        ++view->count;
                    }
                    
                    if(is_valid_item_selected(item))
                    {
                        ++items_selected_count;
                    }
                    
                    ++view->count;
                }
            }
        }
    }
    
    // Render multiple pickup
    v4u pickup_rect = init_rect_window(game, &view->move, 512, ui->window_scroll_start_y);
    
    String32 items_selected_text = {0};
    if(items_selected_count > 1)
    {
        sprintf(items_selected_text.s, "(%u items)", items_selected_count);
    }
    else if(items_selected_count)
    {
        sprintf(items_selected_text.s, "(%u item)", items_selected_count);
    }
    
    v2u pos = get_window_content_start_pos(pickup_rect, ui->window_offset);
    render_string_and_move(game->renderer, "Pickup what? %u/%u slots %s", &pos, 0, 1, ui->font, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT, items_selected_text.s);
    
    { // Set view end
        view->end = 0;
        v2u test_pos = pos;
        set_view_end_from_items(item_state, view, &test_pos, player_pos, ui->window_scroll_start_y);
    }
    
    enable_window_view_clip_rect(game->renderer, view, pickup_rect, pos);
    render_window_items(game, item_state, ui, view, pickup_rect, player_pos, &pos, mouse_pos, inventory->interact_type, assets->tileset.tex);
    disable_window_view_clip_rect_and_render_scrollbar(game, view, pickup_rect, ui);
    
#if 0
    ui_print_view("Multiple Pickup View", *view);
#endif
    
}

internal void
render_interact_window(Game *game,
                       ItemState *item_state,
                       Inventory *inventory,
                       Assets *assets,
                       UI *ui,
                       v2u mouse_pos)
{
    assert(game);
    assert(item_state);
    assert(inventory);
    assert(inventory->interact_type);
    assert(assets);
    assert(ui);
    
    View *view = &inventory->interact_window.view;
    assert(view->start);
    
    { // Set view count
        view->count = 0;
        
        for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
        {
            b32 render_item_type = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &item_state->array[index];
                
                if(can_render_inventory_item_and_fits_interact_type(item, item_type, inventory->interact_type))
                {
                    if(render_item_type)
                    {
                        render_item_type = false;
                        ++view->count;
                    }
                    
                    ++view->count;
                }
            }
        }
    }
    
    // Render list of interactable items
    v4u interact_rect = init_rect_window(game, &view->move, 512, ui->window_scroll_start_y);
    v2u pos = get_window_content_start_pos(interact_rect, ui->window_offset);
    switch(inventory->interact_type)
    {
        case ItemInteractType_Identify: render_string_and_move(game->renderer, "Identify which item?", &pos, 0, 1, ui->font); break;
        case ItemInteractType_EnchantWeapon: render_string_and_move(game->renderer, "Enchant which weapon?", &pos, 0, 1, ui->font); break;
        case ItemInteractType_EnchantArmor: render_string_and_move(game->renderer, "Enchant which armor?", &pos, 0, 1, ui->font); break;
        case ItemInteractType_Uncurse: render_string_and_move(game->renderer, "Uncurse which item?", &pos, 0, 1, ui->font); break;
        
        invalid_default_case;
    }
    
    // Set view end
    set_view_end_from_inventory_items(inventory, view, pos, ui->window_scroll_start_y, true);
    
    enable_window_view_clip_rect(game->renderer, view, interact_rect, pos);
    render_window_items(game, item_state, ui, view, interact_rect, make_v2u(0, 0), &pos, mouse_pos, inventory->interact_type, assets->tileset.tex);
    disable_window_view_clip_rect_and_render_scrollbar(game, view, interact_rect, ui);
    
#if 0
    ui_print_view("Interact View", *view);
#endif
    
}

internal void
render_inventory_window(Game *game,
                        ItemState *item_state,
                        Inventory *inventory,
                        Assets *assets,
                        UI *ui,
                        v2u mouse_pos)
{
    assert(game);
    assert(item_state);
    assert(inventory);
    assert(assets);
    assert(ui);
    
    View *view = &inventory->window.view;
    assert(view->start);
    
    { // Set view count
        view->count = 0;
        add_items_to_view_count(item_state, make_v2u(0, 0), &view->count);
    }
    
    // If inventory view has gone over the bottom, set it at end.
    if(inventory->validate_view &&
       is_view_scrollable(*view) &&
       inventory->window.view.count < (inventory->window.view.start + inventory->window.view.end - 1))
    {
        set_view_at_end(view);
        inventory->validate_view = false;
    }
    
    v4u inventory_rect = init_rect_window(game, &view->move, 640, ui->window_scroll_start_y);
    v2u pos = get_window_content_start_pos(inventory_rect, ui->window_offset);
    render_string_and_move(game->renderer, "Inventory: %u/%u slots", &pos, 0, 1, ui->font, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
    
    // Set view end
    set_view_end_from_inventory_items(inventory, view, pos, ui->window_scroll_start_y, false);
    
    enable_window_view_clip_rect(game->renderer, view, inventory_rect, pos);
    render_window_items(game, item_state, ui, view, inventory_rect, make_v2u(0, 0), &pos, mouse_pos, inventory->interact_type, assets->tileset.tex);
    disable_window_view_clip_rect_and_render_scrollbar(game, view, inventory_rect, ui);
    
#if 0
    ui_print_view("Inventory View", *view);
#endif
    
}

internal void
render_examine_window(Game *game, Assets *assets, UI *ui, v2u player_pos)
{
    assert(game);
    assert(assets);
    assert(ui);
    assert(!is_v2u_zero(player_pos));
    
    ExamineMode *examine = &game->examine;
    
    if(examine->type)
    {
        switch(examine->type)
        {
            case ExamineType_Item: render_item_examine_window(game, examine->item, assets, ui, false); break;
            case ExamineType_Entity: render_entity_examine_window(game, examine->entity, assets, ui); break;
            case ExamineType_Spell: render_spell_examine_window(game, assets, ui, examine->spell, examine->entity->pos, player_pos); break;
            case ExamineType_Trap: render_trap_examine_window(game, examine->trap, assets, ui); break;
            case ExamineType_Tile: render_tile_examine_window(game, examine->tile_id, assets, ui); break;
            
            invalid_default_case;
        }
    }
    else
    {
        render_outline_rect(game->renderer, get_game_dest(game->camera.rect, examine->pos), Color_Yellow);
    }
}

internal void
render_multiple_examine_window(Game *game,
                               EntityState *entity_state,
                               ItemState *item_state,
                               Inventory *inventory,
                               Dungeon *dungeon,
                               Assets *assets,
                               UI *ui,
                               v2u mouse_pos)
{
    assert(game);
    assert(entity_state);
    assert(item_state);
    assert(inventory);
    assert(dungeon);
    assert(assets);
    assert(ui);
    
    ExamineMode *examine = &game->examine;
    View *view = &item_state->examine_window.view;
    assert(view->start);
    
    { // Set view count
        view->count = 0;
        
        for(ExamineType examine_type = ExamineType_Entity;
            examine_type <= ExamineType_Trap;
            ++examine_type)
        {
            if(examine_type == ExamineType_Entity)
            {
                // Add entity to view count
                Entity *entity = get_dungeon_pos_entity(dungeon->tiles, examine->pos);
                if(entity->type != EntityType_Player)
                {
                    item_state->examine_window_entity = entity;
                    if(item_state->examine_window_entity) view->count += 2;
                }
            }
            else if(examine_type == ExamineType_Item)
            {
                add_items_to_view_count(item_state, examine->pos, &view->count);
            }
            else if(examine_type == ExamineType_Trap)
            {
                // Add dungeon trap to view count
                item_state->examine_window_trap = get_dungeon_pos_trap(&dungeon->traps, examine->pos);
                if(item_state->examine_window_trap) view->count += 2;
            }
        }
    }
    
    // Render multiple examine
    v4u examine_rect = init_rect_window(game, &view->move, 512, ui->window_scroll_start_y);
    
    v2u window_pos = get_window_content_start_pos(examine_rect, ui->window_offset);
    render_string_and_move(game->renderer, "Examine what?", &window_pos, 0, 1, ui->font);
    
    Entity *entity = item_state->examine_window_entity;
    DungeonTrap *trap = item_state->examine_window_trap;
    
    { // Set view end
        view->end = 0;
        v2u test_pos = window_pos;
        
        for(ExamineType examine_type = ExamineType_Entity;
            examine_type <= ExamineType_Trap;
            ++examine_type)
        {
            if(entity &&
               examine_type == ExamineType_Entity &&
               update_window_test_pos(view, &test_pos, ui->window_scroll_start_y, 2))
            {
                break;
            }
            else if(examine_type == ExamineType_Item &&
                    set_view_end_from_items(item_state, view, &test_pos, examine->pos, ui->window_scroll_start_y))
            {
                break;
            }
            else if(trap &&
                    examine_type == ExamineType_Trap &&
                    update_window_test_pos(view, &test_pos, ui->window_scroll_start_y, 2))
            {
                break;
            }
        }
    }
    
    enable_window_view_clip_rect(game->renderer, view, examine_rect, window_pos);
    
    // Render examine contents
    for(ExamineType examine_type = ExamineType_Entity;
        examine_type <= ExamineType_Trap;
        ++examine_type)
    {
        if(examine_type == ExamineType_Entity && entity)
        {
            v2u entity_header_pos = get_examine_type_header_pos(window_pos, view);
            render_examine_type_header(game, "Entities", examine_rect, &window_pos, &entity_header_pos, view->entry_size, ui);
            
            if(!entity->select_letter)
            {
                entity->select_letter = add_new_char_to_owners(ui->temp_owners, entity, OwnerType_Entity);
            }
            
            v2u entity_tile_pos = render_tile_and_mouse_highlight(game->renderer, view, entity_header_pos.x, window_pos, mouse_pos, entity, OwnerType_Entity, assets->tileset.tex, ui);
            v2u entity_name_pos = get_window_entry_name_pos(entity_tile_pos, ui);
            String8 entity_select_letter = get_letter_string(entity->select_letter);
            
            render_string(game->renderer, "%s%s", &entity_name_pos, ui->font, entity_select_letter.s, entity->name.s);
            next_defer_window_entry(&window_pos, view->entry_size);
        }
        else if(examine_type == ExamineType_Item)
        {
            render_window_items(game, item_state, ui, view, examine_rect, examine->pos, &window_pos, mouse_pos, inventory->interact_type, assets->tileset.tex);
        }
        else if(examine_type == ExamineType_Trap && trap)
        {
            v2u trap_header_pos = get_examine_type_header_pos(window_pos, view);
            render_examine_type_header(game, "Traps", examine_rect, &window_pos, &trap_header_pos, view->entry_size, ui);
            
            if(!trap->select_letter)
            {
                trap->select_letter = add_new_char_to_owners(ui->temp_owners, trap, OwnerType_Trap);
            }
            
            v2u trap_tile_pos = render_tile_and_mouse_highlight(game->renderer, view, trap_header_pos.x, window_pos, mouse_pos, trap, OwnerType_Trap, assets->tileset.tex, ui);
            v2u trap_name_pos = get_window_entry_name_pos(trap_tile_pos, ui);
            String8 trap_select_letter = get_letter_string(trap->select_letter);
            
            render_string(game->renderer, "%s%s", &trap_name_pos, ui->font, trap_select_letter.s, trap->name.s);
            next_defer_window_entry(&window_pos, view->entry_size);
        }
    }
    
    disable_window_view_clip_rect_and_render_scrollbar(game, view, examine_rect, ui);
    
#if 0
    ui_print_view("Multiple Examine View", *view);
#endif
    
}

internal void
update_and_render_mark_input(SDL_Renderer *renderer, Font *font, Mark *mark, v2u pos, u32 parent_width)
{
    assert(renderer);
    assert(font);
    assert(mark);
    assert(mark->cursor_blink_duration);
    
    View *view = &mark->view;
    assert(view->start);
    assert(view->end);
    
    u32 height_pad = 4;
    
    { // Render input rect
        assert(view->end <= 64); // For estimation_string
        String64 estimation_string = {0};
        
        // If mark is scrolling, input rect width is the text of the mark array before scrolling.
        // else input rect width is an estimation.
        if(is_view_scrollable(*view))
        {
            for(u32 index = 0; index <= view->end; ++index)
            {
                estimation_string.s[index] = mark->array[index];
            }
            
            mark->input_rect.w = get_text_width(estimation_string.s, font, false);
        }
        else
        {
            for(u32 index = 0; index <= view->end; ++index)
            {
                estimation_string.s[index] = 'a';
            }
            
            mark->input_rect.w = get_text_width(estimation_string.s, font, false);
        }
        
        mark->input_rect.h = font->size + height_pad;
        
        if(parent_width)
        {
            // Center to parent
            pos.x += get_centering_offset(parent_width, mark->input_rect.w);
        }
        
        mark->input_rect.x = pos.x;
        mark->input_rect.y = pos.y;
        
        render_fill_rect(renderer, mark->input_rect, Color_WindowShadow, false);
        render_outline_rect(renderer, mark->input_rect, Color_WindowBorder);
        
#if 0
        printf("view->count: %u\n", view->count);
        printf("view->start: %u\n", view->start);
        printf("view->end: %u\n", view->end);
        printf("font->size: %u\n", font->size);
        printf("mark->input_rect.w: %u\n", mark->input_rect.w);
        printf("mark->input_rect.w / font->size: %u\n\n", mark->input_rect.w / font->size);
#endif
        
    }
    
    u32 cursor_x = 0;
    
    { // Render input
        v2u text_pos =
        {
            mark->input_rect.x + 4,
            mark->input_rect.y + get_centering_offset(mark->input_rect.h, font->size) + 1
        };
        cursor_x = text_pos.x;
        
        if(view->count)
        {
            v2u char_pos = text_pos;
            for(u32 index = view->start; index < get_view_range(*view); ++index)
            {
                char mark_char = mark->array[index - 1];
                
                render_string(renderer, "%c", &char_pos, font, mark_char);
                char_pos.x += get_glyph_advance(mark_char, font);
                
                if(mark->render_cursor && (index == mark->cursor))
                {
                    cursor_x = char_pos.x;
                }
            }
        }
    }
    
    if(mark->is_active)
    {
        // Update cursor
        if(mark->cursor_blink_start)
        {
            if(get_sdl_ticks_difference(mark->cursor_blink_start) >= mark->cursor_blink_duration)
            {
                mark->cursor_blink_start = 0;
                mark->render_cursor = !mark->render_cursor;
            }
        }
        else
        {
            mark->cursor_blink_start = SDL_GetTicks();
        }
        
        // Render cursor
        if(mark->render_cursor)
        {
            v4u cursor_rect =
            {
                cursor_x,
                mark->input_rect.y + (height_pad / 2),
                1,
                mark->input_rect.h - height_pad
            };
            
            render_fill_rect(renderer, cursor_rect, Color_White, false);
        }
    }
    
#if 0
    ui_print_view("Mark View", *view);
#endif
    
}

internal void
render_item_mark_window(Game *game, Item *item, Assets *assets, Mark *mark, Font *font, u32 window_scroll_start_y)
{
    v4u window_rect = {0, 0, 250, 100};
    render_centered_rect(game, &window_rect, window_scroll_start_y);
    
    { // Render header
        char *header = "Mark with what?";
        if(is_set(item->flags, ItemFlag_Marked))
        {
            header = "Replace mark with what?";
        }
        
        u32 header_width = get_text_width(header, font, false);
        v2u header_pos =
        {
            window_rect.x + get_centering_offset(window_rect.w, header_width),
            window_rect.y + 25
        };
        
        render_string(game->renderer, header, &header_pos, font);
    }
    
    v2u mark_input_pos =
    {
        window_rect.x,
        window_rect.y + get_font_newline(font->size) * 3
    };
    
    update_and_render_mark_input(game->renderer, font, mark, mark_input_pos, window_rect.w);
}

internal void
render_ui(Game *game,
          v2u mouse_pos,
          EntityState *entity_state,
          ItemState *item_state,
          Inventory *inventory,
          Dungeon *dungeon,
          Assets *assets,
          UI *ui)
{
    assert(MAX_LOG_MESSAGE_COUNT <= MAX_DEFER_COUNT); // Need enough space for deferred full log messages.
    assert(!ui->mouse_highlight.c); // Never used, should never be set.
    ui->mouse_highlight.type = OwnerType_None;
    
    ExamineMode *examine = &game->examine;
    Entity *player = get_player_entity();
    
    v4u stats_rect = {0, ui->window_scroll_start_y, 388, assets->stat_and_log_window_h};
    render_window(game, stats_rect, 2);
    
    v4u short_log_rect =
    {
        stats_rect.w + 4,
        ui->window_scroll_start_y,
        game->window_size.w - short_log_rect.x,
        assets->stat_and_log_window_h
    };
    render_window(game, short_log_rect, 2);
    
    { // Render player stats
        v2u left =
        {
            ui->window_offset,
            ui->window_scroll_start_y + ui->window_offset
        };
        
        v2u right =
        {
            left.x + 160,
            left.y
        };
        
        render_string_and_move(game->renderer, player->name.s, &left, 0, 1, ui->font);
        render_string_and_move(game->renderer, "Health:    %u/%u", &left, 0, 1, ui->font, player->hp, player->max_hp);
        render_string_and_move(game->renderer, "Strength:     %u", &left, 0, 1, ui->font, player->stats.str);
        render_string_and_move(game->renderer, "Intelligence: %u", &left, 0, 1, ui->font, player->stats.intel);
        render_string_and_move(game->renderer, "Dexterity:    %u", &left, 0, 1, ui->font, player->stats.dex);
        render_string_and_move(game->renderer, "Evasion:      %u", &left, 0, 1, ui->font, player->stats.ev);
        render_string_and_move(game->renderer, "Defence:      %u", &left, 0, 1, ui->font, player->stats.def);
        render_string_and_move(game->renderer, "Weight:       %u", &left, 0, 1, ui->font, player->p.weight);
        
        { // Render healthbar
            right.y += get_font_newline(ui->font->size) - 1;
            
            // Healthbar border
            v4u healthbar_outside = {right.x, right.y, 204, 16};
            render_fill_rect(game->renderer, healthbar_outside, Color_WindowBorder, false);
            
            // Healthbar background
            v4u healthbar_inside = get_border_adjusted_rect(healthbar_outside, 1);
            render_fill_rect(game->renderer, healthbar_inside, Color_WindowShadow, false);
            
            if(player->hp)
            {
                EntityRegen *regen = &player->regen;
                if(regen->next_turn)
                {
                    v4u regen_rect = healthbar_inside;
                    u32 hp_after_regen = player->hp + get_percent_value_from(regen->percent, player->max_hp);
                    regen_rect.w = get_ratio(hp_after_regen, player->max_hp, healthbar_inside.w);
                    render_fill_rect(game->renderer, regen_rect, Color_DarkRed, false);
                }
                
                // Healthbar foreground
                healthbar_inside.w = get_ratio(player->hp, player->max_hp, healthbar_inside.w);
                render_fill_rect(game->renderer, healthbar_inside, Color_LightRed, false);
            }
        }
        
        render_string_and_move(game->renderer, "Time:          %.01f", &right, 1, 1, ui->font, game->time);
        render_string_and_move(game->renderer, "Action Time:   %.01f", &right, 0, 1, ui->font, player->action_time);
        render_string_and_move(game->renderer, "Dungeon Level: %u", &right, 0, 1, ui->font, dungeon->level);
    }
    
    // Render short log
    assert(ui->short_log_view.end);
    
    v2u short_log_pos =
    {
        short_log_rect.x + ui->window_offset,
        short_log_rect.y + (ui->font->size / 2)
    };
    
    for(u32 index = ui->short_log_view.start;
        index < get_view_range(ui->short_log_view);
        ++index)
    {
        LogMessage *message = &ui->log_messages[index];
        if(!message->is_set) break;
        
        render_string(game->renderer, message->string.s, &short_log_pos, ui->font);
        pos_newline(&short_log_pos, ui->font->size, 1);
    }
    
    // Render full log
    if(is_set(ui->flags, UIFlag_FullLogOpen))
    {
        View *full_log_view = &ui->full_log.view;
        assert(full_log_view->start);
        
        { // Set view count
            full_log_view->count = 0;
            
            for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
            {
                if(!ui->log_messages[index].is_set) break;
                ++full_log_view->count;
            }
        }
        
        v4u full_log_rect = init_rect_window(game, &full_log_view->move, 672, ui->window_scroll_start_y);
        
        v2u full_log_pos = get_window_content_start_pos(full_log_rect, ui->window_offset);
        render_string_and_move(game->renderer, "Log", &full_log_pos, 0, 1, ui->font);
        render_wide_separator(game, full_log_pos, full_log_rect, full_log_pos.y + 4);
        pos_newline(&full_log_pos, ui->font->size, 1);
        
        { // Set view end
            full_log_view->end = 0;
            v2u test_pos = full_log_pos;
            
            for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
            {
                LogMessage *message = &ui->log_messages[index];
                if(update_window_test_pos(full_log_view, &test_pos, ui->window_scroll_start_y, 1)) break;
            }
        }
        
        if(full_log_view->set_at_end_on_open &&
           is_view_scrollable(*full_log_view))
        {
            full_log_view->set_at_end_on_open = false;
            set_view_at_end(full_log_view);
        }
        
        enable_window_view_clip_rect(game->renderer, full_log_view, full_log_rect, full_log_pos);
        
        // Render full log contents
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            if(!message->is_set) break;
            
            v2u message_pos =
            {
                full_log_pos.x,
                full_log_pos.y + full_log_view->content_rect.y
            };
            
            defer_string(message->string.s, &message_pos, 0, 0, ui);
            next_defer_window_entry(&full_log_pos, full_log_view->entry_size);
        }
        
        process_defers(game, assets->tileset, ui);
        disable_window_view_clip_rect_and_render_scrollbar(game, full_log_view, full_log_rect, ui);
        
#if 0
        ui_print_view("Full Log View", *full_log_view);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlag_Mark))
    {
        render_item_mark_window(game, inventory->examine_item, assets, &item_state->temp_mark, ui->font, ui->window_scroll_start_y);
    }
    else if(is_set(examine->flags, ExamineFlag_Open))
    {
        render_examine_window(game, assets, ui, player->pos);
    }
    else if(is_set(inventory->flags, InventoryFlag_Examine))
    {
        render_item_examine_window(game, inventory->examine_item, assets, ui, true);
    }
    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
    {
        render_multiple_examine_window(game, entity_state, item_state, inventory, dungeon, assets, ui, mouse_pos);
    }
    else if(inventory->interact_type)
    {
        render_interact_window(game, item_state, inventory, assets, ui, mouse_pos);
    }
    else if(is_set(inventory->flags, InventoryFlag_Open))
    {
        render_inventory_window(game, item_state, inventory, assets, ui, mouse_pos);
    }
    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
    {
        render_multiple_pickup_window(game, item_state, inventory, assets, ui, mouse_pos, player->pos);
    }
    else if(is_set(ui->flags, UIFlag_PlayerStatusOpen))
    {
        render_player_status_window(game, player, assets, ui);
    }
}