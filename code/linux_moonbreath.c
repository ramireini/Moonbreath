#include "types.h"
#include "util.c"
#include "lighting.c"
#include "animation.c"
#include "render.c"
#include "ui.c"
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

// NOTE(Rami): We could just have an array of entities and
// have a separate array for information on all entities (like monsters, npcs etc)
// And again we could decide to get that information from a .cfg file or
// have it hardcoded in
// This would allow us to have an enum which is the entity type in all array members
// This would be useful for example in a function that returns a string of how
// the entity attacked another entity because means of attacking are different
// depending on who is doing the attacking and what they are using.

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