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
render_separator(Game *game, v2u pos, v4u rect, s32 fill_rect_y)
{
    assert(game);
    assert(!is_v2u_zero(pos));
    assert(!is_v4u_zero(rect));
    
    v4u fill_rect =
    {
        pos.x,
        fill_rect_y,
        rect.w - ((pos.x - rect.x) * 2),
        1
    };
    
    render_fill_rect(game, fill_rect, Color_WindowShadow);
}

internal void
next_defer_window_entry(v2u *pos, u32 entry_size)
{
    assert(pos);
    assert(entry_size);
    
    pos->y += entry_size;
}

internal v2u
get_tile_pos(v2u pos, u32 view_move_offset)
{
    assert(!is_v2u_zero(pos));
    
    v2u result = {pos.x + 8, pos.y + view_move_offset};
    return(result);
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
    
    render_text(game, string, trap_header_pos, ui->font);
    render_separator(game, *window_pos, rect, trap_header_pos->y - 4);
    next_defer_window_entry(window_pos, view_entry_size);
}

internal b32
can_render_non_inventory_item(Item *item, ItemType type, v2u pos, u32 dungeon_level)
{
    assert(type);
    assert(!is_v2u_zero(pos));
    assert(is_dungeon_level_valid(dungeon_level));
    
    b32 result = (is_item_valid_and_not_in_inventory_and_on_pos(item, pos, dungeon_level) && item->type == type);
    return(result);
}

internal b32
can_render_inventory_item(Item *item, ItemType type, u32 dungeon_level)
{
    assert(type);
    assert(is_dungeon_level_valid(dungeon_level));
    
    b32 result = (is_item_valid_and_in_inventory(item, dungeon_level) && item->type == type);
    return(result);
}

internal void
add_items_to_view_count(ItemState *items,
                        v2u pos, 
                        u32 dungeon_level,
                        u32 *view_count)
{
    assert(items);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(view_count);
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            
            b32 can_process = false;
            
            if(!is_v2u_zero(pos) &&
                   can_render_non_inventory_item(item, item_type, pos, dungeon_level))
            {
                can_process = true;
            }
            else if(is_v2u_zero(pos) &&
                        can_render_inventory_item(item, item_type, dungeon_level))
            {
                can_process = true;
            }
            
            if(can_process)
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

internal void
render_tile(SDL_Renderer *renderer, SDL_Texture *texture, v4u tile_src, v2u pos)
{
    assert(renderer);
    assert(texture);
    assert(!is_v4u_zero(tile_src));
    assert(!is_v2u_zero(pos));
    
    v4u dest = {pos.x, pos.y, 32, 32};
    SDL_RenderCopy(renderer, texture, (SDL_Rect *)&tile_src, (SDL_Rect *)&dest);
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
render_window_item(Game *game,
  ItemState *items,
  View *view,
  v4u rect,
  v2u pos,
  v2u *window_pos,
  u32 dungeon_level,
  SDL_Texture *texture,
  UI *ui)
{
    assert(game);
    assert(items);
    assert(view);
    assert(!is_v4u_zero(rect));
    assert(window_pos);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(texture);
    assert(ui);
    
    for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
    {
        b32 render_item_type = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            
            b32 can_process = false;
            
            if(!is_v2u_zero(pos) &&
                   can_render_non_inventory_item(item, item_type, pos, dungeon_level))
            {
                can_process = true;
            }
            else if(is_v2u_zero(pos) &&
                        can_render_inventory_item(item, item_type, dungeon_level))
            {
                can_process = true;
            }
                
                if(can_process)
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
                    
                    v2u item_header_pos = get_examine_type_header_pos(*window_pos, view);
                    render_examine_type_header(game, item_type_text, rect, window_pos, &item_header_pos, view->entry_size, ui);
                }
                
                // Render item line
                v2u item_tile_pos = get_tile_pos(*window_pos, view->move.offset);
                render_tile(game->renderer, texture, item->tile_src, item_tile_pos);
                
                // Only update select letter if we're not in the inventory
                if(!is_v2u_zero(pos) && !item->select_letter)
                {
                    item->select_letter = get_new_letter(ui->select_letters, item, LetterParentType_Item);
                }
                
                v2u item_name_pos = get_window_entry_name_pos(item_tile_pos, ui);
                String8 item_letter = get_item_letter_string(item);
                String128 item_mark = get_item_mark_string(item);
                
                if(is_item_consumable(item->type))
                {
                    if(is_set(item->flags, ItemFlag_IsIdentified))
                    {
                        render_text(game, "%s%s%s%s", &item_name_pos, ui->font, item_letter.s, item->name.s, get_item_stack_string(item).s, item_mark.s);
                    }
                    else
                    {
                        render_text(game, "%s%s%s%s%s", &item_name_pos, ui->font, item_letter.s, item->c.depiction.s, get_item_id_text(item->id), get_item_stack_string(item).s, item_mark.s);
                    }
                }
                else
                {
                    if(is_set(item->flags, ItemFlag_IsIdentified))
                    {
                        String32 equipped_string = {0};
                        
                        if(is_set(item->flags, ItemFlag_IsEquipped))
                        {
                            sprintf(equipped_string.s, " (equipped)");
                        }
                        
                        render_text(game, "%s%s%s%s%s%s", &item_name_pos, ui->font,
                                    get_item_status_color(item),
                                    item_letter.s,
                                    get_item_status_prefix(item),
                                    get_full_item_name(item).s,
                                    equipped_string.s,
                                    item_mark.s);
                    }
                    else
                    {
                        render_text(game, "%s%s%s", &item_name_pos, ui->font, item_letter.s, get_item_id_text(item->id), item_mark.s);
                    }
                }
                
                next_defer_window_entry(window_pos, view->entry_size);
            }
        }
    }
}

internal b32
update_window_test_pos(v2u *test_pos, View *view, u32 window_scroll_start_y)
{
    assert(test_pos);
    assert(view);
    assert(view->entry_size);
    assert(window_scroll_start_y);
    
    b32 should_break = false;
    
    if(can_fit_view_entry(test_pos->y, view->entry_size, window_scroll_start_y))
    {
        ++view->end;
        next_defer_window_entry(test_pos, view->entry_size);
    }
    else
    {
        should_break = true;
    }
    
    return(should_break);
}

internal v4u
get_and_set_window_clip_rect(SDL_Renderer *renderer, View *view, v4u rect, v2u pos)
{
    assert(renderer);
    assert(view);
    assert(!is_v2u_zero(pos));
    assert(!is_v4u_zero(rect));
    
    v4u result =
    {
        pos.x,
        pos.y,
        rect.w - ((pos.x - rect.x) * 2),
        view->end * view->entry_size
    };
    
    SDL_RenderSetClipRect(renderer, (SDL_Rect *)&result);
    return(result);
}

