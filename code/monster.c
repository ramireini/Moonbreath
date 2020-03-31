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
            
            // TODO(rami): Test only value.
            monster->action_speed = 1.0f;
            
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
    if((s32)player.hp <= 0)
    {
        player.hp = 0;
    }
    
    // TODO(rami): We need to think about this more.
    char attack[64] = {0};
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
                monster->is_flipped = true;
            } break;
            
            case dir_right:
            {
                ++monster->new_pos.x;
                monster->is_flipped = false;
            } break;
            
            case dir_up_left:
            {
                --monster->new_pos.y;
                --monster->new_pos.x;
                monster->is_flipped = true;
            } break;
            
            case dir_up_right:
            {
                --monster->new_pos.y;
                ++monster->new_pos.x;
                monster->is_flipped = false;
            } break;
            
            case dir_down_left:
            {
                ++monster->new_pos.y;
                --monster->new_pos.x;
                monster->is_flipped = true;
            } break;
            
            case dir_down_right:
            {
                ++monster->new_pos.y;
                ++monster->new_pos.x;
                monster->is_flipped = false;
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
            
            for(u32 action_count = 0;
                action_count < (1.0f / monster->action_speed);
                ++action_count)
            {
                if(monster->in_combat)
                {
                    // NOTE(rami): Turn monster sprite towards target.
                    monster->is_flipped = (player.pos.x < monster->pos.x);
                    
                    v2u next_pos = next_pathfind_pos((u32 *)pathfind_map, dungeon.w, monster);
                    //printf("next_pos: %u, %u\n", next_pos.x, next_pos.y);
                    
                    if(V2u_equal(next_pos, player.pos))
                    {
                        monster_attack_player(monster_info);
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
        }
    }
}

internal void
remove_monster(monster_t *monster)
{
    set_dungeon_occupied(monster->pos, false);
    set_dungeon_tile_blood(monster->pos);
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
        if(monster->id)
        {
            u32 monster_info_index = monster_info_index_from_monster_id(monster->id);
            monster_info_t *monster_info = &monster_information[monster_info_index];
            
            if(is_seen(monster->pos))
            {
                monster->has_been_seen = true;
                monster->is_ghost_pos_stored = false;
                
                v2u pos = get_game_pos(monster->pos);
                v4u src = {tile_mul(monster_info->tile.x), tile_mul(monster_info->tile.y), monster_info->w, monster_info->h};
                v4u dest = {pos.x, pos.y, monster_info->w, monster_info->h};
                
                SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->is_flipped);
                
                // Render Monster HP Bar
                if(monster->in_combat)
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
#if 1
            else
            {
                if(is_seen(monster->ghost_pos))
                {
                    monster->has_been_seen = false;
                    monster->ghost_pos = V2u(0, 0);
                }
                else
                {
                    if(monster->has_been_seen)
                    {
                        if(!monster->is_ghost_pos_stored)
                        {
                            monster->ghost_pos = monster->new_pos;
                            monster->ghost_is_flipped = monster->is_flipped;
                            
                            monster->is_ghost_pos_stored = true;
                        }
                        
                        v2u pos = get_game_pos(monster->ghost_pos);
                        v4u src = {tile_mul(monster_info->tile.x), tile_mul(monster_info->tile.y), monster_info->w, monster_info->h};
                        v4u dest = {pos.x, pos.y, monster_info->w, monster_info->h};
                        
                        SDL_SetTextureColorMod(textures.sprite_sheet.tex, 64, 64, 64);
                        SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->ghost_is_flipped);
                        SDL_SetTextureColorMod(textures.sprite_sheet.tex, 255, 255, 255);
                    }
                }
            }
#endif
        }
    }
}