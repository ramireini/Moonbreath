internal void
ui_next_line(v2u *pos, u32 multiplier, UI *ui)
{
    pos->y += (ui->font_newline * multiplier);
}

internal void
update_defer_rect_width(u32 start_x, char *text, UI *ui)
{
    assert(text);
    assert(ui);
    
    u32 width = start_x + get_text_width(ui->font, text);
    if(width > ui->defer_rect.w)
    {
        ui->defer_rect.w = width + ui->font_newline;
    }
}

internal void
defer_text(char *text, v2u *pos, u32 next_line_multiplier, UI *ui, ...)
{
    assert(ui);
    assert(text);
    assert(pos);
    
    ui_next_line(pos, next_line_multiplier, ui);
    
    String256 text_format = {0};
    
    va_list arg_list;
    va_start(arg_list, ui);
    vsnprintf(text_format.s, sizeof(text_format), text, arg_list);
    va_end(arg_list);
    
    update_defer_rect_width(pos->x, text_format.s, ui);
    
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Text;
            strcpy(defer->text.s, text_format.s);
            defer->pos = *pos;
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_tile(v2u pos, v4u tile_src, UI *ui)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Tile;
            defer->pos = pos;
            defer->tile_src = tile_src;
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_fill_rect(u32 x, u32 y, u32 w, u32 h, Color color, UI *ui)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_FillRect;
            defer->fill_rect = make_v4u(x, y, w, h);
            defer->fill_rect_color = color;
            
            return;
        }
    }
    
    assert(0);
}

internal void
render_defers(Game *game, Texture tileset, UI *ui)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defers[index];
        
        v2u actual_defer_pos =
        {
            defer->pos.x + ui->defer_rect.x,
            defer->pos.y + ui->defer_rect.y
        };
        
        if(defer->type == DeferType_Text)
        {
            render_text(game, defer->text.s, actual_defer_pos.x, actual_defer_pos.y, ui->font, 0);
        }
        else if(defer->type == DeferType_Tile)
        {
            v4u tile_dest =
            {
                actual_defer_pos.x,
                actual_defer_pos.y,
                32,
                32
            };
            
            SDL_RenderCopy(game->renderer,
                           tileset.tex,
                           (SDL_Rect *)&defer->tile_src,
                           (SDL_Rect *)&tile_dest);
        }
        else if(defer->type == DeferType_FillRect)
        {
            set_render_color(game, Color_DarkGray);
            
            defer->fill_rect.x + ui->defer_rect.x;
            defer->fill_rect.y + ui->defer_rect.y;
            render_fill_rect(game, defer->fill_rect, defer->fill_rect_color);
        }
    }
    
    zero_struct(ui->defer_rect);
    zero_array(ui->defers, MAX_DEFER_COUNT);
}

internal v2u
get_header_pos(u32 y_multiplier, UI *ui)
{
    v2u result =
    {
        ui->defer_rect.x + (ui->window_offset * 2),
        ui->defer_rect.y + (ui->window_offset * y_multiplier)
    };
    
    return(result);
}

internal v2u
start_defer_rect(u32 y_multiplier, UI *ui)
{
    v2u result = get_header_pos(y_multiplier, ui);
    zero_struct(ui->defer_rect);
    
    return(result);
}

internal void
init_defer_window(DeferWindow *window, UI *ui)
{
    window->entry_count = 0;
    window->pos = start_defer_rect(2, ui);
}

internal v2u
get_window_entry_name_pos(v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x + (ui->font_newline * 3),
        pos.y + (ui->font->size / 2)
    };
    
    return(result);
}

internal v2u
render_window_tile(v4u tile_src, v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x + 8,
        pos.y
    };
    
    defer_tile(result, tile_src, ui);
    
    return(result);
}

internal v2u
render_window_separator(v4u rect, v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x,
        pos.y + (ui->font_newline / 2)
    };
    
    defer_fill_rect(result.x,
                    result.y - 4,
                    rect.w - (result.x * 2),
                    1,
                        Color_WindowAccent,
                        ui);
    
    return(result);
}

internal void
render_item_type_header(v4u rect, v2u pos, ItemType type, UI *ui)
{
    v2u header_pos = render_window_separator(rect, pos, ui);
    
    switch(type)
    {
        case ItemType_Weapon: defer_text("Weapons", &header_pos, 0, ui); break;
        case ItemType_Armor: defer_text("Armors", &header_pos, 0, ui); break;
        case ItemType_Potion: defer_text("Potions", &header_pos, 0, ui); break;
        case ItemType_Scroll: defer_text("Scrolls", &header_pos, 0, ui); break;
        case ItemType_Ration: defer_text("Rations", &header_pos, 0, ui); break;
        
        invalid_default_case;
    }
}

