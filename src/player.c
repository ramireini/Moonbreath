#include <player.h>

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
// player as they are and not flip the texture at all
void update_player(uint8 *level)
{
  if(player->entity->hp <= 0)
  {
    // NOTE(Rami): Need to think about the process of the player dying more closely.
    add_console_msg("Player is dead now", HEX_COLOR_BLUE);
  }

  bool32 can_move = true;

  if(can_move)
  {
    if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == TILE_WALL_STONE)
    {
      add_console_msg("The wall stops you from moving", HEX_COLOR_WHITE);
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the door open", HEX_COLOR_WHITE);
      level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] = TILE_DOOR_OPEN;
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == TILE_PATH_UP)
    {
      add_console_msg("A path to the surface, [A]scend to flee the mountain", HEX_COLOR_WHITE);
      can_move = false;
    }
    else if(level[(player->new_y * LEVEL_WIDTH_IN_TILES) + player->new_x] == TILE_PATH_DOWN)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", HEX_COLOR_WHITE);
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
            add_console_msg("You attack the Slime for %d damage", HEX_COLOR_WHITE, player->entity->damage);
            slimes[i]->in_combat = true;
          }
          else
          {
            add_console_msg("You killed the Slime!", HEX_COLOR_ORANGE);
          }

          break;
        }
      }
    }
  }

  // NOTE(Rami): Forcing for testing.
  can_move = true;
  if(can_move)
  {
    player->entity->x = player->new_x;
    player->entity->y = player->new_y;
  }

  player->turn++;
  key_pressed = 0;
}

void render_player()
{
  if(time_elapsed > player->entity->frame_last_changed_time + player->entity->delay_between_frames)
  {
    player->entity->current_frame = (player->entity->current_frame < (player->entity->total_frames - 1)) ? player->entity->current_frame + 1 : 0;
    player->entity->frame_last_changed_time = time_elapsed;
  }

  SDL_Rect src = {tile_mul(player->entity->current_frame), 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect dst = {tile_mul(player->entity->x) - camera.x, tile_mul(player->entity->y) - camera.y, player->entity->w, player->entity->h};

  SDL_RenderCopy(renderer, textures[TEX_PLAYER_SPRITE_SHEET], &src, &dst);

  // sword one
  int32 sword_one = 0;
  SDL_Rect sword_one_dst = {player->entity->x - camera.x + 0, player->entity->y - camera.y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  int32 sword_two = 0;
  SDL_Rect sword_two_dst = {player->entity->x - camera.x + 11, player->entity->y - camera.y - 3, player->entity->w, player->entity->h};

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
          SDL_RenderCopy(renderer, textures[TEX_ITEM_TILESET], &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          SDL_RenderCopy(renderer, textures[TEX_ITEM_TILESET], &item_src, &sword_two_dst);
        }
      }
    }
  }
}

// NOTE(Rami): Delete later?
void place_player(int32 tile_x, int32 tile_y)
{
    player->entity->x = tile_x;
    player->entity->y = tile_y;

    player->new_x = player->entity->x;
    player->new_y = player->entity->y;
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