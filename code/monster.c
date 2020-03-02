internal void
set_monster_spawn_chance(monster_type type, monster_tier tier)
{
    // NOTE(rami): Monster types start at 1 so we need to decrement by one
    // to get the correct array element.
    --type;
    
    // TODO(rami): Remove the things that get set to zero later.
    // TODO(rami): Medium and High tier monsters should start later,
    // Low tier monsters should have good chances at starting levels and slowly
    // fade as we go into higher levels.
    if(tier == monster_tier_low)
    {
        monster_spawn_chance[type][0] = 50;
        monster_spawn_chance[type][1] = 40;
        monster_spawn_chance[type][2] = 30;
        monster_spawn_chance[type][3] = 20;
        monster_spawn_chance[type][4] = 10;
        monster_spawn_chance[type][5] = 5;
        monster_spawn_chance[type][6] = 0;
        monster_spawn_chance[type][7] = 0;
        monster_spawn_chance[type][8] = 0;
        monster_spawn_chance[type][9] = 0;
    }
    else if(tier == monster_tier_medium)
    {
        monster_spawn_chance[type][0] = 0;
        monster_spawn_chance[type][1] = 0;
        monster_spawn_chance[type][2] = 5;
        monster_spawn_chance[type][3] = 10;
        monster_spawn_chance[type][4] = 20;
        monster_spawn_chance[type][5] = 50;
        monster_spawn_chance[type][6] = 40;
        monster_spawn_chance[type][7] = 30;
        monster_spawn_chance[type][8] = 20;
        monster_spawn_chance[type][9] = 10;
    }
    else
    {
        monster_spawn_chance[type][0] = 0;
        monster_spawn_chance[type][1] = 0;
        monster_spawn_chance[type][2] = 0;
        monster_spawn_chance[type][3] = 0;
        monster_spawn_chance[type][4] = 0;
        monster_spawn_chance[type][5] = 0;
        monster_spawn_chance[type][6] = 10;
        monster_spawn_chance[type][7] = 20;
        monster_spawn_chance[type][8] = 30;
        monster_spawn_chance[type][9] = 40;
    }
}