internal void
next_defer_rect_line(v2u *pos, UI *ui)
{
    pos->y += ui->window_entry_size;
}

internal void
render_defer_rect_item(UI *ui,
                       DeferWindow *window,
                       Item *item,
                       ItemType type,
                       Inventory *inventory,
                       b32 *needs_header)
                       {
    if(*needs_header)
    {
        *needs_header = false;
        ++window->entry_count;
        
        if(is_entry_in_view(window->view, window->entry_count))
        {
            render_item_type_header(ui->defer_rect, window->pos, type, ui);
            next_defer_rect_line(&window->pos, ui);
        }
    }
    
    ++window->entry_count;
    
    if(is_entry_in_view(window->view, window->entry_count))
    {
        v2u picture_pos = render_window_tile(item->tile_src, window->pos, ui);
        v2u name_pos = get_window_entry_name_pos(picture_pos, ui);
        
        String8 letter = get_item_letter_string(item);
        String128 mark = get_item_mark_string(item);
        
        if(is_item_consumable(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
                defer_text("%s%s%s%s", &name_pos, 0, ui, letter.s, item->name.s, get_item_stack_string(item).s, mark.s);
            }
            else
            {
                defer_text("%s%s%s%s%s", &name_pos, 0, ui, letter.s, item->c.depiction.s, get_item_id_text(item->id), get_item_stack_string(item).s, mark.s);
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
                
                defer_text("%s%s%s%s%s%s",
                               &name_pos, 0, ui,
                           get_item_status_color(item),
                           letter.s,
                           get_item_status_prefix(item),
                           get_full_item_name(item).s,
                           equipped_string.s,
                           mark.s);
            }
            else
            {
                defer_text("%s%s%s", &name_pos, 0, ui, letter.s, get_item_id_text(item->id), mark.s);
            }
        }
        
        next_defer_rect_line(&window->pos, ui);
    }
                       }

internal u32
get_default_defer_window_width()
{
    u32 result = 512;
    return(result);
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

internal void
ui_print_view(char *name, View view)
{
    printf("%s - count: %u\n", name, view.count);
    printf("%s - start: %u\n", name, view.start);
    printf("%s - end: %u\n\n", name, view.end);
}

internal b32
can_render_window_item(Item *item, ItemType type, u32 dungeon_level, v2u pos)
{
    b32 result = false;
    
    if(is_item_valid_and_not_in_inventory(item, dungeon_level) &&
       item->type == type &&
       is_v2u_equal(item->pos, pos))
    {
        result = true;
    }
    
    return(result);
}

internal void
log_add_okay(UI *ui)
{
    log_add("%sOkay.", ui, start_color(Color_Yellow));
}

internal void
render_window_option(char *text, v2u *pos, UI *ui)
{
    defer_text(text, pos, 0, ui);
    pos->x += get_text_width(ui->font, text) + 10;
    }

internal u32
get_centering_offset(u32 total_size, u32 part_size)
{
    assert(total_size > part_size);
    
    u32 result = (total_size - part_size) / 2;
    return(result);
}

internal u32
get_view_range(View view)
{
    u32 result = view.start + view.end;
    return(result);
}

internal b32
is_view_scrolling(View view, u32 count)
{
    b32 result = view.end && (count > view.end);
    return(result);
}

internal void
set_view_at_start(View *view)
{
    view->start = 1;
}

internal void
set_view_at_end(View *view)
{
    view->start = view->count - view->end + 1;
}

internal void
init_view_end(View *view, u32 entry_count)
{
    if(!view->end)
    {
        view->end = entry_count;
    }
}

internal b32
item_fits_using_item_type(ItemUseType type, Item *item)
{
    if(type == ItemUseType_EnchantArmor && item->type == ItemType_Armor) return(true);
    else if(type == ItemUseType_EnchantWeapon && item->type == ItemType_Weapon) return(true);
    else if(type == ItemUseType_Identify && !is_set(item->flags, ItemFlag_IsIdentified)) return(true);
    else if(type == ItemUseType_Uncurse && is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed)) return(true);
    
    return(false);
}

internal v2u
get_examine_window_header_pos(v2u pos, UI *ui)
{
    v2u result = pos;
    
    result.x += ui->window_offset * 4;
    result.y += ui->font->size / 2;
    
    return(result);
}

internal void
move_view_towards_start(View *view)
{
    if(!is_zero(view->start - 1))
    {
        --view->start;
    }
}

