#include "inc/types.h"
#include "util_io.c"
#include "render.c"
#include "interface.c"
#include "level_gen.c"
#include "util_conf.c"
#include "assets.c"
#include "item.c"
#include "update.c"
#include "player.c"
#include "slime.c"
#include "game.c"

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust?
// (^ Compression, pull things out, make them simpler)

// NOTE(Rami): Think if we really want our entities to be an array of pointers or not.

// NOTE(Rami): Death animations for monsters like the slime?

// NOTE(Rami):
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions.

// NOTE(Rami): Just put an game_update() and a game_render() (or game_update_and_render) function in game_run()
// and put all of the corresponding stuff into those two?

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