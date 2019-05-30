#include "types.h"
#include "util.c"
#include "lighting.c"
#include "animation.c"
#include "render.c"
#include "interface.c"
#include "level_gen.c"
#include "pathfinding.c"
#include "util_conf.c"
#include "assets.c"
#include "item.c"
#include "player.c"
#include "update.c"
#include "slime.c"
#include "game.c"

// NOTE(Rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust?
// (^ Compression, pull things out, make them simpler)

// NOTE(Rami): Fix inventory rendering since it's shit
// NOTE(Rami): Turn the running variable into a state
// NOTE(Rami): Example death animation for the slime?
// NOTE(Rami): Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions
// NOTE(Rami): Decent combat?

i32 main()
{
  if(init_game())
  {
    run_game();
    exit_game();
    return EXIT_SUCCESS;
  }

  exit_game();
  return EXIT_FAILURE;
}