internal void
update_view_scrolling(View *view, Input *input)
{
    if(was_pressed(&input->Button_ScrollUp))
    {
        move_view_towards_start(view);
    }
    else if(was_pressed(&input->Button_ScrollDown))
    {
        if(get_view_range(*view) <= view->count)
        {
            ++view->start;
        }
    }
    else if(was_pressed(&input->Key_Home))
    {
        if(is_view_scrolling(*view, view->count))
        {
        set_view_at_start(view);
        }
    }
    else if(was_pressed(&input->Key_End))
    {
        if(is_view_scrolling(*view, view->count))
        {
        set_view_at_end(view);
        }
    }
}

internal b32
can_render_defer_window_item(Item *item, ItemType type, v2u pos, u32 dungeon_level)
{
    b32 result = (is_item_valid_and_not_in_inventory(item, dungeon_level) &&
                  item->type == type &&
                      is_v2u_equal(item->pos, pos));
    
    return(result);
}

internal b32
can_render_inventory_item(Item *item, ItemType type, u32 dungeon_level)
{
    b32 result = (is_item_valid_and_in_inventory(item, dungeon_level) && item->type == type);
    return(result);
}

internal b32
can_add_defer_rect_entry(v2u pos, u32 window_entry_size, u32 window_scroll_start_y)
{
    b32 result = (pos.y + (window_entry_size * 2) < window_scroll_start_y);
    return(result);
}

internal void
add_defer_window_entry(DeferWindow *window, UI *ui)
{
    if(can_add_defer_rect_entry(window->pos, ui->window_entry_size, ui->window_scroll_start_y))
    {
        next_defer_rect_line(&window->pos, ui);
    }
    else
    {
        init_view_end(&window->view, window->entry_count);
    }
    
    ++window->entry_count;
}

internal void
add_defer_window_header_and_body_entry(DeferWindow *window, UI *ui)
{
    add_defer_window_entry(window, ui);
    add_defer_window_entry(window, ui);
}

internal void
render_scrollbar_on_rect(Game *game, UI *ui, v4u rect, View *view)
{
    assert(game);
    assert(ui);
    assert(rect.w);
    assert(rect.h);
    
    if(view && is_view_scrolling(*view, view->count))
    {
        u32 rect_gutter_x = rect.x + rect.w - 10;
        
        v4u gutter = {0};
        gutter.x = rect_gutter_x;
        gutter.y = rect.y;
        gutter.w = 2;
        
        // Get the correct gutter height
        u32 scrollbar_size = (rect.h - (ui->font_newline * 4)) / view->count;
        gutter.h = scrollbar_size * view->count;
        
        // Center gutter vertically relative to rect
        gutter.y += get_centering_offset(rect.h, gutter.h);
        render_fill_rect(game, gutter, Color_WindowAccent);
        
        v4u scrollbar = {0};
        scrollbar.x = rect_gutter_x;
        scrollbar.y = gutter.y + (scrollbar_size * (view->start - 1));
        scrollbar.w = gutter.w;
        scrollbar.h = scrollbar_size * view->end;
        render_fill_rect(game, scrollbar, Color_WindowBorder);
    }
}