internal b32
process_view_end_items(ItemState *items,
                   View *view,
                   v2u *test_pos,
                   v2u examine_pos,
                   u32 dungeon_level,
                   u32 window_scroll_start_y)
{
    assert(items);
    assert(view);
    assert(test_pos);
    assert(!is_v2u_zero(examine_pos));
    assert(is_dungeon_level_valid(dungeon_level));
    assert(window_scroll_start_y);
    
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_not_in_inventory_and_on_pos(item, examine_pos, dungeon_level) &&
               update_window_test_pos(test_pos, view, window_scroll_start_y))
        {
            result = true;
            break;
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

internal void
update_scrollbar_move_size(View *view, v4u rect, u32 font_size)
{
    assert(view);
    assert(!is_v4u_zero(rect));
    assert(font_size);
    
    view->scrollbar_move.size = 0;
    if(view->count)
    {
        view->scrollbar_move.size = (rect.h - (font_size * 4)) / view->count;
    }
    }

internal v4u
get_static_window_rect(u32 window_width, u32 window_scroll_start_y)
{
    assert(window_width);
    assert(window_scroll_start_y);
    
    v4u result = {0};
    
    result.w = 512;
    result.h = window_scroll_start_y - 8;
    result.x = (window_width / 2) - (result.w / 2);
    result.y = get_centering_offset(window_scroll_start_y, result.h);
    
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
    b32 result = (params.move_count &&
                      is_direction_vertical(params.direction) &&
                      params.step_multiplier);
    
    return(result);
}

internal void
extend_view_move(ViewMove *move, ViewMoveParams params)
{
    assert(move && move->is_moving);
    assert(are_view_params_valid);
    
    f32 total_view_move_size = (move->size * params.move_count);
    
    if(params.direction == Direction_Up)
    {
        move->moving_offset_target += total_view_move_size;
    }
    else
    {
        move->moving_offset_target -= total_view_move_size;
    }
    
    move->size_count += params.move_count;
    move->step = lerp(0.0f, move->size * move->size_count, params.step_multiplier);
}

internal void
start_view_move(ViewMove *move, ViewMoveParams params)
{
    assert(move && !move->is_moving);
    assert(are_view_params_valid);
    
    move->is_moving = true;
    move->can_extend = params.can_be_extended;
    move->direction = params.direction;
    
    move->moving_offset = (f32)move->offset;
    move->moving_offset_target = move->moving_offset;
    
    assert(!move->size_count);
    extend_view_move(move, params);
    }

internal void
update_view_move(ViewMove *move)
{
    assert(move);
    
    if(move->is_moving)
    {
        assert(move->step);
        
        b32 move_should_end = false;
        
        if(move->direction == Direction_Up)
        {
            move->moving_offset += move->step;
            
            if(move->moving_offset > move->moving_offset_target)
            {
                move_should_end = true;
            }
        }
        else
        {
            move->moving_offset -= move->step;
            
            if(move->moving_offset < move->moving_offset_target)
            {
                move_should_end = true;
            }
        }
        
            if(move_should_end)
            {
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
            
            move->is_moving = false;
            move->can_extend = false;
            move->can_be_interrupted = false;
            move->size_count = 0;
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
    
    if(params.direction == Direction_Up)
    {
         view->start -= params.move_count;
    }
    else
    {
        view->start += params.move_count;
    }
}

internal void
start_view_move_and_scrollbar_move(View *view, ViewMoveParams params)
{
    assert(view);
    assert(are_view_params_valid(params));
    
    start_view_move(&view->move, params);
        start_view_move(&view->scrollbar_move, params);
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
        
        // If we went over the custom width, we're no longer custom
        if(ui->defer_window_has_custom_width)
        {
            ui->defer_window_has_custom_width = false;
        }
    }
    
    if(lines_after)
    {
        pos_newline(pos, ui->font->size, lines_after);
    }
    }

internal void
defer_text(char *text, v2u *pos, u32 lines_before, u32 lines_after, UI *ui, ...)
{
    assert(text);
    assert(pos);
    assert(ui);
    
    String256 text_format = {0};
    va_list arg_list;
    va_start(arg_list, ui);
    vsnprintf(text_format.s, sizeof(text_format), text, arg_list);
    va_end(arg_list);
    
    if(lines_before)
    {
        pos_newline(pos, ui->font->size, lines_before);
    }
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Text;
            strcpy(defer->text.s, text_format.s);
            defer->pos = *pos;
            
            update_defer_window_rect(text_format.s, pos, lines_after, ui);
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
            
            if(move_pos_by_tile_height)
            {
                pos->y += tile_src.h;
            }
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_rect(u32 x, u32 y, u32 w, u32 h, Color color, UI *ui, b32 is_filled_rect)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            if(is_filled_rect)
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
process_defers(Game *game, Texture tileset, UI *ui)
{
    assert(game);
    assert(ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        v2u final_defer_pos =
        {
            defer->pos.x + ui->defer_window_rect.x,
            defer->pos.y + ui->defer_window_rect.y
        };
        
        v4u final_defer_rect =
        {
            defer->rect.x + ui->defer_window_rect.x,
            defer->rect.y + ui->defer_window_rect.y,
                defer->rect.w,
            defer->rect.h
        };
        
        if(defer->type == DeferType_Text)
        {
            render_text(game, defer->text.s, &final_defer_pos, ui->font);
        }
        else if(defer->type == DeferType_Tile)
        {
            render_tile(game->renderer, tileset.tex, defer->tile_src, final_defer_pos);
        }
        else if(defer->type == DeferType_FillRect)
        {
            render_fill_rect(game, final_defer_rect, defer->rect_color);
        }
        else if(defer->type == DeferType_OutlineRect)
        {
            render_outline_rect(game, final_defer_rect, defer->rect_color);
        }
    }
    
    zero_struct(ui->defer_window_rect);
    zero_array(ui->defers, MAX_DEFER_COUNT);
}

internal void
center_rect_to_window(v2u window_size, v4u *rect, u32 window_scroll_start_y)
{
    assert(!is_v2u_zero(window_size));
    assert(rect);
    assert(window_scroll_start_y);
    
    if(rect->w && rect->w)
    {
        rect->x = (window_size.w / 2) - (rect->w / 2);
        rect->y = get_centering_offset(window_scroll_start_y, rect->h);
        
        // Allow the rectangle to be seen if it's wrong
        if(rect->y > window_size.h)
        {
            rect->y = 0;
        }
    }
}

internal void
render_centered_rect(Game *game, v4u *rect, u32 window_scroll_start_y)
{
    assert(game);
    assert(rect);
    assert(window_scroll_start_y);
    
    center_rect_to_window(game->window_size, rect, window_scroll_start_y);
    render_rect(game, *rect, 2);
}

internal void
init_defer_window(DeferWindow *window, u32 x_multiplier, u32 y_multiplier, UI *ui)
{
    window->view.count = 0;
    zero_struct(ui->defer_window_rect);
    
    ui->defer_window_padding.x = ui->window_offset * x_multiplier;
    ui->defer_window_padding.y = ui->window_offset * y_multiplier;
    
    window->pos.x = ui->defer_window_rect.x + ui->defer_window_padding.x;
    window->pos.y = ui->defer_window_rect.y + ui->defer_window_padding.y;
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
init_defer_window_and_set_custom_width(DeferWindow *window, UI *ui)
{
    init_defer_window(window, 2, 2, ui);
    
    // Set custom width
    ui->defer_window_has_custom_width = true;
    ui->defer_window_rect.w = 512;
}

internal void
defer_window_separator(v4u rect, v2u pos, UI *ui)
{
    assert(!is_v4u_zero(rect));
    assert(!is_v2u_zero(pos));
    assert(ui);
    
    defer_rect(pos.x, pos.y - 4, rect.w - (pos.x * 2), 1, Color_WindowShadow, ui, true);
}

#if MOONBREATH_SLOW
internal void
print_letters(Letter *letters)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->parent_type)
        {
            printf("Letters[%u]\n", i);
            printf("Parent Type: %u\n", letter->parent_type);
            printf("Parent Name: %s\n", letter->item->name.s);
            printf("Letter: %c\n\n", letter->c);
        }
    }
    
    printf("\n");
}
#endif

internal String8
get_letter_string(char letter)
{
    String8 result = {0};
    
    if(letter)
    {
        sprintf(result.s, "%c - ", letter);
    }
    
    return(result);
}

internal String8
get_item_letter_string(Item *item)
{
    String8 result = {0};
    
    if(item->select_letter)
    {
        if(is_set(item->flags, ItemFlag_IsSelected))
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
reset_letters(Letter *letters)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->parent_type)
        {
            assert(letter->c);
            
            switch(letter->parent_type)
            {
                case LetterParentType_Item:
                {
                    assert(letter->item->select_letter);
                    
                    letter->item->select_letter = 0;
                } break;
                
                case LetterParentType_Spell:
                {
                    assert(letter->spell->select_letter);
                    
                    letter->spell->select_letter = 0;
                } break;
                
                case LetterParentType_Entity:
                {
                    assert(letter->entity->select_letter);
                    
                    letter->entity->select_letter = 0;
                } break;
                
                case LetterParentType_DungeonTrap:
                {
                    assert(letter->trap->select_letter);
                    
                    letter->trap->select_letter = 0;
                } break;
                
                invalid_default_case;
            }
            
            letter->parent_type = LetterParentType_None;
            }
    }
}

