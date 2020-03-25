internal u32
monster_info_index_from_monster_id(monster_id id)
{
    u32 result = id - 1;
    return(result);
}

internal u32
add_monster_info(u32 info_index,
                 char *name,
                 u32 w,
                 u32 h,
                 u32 level,
                 u32 max_hp,
                 u32 damage,
                 u32 armor,
                 u32 evasion,
                 u32 attack_speed,
                 u32 move_speed,
                 u32 tile_x,
                 u32 tile_y)
{
    assert(info_index < array_count(monster_information));
    
    monster_info_t *monster_info = &monster_information[info_index];
    monster_info->id = ++info_index;
    strcpy(monster_info->name, name);
    monster_info->w = w;
    monster_info->h = h;
    monster_info->level = level;
    monster_info->max_hp = max_hp;
    monster_info->damage = damage;
    monster_info->armor = armor;
    monster_info->evasion = evasion;
    monster_info->attack_speed = attack_speed;
    monster_info->move_speed = move_speed;
    monster_info->tile = V2u(tile_x, tile_y);
    
    return(info_index);
}

internal void
add_monster(monster_id id, u32 x, u32 y)
{
    assert(id != monster_none);
    
    for(u32 monster_index = 0;
        monster_index < array_count(monsters);
        ++monster_index)
    {
        monster_t *monster = &monsters[monster_index];
        u32 monster_info_index = monster_info_index_from_monster_id(id);
        monster_info_t *monster_info = &monster_information[monster_info_index];
        
        if(!monster->id)
        {
            monster->id = id;
            monster->ai = ai_wandering; // TODO(rami): AI is assumed here.
            monster->hp = monster_info->max_hp;
            monster->max_hp = monster->hp;
            monster->pos = V2u(x, y);
            monster->new_pos = monster->pos;
            
            set_dungeon_occupied(monster->pos, true);
            
            return;
        }
    }
    
    assert(false);
}

internal void
get_monster_attack_message(monster_id id, char *message)
{
    switch(id)
    {
        case monster_slime:
        {
            strcpy(message, "The Slime bites you for");
        } break;
        
        case monster_skeleton:
        {
            u32 random = random_number(1, 2);
            if(random == 1)
            {
                strcpy(message, "The Skeleton punches you for");
            }
            else
            {
                strcpy(message, "The Skeleton kicks you for");
            }
        } break;
        
        case monster_armored_skeleton:
        {
            strcpy(message, "The Skeleton slashes you for");
        } break;
        
        case monster_orc_warrior:
        {
            u32 random = random_number(1, 2);
            if(random == 1)
            {
                strcpy(message, "The Orc Warrior punches you for");
            }
            else
            {
                strcpy(message, "The Orc Warrior kicks you for");
            }
        } break;
        
        case monster_cave_bat:
        {
            u32 random = random_number(1, 2);
            if(random == 1)
            {
                strcpy(message, "The Cave Bat bites you for");
            }
            else
            {
                strcpy(message, "The Cave Bat slashes you for");
            }
        } break;
        
        case monster_python:
        {
            strcpy(message, "The Python bites you for");
        } break;
        
        case monster_kobold:
        {
            u32 random = random_number(1, 2);
            if(random == 1)
            {
                strcpy(message, "The Kobold slashes you for");
            }
            else
            {
                strcpy(message, "The Kobold stabs you for");
            }
        } break;
        
        case monster_ogre:
        {
            strcpy(message, "The Ogre bashes you for");
        } break;
        
        invalid_default_case;
    }
}

internal void
monster_attack_player(monster_info_t *monster_info)
{
    player.hp -= monster_info->damage;
    if(player.hp > player.max_hp)
    {
        player.hp = 0;
    }
    
    char attack[64] = {0};
    // TODO(rami): We need to think about this more.
    //get_monster_attack_message(monster->type, attack);
    
    add_log_message("%s %u damage.", color_white, attack, monster_info->damage);
    add_pop_text("%u", player.pos, text_normal_attack, monster_info->damage);
}

