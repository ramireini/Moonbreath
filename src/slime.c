void slime_create(iv2_t p)
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
      slimes[i].entity.anim.frame_delay = 200 + rnum(anim_min_offset, anim_max_offset);
      slimes[i].entity.anim.frame_last_changed = 0;
      return;
    }
  }
}

void slime_delete(i32 i)
{
  set_occupied(slimes[i].entity.pos, false);
  memset(&slimes[i], 0, sizeof(slime_t));
}

void slime_update()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      if(slimes[i].entity.hp <= 0)
      {
        slime_delete(i);
        continue;
      }

      bool can_move = true;

      if(slimes[i].in_combat)
      {
        path_t *path = pathfind(slimes[i].entity.pos, player.entity.pos);
        if(path->success)
        {
          if(iv2_equal(path->list[0], player.entity.pos))
          {
            attack_entity(&slimes[i].entity, &player.entity);
          }
          else
          {
            slimes[i].entity.new_pos = path->list[0];
          }
        }

        free(path);
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

      // if(!traversable(slimes[i].entity.new_pos) ||
      //    iv2_equal(slimes[i].entity.new_pos, player.entity.pos))
      // {
      //   can_move = false;
      // }

      if(can_move)
      {
        set_occupied(slimes[i].entity.pos, false);
        slimes[i].entity.pos = slimes[i].entity.new_pos;
        set_occupied(slimes[i].entity.new_pos, true);
      }
    }
  }
}

void slime_render()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      animation_update(&slimes[i].entity);

      SDL_Rect src = {tile_mul(slimes[i].entity.anim.frame_num), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(slimes[i].entity.pos.x) - game_state.camera.x, tile_mul(slimes[i].entity.pos.y) - game_state.camera.y, TILE_SIZE, TILE_SIZE};

      if(lighting_lit(slimes[i].entity.pos))
      {
        SDL_RenderCopy(game_state.renderer, assets.textures[tex_monster_sprite_sheet], &src, &dst);
      }
    }
  }
}