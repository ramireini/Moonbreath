#include "types.h"
#include "util.c"
#include "lighting.c"
#include "sprite.c"
#include "render.c"
#include "ui.c"
#include "level_gen.c"
#include "pathfind.c"
#include "conf.c"
#include "assets.c"
#include "pop_up_text.c"
#include "monster.c"
#include "item.c"
#include "player.c"
#include "game.c"

/*
  Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
  (^ Compression, pull things out, make them simpler)
*/

/*
  - Big syntax change
  - Turn post increments to pre increments

  Pixel art:
    - Change sword art
    - Change health potion art
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