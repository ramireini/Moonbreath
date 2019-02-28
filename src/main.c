#include <types.h>
#include <util_io.c>
#include <text_render.c>
#include <interface.c>
#include <level_gen.c>
#include <util_conf.c>
#include <assets.c>
#include <item.c>
#include <update.c>
#include <player.c>
#include <slime.c>
#include <render.c>
#include <game.c>

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust?
// (^ Compression, pull things out, make them simpler)

// NOTE(Rami): Think if we really want to have pointers for entities
// instead of having them on the stack.

// NOTE(Rami): Death animations for monsters like the slime?

// NOTE(Rami): Replace the global state key with a key pointer, query the keyboard state every frame.
// Uint8* SDL_GetKeyboardState(int* numkeys)
// Returns a pointer to an array of key states. A value of 1 means that the key is pressed and a value of 0 means that it is not.

// NOTE(Rami):
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions.

i32 main()
{
  if(!game_init())
  {
    return EXIT_FAILURE;
  }

  game_run();
  game_exit();

  return EXIT_SUCCESS;
}