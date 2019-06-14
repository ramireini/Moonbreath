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
#include "game.c"

/*
  Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
  (^ Compression, pull things out, make them simpler)
*/

/*
  - Big syntax change

  - While the inventory is open and actions are being taken inside of it,
    the game turns should not be able to change

  - Have Effects and Messages be like this:
    Add damage effect, "You hit the monster"
    Add heal effect, "You drink the potion"'

  - When you have a pop up text that's longer than 1 character,
    it won't be centered, how can we make it adapt?

  - Decreasing alpha as time goes on for pop up text?

  - After triggering a pop up text, moving will cause it to follow you,
    which we don't want

  - When a slime is trying to attack you and you are moving inside walls,
    they are still following and hitting you when they get next to you,
    they should go off aggro if they can't find you, this should include
    being in a wall
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