internal v4u
get_border_adjusted_rect(v4u rect, u32 border_size)
{
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
center_rect_to_window(v2u window_size, Assets *assets, v4u *rect)
{
    if(rect->w && rect->w)
    {
    rect->x = (window_size.w / 2) - (rect->w / 2);
        rect->y = get_centering_offset(window_size.h - assets->stat_and_log_window_h, rect->h);
        
        // This is so the rectangle can still be seen even if its wrong
        if(rect->y > window_size.h)
        {
            rect->y = 0;
        }
    }
}

internal void
render_centered_rect(Game *game, Assets *assets, v4u *rect)
{
    center_rect_to_window(game->window_size, assets, rect);
    render_rect(game, *rect, 2);
}

internal void
end_defer_rect(Game *game, Assets *assets, View *view, v2u pos, u32 next_line_multiplier, UI *ui)
{
    ui_next_line(&pos, next_line_multiplier, ui);
    
    ui->defer_rect.h = pos.y;
    
    render_centered_rect(game, assets, &ui->defer_rect);
    render_scrollbar_on_rect(game, ui, ui->defer_rect, view);
    
    render_defers(game, assets->tileset, ui);
}

internal void
end_defer_rect_window(Game *game, Assets *assets, DeferWindow *window, UI *ui)
{
    window->view.count = window->entry_count;
    end_defer_rect(game, assets, &window->view, window->pos, 1, ui);
}

internal void
render_entity_examine_window(Game *game, Entity *entity, Assets *assets, UI *ui)
{
    assert(is_entity_valid_and_enemy(entity));
    
    v2u pos = start_defer_rect(1, ui);
    
    // Render entity stats
    v2u name_pos = get_examine_window_header_pos(pos, ui);
    
    if(is_set(entity->flags, EntityFlag_IsInvisible))
    {
        defer_tile(pos, get_dungeon_tileset_rect(DungeonTileID_EntityInvisible), ui);
        
        defer_text("Something", &name_pos, 0, ui);
        defer_text("An unknown being.", &pos, 3, ui);
    }
    else
    {
        defer_tile(pos, entity->tile_src, ui);
        
        defer_text("%s", &name_pos, 0, ui, entity->name.s);
        defer_text("Max HP: %u", &pos, 2, ui, entity->max_hp);
        defer_text("Damage: %u", &pos, 1, ui, entity->e.damage);
        defer_text("Defence: %u", &pos, 1, ui, entity->defence);
        
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
        
        defer_text("Evasion: %s (%u)", &pos, 1, ui, evasion_text, entity->evasion);
        defer_text("Speed: %.01f", &pos, 1, ui, entity->action_time);
        
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
                    defer_text("It has the following resistances:", &pos, 2, ui);
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
                
                defer_text("%s: %s%s (%d)", &pos, 1, ui, damage_type_text, damage_type_text_padding.s, resistance_value_string.s, resistance);
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
                    defer_text("It is affected by the following spells:", &pos, 2, ui);
                }
                
                Spell *spell = status->spell;
                if(!spell->select_letter)
                {
                    spell->select_letter = get_new_letter(ui->select_letters, status->spell, LetterParentType_Spell);
                }
                
                defer_text("%s%s", &pos, 1, ui, get_letter_string(spell->select_letter).s, status->name.s);
            }
        }
        
        // Render entity spells
        if(entity->e.spell_count)
        {
            defer_text("It has the following spells:", &pos, 2, ui);
            
            for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
            {
                Spell *spell = &entity->e.spells[spell_index];
                if(spell->type)
                {
                    if(!spell->select_letter)
                    {
                        spell->select_letter = get_new_letter(ui->select_letters, spell, LetterParentType_Spell);
                    }
                    
                    defer_text("%s%s", &pos, 1, ui, get_letter_string(spell->select_letter).s, spell->name.s);
                }
            }
            
            defer_text("Press the key next to a spell to view it.", &pos, 2, ui);
        }
    }
    
    end_defer_rect(game, assets, 0, pos, 2, ui);
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
    
    v2u pos = start_defer_rect(1, ui);
    defer_text(spell->name.s, &pos, 0, ui);
    defer_text(spell->description.s, &pos, 2, ui);
    
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
    
    defer_text("%s %s.", &pos, 1, ui, spell_range_text, in_spell_range_text);
    end_defer_rect(game, assets, 0, pos, 2, ui);
}

internal void
render_tile_examine_window(Game *game, DungeonTileID tile, Assets *assets, UI *ui)
{
    assert(tile);
    
    v2u pos = start_defer_rect(1, ui);
    defer_tile(pos, get_dungeon_tileset_rect(tile), ui);
    
    v2u header_pos = get_examine_window_header_pos(pos, ui);
    defer_text("%s", &header_pos, 0, ui, get_dungeon_tile_name(tile));
    
    u32 text_multiplier = 3;
    char *text = get_dungeon_tile_description(tile);
    if(text)
    {
        text_multiplier = 2;
        defer_text(text, &pos, 3, ui);
        }
    
    end_defer_rect(game, assets, 0, pos, text_multiplier, ui);
    }

internal void
render_trap_examine_window(Game *game, DungeonTrap *trap, Assets *assets, UI *ui)
{
    assert(trap);
    
    v2u pos = start_defer_rect(1, ui);
    defer_tile(pos, trap->tile_src, ui);
    
    v2u header_pos = get_examine_window_header_pos(pos, ui);
    defer_text("%s%s", &header_pos, 0, ui, get_letter_string(trap->select_letter).s, trap->name.s);
    defer_text(trap->description.s, &pos, 3, ui);
    
    end_defer_rect(game, assets, 0, pos, 2, ui);
}

