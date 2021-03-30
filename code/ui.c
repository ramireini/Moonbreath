internal void
log_add_okay(UI *ui)
{
    log_add(ui, "%sOkay.", start_color(Color_Yellow));
}

internal void
log_add_item_cursed_unequip(UI *ui, Item *item)
{
    log_add(ui, "You try to unequip the %s.. but a force stops you from doing so!", item->name);
}

internal void
log_add_item_action_text(UI *ui, Item *item, ItemActionType action)
{
    char action_text[8] = {0};
    
    switch(action)
    {
        case ItemActionType_PickUp: strcpy(action_text, "pick up"); break;
        case ItemActionType_Drop: strcpy(action_text, "drop"); break;
        case ItemActionType_Equip: strcpy(action_text, "equip"); break;
        case ItemActionType_Unequip: strcpy(action_text, "unequip"); break;
        
        invalid_default_case;
    }
    
    log_add(ui, "You %s the %s%s%s%s%s%s",
            action_text,
            get_item_status_color(item),
            get_item_status_prefix(item),
            get_full_item_name(item).str,
            get_item_stack_string(item).str,
            get_item_mark_string(item).str,
            end_color());
}

internal void
render_window_option(UI *ui, char *text, v2u *pos)
{
    defer_text(ui, text, pos->x, pos->y);
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
get_ui_padding()
{
    u32 result = 10;
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
    b32 result = false;
    
    if((type == UsingItemType_Identify && !is_set(item->flags, ItemFlags_IsIdentified)) ||
       (type == UsingItemType_EnchantWeapon && item->type == ItemType_Weapon) ||
       (type == UsingItemType_EnchantArmor && item->type == ItemType_Armor) ||
           (type == UsingItemType_Uncurse && is_set(item->flags, ItemFlags_IsIdentified | ItemFlags_IsCursed)))
    {
        result = true;
    }
    
    return(result);
}

internal void
render_item_type_header(UI *ui, v4u rect, v2u pos, ItemType type)
{
    v2u header = {pos.x, pos.y + (ui->font_newline / 2)};
    defer_fill_rect(ui,
                        header.x,
                        header.y - 4,
                        rect.w - (header.x * 2),
                        1,
                    Color_WindowAccent);
    
    switch(type)
    {
        
        case ItemType_Weapon: defer_text(ui, "Weapon", header.x, header.y); break;
        case ItemType_Armor: defer_text(ui, "Armor", header.x, header.y); break;
        case ItemType_Potion: defer_text(ui, "Potion", header.x, header.y); break;
        case ItemType_Scroll: defer_text(ui, "Scroll", header.x, header.y); break;
        case ItemType_Ration: defer_text(ui, "Ration", header.x, header.y); break;
        
        invalid_default_case;
    }
}

internal v2u
get_header_text_pos(UI *ui, v2u header)
{
    v2u result = header;
    
    result.x += ui->window_offset * 4;
    result.y += ui->font->size / 2;
    
    return(result);
}

internal v2u
get_header_pos(UI *ui, v4u rect, u32 y_multiplier)
{
    v2u result =
    {
        rect.x + (ui->window_offset * 2),
        rect.y + (ui->window_offset * y_multiplier)
    };
    
    return(result);
}

internal v2u
set_defer_rect_and_get_header_pos(UI *ui)
{
    ui->defer_rect = make_v4u(0, 0, 0, 0);
    v2u result = get_header_pos(ui, ui->defer_rect, 2);
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
    else if(was_pressed(&input->Key_PageUp))
    {
        if(is_view_scrolling(*view, view->count))
        {
            view->start -= view->end;
            if(is_zero(view->start - 1))
            {
                set_view_at_start(view);
            }
        }
    }
    else if(was_pressed(&input->Key_PageDown))
    {
        view->start += view->end;
        if(get_view_range(*view) > view->count)
        {
            set_view_at_end(view);
        }
    }
}

internal b32
entry_has_space(v2u pos, u32 entry_size, u32 window_asset_y)
{
    b32 result = (pos.y + (entry_size * 2) < window_asset_y);
    return(result);
}

internal void
render_scrollbar(Game *game, UI *ui, v4u rect, View *view)
{
    if(is_view_scrolling(*view, view->count))
    {
        assert(rect.w && rect.h);
        
        u32 rect_gutter_x = rect.x + rect.w - get_ui_padding();
        
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
center_window_to_available_screen(v2u game_window_size, Assets *assets, v4u *rect)
{
    if(rect->w && rect->w)
    {
    rect->x = (game_window_size.w / 2) - (rect->w / 2);
    rect->y = get_centering_offset(game_window_size.h - assets->stat_and_log_window_h, rect->h);
           }
}

internal void
center_and_render_window_to_available_screen(Game *game, Assets *assets, v4u *rect)
{
    center_window_to_available_screen(game->window_size, assets, rect);
    render_window(game, *rect, 2);
}

internal void
process_window_end(Game *game, Assets *assets, UI *ui, View *view, v2u pos)
{
    ui->defer_rect.h = pos.y;
    
    center_and_render_window_to_available_screen(game, assets, &ui->defer_rect);
    
    if(view)
    {
    render_scrollbar(game, ui, ui->defer_rect, view);
    }
    
    process_defer(game, assets, ui);
}

internal void
render_examine_item(Game *game, Item *item, UI *ui, v2u *pos, CameFrom came_from)
{
    // Render item picture and name
    defer_texture(ui, *pos, item->tile_src);
    
    v2u header = get_header_text_pos(ui, *pos);
    defer_text(ui, "%s%s%s%s%s%s",
               header.x, header.y,
               get_item_status_color(item),
                   get_item_letter_string(item).str,
               get_item_status_prefix(item),
                   get_full_item_name(item).str,
                   get_item_stack_string(item).str,
               get_item_mark_string(item).str);
    
    pos->y += ui->font_newline * 2;
    
    if(is_set(item->flags, ItemFlags_IsIdentified))
    {
        if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Damage: %d", pos->x, pos->y, item->w.damage + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Accuracy: %d", pos->x, pos->y, item->w.accuracy + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Attack Speed: %.1f", pos->x, pos->y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Defence: %d", pos->x, pos->y, item->a.defence + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Weight: %d", pos->x, pos->y, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            pos->y += ui->font_newline;
            defer_text(ui, "%s", pos->x, pos->y, item->description);
        }
    }
    else
    {
        if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Base Damage: %u", pos->x, pos->y, item->w.damage);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Accuracy: %d", pos->x, pos->y, item->w.accuracy);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Attack Speed: %.1f", pos->x, pos->y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Base Defence: %d", pos->x, pos->y, item->a.defence);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Weight: %d", pos->x, pos->y, item->a.weight);
        }
        else if(item->type == ItemType_Potion)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Consuming potions will bestow you with different effects.", pos->x, pos->y);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Some of these effects will be helpful, while others harmful.", pos->x, pos->y);
        }
        else if(item->type == ItemType_Scroll)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Reading scrolls will bring out different magical effects.", pos->x, pos->y);
        }
    }
    
    pos->y += ui->font_newline * 2;
    
    if(is_set(item->flags, ItemFlags_IsIdentified | ItemFlags_IsCursed))
    {
        defer_text(ui, "It is a cursed item.", pos->x, pos->y);
        pos->y += ui->font_newline;
    }
    
    if(is_item_equipment(item->type))
    {
        defer_text(ui, "It is of %s rarity.", pos->x, pos->y, get_item_rarity_text(item->rarity));
        
            if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            
            defer_text(ui, "It is a %s weapon.", pos->x, pos->y, get_item_handedness_text(item->w.handedness));
        }
        
        pos->y += ui->font_newline * 2;
    }
    
    if(came_from == CameFrom_Inventory)
    {
        u32 padding = get_ui_padding();
        render_window_option(ui, "(a)djust", pos);
        
        if(is_item_equipment(item->type))
        {
            if(is_set(item->flags, ItemFlags_IsEquipped))
            {
                render_window_option(ui, "(u)nequip", pos);
            }
            else
            {
                render_window_option(ui, "(e)quip", pos);
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            render_window_option(ui, "(c)onsume", pos);
        }
        else if(item->type == ItemType_Scroll)
        {
            render_window_option(ui, "(r)ead", pos);
        }
        
        render_window_option(ui, "(d)rop", pos);
        render_window_option(ui, "(m)ark", pos);
        
        pos->y += ui->font_newline * 2;
    }
}

