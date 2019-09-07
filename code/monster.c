internal void
set_monster_spawn_chances()
{
    // TODO(rami): Do this for all monsters
    monster_spawn_chance[monster_slime - 1][0] = 70;
    monster_spawn_chance[monster_slime - 1][1] = 30;
    
    monster_spawn_chance[monster_skeleton - 1][0] = 30;
    monster_spawn_chance[monster_skeleton - 1][1] = 70;
}

internal void
set_monster_sprite_state(u32 i, entity_state state)
{
    monsters[i].state = state;
    
    if(monsters[i].state == state_idle)
    {
        monsters[i].sprite.current_frame = monsters[i].sprite.idle_start_frame;
    }
    else if(monsters[i].state == state_died)
    {
        monsters[i].sprite.current_frame = monsters[i].sprite.died_start_frame;
        // NOTE(rami): This needs to be set at the time of state change
        // so that we don't immediately skip the first frame of the
        // monsters death animation.
        monsters[i].sprite.died_frame_last_changed = SDL_GetTicks();
    }
}

internal b32
is_blocked_by_monsters(v2u pos)
{
    b32 result = false;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        if(V2u_equal(pos, monsters[i].pos))
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal monster_type
get_monster_for_level()
{
    monster_type result = monster_none;
    
    u32 rand = rand_num(0, 100);
    u32 count = 0;
    
    for(u32 i = 0; i < monster_total; ++i)
    {
        count += monster_spawn_chance[i][level.current_level - 1];
        if(count >= rand)
        {
            result = i + 1;
            break;
        }
    }
    
    return(result);
}

internal void
add_monster(monster_type type, u32 x, u32 y)
{
    v2u pos = V2u(x, y);
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        if(!monsters[i].type)
        {
            monsters[i].state = state_idle;
            monsters[i].type = type;
            monsters[i].ai = ai_wandering;
            
            monsters[i].pos = pos;
            monsters[i].new_pos = pos;
            set_occupied(monsters[i].pos, true);
            
            switch(type)
            {
                case monster_slime:
                {
                    strcpy(monsters[i].name, "Slime");
                    monsters[i].size = V2u(32, 32);
                    monsters[i].max_hp = 4;
                    monsters[i].hp = 4;
                    monsters[i].damage = 1;
                    monsters[i].speed = 1;
                    monsters[i].level = 1;
                    
                    monsters[i].sprite.idle_start_frame = V2u(0, 1);
                    monsters[i].sprite.idle_frame_count = 3;
                    monsters[i].sprite.current_frame = monsters[i].sprite.idle_start_frame;
                    
                    if(rand_num(0, 1))
                    {
                        monsters[i].sprite.idle_frame_duration = 300 - anim_offset;
                    }
                    else
                    {
                        monsters[i].sprite.idle_frame_duration = 300 + anim_offset;
                    }
                    
                    monsters[i].sprite.died_start_frame = V2u(0, 2);
                    monsters[i].sprite.died_frame_count = 3;
                    monsters[i].sprite.died_frame_duration = 150;
                } break;
                
                case monster_skeleton:
                {
                    strcpy(monsters[i].name, "Skeleton");
                    monsters[i].size = V2u(32, 32);
                    monsters[i].max_hp = 6;
                    monsters[i].hp = 6;
                    monsters[i].damage = 2;
                    monsters[i].speed = 1;
                    monsters[i].level = 2;
                    
                    monsters[i].sprite.idle_start_frame = V2u(0, 3);
                    monsters[i].sprite.idle_frame_count = 3;
                    monsters[i].sprite.current_frame = monsters[i].sprite.idle_start_frame;
                    
                    if(rand_num(0, 1))
                    {
                        monsters[i].sprite.idle_frame_duration = 600 - anim_offset;
                    }
                    else
                    {
                        monsters[i].sprite.idle_frame_duration = 600 + anim_offset;
                    }
                    
                    monsters[i].sprite.died_start_frame = V2u(0, 4);
                    monsters[i].sprite.died_frame_count = 3;
                    monsters[i].sprite.died_frame_duration = 150;
                } break;
            }
            
            return;
        }
    }
}

