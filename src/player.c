#include <player.h>

player_t *player;

void create_player(char *name, int32 tile, int32 level, int32 money, int32 hp, int32 max_hp, int32 xp, int32 x, int32 y, int32 w, int32 h, int32 speed, int32 fov, int32 damage, int32 armor)
{
  player = malloc(sizeof(player_t));
  player->entity = malloc(sizeof(entity_t));

  player->new_x = 0;
  player->new_y = 0;
  player->name = name;
  player->level = level;
  player->money = money;
  player->max_hp = max_hp;
  player->xp = xp;
  player->speed = speed;
  player->turn = 0;
  player->inventory_display = false;
  player->inventory_item_count = 0;
  player->inventory_item_selected = 0;

  player->entity->tile = tile;
  player->entity->hp = hp;
  player->entity->damage = damage;
  player->entity->armor = armor;
  player->entity->fov = fov;
  player->entity->x = x;
  player->entity->y = y;
  player->entity->w = w;
  player->entity->h = h;
}

// NOTE(Rami): Think about if we really want x-flip,
// we could basically have the player turn when moving left or right but
// not when moving up or down. Another option would be to just render the
// player as they are and not flip the texture at all
void update_player(char *level)
{
  // NOTE(Rami): Check player hp, so we can die
  if(player->entity->hp <= 0)
  {
    // NOTE(Rami): 
    add_console_msg("Player is dead now", TEXT_COLOR_BLUE);
  }

  bool32 can_move = true;

  int32 array_x = to_tiles(player->new_x);
  int32 array_y = to_tiles(player->new_y);

  if(can_move)
  {
    if(level[(array_y * LEVEL_SIZE) + array_x] == TILE_WALL_STONE)
    {
      add_console_msg("The wall stops you from moving", TEXT_COLOR_WHITE);
      can_move = false;
    }
    else if(level[(array_y * LEVEL_SIZE) + array_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the door open", TEXT_COLOR_WHITE);
      level[(array_y * LEVEL_SIZE) + array_x] = TILE_DOOR_OPEN;
      can_move = false;
    }
    else if(level[(array_y * LEVEL_SIZE) + array_x] == TILE_PATH_UP)
    {
      add_console_msg("A path to the surface, [A]scend to flee the mountain", TEXT_COLOR_WHITE);
      can_move = false;
    }
    else if(level[(array_y * LEVEL_SIZE) + array_x] == TILE_PATH_DOWN)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", TEXT_COLOR_WHITE);
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

          if(!attack(player->entity, slimes[i]->entity))
          {
            add_console_msg("You hit the Slime for %d damage", TEXT_COLOR_GREEN, player->entity->damage);
            slimes[i]->in_combat = true;
          }
          else
          {
            add_console_msg("You killed the Slime!", TEXT_COLOR_YELLOW);
          }

          break;
        }
      }
    }
  }

  // NOTE(Rami): Forcing for testing
  // can_move = true;
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
  SDL_Rect src = {to_pixels(player->entity->tile), 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect dst = {player->entity->x - camera.x, player->entity->y - camera.y, player->entity->w, player->entity->h};

  SDL_RenderCopy(renderer, textures[TEX_PLAYER_SPRITE_SHEET], &src, &dst);

  // sword one
  int32 sword_one = 0;
  SDL_Rect sword_one_dst = {player->entity->x - camera.x + 0, player->entity->y - camera.y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  int32 sword_two = 0;
  SDL_Rect sword_two_dst = {player->entity->x - camera.x + 11, player->entity->y - camera.y - 3, player->entity->w, player->entity->h};

  // NOTE(Rami): Fix this later, issue with the sword dual wield

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
          item_src.x = to_pixels(items_info[items[i].item_id - 1].tile);

          // render it
          SDL_RenderCopy(renderer, textures[TEX_ITEM_TILESET], &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = to_pixels(items_info[items[i].item_id - 1].tile);

          SDL_RenderCopy(renderer, textures[TEX_ITEM_TILESET], &item_src, &sword_two_dst);
        }
      }
    }
  }
}

void place_player(int32 tile_x, int32 tile_y)
{
    player->entity->x = to_pixels(tile_x);
    player->entity->y = to_pixels(tile_y);

    player->new_x = player->entity->x;
    player->new_y = player->entity->y;
}

void free_player()
{
  free(player->entity);
  free(player);
  player = NULL;
}