internal void
render_item_examine_window(Game *game, Item *item, Assets *assets, UI *ui, b32 show_inventory_options)
{
    assert(item);
    
    v2u pos = start_defer_rect(1, ui);
    
    // Render item picture and name
    defer_tile(pos, item->tile_src, ui);
    
    v2u header_pos = get_examine_window_header_pos(pos, ui);
    defer_text("%s%s%s%s%s%s",
                   &header_pos, 0, ui,
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
                defer_text("Damage: %d", &pos, 3, ui, item->w.damage + item->enchantment_level);
                defer_text("Accuracy: %d", &pos, 1, ui, item->w.accuracy + item->enchantment_level);
                defer_text("Attack Speed: %.1f", &pos, 1, ui, item->w.speed);
            }
            else
            {
                defer_text("Base Damage: %u", &pos, 3, ui, item->w.damage);
                defer_text("Base Accuracy: %d", &pos, 1, ui, item->w.accuracy);
                defer_text("Base Attack Speed: %.1f", &pos, 1, ui, item->w.speed);
            }
        }
        else if(item->type == ItemType_Armor)
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
                defer_text("Defence: %d", &pos, 3, ui, item->a.defence + item->enchantment_level);
                defer_text("Weight: %d", &pos, 1, ui, item->a.weight);
            }
            else
            {
                defer_text("Base Defence: %d", &pos, 3, ui, item->a.defence);
                defer_text("Base Weight: %d", &pos, 1, ui, item->a.weight);
            }
        }
        else if(is_item_consumable(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
                defer_text("%s", &pos, 3, ui, item->description.s);
            }
            else
            {
                if(item->type == ItemType_Potion)
                {
                    defer_text("Consuming potions will bestow you with different effects.", &pos, 3, ui);
                    defer_text("Some of these effects will be helpful, while others harmful.", &pos, 1, ui);
                }
                else if(item->type == ItemType_Scroll)
                {
                    defer_text("Reading scrolls will bring out different magical effects.", &pos, 3, ui);
                }
            }
            }
    
    ui_next_line(&pos, 2, ui);
    
    u32 text_multiplier = 0;
    if(is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed))
    {
        text_multiplier = 1;
        defer_text("It is a cursed item.", &pos, 0, ui);
    }
    
    if(is_item_equipment(item->type))
    {
        defer_text("It is of %s rarity.", &pos, text_multiplier, ui, get_item_rarity_text(item->rarity));
        
            if(item->type == ItemType_Weapon)
        {
            defer_text("It is a %s weapon.", &pos, 1, ui, get_item_handedness_text(item->w.handedness));
        }
    }
    
    if(show_inventory_options)
    {
        ui_next_line(&pos, 2, ui);
        render_window_option("(a)djust", &pos, ui);
        
        if(is_item_equipment(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsEquipped))
            {
                render_window_option("(u)nequip", &pos, ui);
            }
            else
            {
                render_window_option("(e)quip", &pos, ui);
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            render_window_option("(c)onsume", &pos, ui);
        }
        else if(item->type == ItemType_Scroll)
        {
            render_window_option("(r)ead", &pos, ui);
        }
        
        render_window_option("(d)rop", &pos, ui);
        render_window_option("(m)ark", &pos, ui);
    }
    
    end_defer_rect(game, assets, 0, pos, 2, ui);
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
    u32 result = (u32)(font_size * 1.15f);
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
        update_log_view(&ui->full_log_view, index);
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
render_defer_window_items(DeferWindow *window,
                          ItemState *items,
                          Inventory *inventory,
                          v2u pos,
                          u32 dungeon_level,
                          UI *ui)
{
    ui_next_line(&window->pos, 1, ui);
    
    for(ItemType type = ItemType_None + 1; type < ItemType_Count; ++type)
    {
        b32 needs_header = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            
            if(can_render_defer_window_item(item, type, pos, dungeon_level))
            {
                if(!item->select_letter)
                {
                    item->select_letter = get_new_letter(ui->select_letters, item, LetterParentType_Item);
                }
                
                render_defer_rect_item(ui, window, item, type, inventory, &needs_header);
            }
        }
    }
}

internal void
update_defer_window_entry_items(DeferWindow *window,
                             ItemState *items,
                             v2u pos,
                             u32 dungeon_level,
                             UI *ui)
{
    v2u test_pos = {0};
    u32 test_entry_count = 0;
    
    for(ItemType type = ItemType_None + 1; type < ItemType_Count; ++type)
    {
        b32 needs_header = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            
            if(can_render_defer_window_item(item, type, pos, dungeon_level))
            {
                if(needs_header)
                {
                    needs_header = false;
                    add_defer_window_entry(window, ui);
                }
                
                add_defer_window_entry(window, ui);
            }
        }
    }
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
    DeferWindow *window = &items->pickup_window;
    assert(window->view.start);
    
    // Set view end
    if(!window->view.end)
    {
        update_defer_window_entry_items(window, items, player_pos, dungeon_level, ui);
    }
    
    init_defer_window(window, ui);
    ui->defer_rect.w = get_default_defer_window_width();
    
    // Get select count
    u32 select_count = 0;
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_selected(item, dungeon_level))
        {
            ++select_count;
        }
    }
    
    String32 select_text = {0};
    if(select_count > 1)
    {
        sprintf(select_text.s, "(%u items)", select_count);
    }
    else if(select_count)
    {
        sprintf(select_text.s, "(%u item)", select_count);
    }
    
    defer_text("Pickup what? %u/%u slots %s", &window->pos, 0, ui, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT, select_text.s);
    render_defer_window_items(window, items, inventory, player_pos, dungeon_level, ui);
    end_defer_rect_window(game, assets, window, ui);
    
