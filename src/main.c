#include <game.h>

// TODO:
//
// All the // NOTE(Rami):'s
//
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows.
// This would also make it easier to generate the interface for multiple game resolutions.
//
// Implement diagonal controls if we decide to have them

SDL_Window *window;
SDL_Renderer *renderer;

int game_is_running;
SDL_Keycode key_pressed;

font_t *fonts[FONT_COUNT];
SDL_Texture *textures[TEXTURE_COUNT];

item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];
console_message_t messages[MESSAGE_COUNT];

int main(int argc, char **argv)
{
  char *level = malloc(LEVEL_SIZE * LEVEL_SIZE);
  char *fov = malloc(LEVEL_SIZE * LEVEL_SIZE);

  // camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  player_t *player = malloc(sizeof(player_t));
  strcpy(player->name, "Frozii");
  player->level = 0;
  player->money = 0;
  player->hp = 5;
  player->max_hp = 10;
  player->xp = 0;
  player->x = 0;
  player->y = 0;
  player->w = TILE_SIZE;
  player->h = TILE_SIZE;
  player->speed = 1;
  player->fov = 6;
  player->attack = 5;
  player->armor = 6;
  player->turns_taken = 0;
  player->inventory_display = 0;
  player->inventory_item_count = 0;
  player->inventory_hl_index = 0;

  // NOTE(Rami): 
  // print the tile we want based on the number in the level array
  #if 0
    for(int y = 0; y < LEVEL_SIZE; y++)
    {
        for(int x = 0; x < LEVEL_SIZE; x++)
        {
            if(level[y * LEVEL_SIZE + x] == TILE_FLOOR_GRASS)
            {
              printf(".");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_WALL_STONE)
            {
              printf("#");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_DOOR_CLOSED)
            {
              printf("D");
            }
            else if(level[y * LEVEL_SIZE + x] == TILE_PATH_UP || level[y * LEVEL_SIZE + x] == TILE_PATH_DOWN)
            {
              printf("S");
            }
        }

        printf("\n");
    }
  #endif

  if(game_init())
  {
    printf("Game failed to initialize\n");
    game_is_running = 0;
  }

  game_run(level, player, fov, &camera);

  game_exit(level, player, fov);

  return 0;
}