internal void
create_slimes(v2_t pos)
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
      slimes[i].entity.speed = 1;
      slimes[i].entity.x = pos.x;
      slimes[i].entity.y = pos.y;
      slimes[i].entity.new_x = pos.x;
      slimes[i].entity.new_y = pos.y;
      slimes[i].entity.w = TILE_SIZE;
      slimes[i].entity.h = TILE_SIZE;
      slimes[i].entity.anim.frame_current = 0;
      slimes[i].entity.anim.frame_count = 4;
      slimes[i].entity.anim.frame_delay = 200 + rand_num(anim_min_offset, anim_max_offset);
      slimes[i].entity.anim.frame_last_changed = 0;
      return;
    }
  }
}

internal void
delete_slimes(i32 i)
{
  set_occupied(v2(slimes[i].entity.x, slimes[i].entity.y), false);
  memset(&slimes[i], 0, sizeof(slime_t));
}

internal void
update_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      if(slimes[i].entity.hp <= 0)
      {
        delete_slimes(i);
        continue;
      }

      b32 can_move = true;

      if(slimes[i].in_combat)
      {
        path_t *path = pathfind(v2(slimes[i].entity.x, slimes[i].entity.y), v2(player.entity.x, player.entity.y));
        if(path->success)
        {
          if(v2_equal(v2(path->list[0].x, path->list[0].y), v2(player.entity.x, player.entity.y)))
          {
            attack_entity(&slimes[i].entity, &player.entity);
          }
          else
          {
            slimes[i].entity.new_x = path->list[0].x;
            slimes[i].entity.new_y = path->list[0].y;
          }
        }

        free(path);
      }

      // NOTE(Rami): Have some kinda AI function so we can have different
      // out of combat behaviours like the one below.

      // int rand_dir = rand_num(dir_left, dir_right);
      // if(rand_dir == dir_left)
      // {
      //   slimes[i].entity.new_pos.x = slimes[i].entity.pos.x - 1;
      // }
      // else
      // {
      //   slimes[i].entity.new_pos.x = slimes[i].entity.pos.x + 1;
      // }

      // rand_dir = rand_num(dir_up, dir_down);
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
        set_occupied(v2(slimes[i].entity.x, slimes[i].entity.y), false);
        slimes[i].entity.x = slimes[i].entity.new_x;
        slimes[i].entity.y = slimes[i].entity.new_y;
        set_occupied(v2(slimes[i].entity.new_x, slimes[i].entity.new_y), true);
      }
    }
  }
}

internal void
render_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i].active)
    {
      update_animation(&slimes[i].entity);

      SDL_Rect src = {tile_mul(slimes[i].entity.anim.frame_current), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(slimes[i].entity.x) - game.camera.x, tile_mul(slimes[i].entity.y) - game.camera.y, TILE_SIZE, TILE_SIZE};

      if(is_lit(v2(slimes[i].entity.x, slimes[i].entity.y)))
      {
        v4_t color = get_color_for_lighting_value(v2(slimes[i].entity.x, slimes[i].entity.y));
        SDL_SetTextureColorMod(assets.textures[tex_monster_sprite_sheet], color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, assets.textures[tex_monster_sprite_sheet], &src, &dst);
      }
    }
  }
}