// NOTE(Rami): Consider adding the function prototypes to the .h files
// so we can include the .c files in whatever order.
#include "types.h"
#include "util_io.c"
#include "level_gen.c"
#include "text_render.c"
#include "player.c"
#include "game.c"
#include "interface.c"
#include "item.c"
#include "update.c"
#include "slime.c"
#include "render.c"

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust? (<- Compression, pull things out, make them simpler)

// TODO:
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions.

// Have a separate asset.h/.c for fonts/textures/etc.
// so that font stuff doesn't have to stay with text_render stuff.
// Along with this create the free_assets function and move the asset structs to its .h file.

int32 main()
{
  if(!game_init()) {return EXIT_FAILURE;}
  game_run();
  game_exit();

  return EXIT_SUCCESS;
}