internal b32
is_entry_in_view(View view, u32 entry)
{
    b32 result = (!view.end || (entry >= view.start &&
                                entry < get_view_range(view)));
    
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
log_add(UI *ui, char *text, ...)
{
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted.str, sizeof(formatted), text, arg_list);
    va_end(arg_list);
    
    // Copy the new text to a vacant log index if there is one
    for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        update_log_view(&ui->full_log_view, index);
        update_log_view(&ui->short_log_view, index);
        
        if(!ui->log_messages[index].str[0])
        {
            strcpy(ui->log_messages[index].str, formatted.str);
            return;
        }
    }
    
    // Move all texts up by one
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        strcpy(ui->log_messages[index - 1].str, ui->log_messages[index].str);
    }
    
    // Copy the new text to the bottom
    strcpy(ui->log_messages[MAX_LOG_MESSAGE_COUNT - 1].str, formatted.str);
}

internal void
render_item_window(Game *game,
                   ItemState *items,
                   Inventory *inventory,
                   View *view,
                   UI *ui,
                   Assets *assets,
                   v2u player_pos,
                   u32 dungeon_level,
                   CameFrom came_from,
                   u32 screen_bottom_y)
{
    assert(came_from);
    assert(view);
    assert(view->start);
    
    v2u pos = set_defer_rect_and_get_header_pos(ui);
    
    // This sets the view->end value which tells us how many items the view can show.
    // We go through the items and see when they would go past screen_bottom_y and set
    // view->end based on that.
    if(!view->end)
    {
        v2u test_pos = {0};
        u32 test_entry_count = 0;
        
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_item_type_header = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                b32 can_process = false;
                Item *item = &items->array[index];
                
                if(came_from == CameFrom_Inventory && is_item_valid_and_in_inventory(item, dungeon_level))
                {
                    can_process = true;
                }
                else if(came_from == CameFrom_Examine && is_item_valid_and_not_in_inventory(item, dungeon_level))
                {
                    can_process = true;
                }
                else if(came_from == CameFrom_Pickup && is_item_valid_and_not_in_inventory(item, dungeon_level))
                {
                    can_process = true;
                }
                
                if(can_process)
                {
                    if(item->type == type)
                    {
                        if(needs_item_type_header)
                        {
                            needs_item_type_header = false;
                            ++test_entry_count;
                            
                            if(entry_has_space(test_pos, inventory->entry_size, screen_bottom_y))
                            {
                                test_pos.y += inventory->entry_size;
                            }
                            else
                            {
                                init_view_end(view, test_entry_count);
                            }
                        }
                        
                        if(entry_has_space(test_pos, inventory->entry_size, screen_bottom_y))
                        {
                            test_pos.y += inventory->entry_size;
                        }
                        else
                        {
                            init_view_end(view, test_entry_count);
                        }
                        
                        ++test_entry_count;
                    }
            }
            }
        }
    }
    
    u32 entry_count = 0;
    
    // Render window header
    if(came_from == CameFrom_Inventory)
    {
        if(inventory->item_use_type)
        {
            switch(inventory->item_use_type)
            {
                case UsingItemType_Identify: defer_text(ui, "Identify which item?", pos.x, pos.y); break;
                case UsingItemType_EnchantWeapon: defer_text(ui, "Enchant which weapon?", pos.x, pos.y); break;
                case UsingItemType_EnchantArmor: defer_text(ui, "Enchant which armor?", pos.x, pos.y); break;
                case UsingItemType_Uncurse: defer_text(ui, "Uncurse which item?", pos.x, pos.y); break;
                
                invalid_default_case;
            }
        }
        else
        {
            defer_text(ui, "Inventory: %u/%u slots", pos.x, pos.y, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
        }
    }
    else if(came_from == CameFrom_Examine)
    {
        defer_text(ui, "Examine which item?", pos.x, pos.y);
    }
    else if(came_from == CameFrom_Pickup)
    {
        char select_text[32] = {0};
        
        u32 select_item_count = 0;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            if(is_item_valid_and_selected(item, dungeon_level))
            {
                ++select_item_count;
            }
        }
        
        if(select_item_count > 1)
        {
            sprintf(select_text, " (%u items selected)", select_item_count);
        }
        else if(select_item_count)
        {
            sprintf(select_text, " (%u item selected)", select_item_count);
        }
        
        //printf("select_text: %s\n", select_text);
        
        defer_text(ui, "Pick up which items? %u/%u slots%s", pos.x, pos.y, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT, select_text);
    }
    
    pos.y += ui->font_newline;
    
    // Render window items
    for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
    {
        b32 needs_item_type_header = true;
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            b32 can_process = false;
            Item *item = &items->array[index];
            
            if(came_from == CameFrom_Inventory &&
                   is_item_valid_and_in_inventory(item, dungeon_level) &&
               item->type == type &&
                   (!inventory->item_use_type || item_fits_using_item_type(inventory->item_use_type, item)))
            {
                can_process = true;
            }
            else if(came_from == CameFrom_Examine &&
                        is_item_valid_and_not_in_inventory(item, dungeon_level) &&
                    item->type == type &&
                        is_v2u_equal(item->pos, game->examine.pos))
            {
                can_process = true;
            }
            else if(came_from == CameFrom_Pickup &&
                        is_item_valid_and_not_in_inventory(item, dungeon_level) &&
                    item->type == type &&
                        is_v2u_equal(item->pos, player_pos))
            {
                can_process = true;
            }
            
            if(can_process)
            {
                if(needs_item_type_header)
                {
                    needs_item_type_header = false;
                    ++entry_count;
                    
                    if(is_entry_in_view(*view, entry_count))
                    {
                        render_item_type_header(ui, ui->defer_rect, pos, type);
                        pos.y += inventory->entry_size;
                    }
                }
                
                ++entry_count;
                
                if(is_entry_in_view(*view, entry_count))
                {
                    v2u picture_pos = {pos.x + 8, pos.y};
                    defer_texture(ui, picture_pos, item->tile_src);
                    
                    v2u name_pos =
                    {
                        picture_pos.x + (ui->font_newline * 3),
                        picture_pos.y + (ui->font->size / 2)
                    };
                    
                    if((came_from == CameFrom_Examine || came_from == CameFrom_Pickup) &&
                           !item->select_letter)
                    {
                        item->select_letter = get_free_item_letter(items, dungeon_level, LetterType_SelectLetter);
                    }
                    
                    String128 letter = get_item_letter_string(item);
                    String128 mark_text = get_item_mark_string(item);
                    
                    if(is_item_consumable(item->type))
                    {
                        if(is_set(item->flags, ItemFlags_IsIdentified))
                        {
                            defer_text(ui, "%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->name, get_item_stack_string(item).str, mark_text.str);
                        }
                        else
                        {
                            defer_text(ui, "%s%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->c.depiction, get_item_id_text(item->id), get_item_stack_string(item).str, mark_text.str);
                        }
                    }
                    else
                    {
                        if(is_set(item->flags, ItemFlags_IsIdentified))
                        {
                            char equipped_text[16] = {0};
                            if(is_set(item->flags, ItemFlags_IsEquipped))
                            {
                                sprintf(equipped_text, " (equipped)");
                            }
                            
                            defer_text(ui, "%s%s%s%s%s%s",
                                       name_pos.x, name_pos.y,
                                       get_item_status_color(item),
                                       letter.str,
                                       get_item_status_prefix(item),
                                       get_full_item_name(item).str,
                                       equipped_text,
                                       mark_text.str);
                        }
                        else
                        {
                            defer_text(ui, "%s%s%s", name_pos.x, name_pos.y, letter.str, get_item_id_text(item->id), mark_text.str);
                        }
                    }
                    
                    pos.y += inventory->entry_size;
                }
            }
        }
    }
    
    pos.y += ui->font_newline;
    view->count = entry_count;
    process_window_end(game, assets, ui, view, pos);
    
#if 0
    printf("view.count: %u\n", view->count);
    printf("view.start: %u\n", view->start);
    printf("view.end: %u\n\n", view->end);
#endif
    
}

