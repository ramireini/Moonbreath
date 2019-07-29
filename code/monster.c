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
add_monster(monster_type type, u32 x, u32 y)
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(!monster[i].type)
        {
            monster[i].type = type;
            monster[i].ai = ai_wandering;
            monster[i].pos = V2u(x, y);
            
            // TODO(rami): Turn into a switch
            if(type == monster_slime)
            {
                monster[i].sprite.start_frame = V2u(0, 1);
                monster[i].sprite.current_frame = monster[i].sprite.start_frame;
                monster[i].sprite.frame_count = 4;
                
                if(rand_num(0, 1))
                {
                    monster[i].sprite.frame_duration = 200 - anim_offset;
                }
                else
                {
                    monster[i].sprite.frame_duration = 200 + anim_offset;
                }
                
                monster[i].size = V2u(32, 32);
                monster[i].max_hp = 4;
                monster[i].hp = 4;
                monster[i].damage = 1;
                monster[i].speed = 1;
                monster[i].level = 1;
            }
            else if(type == monster_skeleton)
            {
                monster[i].sprite.start_frame = V2u(0, 2);
                monster[i].sprite.current_frame = monster[i].sprite.start_frame;
                monster[i].sprite.frame_count = 6;
                
                if(rand_num(0, 1))
                {
                    monster[i].sprite.frame_duration = 400 - anim_offset;
                }
                else
                {
                    monster[i].sprite.frame_duration = 400 + anim_offset;
                }
                
                monster[i].size = V2u(32, 32);
                monster[i].max_hp = 6;
                monster[i].hp = 6;
                monster[i].damage = 2;
                monster[i].speed = 1;
                monster[i].level = 2;
            }
            
            break;
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
    player.hp -= monster[i].damage;
    if(player.hp > player.max_hp)
    {
        player.hp = 0;
    }
}

internal void
apply_monster_ai(monster_ai ai)
{
    // NOTE(rami): Implement
}

internal void
update_monsters()
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(monster[i].type)
        {
            if(monster[i].in_combat)
            {
                path_t *path = pathfind(monster[i].pos,
                                        player.pos,
                                        pathfind_cardinal);
                if(path->found)
                {
                    if(V2u_equal(path->list[0], player.pos))
                    {
                        monster_attack_player(i);
                        
                        char attack[64] = {0};
                        get_monster_attack_message(monster[i].type, attack);
                        
                        add_console_message("%s %u damage", color_white, attack, monster[i].damage);
                        
                        add_pop_up_text("%u", player.pos,
                                        (player.size.w / 2) / 2, -8,
                                        text_normal_attack,
                                        monster[i].damage);
                    }
                    else
                    {
                        b32 can_move = true;
                        
                        for(u32 i = 0; i < MONSTER_COUNT; ++i)
                        {
                            if(V2u_equal(monster[i].pos, path->list[0]))
                            {
                                can_move = false;
                            }
                        }
                        
                        if(can_move)
                        {
                            monster[i].pos = path->list[0];
                        }
                    }
                }
                else
                {
                    monster[i].in_combat = 0;
                }
                
                free(path);
            }
            else
            {
                // NOTE(rami):
                // Later we should have a new struct entry which has the
                // type of AI we want to apply for every monster so the
                // function can use that instead of what we pass to it.
                apply_monster_ai(monster[i].ai);
            }
        }
    }
}

internal void
render_monsters()
{
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(monster[i].type)
        {
            update_sprite(&monster[i].sprite);
            
            v2u pos = get_game_position(monster[i].pos);
            
            v4u src = V4u(tile_mul(monster[i].sprite.current_frame.x), tile_mul(monster[i].sprite.current_frame.y),
                          monster[i].size.w, monster[i].size.h);
            
            v4u dest = V4u(pos.x, pos.y, monster[i].size.w, monster[i].size.h);
            
            if(is_seen(monster[i].pos))
            {
                SDL_RenderCopy(game.renderer, texture[tex_sprite_sheet], (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
        }
    }
}

internal void
remove_monster(u32 i)
{
    memset(&monster[i], 0, sizeof(monster_t));
}