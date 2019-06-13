#include "types.h"
#include "util.c"
#include "lighting.c"
#include "animation.c"
#include "render.c"
#include "ui.c"
#include "level_gen.c"
#include "pathfind.c"
#include "conf.c"
#include "assets.c"
#include "monster.c"
#include "item.c"
#include "player.c"
#include "update.c"
#include "game.c"

/*
  Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
  (^ Compression, pull things out, make them simpler)
*/

/*
  - A way to add and use colors
  - Add ability to pass different types to pop up text
  - Split asset array for easier access
  - Big syntax change
  - Can we clean up the way update/render functions look in game.c
*/

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