#include <types.h>
#include <level_gen.c>
#include <util_io.c>
#include <assets.c>
#include <game.c>
#include <text_render.c>
#include <update.c>
#include <render.c>
#include <interface.c>
#include <item.c>
#include <player.c>
#include <slime.c>

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust?
// (<- Compression, pull things out, make them simpler)

// NOTE(Rami): See if there is a way of removing the function prototypes
// and the extern statements from the headers.

// TODO:
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions.

i32 main()
{
  if(!game_init()) {return EXIT_FAILURE;}
  game_run();
  game_exit();

  return EXIT_SUCCESS;
}