internal void
render_ui(Game *game,
          Input *input,
          Entity *player,
          ItemState *items,
          Inventory *inventory,
           Dungeon *dungeon,
          Assets *assets,
          UI *ui)
{
    Examine *examine = &game->examine;
    u32 screen_bottom_y = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_rect = {0, screen_bottom_y, 388, assets->stat_and_log_window_h};
    render_window(game, stat_rect, 2);
    
    v4u short_log_rect = {stat_rect.w + 4, screen_bottom_y, game->window_size.w - short_log_rect.x, assets->stat_and_log_window_h};
    render_window(game, short_log_rect, 2);
    
    // Render Stats
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
    
    { // Left side
        render_text(game, player->name, left.x, left.y, ui->font, 0);
        
        left.y += ui->font_newline;
        render_text(game, "Health:    %u/%u", left.x, left.y, ui->font, 0, player->hp, player->max_hp);
        
        left.y += ui->font_newline;
        render_text(game, "Strength:     %u", left.x, left.y, ui->font, 0, player->strength);
        
        left.y += ui->font_newline;
        render_text(game, "Intelligence: %u", left.x, left.y, ui->font, 0, player->intelligence);
        
        left.y += ui->font_newline;
        render_text(game, "Dexterity:    %u", left.x, left.y, ui->font, 0, player->dexterity);
        
        left.y += ui->font_newline;
        render_text(game, "Evasion:      %u", left.x, left.y, ui->font, 0, player->evasion);
        
        left.y += ui->font_newline;
        render_text(game, "Defence:      %u", left.x, left.y, ui->font, 0, player->defence);
        
        left.y += ui->font_newline;
        render_text(game, "Weight:       %u", left.x, left.y, ui->font, 0, player->p.weight);
    }
    
    { // Right side
        
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
        
        right.y += ui->font_newline;
        render_text(game, "Time:          %.01f", right.x, right.y, ui->font, 0, game->time);
        
        right.y += ui->font_newline;
        render_text(game, "Action Time:   %.01f", right.x, right.y, ui->font, 0, player->action_count);
        
        right.y += ui->font_newline;
        render_text(game, "Dungeon Level: %u", right.x, right.y, ui->font, 0, dungeon->level);
    }
    
    // Short Log
    assert(ui->short_log_view.end);
    
    v2u full_log_message_pos =
    {
        short_log_rect.x + ui->window_offset,
        short_log_rect.y + (ui->font->size / 2)
    };
    
    for(u32 index = ui->short_log_view.start; index < get_view_range(ui->short_log_view); ++index)
    {
        if(ui->log_messages[index].str[0])
        {
            render_text(game, ui->log_messages[index].str,
                        full_log_message_pos.x, full_log_message_pos.y,
                        ui->font, 0);
            
            full_log_message_pos.y += ui->font_newline;
        }
    }
    
    // Full Log
    if(ui->is_full_log_open)
    {
        ui->defer_rect = make_v4u(0, 0, 0, 0);
        
        v2u full_log_message_pos =
        {
            ui->defer_rect.x + ui->window_offset,
            ui->defer_rect.y + ui->window_offset
        };
        
        v2u test_message_pos = full_log_message_pos;
        ui->full_log_view.count = 0;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                ++ui->full_log_view.count;
                
                if(entry_has_space(test_message_pos, ui->font_newline * 2, screen_bottom_y))
                {
                    test_message_pos.y += ui->font_newline;
                }
                else
                {
                    init_view_end(&ui->full_log_view, index + 1);
                }
            }
        }
        
        if(is_view_scrolling(ui->full_log_view, ui->full_log_view.count) && !ui->is_full_log_view_set_at_end)
        {
            ui->is_full_log_view_set_at_end = true;
            set_view_at_end(&ui->full_log_view);
        }
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                if(is_entry_in_view(ui->full_log_view, index + 1))
                {
                    defer_text(ui, ui->log_messages[index].str, full_log_message_pos.x, full_log_message_pos.y);
                    full_log_message_pos.y += ui->font_newline;
                }
            }
        }
        
        full_log_message_pos.y += (ui->font_newline / 2);
        process_window_end(game, assets, ui, &ui->full_log_view, full_log_message_pos);
        
