#include <slime.h>

slime_t *slimes[SLIME_COUNT];

// NOTE(Rami): Creation of multiple slimes?
void create_slimes(uint32 x, uint32 y)
{
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(!slimes[i])
    {
      slimes[i] = malloc(sizeof(slime_t));
      slimes[i]->entity = malloc(sizeof(entity_t));

      slimes[i]->in_combat = false;

      slimes[i]->entity->hp = 4;
      slimes[i]->entity->damage = 1;
      slimes[i]->entity->armor = 0;
      slimes[i]->entity->fov = 4;
      slimes[i]->entity->x = x;
      slimes[i]->entity->y = y;
      slimes[i]->entity->w = TILE_SIZE;
      slimes[i]->entity->h = TILE_SIZE;
      slimes[i]->entity->current_frame = 0;
      slimes[i]->entity->total_frames = 4;
      slimes[i]->entity->delay_between_frames = 400;
      slimes[i]->entity->frame_last_changed_time = 0;
      return;
    }
  }

  // NOTE(Rami): 
  printf("[Error] Slime array is already full\n");
}
void update_slimes()
{
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(slimes[i]->entity->hp <= 0)
      {
        free_slimes(i);
        continue;
      }

      if(slimes[i]->in_combat)
      {
        if(line_of_sight(slimes[i]->entity->x, slimes[i]->entity->y, player->entity->x, player->entity->y))
        {
          if(distance(slimes[i]->entity->x, slimes[i]->entity->y, player->entity->x, player->entity->y) == 1)
          {
            attack_entity(slimes[i]->entity, player->entity);
            add_console_msg("Slime attacks you for %d damage", RGBA_COLOR_WHITE_S, slimes[i]->entity->damage);
          }
          else
          {
            int32 sx = slimes[i]->entity->x < player->entity->x ? 1 : -1;
            int32 sy = slimes[i]->entity->y < player->entity->y ? 1 : -1;

            if(slimes[i]->entity->x != player->entity->x)
            {
              slimes[i]->entity->x += sx;
            }

            if(slimes[i]->entity->y != player->entity->y)
            {
              slimes[i]->entity->y += sy;
            }
          }
        }

        continue;
      }

      // NOTE(Rami): Have some kinda AI function so we can have different
      // out of combat behaviours like the one below.
      int new_x = slimes[i]->entity->x;
      int new_y = slimes[i]->entity->y;

      int rand_dir = num_between(0, 2);
      if(rand_dir == 0)
      {
        new_x -= 1;
      }
      else if(rand_dir == 1)
      {
        new_x += 1;
      }

      rand_dir = num_between(0, 2);
      if(rand_dir == 0)
      {
        new_y -= 1;
      }
      else if(rand_dir == 1)
      {
        new_y += 1;
      }

      if(is_traversable_pos(new_x, new_y) &&
         !is_player_pos(new_x, new_y))
      {
        slimes[i]->entity->x = new_x;
        slimes[i]->entity->y = new_y;
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
  for(int32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(time_elapsed > slimes[i]->entity->frame_last_changed_time + slimes[i]->entity->delay_between_frames)
      {
        slimes[i]->entity->current_frame = (slimes[i]->entity->current_frame < (slimes[i]->entity->total_frames - 1)) ? player->entity->current_frame + 1 : 0;
        slimes[i]->entity->frame_last_changed_time = time_elapsed;
      }

      SDL_Rect src = {tile_mul(slimes[i]->entity->current_frame), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(slimes[i]->entity->x) - camera.x, tile_mul(slimes[i]->entity->y) - camera.y, TILE_SIZE, TILE_SIZE};
      SDL_RenderCopy(renderer, textures[tex_monster_sprite_sheet], &src, &dst);
    }
    else
    {
      break;
    }
  }
}

void free_slimes(int32 i)
{
  if(slimes[i])
  {
    if(slimes[i]->entity)
    {
      free(slimes[i]->entity);
    }

    free(slimes[i]);
    slimes[i] = NULL;
  }
}