internal void
add_monster(monster_type type, v2u pos)
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(!monster->type)
        {
            monster->type = type;
            monster->ai = ai_wandering;
            
            monster->pos = pos;
            monster->new_pos = pos;
            set_dungeon_occupied(monster->pos, true);
            
            switch(type)
            {
                case monster_test_1:
                {
                    strcpy(monster->name, "Monster 1");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(10, 0);
                } break;
                
                case monster_test_2:
                {
                    strcpy(monster->name, "Monster 2");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(11, 0);
                } break;
                
                case monster_test_3:
                {
                    strcpy(monster->name, "Monster 3");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(12, 0);
                } break;
                
                case monster_test_4:
                {
                    strcpy(monster->name, "Monster 4");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(13, 0);
                } break;
                
                case monster_test_5:
                {
                    strcpy(monster->name, "Monster 5");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(14, 0);
                } break;
                
                case monster_test_6:
                {
                    strcpy(monster->name, "Monster 6");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(15, 0);
                } break;
                
                case monster_test_7:
                {
                    strcpy(monster->name, "Monster 7");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(16, 0);
                } break;
                
                case monster_test_8:
                {
                    strcpy(monster->name, "Monster 8");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(17, 0);
                } break;
                
                case monster_test_9:
                {
                    strcpy(monster->name, "Monster 9");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(18, 0);
                } break;
                
                case monster_test_10:
                {
                    strcpy(monster->name, "Monster 10");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(19, 0);
                } break;
                
                case monster_test_11:
                {
                    strcpy(monster->name, "Monster 11");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(20, 0);
                } break;
                
                case monster_test_12:
                {
                    strcpy(monster->name, "Monster 12");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(21, 0);
                } break;
                
                case monster_test_13:
                {
                    strcpy(monster->name, "Monster 13");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(22, 0);
                } break;
                
                case monster_test_14:
                {
                    strcpy(monster->name, "Monster 14");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(23, 0);
                } break;
                
                case monster_test_15:
                {
                    strcpy(monster->name, "Monster 15");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(24, 0);
                } break;
                
                case monster_test_16:
                {
                    strcpy(monster->name, "Monster 16");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(25, 0);
                } break;
                
                case monster_test_17:
                {
                    strcpy(monster->name, "Monster 17");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(26, 0);
                } break;
                
                case monster_test_18:
                {
                    strcpy(monster->name, "Monster 18");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(27, 0);
                } break;
                
                case monster_baby_slime:
                {
                    strcpy(monster->name, "Baby Slime");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(1, 0);
                } break;
                
                case monster_slime:
                {
                    strcpy(monster->name, "Slime");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 3;
                    monster->hp = monster->max_hp;
                    monster->damage = 2;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(2, 0);
                } break;
                
                case monster_cave_bat:
                {
                    strcpy(monster->name, "Cave Bat");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(6, 0);
                } break;
                
                case monster_python:
                {
                    strcpy(monster->name, "Python");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 2;
                    monster->move_speed = 1;
                    monster->level = 2;
                    monster->tile = V2u(7, 0);
                } break;
                
                case monster_skeleton:
                {
                    strcpy(monster->name, "Skeleton");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 5;
                    monster->hp = monster->max_hp;
                    monster->damage = 3;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 2;
                    monster->tile = V2u(3, 0);
                } break;
                
                case monster_armored_skeleton:
                {
                    strcpy(monster->name, "Armored Skeleton");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 5;
                    monster->hp = monster->max_hp;
                    monster->damage = 4;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(4, 0);
                } break;
                
                case monster_orc_warrior:
                {
                    strcpy(monster->name, "Orc Warrior");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 6;
                    monster->hp = monster->max_hp;
                    monster->damage = 3;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(5, 0);
                } break;
                
                case monster_kobold:
                {
                    strcpy(monster->name, "Kobold");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 6;
                    monster->hp = monster->max_hp;
                    monster->damage = 4;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(8, 0);
                } break;
                
                case monster_ogre:
                {
                    strcpy(monster->name, "Ogre");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 8;
                    monster->hp = monster->max_hp;
                    monster->damage = 6;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 4;
                    monster->tile = V2u(9, 0);
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
}

internal void
get_monster_attack_message(monster_type type, char *message)
{
    switch(type)
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
monster_attack_player(monster_t *monster)
{
    player.hp -= monster->damage;
    if(player.hp > player.max_hp)
    {
        player.hp = 0;
    }
    
    // TODO(rami): We need to enable and think about this more.
    char attack[64] = {0};
    //get_monster_attack_message(monster->type, attack);
    
    add_log_message("%s %u damage.", color_white, attack, monster->damage);
    add_pop_text("%u", player.pos, text_normal_attack, monster->damage);
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
                monster->tile_flip = true;
            } break;
            
            case dir_right:
            {
                ++monster->new_pos.x;
                monster->tile_flip = false;
            } break;
            
            case dir_up_left:
            {
                --monster->new_pos.y;
                --monster->new_pos.x;
                monster->tile_flip = true;
            } break;
            
            case dir_up_right:
            {
                --monster->new_pos.y;
                ++monster->new_pos.x;
                monster->tile_flip = false;
            } break;
            
            case dir_down_left:
            {
                ++monster->new_pos.y;
                --monster->new_pos.x;
                monster->tile_flip = true;
            } break;
            
            case dir_down_right:
            {
                ++monster->new_pos.y;
                ++monster->new_pos.x;
                monster->tile_flip = false;
            } break;
        }
    }
}

internal void
update_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            for(u32 speed_index = 0; speed_index < monster->move_speed; ++speed_index)
            {
                if(monster->in_combat)
                {
                    // NOTE(rami): Turn monster sprite towards target.
                    monster->tile_flip = (player.pos.x < monster->pos.x);
                    
                    v2u next_pos = next_pathfind_pos(dungeon.pathfind_map, dungeon.w, monster);;
                    if(!monster->has_attacked && V2u_equal(next_pos, player.pos))
                    {
                        for(u32 i = 0; i < monster->attack_speed; ++i)
                        {
                            monster_attack_player(monster);
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
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type &&
           is_seen(monster->pos))
        {
            v2u pos = get_game_pos(monster->pos);
            v4u src = {tile_mul(monster->tile.x), tile_mul(monster->tile.y), monster->w, monster->h};
            v4u dest = {pos.x, pos.y, monster->w, monster->h};
            
            SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->tile_flip);
            
            // Render Monster HP Bar
            if(monster->in_combat &&
               monster->hp)
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