#if 0
    ui_print_view("Multiple Pickup", window->view);
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
    DeferWindow *window = &inventory->window;
    assert(window->view.start);
    
    { // Update inventory view if an item was used
        if(inventory->view_update_item_type)
        {
            b32 item_with_same_type_exists = false;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *inventory_item = inventory->slots[index];
                if(inventory_item &&
                   inventory_item->type == inventory->view_update_item_type)
                {
                    item_with_same_type_exists = true;
                    break;
                }
            }
            
            if(item_with_same_type_exists)
            {
                --window->view.count;
            }
            else
            {
                // Take away the item and the item header from view count
                window->view.count -= 2;
            }
            
            inventory->view_update_item_type = ItemType_None;
        }
        
        if(is_view_scrolling(window->view, window->view.count))
        {
            // Set the view at end if something was removed from the bottom of the inventory
            if(get_view_range(window->view) > window->view.count)
            {
                set_view_at_end(&window->view);
            }
        }
        else
        {
            set_view_at_start(&window->view);
        }
    }
    
    // Set view end
    if(!window->view.end)
    {
        v2u test_pos = {0};
        u32 test_entry_count = 0;
        
        for(ItemType type = ItemType_None + 1; type < ItemType_Count; ++type)
        {
            b32 needs_header = true;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                
                    if(can_render_inventory_item(item, type, dungeon_level))
                    {
                    if(needs_header)
                        {
                        needs_header = false;
                        add_defer_window_entry(window, ui);
                    }
                    
                    add_defer_window_entry(window, ui);
                    }
            }
        }
    }
    
    init_defer_window(window, ui);
    ui->defer_rect.w = get_default_defer_window_width();
    
    // Render inventory header
        if(inventory->item_use_type)
    {
        char *item_use_text = 0;
        
            switch(inventory->item_use_type)
        {
            case ItemUseType_Identify: item_use_text = "Identify which item?"; break;
            case ItemUseType_EnchantWeapon: item_use_text = "Enchant which weapon?"; break;
            case ItemUseType_EnchantArmor: item_use_text = "Enchant which armor?"; break;
            case ItemUseType_Uncurse: item_use_text = "Uncurse which item?"; break;
                
                invalid_default_case;
        }
        
        defer_text(item_use_text, &window->pos, 0, ui);
        }
        else
        {
        defer_text("Inventory: %u/%u slots", &window->pos, 0, ui, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
        }
    ui_next_line(&window->pos, 1, ui);
    
    // Render inventory items
    for(ItemType type = ItemType_None + 1; type < ItemType_Count; ++type)
            {
        b32 needs_header = true;
        
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *item = inventory->slots[index];
            
                   if(can_render_inventory_item(item, type, dungeon_level) &&
                   (!inventory->item_use_type || item_fits_using_item_type(inventory->item_use_type, item)))
            {
                render_defer_rect_item(ui, window, item, type, inventory, &needs_header);
                }
                }
            }
    
    end_defer_rect_window(game, assets, window, ui);
    
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
render_defer_window_header_and_body(char *header_name,
                                    DeferWindow *window,
                                    void *parent,
                                    LetterParentType type,
                                    UI *ui)
{
    assert(header_name);
    assert(window);
    assert(parent);
    assert(type);
    assert(ui);
    
    // Render header
    ++window->entry_count;
    
    if(is_entry_in_view(window->view, window->entry_count))
    {
        v2u header_pos = render_window_separator(ui->defer_rect, window->pos, ui);
        defer_text(header_name, &header_pos, 0, ui);
        next_defer_rect_line(&window->pos, ui);
    }
    
    // Render body
    Entity *entity = 0;
    DungeonTrap *trap = 0;
    
    switch(type)
    {
        case LetterParentType_Entity: entity = parent; break;
        case LetterParentType_DungeonTrap: trap = parent; break;
        
        invalid_default_case;
    }
    
    ++window->entry_count;
    
    if(is_entry_in_view(window->view, window->entry_count))
    {
        v2u picture_pos = {0};
        
        switch(type)
        {
            case LetterParentType_Entity: picture_pos = render_window_tile(entity->tile_src, window->pos, ui); break;
            case LetterParentType_DungeonTrap: picture_pos = render_window_tile(trap->tile_src, window->pos, ui); break;
            
            invalid_default_case;
        }
        
        v2u name_pos = get_window_entry_name_pos(picture_pos, ui);
        
        switch(type)
        {
            case LetterParentType_Entity:
            {
                if(!entity->select_letter)
                {
                    entity->select_letter = get_new_letter(ui->select_letters, parent, type);
                }
            } break;
            
            case LetterParentType_DungeonTrap:
            {
                if(!trap->select_letter)
                {
                    trap->select_letter = get_new_letter(ui->select_letters, parent, type);
                }
            } break;
            
            invalid_default_case;
        }
        
        switch(type)
        {
            case LetterParentType_Entity: defer_text("%s%s", &name_pos, 0, ui, get_letter_string(entity->select_letter).s, entity->name.s); break;
            case LetterParentType_DungeonTrap: defer_text("%s%s", &name_pos, 0, ui, get_letter_string(trap->select_letter).s, trap->name.s); break;
            
            invalid_default_case;
        };
        
        next_defer_rect_line(&window->pos, ui);
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
    Examine *examine = &game->examine;
    DeferWindow *window = &items->examine_window;
    assert(window->view.start);
    
    // Set view end
    if(!window->view.end)
    {
        v2u test_pos = {0};
        u32 test_entry_count = 0;
        
        for(ExamineType examine_type = ExamineType_None + 1;
            examine_type < ExamineType_Count;
            ++examine_type)
        {
            if(examine_type == ExamineType_Entity)
            {
                items->examine_window_entity = get_dungeon_pos_entity(entities, dungeon->level, examine->pos, true);
                if(items->examine_window_entity)
                {
                    add_defer_window_header_and_body_entry(window, ui);
                }
                }
            else if(examine_type == ExamineType_Item)
            {
                update_defer_window_entry_items(window, items, examine->pos, dungeon->level, ui);
            }
                else if(examine_type == ExamineType_Trap)
            {
                items->examine_window_trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, examine->pos);
                if(items->examine_window_trap)
                {
                    add_defer_window_header_and_body_entry(window, ui);
                }
            }
        }
    }
    
    init_defer_window(window, ui);
    ui->defer_rect.w = get_default_defer_window_width();
    
    defer_text("Examine what?", &window->pos, 0, ui);
    Entity *entity = items->examine_window_entity;
    DungeonTrap *trap = items->examine_window_trap;
    
    for(ExamineType examine_type = ExamineType_None + 1;
        examine_type < ExamineType_Count;
        ++examine_type)
    {
        if(examine_type == ExamineType_Entity && entity)
        {
            render_defer_window_header_and_body("Enemies", window, entity, LetterParentType_Entity, ui);
        }
        else if(examine_type == ExamineType_Item)
        {
            render_defer_window_items(window, items, inventory, examine->pos, dungeon->level, ui);
        }
        else if(examine_type == ExamineType_Trap && trap)
        {
            render_defer_window_header_and_body("Traps", window, trap, LetterParentType_DungeonTrap, ui);
            }
            }
    
    end_defer_rect_window(game, assets, window, ui);
    
    #if 0
    ui_print_view("Multiple Examine", window->view);
#endif
    
    }

