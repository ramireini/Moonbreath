#include <game.h>

// TODO:
//
// All the // NOTE(Rami):'s
//
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows.
// This would also make it easier to generate the interface for multiple game resolutions.
//
// Implement diagonal controls if we decide to have them.

SDL_Window *window;
SDL_Renderer *renderer;

int game_is_running;
SDL_Keycode key_pressed;

// NOTE(Rami): make some of these arrays not global?
font_t *fonts[FONT_COUNT];
SDL_Texture *textures[TEXTURE_COUNT];

item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];
console_message_t messages[MESSAGE_COUNT];

int main()
{
  char *level = malloc(LEVEL_SIZE * LEVEL_SIZE);
  char *fov = malloc(LEVEL_SIZE * LEVEL_SIZE);

  // Camera
  SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

  if(game_init())
  {
    printf("Game failed to initialize\n");
    game_is_running = 0;
  }

  game_run(level, fov, &camera);
  game_exit(level, fov);

  // NOTE(Rami): visit later
  // for(int i = 0; i < ENTITY_COUNT; i++)
  // {
  //   printf("\n[Element: %d]\n", i);

  //   if(!entities[i].type)
  //   {
  //     printf("Type is Good\n");
  //   }
  //   else
  //   {
  //     printf("Type is Bad\n");
  //   }

  //   if(!entities[i].player)
  //   {
  //     printf("Player is Good\n");
  //   }
  //   else
  //   {
  //     printf("Player is Bad\n");
  //   }

  //   if(!entities[i].monster)
  //   {
  //     printf("Monster is Good\n");
  //   }
  //   else
  //   {
  //     printf("Monster is Bad\n");
  //   }
  // }

  return 0;
}