internal void
init_letters(Letter *letters)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        assert(!letter->parent_type);
        assert(!letter->c);
        
        u32 alphabet_size = 26;
        
        if(i < alphabet_size)
        {
            letter->c = 97 + i;
        }
        else
        {
            letter->c = 65 + (i - alphabet_size);
        }
        
        assert(is_alpha(letter->c));
        }
}

internal void
clear_letter(Letter *letters, char *clear_c)
{
    assert(is_alpha(*clear_c));
    
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->parent_type && letter->c == *clear_c)
        {
            letter->parent_type = LetterParentType_None;
            *clear_c = 0;
            
            return;
        }
    }
    
    assert(0);
}

internal Letter *
get_letter(Letter *letters, char search_c)
{
    Letter *result = 0;
    
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->c == search_c)
        {
            result = letter;
            break;
        }
    }
    
    return(result);
}

internal char
set_letter(Letter *letters, Letter *letter, void *parent, LetterParentType parent_type)
{
    assert(letters);
    assert(letter);
    assert(!letter->parent_type);
    assert(letter->c);
    
    letter->parent_type = parent_type;
    
    switch(letter->parent_type)
    {
        case LetterParentType_Item: letter->item = parent; break;
        case LetterParentType_Spell: letter->spell = parent; break;
        case LetterParentType_Entity: letter->entity = parent; break;
        case LetterParentType_DungeonTrap: letter->trap = parent; break;
        
        invalid_default_case;
    }
    
    return(letter->c);
    }

internal char
get_new_letter(Letter *letters, void *parent, LetterParentType parent_type)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(!letter->parent_type)
        {
            char result = set_letter(letters, letter, parent, parent_type);
            return(result);
        }
    }
    
    assert(0);
}

#if MOONBREATH_SLOW
internal void
ui_print_view(char *name, View view)
{
    printf("%s - count: %u\n", name, view.count);
    printf("%s - start: %u\n", name, view.start);
    printf("%s - end: %u\n\n", name, view.end);
}
#endif

internal void
log_add_okay(UI *ui)
{
    assert(ui);
    
    log_add("%sOkay.", ui, start_color(Color_Yellow));
}

internal void
defer_window_option(char *text, v2u *pos, UI *ui)
{
    assert(text);
    assert(pos);
    assert(ui);
    
    defer_text(text, pos, 0, 0, ui);
    pos->x += (get_text_width(text, ui->font, false) + 10);
    }

internal b32
is_view_scrolling_with_count(View view, u32 view_count)
{
    b32 result = view.end && (view_count > view.end);
    return(result);
}

internal b32
is_view_scrolling(View view)
{
    b32 result = view.end && (view.count > view.end);
    return(result);
}

internal void
reset_view_move(ViewMove *move)
{
    assert(move);
    
    move->is_moving = false;
    move->can_extend = false;
    move->direction = Direction_None;
    
    move->offset = 0;
    move->moving_offset = 0.0f;
    move->moving_offset_target = 0.0f;
    
    move->step = 0.0f;
    move->size_count = 0;
    }

internal void
reset_all_view_moves(View *view)
{
    assert(view);
    
    reset_view_move(&view->move);
    reset_view_move(&view->scrollbar_move);
}

internal void
set_view_at_start(View *view)
{
    assert(view);
    
    view->start = 1;
    }

internal void
set_view_at_start_and_reset_all_view_moves(View *view)
{
    assert(view);
    
    set_view_at_start(view);
    reset_all_view_moves(view);
}

internal u32
get_view_end(View view)
{
    assert(is_view_scrolling(view));
    
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
    
    if(is_view_scrolling(*view))
    {
        view->start = get_view_end(*view);
        
        view->move.offset = get_view_offset_from_start(view->start, view->move.size);
        view->scrollbar_move.offset = get_view_offset_from_start(view->start, view->scrollbar_move.size);
    }
    }