internal void
render_item_mark_window(Game *game, ItemState *items, Item *item, Assets *assets, UI *ui)
{
    v4u mark_rect = {0, 0, 250, 100};
    render_centered_rect(game, assets, &mark_rect);
    
    { // Render header
        char *header_text = "Mark with what?";
        if(is_set(item->flags, ItemFlag_IsMarked))
        {
            header_text = "Replace mark with what?";
        }
        
        v2u header_pos =
        {
            mark_rect.x + get_centering_offset(mark_rect.w, get_text_width(ui->font, header_text)),
            mark_rect.y + 25
        };
        
        render_text(game, header_text, header_pos.x, header_pos.y, ui->font, 0);
    }
    
    // Render input box
    u32 height_padding = 4;
    v4u input_rect = {mark_rect.x, mark_rect.y, ui->font->size * 14, ui->font->size + height_padding};
    
    input_rect.x += get_centering_offset(mark_rect.w, input_rect.w);
    input_rect.y += ui->font_newline * 3;
    
    render_fill_rect(game, input_rect, Color_WindowAccent);
    render_outline_rect(game, input_rect, Color_WindowBorder);
    
    // Update cursor
    Mark *temp_mark = &items->temp_mark;
    
    if(!temp_mark->cursor_render_start)
    {
        temp_mark->cursor_render_start = SDL_GetTicks();
    }
    
    if(get_sdl_ticks_difference(temp_mark->cursor_render_start) >= temp_mark->cursor_blink_duration)
    {
        temp_mark->cursor_render_start = 0;
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
        
        render_text(game, "%c", character_pos.x, character_pos.y, ui->font, 0, temp_mark->array[mark_index]);
        character_pos.x += get_glyph_width(ui->font, temp_mark->array[mark_index]);
        
        if(temp_mark->render_cursor && (index == temp_mark->cursor_index))
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
    ui_print_view("Mark View", mark->view);
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
    
    ui->window_scroll_start_y = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_rect =
    {
        0,
        ui->window_scroll_start_y,
        388,
        assets->stat_and_log_window_h
    };
    render_rect(game, stat_rect, 2);
    
    v4u short_log_rect =
    {
        stat_rect.w + 4,
        ui->window_scroll_start_y,
        game->window_size.w - short_log_rect.x,
        assets->stat_and_log_window_h
    };
    render_rect(game, short_log_rect, 2);
    
    // Render stats
    v2u left =
    {
        ui->window_offset,
        (game->window_size.h - assets->stat_and_log_window_h) + ui->window_offset
    };
    
    v2u right =
    {
        left.x + 160,
        left.y
    };
    
    { // Render stats
        defer_text(player->name.s, &left, 0, ui);
        defer_text("Health:    %u/%u", &left, 1, ui, player->hp, player->max_hp);
        defer_text("Strength:     %u", &left, 1, ui, player->strength);
        defer_text("Intelligence: %u", &left, 1, ui, player->intelligence);
        defer_text("Dexterity:    %u", &left, 1, ui, player->dexterity);
        defer_text("Evasion:      %u", &left, 1, ui, player->evasion);
        defer_text("Defence:      %u", &left, 1, ui, player->defence);
        defer_text("Weight:       %u", &left, 1, ui, player->p.weight);
    
        { // Render healthbar
            right.y += ui->font_newline - 1;
            
            // Healthbar border
            v4u healthbar_outside = {right.x, right.y, 204, 16};
            render_fill_rect(game, healthbar_outside, Color_WindowBorder);
            
            // Healthbar background
            v4u healthbar_inside = get_border_adjusted_rect(healthbar_outside, 1);
            render_fill_rect(game, healthbar_inside, Color_WindowAccent);
            
            if(player->hp > 0)
            {
                healthbar_inside.w = get_ratio(player->hp, player->max_hp, healthbar_inside.w);
                render_fill_rect(game, healthbar_inside, Color_DarkRed);
            }
        }
        
        defer_text("Time:          %.01f", &right, 1, ui, game->time);
        defer_text("Action Time:   %.01f", &right, 1, ui, player->action_time);
        defer_text("Dungeon Level: %u", &right, 1, ui, dungeon->level);
        
    render_defers(game, assets->tileset, ui);
    }
    
    // Render Short Log
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
            render_text(game, message->string.s, short_log_pos.x, short_log_pos.y, ui->font, 0);
            ui_next_line(&short_log_pos, 1, ui);
        }
    }
    
    // Render Full Log
    if(ui->full_log_open)
    {
        ui->full_log_view.count = 0;
        zero_struct(ui->defer_rect);
        
        v2u full_log_pos =
        {
            ui->defer_rect.x + ui->window_offset,
            ui->defer_rect.y + ui->window_offset
        };
        
        // Set full log view end index
        v2u test_message_pos = full_log_pos;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            
            if(message->is_set)
            {
                ++ui->full_log_view.count;
                
                if(can_add_defer_rect_entry(test_message_pos, ui->font_newline * 2, ui->window_scroll_start_y))
                {
                    ui_next_line(&test_message_pos, 1, ui);
                }
                else
                {
                    init_view_end(&ui->full_log_view, index + 1);
                }
            }
        }
        
        // Always set the log view at end if we're scrolling
        if(is_view_scrolling(ui->full_log_view, ui->full_log_view.count) &&
               !ui->full_log_at_end)
        {
            ui->full_log_at_end = true;
            set_view_at_end(&ui->full_log_view);
        }
        
        // Render log messages
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            
            if(message->is_set)
            {
                if(is_entry_in_view(ui->full_log_view, index + 1))
                {
                    defer_text(message->string.s, &full_log_pos, 0, ui);
                    ui_next_line(&full_log_pos, 1, ui);
                }
                else
                {
                    // Even if the entry is not in view, update the ui defer rect so that the
                    // full log width is the same as the longest log message.
                    update_defer_rect_width(full_log_pos.x, message->string.s, ui);
                }
            }
        }
        
        full_log_pos.y += (ui->font_newline / 2);
        end_defer_rect(game, assets, &ui->full_log_view, full_log_pos, 1, ui);
        
        #if 0
        ui_print_view("Full Log View", ui->full_log_view);
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