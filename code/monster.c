internal i32
monster_is_alive(i32 i)
{
  i32 result = 1;

  if(monster[i].hp <= 0)
  {
    result = 0;
  }

  return(result);
}

internal void
get_monster_name(monster_type type, char *buffer)
{
  // NOTE(rami): Turn into switch when the time comes
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
add_monster(monster_type type, i32 x, i32 y)
{
  for(i32 i = 0; i < MONSTER_COUNT; ++i)
  {
    if(!monster[i].type)
    {
      monster[i].type = type;
      monster[i].ai = ai_wander;

      if(type == monster_slime)
      {
        monster[i].sprite.start_frame = v2(0, 1);
        monster[i].sprite.current_frame = monster[i].sprite.start_frame;
        monster[i].sprite.frame_count = 4;
        monster[i].sprite.frame_duration = 200 + get_num(min_offset, max_offset);
        monster[i].sprite.frame_last_changed = 0;

        monster[i].x = x;
        monster[i].y = y;
        monster[i].w = 32;
        monster[i].h = 32;
        monster[i].in_combat = 0;
        monster[i].max_hp = 4;
        monster[i].hp = 4;
        monster[i].damage = 1;
        monster[i].armor = 0;
        monster[i].speed = 1;
        monster[i].level = 1;
      }
      else if(type == monster_skeleton)
      {
        monster[i].sprite.start_frame = v2(0, 2);
        monster[i].sprite.current_frame = monster[i].sprite.start_frame;
        monster[i].sprite.frame_count = 6;
        monster[i].sprite.frame_duration = 600 + get_num(min_offset, max_offset);
        monster[i].sprite.frame_last_changed = 0;

        monster[i].x = x;
        monster[i].y = y;
        monster[i].w = 32;
        monster[i].h = 32;
        monster[i].in_combat = 0;
        monster[i].max_hp = 6;
        monster[i].hp = 6;
        monster[i].damage = 2;
        monster[i].armor = 0;
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
  if(type == monster_slime)
  {
    i32 i = get_num(1, 2);
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
monster_attack_player(i32 i)
{
  player.hp -= monster[i].damage;
  if(player.hp < 0)
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
update_monster()
{
  for(i32 i = 0; i < MONSTER_COUNT; ++i)
  {
    if(monster[i].type)
    {
      if(monster[i].in_combat)
      {
        path_t *path = pathfind(v2(monster[i].x, monster[i].y), v2(player.x, player.y));
        if(path->found)
        {
          if(v2_equal(v2(path->list[0].x, path->list[0].y), v2(player.x, player.y)))
          {
            monster_attack_player(i);

            char attack[64] = {0};
            get_monster_attack_message(monster[i].type, attack);

            add_console_message("%s %d damage", color_white, attack, monster[i].damage);
            add_pop_up_text("%d", player.x, player.y, (player.w / 2) / 2, 8,
                            type_fading, color_white, 20, 500, monster[i].damage);
          }
          else
          {
            b32 can_move = 1;

            for(i32 i = 0; i < MONSTER_COUNT; ++i)
            {
              if(v2_equal(v2(monster[i].x, monster[i].y),
                          v2(path->list[0].x, path->list[0].y)))
              {
                can_move = 0;
              }
            }

            if(can_move)
            {
              monster[i].x = path->list[0].x;
              monster[i].y = path->list[0].y;
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
        // function can use that instead of what we pass to it
        apply_monster_ai(monster[i].ai);
      }
    }
  }
}

internal void
render_monster()
{
  for(i32 i = 0; i < MONSTER_COUNT; ++i)
  {
    if(monster[i].type)
    {
      update_sprite(&monster[i].sprite);

      SDL_Rect src = {tile_mul(monster[i].sprite.current_frame.x),
                      tile_mul(monster[i].sprite.current_frame.y),
                      monster[i].w, monster[i].h};

      iv2 pos = get_real_position(monster[i].x, monster[i].y);
      SDL_Rect dest = {pos.x, pos.y, monster[i].w, monster[i].h};

      iv2 monster_pos = v2(monster[i].x, monster[i].y);
      if(is_lit(monster_pos))
      {
        iv4 color = get_color_for_lighting_value(monster_pos);
        SDL_SetTextureColorMod(texture[tex_sprite_sheet], color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, texture[tex_sprite_sheet], &src, &dest);
      }
    }
  }
}

internal void
remove_monster(i32 i)
{
  memset(&monster[i], 0, sizeof(monster_t));
}