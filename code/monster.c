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
set_monster_sprite_state(monster_t *monster, entity_state state)
{
    monster->state = state;
    
    if(monster->state == state_idle)
    {
        monster->sprite.current_frame = monster->sprite.idle_start_frame;
    }
    else if(monster->state == state_died)
    {
        monster->sprite.current_frame = monster->sprite.died_start_frame;
        
        // NOTE(rami): This needs to be set at the time of state change
        // so that we don't immediately skip the first frame of the
        // monsters death animation.
        monster->sprite.died_frame_last_changed = SDL_GetTicks();
    }
}

internal monster_type
get_monster_for_level()
{
    monster_type result = monster_none;
    
    u32 rand = rand_num(0, 100);
    u32 count = 0;
    
    for(u32 i = 0; i < monster_total; ++i)
    {
        count += monster_spawn_chance[i][dungeon.level - 1];
        if(count >= rand)
        {
            result = i + 1;
            break;
        }
    }
    
    return(result);
}

internal void
add_monster(monster_type type, v2u pos)
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        
        if(!monster->type)
        {
            monster->state = state_idle;
            monster->type = type;
            monster->ai = ai_wandering;
            
            monster->pos = pos;
            monster->new_pos = pos;
            set_occupied(monster->pos, true);
            
            switch(type)
            {
                case monster_slime:
                {
                    strcpy(monster->name, "Slime");
                    monster->size = V2u(32, 32);
                    monster->max_hp = 4;
                    monster->hp = 4;
                    monster->damage = 1;
                    monster->speed = 1;
                    monster->level = 1;
                    
                    monster->sprite.idle_start_frame = V2u(0, 1);
                    monster->sprite.idle_frame_count = 3;
                    monster->sprite.current_frame = monster->sprite.idle_start_frame;
                    
                    if(rand_num(0, 1))
                    {
                        monster->sprite.idle_frame_duration = 300 - anim_offset;
                    }
                    else
                    {
                        monster->sprite.idle_frame_duration = 300 + anim_offset;
                    }
                    
                    monster->sprite.died_start_frame = V2u(0, 2);
                    monster->sprite.died_frame_count = 3;
                    monster->sprite.died_frame_duration = 150;
                } break;
                
                case monster_skeleton:
                {
                    strcpy(monster->name, "Skeleton");
                    monster->size = V2u(32, 32);
                    monster->max_hp = 6;
                    monster->hp = 6;
                    monster->damage = 2;
                    monster->speed = 1;
                    monster->level = 2;
                    
                    monster->sprite.idle_start_frame = V2u(0, 3);
                    monster->sprite.idle_frame_count = 3;
                    monster->sprite.current_frame = monster->sprite.idle_start_frame;
                    
                    if(rand_num(0, 1))
                    {
                        monster->sprite.idle_frame_duration = 600 - anim_offset;
                    }
                    else
                    {
                        monster->sprite.idle_frame_duration = 600 + anim_offset;
                    }
                    
                    monster->sprite.died_start_frame = V2u(0, 4);
                    monster->sprite.died_frame_count = 3;
                    monster->sprite.died_frame_duration = 150;
                } break;
                
                default: break;
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
    
    add_console_text("%s %u damage", color_white, attack, monster->damage);
    add_pop_text("%u", player.pos, text_normal_attack, monster->damage);
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
monster_traverse_path(monster_t *monster, path_t *path)
{
    if(V2u_equal(path->list[0], player.pos))
    {
        monster_attack_player(monster);
    }
    else
    {
        // NOTE(rami): If the monster can move to another position that is
        // not in the pathfinding list and is closer to the player
        // then it will.
        
        // There can be a position that isn't better but it would lead to
        // a path that gets you closer to the target, that condition isn't
        // considered as of now.
        if(is_occupied(path->list[0]))
        {
            u32 current_dist = tile_dist(monster->pos, player.pos);
            
            v2u directions[cardinal_last] = {0};
            directions[up] = V2u(monster->pos.x, monster->pos.y - 1);
            directions[down] = V2u(monster->pos.x, monster->pos.y + 1);
            directions[left] = V2u(monster->pos.x - 1, monster->pos.y);
            directions[right] = V2u(monster->pos.x + 1, monster->pos.y);
            
            for(u32 k = cardinal_first; k < cardinal_last; ++k)
            {
                if(tile_dist(directions[k], player.pos) < current_dist)
                {
                    if(!is_occupied(directions[k]))
                    {
                        monster->new_pos = directions[k];
                        break;
                    }
                }
            }
        }
        else
        {
            monster->new_pos = path->list[0];
        }
        
    }
    
    free(path);
}

internal void
update_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        
        if(monster->type && monster->state)
        {
            if(monster->in_combat)
            {
                // NOTE(rami): Make monster turn towards the target
                if(player.pos.x > monster->pos.x)
                {
                    monster->sprite_flip = false;
                }
                else
                {
                    monster->sprite_flip = true;
                }
                
                path_t *path = pathfind(monster->pos, player.pos, pathfind_cardinal);
                if(path->found)
                {
                    monster_traverse_path(monster, path);
                }
                else
                {
                    monster->in_combat = false;
                }
            }
            else
            {
                apply_monster_ai(&monsters[i]);
            }
            
            if(is_traversable(monster->new_pos))
            {
                if(!is_occupied(monster->new_pos))
                {
                    set_occupied(monster->pos, false);
                    monster->pos = monster->new_pos;
                    set_occupied(monster->pos, true);
                }
            }
            
            // NOTE(rami): This is to keep the new_pos locked.
            monster->new_pos = monster->pos;
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
        monster_t *monster = &monsters[i];
        
        if(monster->type)
        {
            if(is_seen(monster->pos))
            {
                if(update_sprite(&monster->sprite, monster->state))
                {
                    v2u pos = get_game_position(monster->pos);
                    v4u src = V4u(tile_mul(monster->sprite.current_frame.x), tile_mul(monster->sprite.current_frame.y), monster->size.w, monster->size.h);
                    v4u dest = V4u(pos.x, pos.y, monster->size.w, monster->size.h);
                    
                    SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->sprite_flip);
                }
                else
                {
                    remove_monster(monster);
                }
            }
        }
    }
}