#include <game.h>

// TODO:
//
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions

// NOTE(Rami): Not all of these have to be global but they can be.
// The cost is that they exist in the .data section until the program ends.
// We could make these local in the future if we so decide
SDL_Window *window;
SDL_Renderer *renderer;

bool32 game_is_running;
bool32 turn_changed;
SDL_Keycode key_pressed;

font_t *fonts[FONT_COUNT];
SDL_Texture *textures[TEXTURE_COUNT];

item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];
console_message_t messages[MESSAGE_COUNT];

SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};

// NOTE(Rami): 
// Use this in the future to know how many cycles a single idle(only update) and busy(update and render) loop takes,
// from this we can drill down to other parts of the update and render blocks to know
// what is taking the most time
// 
// This will allow us to know where optimizations are needed but remember to let the compiler
// optimize first with -O2 so that we don't do work for nothing
//

/* 
uint64 start, end;
start = rdtsc();
end = rdtsc();
printf("%lu cycles\n", end - start);
uint64 rdtsc()
{
  uint64 a, d;
  __asm__ __volatile__ ("rdtsc" : "=a" (a), "=d" (d));
  return (d << 32) | a;
}
*/

int32 main()
{
  char *level = malloc(LEVEL_SIZE * LEVEL_SIZE);
  char *fov = malloc(LEVEL_SIZE * LEVEL_SIZE);

  if(!game_init())
  {
    printf("Game failed to initialize\n");
    game_is_running = false;
  }

  game_run(level, fov);
  game_exit(level, fov);

  return EXIT_SUCCESS;
}