internal v2u
get_examine_window_header_pos(v2u pos, UI *ui)
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
update_view_scroll(Input *input, View *view)
{
    assert(input);
    assert(view);
    
    if(is_view_scrolling(*view))
    {
        assert(view->move.size);
        assert(view->scrollbar_move.size);
        assert(view->shared_step_multiplier);
        
        ViewMoveParams params = {0};
        params.step_multiplier = view->shared_step_multiplier;
        
        if(was_pressed(&input->Button_ScrollUp) && can_view_go_up(*view))
        {
            params.can_be_extended = true;
            params.move_count = 1;
            params.direction = Direction_Up;
    }
        else if(was_pressed(&input->Button_ScrollDown) && can_view_go_down(*view))
        {
            params.can_be_extended = true;
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
        printf("is_direction_vertical: %u\n", is_direction_vertical(params.direction));
        printf("params.step_multiplier: %.02f\n\n", params.step_multiplier);
        #endif
        
        if(are_view_params_valid(params))
        {
            if(params.can_be_extended &&
               view->move.direction == params.direction &&
                   view->move.is_moving &&
                   view->scrollbar_move.is_moving)
            {
                    assert(params.move_count == 1);
                    
                    move_view_in_direction(view, params);
                    
                    extend_view_move(&view->move, params);
                    extend_view_move(&view->scrollbar_move, params);
            }
            else if(!view->move.is_moving && !view->scrollbar_move.is_moving)
            {
                //view->move.print_info = true;
                //view->scrollbar_move.print_info = true;
                
                move_view_in_direction(view, params);
                start_view_move_and_scrollbar_move(view, params);
            }
        }
    }
}

internal void
render_scrollbar(Game *game, v4u rect, View *view, UI *ui)
{
    assert(game);
    assert(!is_v4u_zero(rect));
    assert(view);
    assert(ui);
    
    ViewMove *scrollbar_move = &view->scrollbar_move;
    
    if(is_view_scrolling(*view) && scrollbar_move->size)
    {
        u32 scrollbar_gutter_x = rect.x + rect.w - 12;
        
        v4u scrollbar_gutter = {0};
    scrollbar_gutter.x = scrollbar_gutter_x;
    scrollbar_gutter.y = rect.y;
    scrollbar_gutter.w = 2;
        
        // Get the correct scrollbar gutter height
        scrollbar_gutter.h = scrollbar_move->size * view->count;
        
        // Render scrollbar gutter vertically in the center of the rect
    scrollbar_gutter.y += get_centering_offset(rect.h, scrollbar_gutter.h);
    render_fill_rect(game, scrollbar_gutter, Color_WindowShadow);
        
        v4u scrollbar = {0};
        scrollbar.x = scrollbar_gutter_x;
        scrollbar.y = scrollbar_gutter.y - scrollbar_move->offset;
    scrollbar.w = scrollbar_gutter.w;
        scrollbar.h = scrollbar_move->size * view->end;
        render_fill_rect(game, scrollbar, Color_WindowBorder);
}
}

internal void
update_all_view_moves(View *view)
{
    assert(view);
    
    update_view_move(&view->move);
    update_view_move(&view->scrollbar_move);
}

internal void
unset_window_clip_rect_and_render_scrollbar(Game *game, View *view, v4u rect, UI *ui)
{
    assert(game);
    assert(view);
    assert(!is_v4u_zero(rect));
    assert(ui);
    
    SDL_RenderSetClipRect(game->renderer, 0);
    render_scrollbar(game, rect, view, ui);
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
    assert(is_entity_valid_and_enemy(entity));
    
    DeferWindow window = {0};
    init_defer_window(&window, 2, 2, ui);
    v2u *pos = &window.pos;
    
    // Render entity stats
    v2u name_pos = get_examine_window_header_pos(*pos, ui);
    
    if(is_set(entity->flags, EntityFlag_IsInvisible))
    {
        defer_text("Something", &name_pos, 0, 1, ui);
        defer_tile(pos, get_dungeon_tileset_rect(DungeonTileID_EntityInvisible), ui, true);
        
        defer_text("An unseen being.", pos, 3, 1, ui);
    }
    else
    {
        defer_text("%s", &name_pos, 0, 1, ui, entity->name.s);
        defer_tile(pos, entity->tile_src, ui, true);
        
        defer_text("Max HP: %u", pos, 1, 1, ui, entity->max_hp);
        defer_text("Damage: %u", pos, 0, 1, ui, entity->e.damage);
        defer_text("Defence: %u", pos, 0, 1, ui, entity->defence);
        
        char evasion_text[24] = {0};
        if(entity->evasion <= 3)
        {
            strcpy(evasion_text, "Very Low");
        }
        else if(entity->evasion <= 7)
        {
            strcpy(evasion_text, "Low");
        }
        else if(entity->evasion <= 13)
        {
            strcpy(evasion_text, "Average");
        }
        else if(entity->evasion <= 17)
        {
            strcpy(evasion_text, "High");
        }
        else
        {
            strcpy(evasion_text, "Very High");
        }
        
        defer_text("Evasion: %s (%u)", pos, 0, 1, ui, evasion_text, entity->evasion);
        defer_text("Speed: %.01f", pos, 0, 1, ui, entity->action_time);
        
        // Find out the longest damage type length and damage type resistance so that
        // we can align text correctly.
        u32 longest_damage_type_length = 0;
        s32 highest_damage_type_resistance = 0;
        for(u32 damage_type_index = 0; damage_type_index < DamageType_Count; ++damage_type_index)
        {
            s32 resistance = entity->resistances[damage_type_index];
            if(resistance != 0)
            {
                char *damage_type_text = get_damage_type_text(damage_type_index);
                u32 current_damage_type_length = get_string_length(damage_type_text);
                
                if(longest_damage_type_length < current_damage_type_length)
                {
                    longest_damage_type_length = current_damage_type_length;
                }
                
                u32 absolute_resistance = get_absolute(resistance);
                if(highest_damage_type_resistance < absolute_resistance)
                {
                    highest_damage_type_resistance = absolute_resistance;
                }
            }
        }
        
#if 0
        printf("longest_damage_type_length: %u\n", longest_damage_type_length);
        printf("highest_damage_type_resistance: %u\n", highest_damage_type_resistance);
#endif
        
        // Render entity damage type resistances
        b32 render_prefix_text = true;

        for(u32 damage_type_index = 0; damage_type_index < DamageType_Count; ++damage_type_index)
        {
            s32 resistance = entity->resistances[damage_type_index];
            if(resistance != 0)
            {
                if(render_prefix_text)
                {
                    render_prefix_text = false;
                    defer_text("It has the following resistances:", pos, 1, 1, ui);
                    }
                
                // This sets the padding after the damage_type_text, we do this so that
                // the resistance_text_value's start aligned.
                char *damage_type_text = get_damage_type_text(damage_type_index);
                
                String32 damage_type_text_padding = {0};
                u32 damage_type_text_padding_length = longest_damage_type_length - get_string_length(damage_type_text);
                
                for(u32 index = 0; index < damage_type_text_padding_length; ++index)
                {
                    damage_type_text_padding.s[index] = ' ';
                }
                
                // Set the character we use based on if value is positive or negative.
                b32 add_extra_resistance_space = false;
                char resistance_char = '-';
                
                if(resistance > 0)
                {
                    add_extra_resistance_space = true;
                    resistance_char = '+';
                }
                
                u32 absolute_resistance = get_absolute(resistance);
                
                // Copy as many of the correct characters as we need into resistance_text_value.
                String8 resistance_value_string = {0};
                
                for(u32 string_index = 0; string_index < absolute_resistance; ++string_index)
                {
                    resistance_value_string.s[string_index] = resistance_char;
                }
                
                // This sets the padding at the end of resistance_text_value, we do this so that
                // the rendered resistance values start aligned.
                u32 length = highest_damage_type_resistance - absolute_resistance;
                u32 padding_length = absolute_resistance + length;
                for(u32 padding_index = absolute_resistance; padding_index < padding_length; ++padding_index)
                {
                    resistance_value_string.s[padding_index] = ' ';
                }
                
                // Positive values need an extra space, hence the boolean for it.
                if(add_extra_resistance_space)
                {
                    resistance_value_string.s[padding_length] = ' ';
                }
                
                defer_text("%s: %s%s (%d)", pos, 0, 1, ui, damage_type_text, damage_type_text_padding.s, resistance_value_string.s, resistance);
            }
        }
        
        // Render entity spells
        if(entity->e.spell_count)
        {
            defer_text("It has the following spells:", pos, 1, 1, ui);
            
            for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
            {
                Spell *spell = &entity->e.spells[spell_index];
                if(spell->type)
                {
                    if(!spell->select_letter)
                    {
                        spell->select_letter = get_new_letter(ui->select_letters, spell, LetterParentType_Spell);
                    }
                    
                    defer_text("%s%s", pos, 0, 1, ui, get_letter_string(spell->select_letter).s, spell->name.s);
                }
            }
        }
        
        // Render entity statuses
        b32 render_status_header = true;
        
        for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
        {
            Status *status = &entity->statuses[index];
            
            if(status->type && status->spell)
            {
                if(render_status_header)
                {
                    render_status_header = false;
                    defer_text("It is affected by the following spells:", pos, 1, 1, ui);
                }
                
                Spell *spell = status->spell;
                if(!spell->select_letter)
                {
                    spell->select_letter = get_new_letter(ui->select_letters, status->spell, LetterParentType_Spell);
                }
                
                defer_text("%s%s", pos, 0, 1, ui, get_letter_string(spell->select_letter).s, status->name.s);
            }
        }
        
        if(entity->e.spell_count)
        {
            defer_text("Press the key next to a spell to view it.", pos, 1, 1, ui);
        }
        }
    
    end_defer_window(game, &window, assets, ui);
        }

internal void
render_spell_examine_window(Game *game,
                                   Spell *spell,
                                   Assets *assets,
                            UI *ui,
                            v2u caster_pos,
                            v2u target_pos)
{
    assert(spell->type);
    
    DeferWindow window = {0};
    init_defer_window(&window, 2, 2, ui);
    v2u *pos = &window.pos;
    
    defer_text(spell->name.s, pos, 0, 1, ui);
    defer_text(spell->description.s, pos, 1, 1, ui);
    
    char spell_range_text[32] = "Spell range is Line of Sight";
    if(spell->range)
    {
        sprintf(spell_range_text, "Spell range is %u", spell->range);
    }
    
    char in_spell_range_text[24] = {0};
    if(is_entity_in_spell_range(caster_pos, target_pos, spell->range))
    {
        sprintf(in_spell_range_text, "(You are in range)");
    }
    
    defer_text("%s %s.", pos, 0, 1, ui, spell_range_text, in_spell_range_text);
    end_defer_window(game, &window, assets, ui);
}

internal void
render_tile_examine_window(Game *game, DungeonTileID tile, Assets *assets, UI *ui)
{
    assert(game);
    assert(tile);
    assert(assets);
    assert(ui);
    
    DeferWindow window = {0};
    init_defer_window(&window, 2, 2, ui);
    v2u *pos = &window.pos;
    
    // Name
    v2u header_pos = get_examine_window_header_pos(*pos, ui);
    defer_text("%s", &header_pos, 0, 0, ui, get_dungeon_tile_name(tile));
    
    // Tile
    defer_tile(pos, get_dungeon_tileset_rect(tile), ui, true);
    
    // Description
    char *tile_description = get_dungeon_tile_description(tile);
    if(tile_description)
    {
        defer_text(tile_description, pos, 1, 1, ui);
        }
    
    end_defer_window(game, &window, assets, ui);
    }

internal void
render_trap_examine_window(Game *game, DungeonTrap *trap, Assets *assets, UI *ui)
{
    assert(trap);
    
    DeferWindow window = {0};
    init_defer_window(&window, 2, 2, ui);
    v2u *pos = &window.pos;
    
    defer_tile(pos, trap->tile_src, ui, false);
    
    v2u header_pos = get_examine_window_header_pos(*pos, ui);
    defer_text("%s%s", &header_pos, 0, 1, ui, get_letter_string(trap->select_letter).s, trap->name.s);
    defer_text(trap->description.s, pos, 3, 1, ui);
    
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
    init_defer_window(&window, 2, 2, ui);
    v2u *pos = &window.pos;
    
    // Render item picture and name
    defer_tile(pos, item->tile_src, ui, false);
    
    v2u header_pos = get_examine_window_header_pos(*pos, ui);
    defer_text("%s%s%s%s%s%s", &header_pos, 0, 1, ui,
               get_item_status_color(item),
                   get_item_letter_string(item).s,
               get_item_status_prefix(item),
                   get_full_item_name(item).s,
                   get_item_stack_string(item).s,
               get_item_mark_string(item).s);
    
        if(item->type == ItemType_Weapon)
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
            defer_text("Damage: %d", pos, 3, 0, ui, item->w.damage + item->enchantment_level);
            defer_text("Accuracy: %d", pos, 1, 0, ui, item->w.accuracy + item->enchantment_level);
            defer_text("Attack Speed: %.1f", pos, 1, 0, ui, item->w.speed);
            }
            else
            {
            defer_text("Base Damage: %u", pos, 3, 0, ui, item->w.damage);
            defer_text("Base Accuracy: %d", pos, 1, 0, ui, item->w.accuracy);
            defer_text("Base Attack Speed: %.1f", pos, 1, 0, ui, item->w.speed);
            }
        }
        else if(item->type == ItemType_Armor)
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
            defer_text("Defence: %d", pos, 3, 0, ui, item->a.defence + item->enchantment_level);
            defer_text("Weight: %d", pos, 1, 0, ui, item->a.weight);
            }
            else
            {
            defer_text("Base Defence: %d", pos, 3, 0, ui, item->a.defence);
            defer_text("Base Weight: %d", pos, 1, 0, ui, item->a.weight);
            }
        }
        else if(is_item_consumable(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
            defer_text("%s", pos, 3, 0, ui, item->description.s);
            }
            else
            {
                if(item->type == ItemType_Potion)
                {
                defer_text("Consuming potions will bestow you with different effects.", pos, 3, 0, ui);
                defer_text("Some of these effects will be helpful, while others harmful.", pos, 1, 0, ui);
                }
                else if(item->type == ItemType_Scroll)
                {
                defer_text("Reading scrolls will bring out different magical effects.", pos, 3, 0, ui);
            }
        }
            }
    
    u32 newlines_before_inventory_options = 0;
    
    if(is_item_equipment(item->type))
    {
        pos_newline(pos, ui->font->size, 2);
        newlines_before_inventory_options = 2;
        
        b32 is_cursed_and_identified = false;
        if(is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed))
        {
            is_cursed_and_identified = true;
            defer_text("It is a cursed item.", pos, 0, 0, ui);
        }
        
        defer_text("It is of %s rarity.", pos, is_cursed_and_identified, 0, ui, get_item_rarity_text(item->rarity));
        
            if(item->type == ItemType_Weapon)
        {
            defer_text("It is a %s weapon.", pos, 1, 0, ui, get_item_handedness_text(item->w.handedness));
        }
    }
    
    if(show_inventory_options)
    {
        pos_newline(pos, ui->font->size, 2);
        defer_window_option("(a)djust", pos, ui);
        
        if(is_item_equipment(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsEquipped))
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

internal b32
is_entry_in_view(View view, u32 entry)
{
    b32 result = (!view.end || (entry >= view.start && entry < get_view_range(view)));
    return(result);
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
    u32 message_count = (view->end - 1);
    if(index > message_count)
    {
        view->start = (index - message_count);
    }
}

internal void
log_add(char *text, UI *ui, ...)
{
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
render_multiple_pickup_window(Game *game,
                              v2u player_pos,
                              ItemState *items,
                              Inventory *inventory,
                              u32 dungeon_level,
                              Assets *assets,
                              UI *ui)
{
    assert(!is_v2u_zero(player_pos));
    assert(is_dungeon_level_valid(dungeon_level));
    
    View *view = &items->pickup_window.view;
    assert(view->start);
    
    u32 items_selected_count = 0;
    
    { // Set view count
        view->count = 0;
        
        for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
        {
            b32 render_item_type = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items->array[index];
                
                    if(can_render_non_inventory_item(item, item_type, player_pos, dungeon_level))
                {
                    if(render_item_type)
                    {
                        render_item_type = false;
                        ++view->count;
                    }
                    
                    ++view->count;
                    
                    if(is_item_valid_and_selected(item, dungeon_level))
                    {
                        ++items_selected_count;
                    }
                }
            }
        }
    }
    
    update_all_view_moves(view);
    
    // Render multiple pickup
    v4u pickup_rect = get_static_window_rect(game->window_size.w, ui->window_scroll_start_y);
    render_rect(game, pickup_rect, 2);
    update_scrollbar_move_size(view, pickup_rect, get_font_newline(ui->font->size));
    
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
        render_text_and_move(game, "Pickup what? %u/%u slots %s", &pos, 0, 1, ui->font, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT, items_selected_text.s);
    
    // Set view end
        view->end = 0;
        v2u test_pos = pos;
    process_view_end_items(items, view, &test_pos, player_pos, dungeon_level, ui->window_scroll_start_y);
    
    v4u pickup_clip_rect = get_and_set_window_clip_rect(game->renderer, view, pickup_rect, pos);
    render_window_item(game, items, view, pickup_rect, player_pos, &pos, dungeon_level, assets->tileset.tex, ui);
    
    //render_outline_rect(game, pickup_clip_rect, Color_Yellow);
    unset_window_clip_rect_and_render_scrollbar(game, view, pickup_rect, ui);
    
#if 0
    ui_print_view("Multiple Pickup View", *view);
#endif
    
}

internal void
render_inventory_window(Game *game,
                   ItemState *items,
                       Inventory *inventory,
                       u32 dungeon_level,
                        Assets *assets,
                        UI *ui)
{
    Examine *examine = &game->examine;
    View *view = &inventory->window.view;
    assert(view->start);
    
{ // Set view count
        view->count = 0;
        add_items_to_view_count(items, make_v2u(0, 0), dungeon_level, &view->count);
        }
    
    update_all_view_moves(view);
    
    // Render inventory
    v4u inventory_rect = get_static_window_rect(game->window_size.w, ui->window_scroll_start_y);
    render_rect(game, inventory_rect, 2);
    update_scrollbar_move_size(view, inventory_rect, get_font_newline(ui->font->size));
    
    if(inventory->dropped_item_type)
    {
        if(is_view_scrolling(*view))
        {
        // We want the view to end up at the end if we drop something and have enough to scroll
        set_view_at_end(view);
        }
        else
        {
            // We want the view to be at the start if we drop something and aren't scrolling
            set_view_at_start_and_reset_all_view_moves(view);
        }
        
        inventory->dropped_item_type = ItemType_None;
    }
    
    v2u pos = get_window_content_start_pos(inventory_rect, ui->window_offset);
    render_text_and_move(game, "Inventory: %u/%u slots", &pos, 0, 1, ui->font, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
    
    // Set view end
        view->end = 0;
        
        b32 should_break = false;
    v2u test_pos = pos;
    
        for(ItemType item_type = ItemType_None + 1; item_type < ItemType_Count; ++item_type)
        {
            if(should_break) break;
            
            b32 render_item_type = true;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                
                if(can_render_inventory_item(item, item_type, dungeon_level))
                {
                if(render_item_type &&
                   update_window_test_pos(&test_pos, view, ui->window_scroll_start_y))
                    {
                            break;
                    }
                    
                    if(update_window_test_pos(&test_pos, view, ui->window_scroll_start_y))
                    {
                        break;
                    }
                }
            }
        }
    
    v4u inventory_clip_rect = get_and_set_window_clip_rect(game->renderer, view, inventory_rect, pos);
    render_window_item(game, items, view, inventory_rect, make_v2u(0, 0), &pos, dungeon_level, assets->tileset.tex, ui);
    
    //render_outline_rect(game, inventory_clip_rect, Color_Yellow);
    unset_window_clip_rect_and_render_scrollbar(game, view, inventory_rect, ui);
    
#if 0
    ui_print_view("Inventory View", *view);
#endif
    
}

internal void
render_examine_window(Game *game, Assets *assets, UI *ui, v2u player_pos)
{
    Examine *examine = &game->examine;
    
    if(examine->type)
    {
        switch(examine->type)
        {
            case ExamineType_Item: render_item_examine_window(game, examine->item, assets, ui, false); break;
            case ExamineType_Entity: render_entity_examine_window(game, examine->entity, assets, ui); break;
            case ExamineType_Spell: render_spell_examine_window(game, examine->spell, assets, ui, examine->entity->pos, player_pos); break;
            case ExamineType_Trap: render_trap_examine_window(game, examine->trap, assets, ui); break;
            case ExamineType_Tile: render_tile_examine_window(game, examine->tile, assets, ui); break;
            
            invalid_default_case;
        }
    }
    else
    {
        v4u dest = get_game_dest(game, examine->pos);
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
    }
}

internal void
render_multiple_examine_window(Game *game,
                               EntityState *entities,
                               ItemState *items,
                               Inventory *inventory,
                               Dungeon *dungeon,
                               Assets *assets,
                               UI *ui)
{
    assert(game);
    assert(entities);
    assert(items);
    assert(inventory);
    assert(dungeon);
    assert(assets);
    assert(ui);
    
    Examine *examine = &game->examine;
    View *view = &items->examine_window.view;
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
                items->examine_window_entity = get_dungeon_pos_entity(entities, dungeon->level, examine->pos, true);
                if(items->examine_window_entity)
                {
                    view->count += 2;
                }
            }
            else if(examine_type == ExamineType_Item)
            {
                add_items_to_view_count(items, examine->pos, dungeon->level, &view->count);
            }
            else if(examine_type == ExamineType_Trap)
            {
                // Add dungeon trap to view count
                items->examine_window_trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, examine->pos);
                if(items->examine_window_trap)
                {
                    view->count += 2;
                }
                }
            }
    }
    
    update_all_view_moves(view);
    
    // Render multiple examine
    v4u examine_rect = get_static_window_rect(game->window_size.w, ui->window_scroll_start_y);
    render_rect(game, examine_rect, 2);
    update_scrollbar_move_size(view, examine_rect, get_font_newline(ui->font->size));
    
    v2u pos = get_window_content_start_pos(examine_rect, ui->window_offset);
    render_text_and_move(game, "Examine what?", &pos, 0, 1, ui->font);
    
    Entity *entity = items->examine_window_entity;
    DungeonTrap *trap = items->examine_window_trap;
    
    // Set view end
    view->end = 0;
    v2u test_pos = pos;
    
    for(ExamineType examine_type = ExamineType_Entity;
            examine_type <= ExamineType_Trap;
        ++examine_type)
    {
        if(entity &&
               examine_type == ExamineType_Entity &&
               update_window_test_pos(&test_pos, view, ui->window_scroll_start_y))
        {
            break;
        }
        else if(examine_type == ExamineType_Item &&
                    process_view_end_items(items, view, &test_pos, examine->pos, dungeon->level, ui->window_scroll_start_y))
        {
            break;
            }
        else if(trap &&
                examine_type == ExamineType_Trap &&
                    update_window_test_pos(&test_pos, view, ui->window_scroll_start_y))
        {
            break;
        }
    }
    
    v4u examine_clip_rect = get_and_set_window_clip_rect(game->renderer, view, examine_rect, pos);
    
    // Render examine contents
    for(ExamineType examine_type = ExamineType_Entity;
        examine_type <= ExamineType_Trap;
        ++examine_type)
    {
        if(examine_type == ExamineType_Entity && entity)
        {
            v2u entity_header_pos = get_examine_type_header_pos(pos, view);
            render_examine_type_header(game, "Entities", examine_rect, &pos, &entity_header_pos, view->entry_size, ui);
            
            v2u entity_tile_pos = get_tile_pos(pos, view->move.offset);
            render_tile(game->renderer, assets->tileset.tex, entity->tile_src, entity_tile_pos);
            
            if(!entity->select_letter)
            {
                entity->select_letter = get_new_letter(ui->select_letters, entity, LetterParentType_Entity);
            }
            
            v2u entity_name_pos = get_window_entry_name_pos(entity_tile_pos, ui);
            String8 entity_select_letter = get_letter_string(entity->select_letter);
            
            render_text(game, "%s%s", &entity_name_pos, ui->font, entity_select_letter.s, entity->name.s);
            next_defer_window_entry(&pos, view->entry_size);
        }
        else if(examine_type == ExamineType_Item)
        {
            render_window_item(game, items, view, examine_rect, examine->pos, &pos, dungeon->level, assets->tileset.tex, ui);
        }
        else if(examine_type == ExamineType_Trap && trap)
        {
            v2u trap_header_pos = get_examine_type_header_pos(pos, view);
             render_examine_type_header(game, "Traps", examine_rect, &pos, &trap_header_pos, view->entry_size, ui);
            
            v2u trap_tile_pos = get_tile_pos(pos, view->move.offset);
            render_tile(game->renderer, assets->tileset.tex, trap->tile_src, trap_tile_pos);
            
            if(!trap->select_letter)
            {
                trap->select_letter = get_new_letter(ui->select_letters, trap, LetterParentType_DungeonTrap);
            }
            
            v2u trap_name_pos = get_window_entry_name_pos(trap_tile_pos, ui);
            String8 trap_select_letter = get_letter_string(trap->select_letter);
            
            render_text(game, "%s%s", &trap_name_pos, ui->font, trap_select_letter.s, trap->name.s);
            next_defer_window_entry(&pos, view->entry_size);
        }
    }
    
    //render_outline_rect(game, examine_clip_rect, Color_Yellow);
    unset_window_clip_rect_and_render_scrollbar(game, view, examine_rect, ui);
    