#if 0
        printf("full_log.count: %u\n", ui->full_log_view.count);
        printf("full_log.start: %u\n", ui->full_log_view.start);
        printf("full_log.end: %u\n\n", ui->full_log_view.end);
#endif
        
    }
    else if(is_set(game->examine.flags, ExamineFlags_Open))
    {
        if(examine->type)
        {
            ui->defer_rect = make_v4u(0, 0, 0, 0);
            v2u pos = get_header_pos(ui, ui->defer_rect, 1);
            
            switch(examine->type)
            {
                case ExamineType_Item:
                {
                    render_examine_item(game, game->examine.item, ui, &pos, CameFrom_Examine);
                } break;
                
                case ExamineType_Entity:
                {
                    Entity *entity = game->examine.entity;
                    assert(entity->type == EntityType_Enemy);
                    
                    defer_texture(ui, pos, entity->tile_src);
                    
                    // Render entity stats
                    v2u name = get_header_text_pos(ui, pos);
                    defer_text(ui, "%s", name.x, name.y, entity->name);
                    pos.y += ui->font_newline * 2;
                    
                    pos.y += ui->font_newline;
                    defer_text(ui, "Max HP: %u", pos.x, pos.y, entity->max_hp);
                    
                    pos.y += ui->font_newline;
                    defer_text(ui, "Damage: %u", pos.x, pos.y, entity->e.damage);
                    
                    pos.y += ui->font_newline;
                    defer_text(ui, "Defence: %u", pos.x, pos.y, entity->defence);
                    
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
                    
                    pos.y += ui->font_newline;
                    defer_text(ui, "Evasion: %s (%u)", pos.x, pos.y, evasion_text, entity->evasion);
                    
                    pos.y += ui->font_newline;
                    defer_text(ui, "Speed: %.01f", pos.x, pos.y, entity->action_count);
                    
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
                                pos.y += ui->font_newline * 2;
                                defer_text(ui, "It has the following resistances:", pos.x, pos.y);
                                
                                render_prefix_text = false;
                            }
                            
                            // This sets the padding after the damage_type_text, we do this so that
                            // the resistance_text_value's start aligned.
                            char *damage_type_text = get_damage_type_text(damage_type_index);
                            char damage_type_text_padding[32] = {0};
                            u32 damage_type_text_padding_length = longest_damage_type_length - get_string_length(damage_type_text);
                            
                            for(u32 index = 0; index < damage_type_text_padding_length; ++index)
                            {
                                damage_type_text_padding[index] = ' ';
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
                            char resistance_text_value[8] = {0};
                            for(u32 string_index = 0; string_index < absolute_resistance; ++string_index)
                            {
                                resistance_text_value[string_index] = resistance_char;
                            }
                            
                            // This sets the padding at the end of resistance_text_value, we do this so that
                            // the rendered resistance values start aligned.
                            u32 length = highest_damage_type_resistance - absolute_resistance;
                            u32 padding_length = absolute_resistance + length;
                            for(u32 padding_index = absolute_resistance; padding_index < padding_length; ++padding_index)
                            {
                                resistance_text_value[padding_index] = ' ';
                            }
                            
                            // Positive values need an extra space, hence the boolean for it.
                            if(add_extra_resistance_space)
                            {
                                resistance_text_value[padding_length] = ' ';
                            }
                            
                            pos.y += ui->font_newline;
                            defer_text(ui, "%s: %s%s (%d)",
                                       pos.x, pos.y,
                                       damage_type_text,
                                       damage_type_text_padding,
                                       resistance_text_value,
                                       resistance);
                        }
                    }
                    
                    // Render entity status effects
                    if(entity_has_any_status_effect(entity))
                    {
                        pos.y += ui->font_newline * 2;
                        defer_text(ui, "It is under the following status effects:", pos.x, pos.y);
                        
                        for(u32 index = StatusEffectType_None + 1; index < StatusEffectType_Count; ++index)
                        {
                            if(entity_has_status_effect(entity, index))
                            {
                                pos.y += ui->font_newline;
                                defer_text(ui, "%s", pos.x, pos.y, get_status_effect_name(index));
                            }
                        }
                    }
                    
                    // Render entity spells
                    char spell_letter = 'a';
                    if(is_set(entity->flags, EntityFlags_UsesMagicAttacks))
                    {
                        pos.y += ui->font_newline * 2;
                        defer_text(ui, "It has the following spells:", pos.x, pos.y);
                        
                        for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
                        {
                            Spell *spell = &entity->e.spells[spell_index];
                            if(spell->id)
                            {
                                pos.y += ui->font_newline;
                                defer_text(ui, "%c - %s", pos.x, pos.y, spell_letter, get_spell_name(spell->id));
                                
                                ++spell_letter;
                            }
                        }
                        
                        pos.y += ui->font_newline * 2;
                        defer_text(ui, "Press the key shown to read a description.", pos.x, pos.y);
                    }
                    
                    pos.y += ui->font_newline * 2;
                } break;
                
                case ExamineType_EntitySpell:
                {
                    Entity *enemy = examine->entity;
                    Spell *spell = examine->spell;
                    assert(spell->type);
                    
                    defer_text(ui, get_spell_name(spell->id), pos.x, pos.y);
                    pos.y += ui->font_newline * 2;
                    
                    defer_text(ui, get_spell_description(spell->id), pos.x, pos.y);
                    pos.y += ui->font_newline * 2;
                    
                    switch(spell->type)
                    {
                        case SpellType_Offensive:
                        {
                            defer_text(ui, "Damage Type: %s", pos.x, pos.y, get_damage_type_text(spell->damage_type));
                            pos.y += ui->font_newline;
                            
                            defer_text(ui, "Damage: %u", pos.x, pos.y, spell->value);
                            pos.y += ui->font_newline;
                        } break;
                        
                        case SpellType_Healing:
                        {
                            defer_text(ui, "Healing: %u", pos.x, pos.y, spell->value);
                            pos.y += ui->font_newline;
                        } break;
                        
                        case SpellType_Buff:
                        {
                            defer_text(ui, "Increase: %u", pos.x, pos.y, spell->value);
                            pos.y += ui->font_newline;
                            
                            defer_text(ui, "Duration: %u", pos.x, pos.y, spell->duration);
                            pos.y += ui->font_newline;
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    char spell_range_text[24] = {0};
                    if(spell->range)
                    {
                        sprintf(spell_range_text, "Range: %u", spell->range);
                    }
                    else
                    {
                        sprintf(spell_range_text, "Range: Line of sight");
                    }
                    
                    char in_spell_range_text[24] = {0};
                    if(in_spell_range(spell->range, enemy->pos, player->pos))
                    {
                        sprintf(in_spell_range_text, "(You are in range)");
                    }
                    
                    defer_text(ui, "%s %s", pos.x, pos.y, spell_range_text, in_spell_range_text);
                    pos.y += ui->font_newline * 2;
                } break;
                
                case ExamineType_Trap:
                {
                    DungeonTrap *trap = examine->trap;
                    defer_texture(ui, pos, trap->tile_src);
                    
                    v2u header = get_header_text_pos(ui, pos);
                    defer_text(ui, trap->name, header.x, header.y);
                    pos.y += ui->font_newline * 3;
                    
                    defer_text(ui, trap->description, pos.x, pos.y);
                    pos.y += ui->font_newline * 2;
                } break;
                
                    case ExamineType_Tile:
                {
                    TileID tile = examine->tile;
                    defer_texture(ui, pos, get_dungeon_tileset_rect(tile));
                    
                    v2u header = get_header_text_pos(ui, pos);
                    defer_text(ui, "%s", header.x, header.y, get_dungeon_tile_name(tile));
                    pos.y += ui->font_newline * 3;
                    
                    defer_text(ui, get_dungeon_tile_info_text(tile), pos.x, pos.y);
                        pos.y += ui->font_newline * 2;
                } break;
                
                    invalid_default_case;
            }
            
            process_window_end(game, assets, ui, 0, pos);
        }
        else
        {
            v4u dest = get_game_dest(game, game->examine.pos);
            SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
        }
    }
    else if(is_set(inventory->flags, InventoryFlags_Marking))
    {
        Item *examine_item = inventory->examine_item;
        
        // Render mark window
        v4u mark_rect = {0, 0, 250, 100};
        center_and_render_window_to_available_screen(game, assets, &mark_rect);
        
        { // Render header text
            char *header_text = "Mark with what?";
            if(is_set(examine_item->flags, ItemFlags_IsMarked))
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
        
        // Update Cursor
        Mark *mark = &ui->mark;
        
        if(!mark->render_start)
            {
                mark->render_start = SDL_GetTicks();
            }
            
        if((SDL_GetTicks() - mark->render_start) >= mark->cursor_blink_duration)
            {
                mark->render_start = 0;
                mark->should_render = !mark->should_render;
            }
        
        // Render input
        v2u text_pos =
        {
            input_rect.x + 4,
            input_rect.y + get_centering_offset(input_rect.h, ui->font->size) + 1
        };
        
        u32 cursor_x = text_pos.x;
        v2u character_pos = text_pos;
        for(u32 index = mark->view.start; index < get_view_range(mark->view); ++index)
        {
            u32 mark_index = index - 1;
            
            render_text(game, "%c", character_pos.x, character_pos.y, ui->font, 0, mark->array[mark_index]);
            character_pos.x += get_glyph_width(ui->font, mark->array[mark_index]);
            
            if(mark->should_render && (index == mark->cursor_index))
            {
                cursor_x = character_pos.x;
            }
        }
        
        // Render cursor
        if(mark->should_render)
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
        printf("mark->index: %u\n", mark->index);
        printf("mark->view.count: %u\n", mark->view.count);
        printf("mark->view.start: %u\n", mark->view.start);
        printf("mark->view.end: %u\n\n", mark->view.end);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlags_Examining))
    {
        Item *item = inventory->examine_item;
        
        ui->defer_rect = make_v4u(0, 0, 0, 0);
        v2u pos = get_header_pos(ui, ui->defer_rect, 1);
        render_examine_item(game, item, ui, &pos, CameFrom_Inventory);
        
        process_window_end(game, assets, ui, 0, pos);
    }
    else if(is_set(inventory->flags, InventoryFlags_Open))
    {
        { // Update view based on used inventory item
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
                --inventory->view.count;
            }
            else
            {
                    // Take away the item and the item header from view count.
                inventory->view.count -= 2;
            }
            
            inventory->view_update_item_type = ItemType_None;
        }
        
        if(is_view_scrolling(inventory->view, inventory->view.count))
        {
                // Adjust view if view was at bottom and something was deleted.
            if(get_view_range(inventory->view) > inventory->view.count)
            {
                set_view_at_end(&inventory->view);
            }
        }
        else
        {
            set_view_at_start(&inventory->view);
            }
        }
        
        render_item_window(game, items, inventory, &inventory->view, ui, assets, player->pos, dungeon->level, CameFrom_Inventory, screen_bottom_y);
    }
    else if(is_set(inventory->flags, InventoryFlags_MultiplePickup))
    {
        render_item_window(game, items, inventory, &inventory->pickup_view, ui, assets, player->pos, dungeon->level, CameFrom_Pickup, screen_bottom_y);
    }
    else if(is_set(inventory->flags, InventoryFlags_MultipleExamine))
    {
        render_item_window(game, items, inventory, &inventory->examine_view, ui, assets, player->pos, dungeon->level, CameFrom_Examine, screen_bottom_y);
    }
}