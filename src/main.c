#include <game.h>

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust? (<- Compression, pull things out, make them simpler)

// TODO:
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

// NOTE(Rami): move to game.c
game_state_t global_state;

int32 main()
{
  if(!game_init()) {return EXIT_FAILURE;}
  game_run();
  game_exit();

  return EXIT_SUCCESS;
}