#if 0
    ui_print_view("Multiple Examine View", *view);
#endif
    
    }

internal void
render_item_mark_window(Game *game, ItemState *items, Item *item, Assets *assets, UI *ui)
{
    v4u mark_rect = {0, 0, 250, 100};
    render_centered_rect(game, &mark_rect, ui->window_scroll_start_y);
    
    { // Render header
        char *header_text = "Mark with what?";
        if(is_set(item->flags, ItemFlag_IsMarked))
        {
            header_text = "Replace mark with what?";
        }
        
        v2u header_pos =
        {
            mark_rect.x + get_centering_offset(mark_rect.w, get_text_width(header_text, ui->font, false)),
            mark_rect.y + 25
        };
        
        render_text(game, header_text, &header_pos, ui->font);
    }
    
    // Render input box
    u32 height_padding = 4;
    v4u input_rect = {mark_rect.x, mark_rect.y, ui->font->size * 14, ui->font->size + height_padding};
    
    input_rect.x += get_centering_offset(mark_rect.w, input_rect.w);
    input_rect.y += get_font_newline(ui->font->size) * 3;
    
    render_fill_rect(game, input_rect, Color_WindowShadow);
    render_outline_rect(game, input_rect, Color_WindowBorder);
    
    // Update cursor
    Mark *temp_mark = &items->temp_mark;
    
    if(!temp_mark->render_cursor_start_ms)
    {
        temp_mark->render_cursor_start_ms = SDL_GetTicks();
    }
    
    if(get_sdl_ticks_difference(temp_mark->render_cursor_start_ms) >= temp_mark->cursor_blink_duration)
    {
        temp_mark->render_cursor_start_ms = 0;
        temp_mark->render_cursor = !temp_mark->render_cursor;
    }
    
    // Render input
    v2u text_pos =
    {
        input_rect.x + 4,
        input_rect.y + get_centering_offset(input_rect.h, ui->font->size) + 1
    };
    
    u32 cursor_x = text_pos.x;
    v2u character_pos = text_pos;
    for(u32 index = temp_mark->view.start; index < get_view_range(temp_mark->view); ++index)
    {
        u32 mark_index = index - 1;
        
        render_text(game, "%c", &character_pos, ui->font, temp_mark->array[mark_index]);
        character_pos.x += get_glyph_width(temp_mark->array[mark_index], ui->font);
        
        if(temp_mark->render_cursor && (index == temp_mark->cursor))
        {
            cursor_x = character_pos.x;
        }
    }
    
    // Render cursor
    if(temp_mark->render_cursor)
    {
        v4u cursor_rect =
        {
            cursor_x,
            input_rect.y + (height_padding / 2),
            1,
            input_rect.h - height_padding
        };
        
        render_fill_rect(game, cursor_rect, Color_White);
    }
    
#if 0
    printf("temp_mark->cursor_index: %u\n", temp_mark->cursor_index);
    ui_print_view("Temp Mark View", temp_mark->view);
#endif
    
}

