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
get_monster_name(monster_type type, char *buffer)
{
    // TODO(rami): Turn into switch when the time comes
    if(type == monster_slime)
    {
        strcpy(buffer, "Slime");
    }
    else if(type == monster_skeleton)
    {
        strcpy(buffer, "Skeleton");
    }
}

internal void
add_monster(monster_type type, v2u pos)
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(!monsters[i].type)
        {
            monsters[i].type = type;
            monsters[i].ai = ai_wandering;
            monsters[i].pos = pos;
            monsters[i].new_pos = pos;
            
            // TODO(rami): Turn into a switch
            if(type == monster_slime)
            {
                monsters[i].sprite.start_frame = V2u(0, 1);
                monsters[i].sprite.current_frame = monsters[i].sprite.start_frame;
                monsters[i].sprite.frame_count = 4;
                
                if(rand_num(0, 1))
                {
                    monsters[i].sprite.frame_duration = 300 - anim_offset;
                }
                else
                {
                    monsters[i].sprite.frame_duration = 300 + anim_offset;
                }
                
                monsters[i].size = V2u(32, 32);
                monsters[i].max_hp = 4;
                monsters[i].hp = 4;
                monsters[i].damage = 1;
                monsters[i].speed = 1;
                monsters[i].level = 1;
            }
            else if(type == monster_skeleton)
            {
                monsters[i].sprite.start_frame = V2u(0, 2);
                monsters[i].sprite.current_frame = monsters[i].sprite.start_frame;
                monsters[i].sprite.frame_count = 6;
                
                if(rand_num(0, 1))
                {
                    monsters[i].sprite.frame_duration = 300 - anim_offset;
                }
                else
                {
                    monsters[i].sprite.frame_duration = 300 + anim_offset;
                }
                
                monsters[i].size = V2u(32, 32);
                monsters[i].max_hp = 6;
                monsters[i].hp = 6;
                monsters[i].damage = 2;
                monsters[i].speed = 1;
                monsters[i].level = 2;
            }
            
            return;
        }
    }
}

internal void
get_monster_attack_message(monster_type type, char *message)
{
    // TODO(rami): Turn into a switch
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
monster_attack_player(u32 i)
{
    player.hp -= monsters[i].damage;
    if(player.hp > player.max_hp)
    {
        player.hp = 0;
    }
}

internal void
apply_monster_ai(monster_ai ai)
{
    // TODO(rami): Implement
}

internal void
update_monsters()
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(monsters[i].type)
        {
            if(monsters[i].in_combat)
            {
                path_t *path = pathfind(monsters[i].pos, player.pos, pathfind_cardinal);
                if(path->found)
                {
                    if(V2u_equal(path->list[0], player.pos))
                    {
                        monster_attack_player(i);
                        
                        char attack[64] = {0};
                        get_monster_attack_message(monsters[i].type, attack);
                        
                        add_console_message("%s %u damage", color_white, attack, monsters[i].damage);
                        add_pop_up_text("%u", player.pos, (player.size.w / 2) / 2, -8, text_normal_attack, monsters[i].damage);
                    }
                    else
                    {
                        b32 can_move = true;
                        
                        for(u32 i = 0; i < MONSTER_COUNT; ++i)
                        {
                            if(V2u_equal(monsters[i].pos, path->list[0]))
                            {
                                can_move = false;
                                break;
                            }
                        }
                        
                        if(can_move)
                        {
                            monsters[i].new_pos = path->list[0];
                        }
                    }
                }
                else
                {
                    monsters[i].in_combat = false;
                }
                
                free(path);
            }
            else
            {
                u32 direction = rand_num(up, right);
                if(direction == up)
                {
                    --monsters[i].new_pos.y;
                }
                else if(direction == down)
                {
                    ++monsters[i].new_pos.y;
                }
                else if(direction == left)
                {
                    --monsters[i].new_pos.x;
                    monsters[i].sprite_flip = true;
                }
                else
                {
                    ++monsters[i].new_pos.x;
                    monsters[i].sprite_flip = false;
                }
                
                // TODO(rami): Later we should have a new struct entry which has the
                // type of AI we want to apply for every monster so the
                // function can use that instead of what we pass to it.
                //apply_monster_ai(monsters[i].ai);
            }
            
            if(is_traversable(monsters[i].new_pos))
            {
                monsters[i].pos = monsters[i].new_pos;
            }
            
            monsters[i].new_pos = monsters[i].pos;
        }
    }
}

internal void
render_monsters()
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(monsters[i].type)
        {
            update_sprite(&monsters[i].sprite);
            
            v2u pos = get_game_position(monsters[i].pos);
            
            v4u src = V4u(tile_mul(monsters[i].sprite.current_frame.x), tile_mul(monsters[i].sprite.current_frame.y),
                          monsters[i].size.w, monsters[i].size.h);
            
            v4u dest = V4u(pos.x, pos.y, monsters[i].size.w, monsters[i].size.h);
            
            if(is_seen(monsters[i].pos))
            {
                SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet], (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monsters[i].sprite_flip);
            }
        }
    }
}

internal void
remove_monster(u32 i)
{
    memset(&monsters[i], 0, sizeof(monster_t));
}