#include "types.h"
#include "util.c"
#include "lighting.c"
#include "animation.c"
#include "render.c"
#include "ui.c"
#include "level_gen.c"
#include "pathfinding.c"
#include "conf.c"
#include "assets.c"
#include "monster.c"
#include "item.c"
#include "player.c"
#include "update.c"
#include "game.c"

// NOTE(rami): Compression oriented programming:
// Make it work, can you clean it/simplify it/make it more robust?
// (^ Compression, pull things out, make them simpler)

// NOTE(rami): Split assets into sound, fonts, textures etc for easier access?
// NOTE(rami): DT testing?
// We will probably want damage numbers that float up and eventually
// disappear and things of that nature and they will need DT
// NOTE(rami): Write a UI box generating function that assembles a box out of texture pieces,
// we would use this for things like console, stats, inventory and inventory item windows
// This would also make it easier to generate the interface for multiple game resolutions
// NOTE(rami): Decent combat?

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