internal void
render_ui(Game *game,
          Input *input,
          EntityState *entities,
          ItemState *items,
          Inventory *inventory,
           Dungeon *dungeon,
          Assets *assets,
          UI *ui)
{
    Examine *examine = &game->examine;
    Entity *player = get_player_entity(entities);
    
    v4u stats_rect =
    {
        0,
        ui->window_scroll_start_y,
        388,
        assets->stat_and_log_window_h
    };
    render_rect(game, stats_rect, 2);
    
    v4u short_log_rect =
    {
        stats_rect.w + 4,
        ui->window_scroll_start_y,
        game->window_size.w - short_log_rect.x,
        assets->stat_and_log_window_h
    };
    render_rect(game, short_log_rect, 2);
    
    { // Render stats
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
        
        render_text_and_move(game, player->name.s, &left, 0, 1, ui->font);
        render_text_and_move(game, "Health:    %u/%u", &left, 0, 1, ui->font, player->hp, player->max_hp);
        render_text_and_move(game, "Strength:     %u", &left, 0, 1, ui->font, player->strength);
        render_text_and_move(game, "Intelligence: %u", &left, 0, 1, ui->font, player->intelligence);
        render_text_and_move(game, "Dexterity:    %u", &left, 0, 1, ui->font, player->dexterity);
        render_text_and_move(game, "Evasion:      %u", &left, 0, 1, ui->font, player->evasion);
        render_text_and_move(game, "Defence:      %u", &left, 0, 1, ui->font, player->defence);
        render_text_and_move(game, "Weight:       %u", &left, 0, 1, ui->font, player->p.weight);
    
        { // Render healthbar
            right.y += get_font_newline(ui->font->size) - 1;
            
            // Healthbar border
            v4u healthbar_outside = {right.x, right.y, 204, 16};
            render_fill_rect(game, healthbar_outside, Color_WindowBorder);
            
            // Healthbar background
            v4u healthbar_inside = get_border_adjusted_rect(healthbar_outside, 1);
            render_fill_rect(game, healthbar_inside, Color_WindowShadow);
            
            if(player->hp > 0)
            {
                healthbar_inside.w = get_ratio(player->hp, player->max_hp, healthbar_inside.w);
                render_fill_rect(game, healthbar_inside, Color_DarkRed);
            }
        }
        
        render_text_and_move(game, "Time:          %.01f", &right, 1, 1, ui->font, game->time);
        render_text_and_move(game, "Action Time:   %.01f", &right, 0, 1, ui->font, player->action_time);
        render_text_and_move(game, "Dungeon Level: %u", &right, 0, 1, ui->font, dungeon->level);
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
        if(message->is_set)
        {
            render_text(game, message->string.s, &short_log_pos, ui->font);
            pos_newline(&short_log_pos, ui->font->size, 1);
        }
    }
    
    // Render full log
    if(ui->full_log_open)
    {
        View *full_log_view = &ui->full_log.view;
        assert(full_log_view->start);
        
        { // Set view count
            full_log_view->count = 0;
            
            for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
            {
                LogMessage *message = &ui->log_messages[index];
                
                if(message->is_set)
                {
                    ++full_log_view->count;
                }
                else
                {
                    break;
                }
                }
            }
        
        update_all_view_moves(full_log_view);
        
        v4u full_log_rect = get_static_window_rect(game->window_size.w, ui->window_scroll_start_y);
        render_rect(game, full_log_rect, 2);
        update_scrollbar_move_size(full_log_view, full_log_rect, get_font_newline(ui->font->size));
        
        v2u full_log_pos = get_window_content_start_pos(full_log_rect, ui->window_offset);
        render_text_and_move(game, "Log", &full_log_pos, 0, 1, ui->font);
        render_separator(game, full_log_pos, full_log_rect, full_log_pos.y + 4);
        pos_newline(&full_log_pos, ui->font->size, 1);
        
        // Set view end
            full_log_view->end = 0;
            v2u test_pos = full_log_pos;
            
            for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
            {
                LogMessage *message = &ui->log_messages[index];
                
                if(update_window_test_pos(&test_pos, full_log_view, ui->window_scroll_start_y))
                {
                    break;
                }
            }
        
        if(is_view_scrolling(*full_log_view) && ui->start_full_log_from_end)
        {
            ui->start_full_log_from_end = false;
            set_view_at_end(full_log_view);
                }
        
        v4u full_log_clip_rect = get_and_set_window_clip_rect(game->renderer, full_log_view, full_log_rect, full_log_pos);
        
        // Render full log contents
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            
            if(message->is_set)
            {
                v2u message_pos =
                {
                    full_log_pos.x,
                    full_log_pos.y + full_log_view->move.offset
                };
                
                render_text(game, message->string.s, &message_pos, ui->font);
                next_defer_window_entry(&full_log_pos, full_log_view->entry_size);
            }
            else
            {
                break;
            }
        }
        
        //render_outline_rect(game, full_log_clip_rect, Color_Yellow);
        unset_window_clip_rect_and_render_scrollbar(game, full_log_view, full_log_rect, ui);
        
        #if 0
        ui_print_view("Full Log View", *full_log_view);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlag_Mark))
    {
        render_item_mark_window(game, items, inventory->examine_item, assets, ui);
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
        render_multiple_examine_window(game, entities, items, inventory, dungeon, assets, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_Open))
    {
        render_inventory_window(game, items, inventory, dungeon->level, assets, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
    {
        render_multiple_pickup_window(game, player->pos, items, inventory, dungeon->level, assets, ui);
    }
}