#include "types.h"
#include "util.c"
#include "lighting.c"
#include "sprite.c"
#include "render.c"
#include "ui.c"
#include "level_gen.c"
#include "pathfind.c"
// NOTE(rami): Work on conf when we need it again
// #include "conf.c"
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
  - Pop up message spacing
  - A better font than classic?
  - Figure out define/enum naming
  - Different font sizes for different item info etc
  - Base human sprite, example armor set to fit on him, align points for the set
  - An way of running the binary from anywhere inside of the program directory instead from a specific place
*/

i32 main()
{
  if(init_game())
  {
    run_game();
    exit_game();
    return(EXIT_SUCCESS);
  }

  exit_game();
  return(EXIT_FAILURE);
}