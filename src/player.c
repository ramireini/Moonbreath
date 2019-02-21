player_t *player;

void create_player()
{
  player = malloc(sizeof(player_t));
  player->entity = malloc(sizeof(entity_t));

  player->new_x = 0;
  player->new_y = 0;
  player->name = "Frozii";
  player->level = 0;
  player->money = 0;
  player->max_hp = 10;
  player->xp = 0;
  player->speed = 1;
  player->turn = 0;
  player->inventory_display = false;
  player->inventory_item_count = 0;
  player->inventory_item_selected = 0;

  player->entity->hp = 5;
  player->entity->damage = 1;
  player->entity->armor = 0;
  player->entity->fov = 6;
  player->entity->x = 0;
  player->entity->y = 0;
  player->entity->w = TILE_SIZE;
  player->entity->h = TILE_SIZE;
  player->entity->current_frame = 0;
  player->entity->total_frames = 4;
  player->entity->delay_between_frames = 400;
  player->entity->frame_last_changed_time = 0;
}

// NOTE(Rami): Think about if we really want x-flip,
// we could basically have the player turn when moving left or right but
// not when moving up or down. Another option would be to just render the
// player as they are and not flip the texture at all.
void update_player()
{
  bool32 can_move = true;

  if(player->entity->hp <= 0)
  {
    // NOTE(Rami): Need to think about the process of the player dying more closely.
    add_console_msg("Player is dead now", RGBA_COLOR_BLUE_S);
    can_move = false;
  }
  else if(player->new_x < 0 || player->new_x >= LEVEL_WIDTH_IN_TILES ||
          player->new_y < 0 || player->new_y >= LEVEL_HEIGHT_IN_TILES)
  {
    can_move = false;
  }

  if(can_move)
  {
    if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == tile_wall_stone)
    {
      add_console_msg("The wall stops you from moving", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == tile_door_closed)
    {
      add_console_msg("You lean forward and push the door open", RGBA_COLOR_WHITE_S);
      level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] = tile_door_open;
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == tile_path_up)
    {
      add_console_msg("A path to the surface, [A]scend to flee the mountain", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == tile_path_down)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
  }

  if(can_move)
  {
    for(int32 i = 0; i < SLIME_COUNT; i++)
    {
      if(slimes[i])
      {
        if(player->new_x == slimes[i]->entity->x &&
           player->new_y == slimes[i]->entity->y)
        {
          can_move = false;

          if(!attack_entity(player->entity, slimes[i]->entity))
          {
            add_console_msg("You attack the Slime for %d damage", RGBA_COLOR_WHITE_S, player->entity->damage);
            slimes[i]->in_combat = true;
          }
          else
          {
            add_console_msg("You killed the Slime!", RGBA_COLOR_ORANGE_S);
          }

          break;
        }
      }
    }
  }

  can_move = true;
  if(can_move)
  {
    player->entity->x = player->new_x;
    player->entity->y = player->new_y;
  }

  player->turn++;
  global_state.key_pressed = 0;
}

void render_player()
{
  if(global_state.time_elapsed > player->entity->frame_last_changed_time + player->entity->delay_between_frames)
  {
    player->entity->current_frame = (player->entity->current_frame < (player->entity->total_frames - 1)) ? player->entity->current_frame + 1 : 0;
    player->entity->frame_last_changed_time = global_state.time_elapsed;
  }

  SDL_Rect src = {tile_mul(player->entity->current_frame), 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect dst = {tile_mul(player->entity->x) - global_state.camera.x, tile_mul(player->entity->y) - global_state.camera.y, player->entity->w, player->entity->h};

  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_player_sprite_sheet], &src, &dst);

  // sword one
  int32 sword_one = 0;
  SDL_Rect sword_one_dst = {player->entity->x - global_state.camera.x + 0, player->entity->y - global_state.camera.y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  int32 sword_two = 0;
  SDL_Rect sword_two_dst = {player->entity->x - global_state.camera.x + 11, player->entity->y - global_state.camera.y - 3, player->entity->w, player->entity->h};

  // source for the item texture
  SDL_Rect item_src;
  item_src.y = 0;
  item_src.w = TILE_SIZE;
  item_src.h = TILE_SIZE;
  
  for(int32 i = 0; i < ITEM_COUNT; i++)
  {
    // if equipped
    if(items[i].is_equipped)
    {
      // if an iron sword
      if(items[i].item_id == ID_IRON_SWORD)
      {
        // if hasn't been rendered before
        if(!sword_one)
        {
          sword_one = 1;

          // get the correct x-axis position for the item tile
          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          // render it
          SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_item_tileset], &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_item_tileset], &item_src, &sword_two_dst);
        }
      }
    }
  }
}

void free_player()
{
  if(player)
  {
    if(player->entity)
    {
      free(player->entity);
    }

    free(player);
    player = NULL;
  }
}