internal void
monster_ai_update(monster_t *monster)
{
    if(monster->ai == ai_wandering)
    {
        u32 direction = random_number(dir_up, dir_down_right);
        switch(direction)
        {
            case dir_up:
            {
                --monster->new_pos.y;
            } break;
            
            case dir_down:
            {
                ++monster->new_pos.y;
            } break;
            
            case dir_left:
            {
                --monster->new_pos.x;
                monster->tile_flipped = true;
            } break;
            
            case dir_right:
            {
                ++monster->new_pos.x;
                monster->tile_flipped = false;
            } break;
            
            case dir_up_left:
            {
                --monster->new_pos.y;
                --monster->new_pos.x;
                monster->tile_flipped = true;
            } break;
            
            case dir_up_right:
            {
                --monster->new_pos.y;
                ++monster->new_pos.x;
                monster->tile_flipped = false;
            } break;
            
            case dir_down_left:
            {
                ++monster->new_pos.y;
                --monster->new_pos.x;
                monster->tile_flipped = true;
            } break;
            
            case dir_down_right:
            {
                ++monster->new_pos.y;
                ++monster->new_pos.x;
                monster->tile_flipped = false;
            } break;
            
            invalid_default_case;
        }
    }
}

internal void
update_monsters()
{
    for(u32 monster_index = 0;
        monster_index < array_count(monsters);
        ++monster_index)
    {
        monster_t *monster = &monsters[monster_index];
        if(monster->id)
        {
            u32 monster_info_index = monster_info_index_from_monster_id(monster->id);
            monster_info_t *monster_info = &monster_information[monster_info_index];
            
            for(u32 move_speed_index = 0;
                move_speed_index < monster_info->move_speed;
                ++move_speed_index)
            {
                if(monster->in_combat)
                {
                    // NOTE(rami): Turn monster sprite towards target.
                    monster->tile_flipped = (player.pos.x < monster->pos.x);
                    
                    v2u next_pos = next_pathfind_pos((u32 *)pathfind_map, dungeon.w, monster);
                    if(!monster->has_attacked && V2u_equal(next_pos, player.pos))
                    {
                        for(u32 attack_speed_index = 0;
                            attack_speed_index < monster_info->attack_speed;
                            ++attack_speed_index)
                        {
                            monster_attack_player(monster_info);
                        }
                        
                        monster->has_attacked = true;
                    }
                    else
                    {
                        monster->new_pos = next_pos;
                    }
                }
                else
                {
                    monster_ai_update(monster);
                }
                
                if(is_dungeon_traversable(monster->new_pos) &&
                   !is_dungeon_occupied(monster->new_pos))
                {
                    set_dungeon_occupied(monster->pos, false);
                    monster->pos = monster->new_pos;
                    set_dungeon_occupied(monster->pos, true);
                }
                
                // NOTE(rami): This is to keep the new_pos locked.
                monster->new_pos = monster->pos;
            }
            
            monster->has_attacked = false;
        }
    }
}

internal void
remove_monster(monster_t *monster)
{
    set_dungeon_occupied(monster->pos, false);
    memset(monster, 0, sizeof(monster_t));
}

internal void
render_monsters()
{
    for(u32 monster_index = 0;
        monster_index < array_count(monsters);
        ++monster_index)
    {
        monster_t *monster = &monsters[monster_index];
        if(monster->id && is_seen(monster->pos))
        {
            u32 monster_info_index = monster_info_index_from_monster_id(monster->id);
            monster_info_t *monster_info = &monster_information[monster_info_index];
            
            v2u pos = get_game_pos(monster->pos);
            v4u src =
            {
                tile_mul(monster_info->tile.x),
                tile_mul(monster_info->tile.y),
                monster_info->w,
                monster_info->h
            };
            
            v4u dest = {pos.x, pos.y, monster_info->w, monster_info->h};
            SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->tile_flipped);
            
            // Render Monster HP Bar
            if(monster->in_combat && monster->hp)
            {
                // HP Bar Outside
                set_render_color(color_black);
                v4u hp_bar_outside = {pos.x, pos.y + 33, 32, 4};
                SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&hp_bar_outside);
                
                // HP Bar Inside
                set_render_color(color_dark_red);
                u32 hp_bar_inside_w = get_ratio(monster->hp, monster->max_hp, 30);
                v4u hp_bar_inside = {pos.x + 1, pos.y + 34, hp_bar_inside_w, 2};
                SDL_RenderFillRect(game.renderer, (SDL_Rect *)&hp_bar_inside);
            }
        }
    }
}