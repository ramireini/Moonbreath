#include "inc/types.h"
#include "util.c"
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

// NOTE(Rami): Example death animation for the slime?

// NOTE(Rami): SDL_SetTextureColorMod(tex, r, g, b);

// NOTE(Rami):
// Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions

// NOTE(Rami): Basic lighting
// NOTE(Rami): Simple combat

i32 main()
{
  if(!game_init()) {return EXIT_FAILURE;}
  game_run();
  game_exit();

  return EXIT_SUCCESS;
}