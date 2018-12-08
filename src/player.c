#include <player.h>

player_t *player;

void create_player(char *name, int tile, int level, int money, int hp, int max_hp, int xp, int x, int y, int w, int h, int speed, int fov, int attack, int armor)
{
  player = malloc(sizeof(player_t));
  player->entity = malloc(sizeof(entity_t));
  player->entity->tile = tile;
  player->entity->x = x;
  player->entity->y = y;
  player->entity->w = w;
  player->entity->h = h;
  player->name = name;
  player->level = level;
  player->money = money;
  player->hp = hp;
  player->max_hp = max_hp;
  player->xp = xp;
  player->speed = speed;
  player->fov = fov;
  player->attack = attack;
  player->armor = armor;
  player->turn = 0;
  player->inventory_display = 0;
  player->inventory_item_count = 0;
  player->inventory_item_selected = 0;
  player->moved = 0;
  player->new_x = 0;
  player->new_y = 0;
}

// NOTE(Rami): At some point think about if we really want x-flip,
// we could basically have the player turn when moving left or right but
// not when moving up or down. Another option would be to just render the
// player as they are and not flip the texture at all.
void update_player(char *level)
{
  if(player->moved)
  {
    // assume we can move
    int can_move = 1;

    // turn into units we can use for arrays
    int temp_x = to_tiles(player->entity->x) + player->new_x;
    int temp_y = to_tiles(player->entity->y) + player->new_y;

    if(level[(temp_y * LEVEL_SIZE) + temp_x] == TILE_WALL_STONE)
    {
      add_console_msg("The wall stops you from moving", TEXT_COLOR_WHITE);
      can_move = 0;
    }
    else if(level[(temp_y * LEVEL_SIZE) + temp_x] == TILE_DOOR_CLOSED)
    {
      add_console_msg("You lean forward and push the door open", TEXT_COLOR_WHITE);
      level[(temp_y * LEVEL_SIZE) + temp_x] = TILE_DOOR_OPEN;
      can_move = 0;
    }
    else if(level[(temp_y * LEVEL_SIZE) + temp_x] == TILE_PATH_UP)
    {
      add_console_msg("A path to the surface, [A]scend to flee the mountain", TEXT_COLOR_WHITE);
      can_move = 0;
    }
    else if(level[(temp_y * LEVEL_SIZE) + temp_x] == TILE_PATH_DOWN)
    {
      add_console_msg("A path that leads further downwards.. [D]escend?", TEXT_COLOR_WHITE);
      can_move = 0;
    }

    // NOTE(Rami): forcing for testing
    can_move = 1;
    if(can_move)
    {
      player->entity->y += to_pixels(player->new_y);
      player->entity->x += to_pixels(player->new_x);
    }

    player->turn++;
    player->new_x = 0;
    player->new_y = 0;
    player->moved = 0;
    key_pressed = 0;
  }
}

void free_player()
{
  free(player->entity);
  free(player);
  player = NULL;
}