internal void
get_monster_attack_message(monster_type type, char *message)
{
    // TODO(rami): Switch
    if(type == monster_slime)
    {
        u32 i = rand_num(1, 2);
        if(i == 1)
        {
            strcpy(message, "The slime releases an acid cloud on you for");
        }
        else
        {
            strcpy(message, "The slime hurls an acid ball at you for");
        }
    }
    else if(type == monster_skeleton)
    {
        strcpy(message, "The skeleton swings at you for");
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
    
    char attack[64] = {0};
    get_monster_attack_message(monster->type, attack);
    
    add_console_message("%s %u damage", color_white, attack, monster->damage);
    add_pop_up_text("%u", player.pos, (player.size.w / 2) / 2, -8, text_normal_attack, monster->damage);
}

internal void
apply_monster_ai(monster_t *monster)
{
    if(monster->ai == ai_wandering)
    {
        u32 direction = rand_num(up, right);
        if(direction == up)
        {
            --monster->new_pos.y;
        }
        else if(direction == down)
        {
            ++monster->new_pos.y;
        }
        else if(direction == left)
        {
            --monster->new_pos.x;
            monster->sprite_flip = true;
        }
        else
        {
            ++monster->new_pos.x;
            monster->sprite_flip = false;
        }
    }
}

internal void
update_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        if(monsters[i].type && monsters[i].state)
        {
            if(monsters[i].in_combat)
            {
                // NOTE(rami): Make monster turn towards the target
                if(player.pos.x > monsters[i].pos.x)
                {
                    monsters[i].sprite_flip = false;
                }
                else
                {
                    monsters[i].sprite_flip = true;
                }
                
                path_t *path = pathfind(monsters[i].pos, player.pos, pathfind_cardinal);
                if(path->found)
                {
                    if(V2u_equal(path->list[0], player.pos))
                    {
                        monster_attack_player(&monsters[i]);
                    }
                    else
                    {
                        // NOTE(rami): If the monster can move to another position that is
                        // not in the pathfinding list and is closer to the player
                        // then it will.
                        
                        // There can be a position that isn't better but it would lead to
                        // a path that gets you closer to the target, that condition isn't
                        // considered as of now.
                        if(is_blocked_by_monsters(path->list[0]))
                        {
                            u32 current_dist = tile_dist(monsters[i].pos, player.pos);
                            
                            v2u directions[cardinal_last] = {0};
                            directions[up] = V2u(monsters[i].pos.x, monsters[i].pos.y - 1);
                            directions[down] = V2u(monsters[i].pos.x, monsters[i].pos.y + 1);
                            directions[left] = V2u(monsters[i].pos.x - 1, monsters[i].pos.y);
                            directions[right] = V2u(monsters[i].pos.x + 1, monsters[i].pos.y);
                            
                            for(u32 k = cardinal_first; k < cardinal_last; ++k)
                            {
                                if(tile_dist(directions[k], player.pos) < current_dist)
                                {
                                    if(!is_blocked_by_monsters(directions[k]))
                                    {
                                        monsters[i].new_pos = directions[k];
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            monsters[i].new_pos = path->list[0];
                        }
                    }
                    
                    free(path);
                }
                else
                {
                    monsters[i].in_combat = false;
                }
            }
            else
            {
                apply_monster_ai(&monsters[i]);
            }
            
            if(is_traversable(monsters[i].new_pos))
            {
                if(!is_occupied(monsters[i].new_pos))
                {
                    set_occupied(monsters[i].pos, false);
                    monsters[i].pos = monsters[i].new_pos;
                    set_occupied(monsters[i].pos, true);
                }
            }
            
            // NOTE(rami): This is to keep the new_pos locked.
            monsters[i].new_pos = monsters[i].pos;
        }
    }
}

internal void
remove_monster(monster_t *monster)
{
    set_occupied(monster->pos, false);
    memset(monster, 0, sizeof(monster_t));
}

internal void
render_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        if(monsters[i].type)
        {
            if(is_seen(monsters[i].pos))
            {
                if(update_sprite(&monsters[i].sprite, monsters[i].state))
                {
                    v2u pos = get_game_position(monsters[i].pos);
                    v4u src = V4u(tile_mul(monsters[i].sprite.current_frame.x), tile_mul(monsters[i].sprite.current_frame.y), monsters[i].size.w, monsters[i].size.h);
                    v4u dest = V4u(pos.x, pos.y, monsters[i].size.w, monsters[i].size.h);
                    
                    SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monsters[i].sprite_flip);
                }
                else
                {
                    remove_monster(&monsters[i]);
                }
            }
        }
    }
}
