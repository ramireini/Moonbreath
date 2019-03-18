// NOTE(Rami): Creation of multiple slimes?
void create_slime(iv2_t p)
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(!slimes[i].active)
    {
      slimes[i].active = true;
      slimes[i].in_combat = false;
      slimes[i].entity.hp = 4;
      slimes[i].entity.damage = 1;
      slimes[i].entity.armor = 0;
      slimes[i].entity.fov = 4;
      slimes[i].entity.move_speed = 1;
      slimes[i].entity.pos.x = p.x;
      slimes[i].entity.pos.y = p.y;
      slimes[i].entity.new_pos.x = p.x;
      slimes[i].entity.new_pos.y = p.y;
      slimes[i].entity.aspect.w = TILE_SIZE;
      slimes[i].entity.aspect.h = TILE_SIZE;
      slimes[i].entity.anim.frame_num = 0;
      slimes[i].entity.anim.frame_count = 4;
      slimes[i].entity.anim.frame_delay = 400;
      slimes[i].entity.anim.frame_last_changed = 0;
      return;
    }
  }

  // NOTE(Rami): 
  debug("Slime array is already full");
}

void kill_slime(i32 i)
{
  slimes[i].active = false;
  slimes[i].in_combat = false;
  slimes[i].entity.hp = 0;
  slimes[i].entity.damage = 0;
  slimes[i].entity.armor = 0;
  slimes[i].entity.fov = 0;
  slimes[i].entity.move_speed = 0;
  slimes[i].entity.pos.x = 0;
  slimes[i].entity.pos.y = 0;
  slimes[i].entity.new_pos.x = 0;
  slimes[i].entity.new_pos.y = 0;
  slimes[i].entity.aspect.w = 0;
  slimes[i].entity.aspect.h = 0;
  slimes[i].entity.anim.frame_num = 0;
  slimes[i].entity.anim.frame_count = 0;
  slimes[i].entity.anim.frame_delay = 0;
  slimes[i].entity.anim.frame_last_changed = 0;
}

void update_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      if(slimes[i].entity.hp <= 0)
      {
        kill_slime(i);
        continue;
      }

      bool can_move = true;

      if(slimes[i].in_combat)
      {
        // if(line_of_sight(slimes[i].entity.pos, player.entity.pos))
        // {
        //   if(dist_in_tiles(slimes[i].entity.pos, player.entity.pos) == 1)
        //   {
        //     attack_entity(&slimes[i].entity, &player.entity);
        //     add_console_msg("Slime attacks you for %d damage", RGBA_COLOR_WHITE_S, slimes[i].entity.damage);
        //   }
        //   else
        //   {
        //     i32 sx = slimes[i].entity.pos.x < player.entity.pos.x ? slimes[i].entity.move_speed : -slimes[i].entity.move_speed;
        //     i32 sy = slimes[i].entity.pos.y < player.entity.pos.y ? slimes[i].entity.move_speed : -slimes[i].entity.move_speed;

        //     if(slimes[i].entity.pos.x != player.entity.pos.x &&
        //        is_traversable((iv2_t){slimes[i].entity.pos.x + sx, slimes[i].entity.pos.y}))
        //     {
        //       slimes[i].entity.pos.x += sx;
        //     }

        //     if(slimes[i].entity.pos.y != player.entity.pos.y &&
        //       is_traversable((iv2_t){slimes[i].entity.pos.x, slimes[i].entity.pos.y + sy}))
        //     {
        //       slimes[i].entity.pos.y += sy;
        //     }
        //   }
        // }

        path_t path = find_path(slimes[i].entity.pos, player.entity.pos);
        if(path.found)
        {
          printf("PATH FOUND\n");

          // NOTE(Rami): Borked
          // slimes[i].entity.new_pos = path.next;
        }
        else
        {
          printf("PATH NOT FOUND\n");
        }

        // continue;
      }

      // NOTE(Rami): Have some kinda AI function so we can have different
      // out of combat behaviours like the one below.

      // int rand_dir = rnum(dir_left, dir_right);
      // if(rand_dir == dir_left)
      // {
      //   slimes[i].entity.new_pos.x = slimes[i].entity.pos.x - 1;
      // }
      // else
      // {
      //   slimes[i].entity.new_pos.x = slimes[i].entity.pos.x + 1;
      // }

      // rand_dir = rnum(dir_up, dir_down);
      // if(rand_dir == dir_up)
      // {
      //   slimes[i].entity.new_pos.y = slimes[i].entity.pos.y - 1;
      // }
      // else
      // {
      //   slimes[i].entity.new_pos.y = slimes[i].entity.pos.y + 1;
      // }

      // if(!is_traversable(slimes[i].entity.new_pos) ||
      //    is_iv2_equal(slimes[i].entity.new_pos, player.entity.pos))
      // {
      //   can_move = false;
      // }

      // NOTE(Rami): 
      can_move = true;
      if(can_move)
      {
        slimes[i].entity.pos.x = slimes[i].entity.new_pos.x;
        slimes[i].entity.pos.y = slimes[i].entity.new_pos.y;
      }
    }
    else
    {
      break;
    }
  }
}

void render_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      if(game_state.time_elapsed > slimes[i].entity.anim.frame_last_changed + slimes[i].entity.anim.frame_delay)
      {
        slimes[i].entity.anim.frame_num = (slimes[i].entity.anim.frame_num < (slimes[i].entity.anim.frame_count - 1)) ? slimes[i].entity.anim.frame_num + 1 : 0;
        slimes[i].entity.anim.frame_last_changed = game_state.time_elapsed;
      }

      SDL_Rect src = {tile_mul(slimes[i].entity.anim.frame_num), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(slimes[i].entity.pos.x) - game_state.camera.x, tile_mul(slimes[i].entity.pos.y) - game_state.camera.y, TILE_SIZE, TILE_SIZE};
      SDL_RenderCopy(game_state.renderer, assets.textures[tex_monster_sprite_sheet], &src, &dst);
    }
    else
    {
      break;
    }
  }
}