#include <game.h>

// NOTE(Rami): Compression oriented programming: Make it work, can you clean it/simplify it/make it more robust? (<- Compression, pull things out, make them simpler)

// TODO:
//
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions.

// NOTE(Rami): Go over .c and .h files and clean them up at some point.
// also note that not every .h file needs to have every function from its
// corresponding .c file because other compilation units don't have to know about
// all of the functions. Having all of them just makes the code bigger and adds
// more work for the preprocessor.

// NOTE(Rami): Not all of these have to be global but they can be.
// The cost is that they exist in the .data section until the program ends instead of living in the stack.
// All global variables/pointers which are not initialized will be set to zero automatically.

// typedef struct
// {
  // SDL_Window *window;
  // SDL_Renderer *renderer;
  // bool32 game_is_running;
  // bool32 turn_changed;
  // SDL_Keycode key_pressed;
  // SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};
  // uint32 time_elapsed;
  // SDL_Texture *textures[TEXTURE_COUNT];
// } game_state;

SDL_Window *window;
SDL_Renderer *renderer;

bool32 game_is_running;
bool32 turn_changed;
SDL_Keycode key_pressed;
SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT};
uint32 time_elapsed;
SDL_Texture *textures[TEXTURE_COUNT];

int32 main()
{
  if(!game_init())
  {
    printf("Game failed to initialize\n");
    game_is_running = false;
  }

  game_run();
  game_exit();

  return EXIT_SUCCESS;
}