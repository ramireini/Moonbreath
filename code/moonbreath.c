#include "moonbreath_platform.h"

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

/*
  Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
  (^ Compression, pull things out, make them simpler)
*/

/*
  - Player item rendering needs to handle the case of two rings
  - Shadows for font glyphs, could possibly make it look way better
  - Certain art needs to tilt towards direction looked at, like the rune shield example
  - After thinking I think we want to at least try having an x-flip on rendering
    entities depending on which direction they are going in,
    this also means pixel art will be drawn in a way where the thing is facing
    a specific direction instead of staring at you
    The skeleton art is okay but the slime needs to be adjusted to fit the above
  - Perhaps we want an array of light sources, so we could add torches
    and add that to be a light source.
  - Base human sprite, example armor set to fit on him, align points for the set
*/

internal void
toggle_fullscreen(SDL_Window *window)
{
  i32 flags = SDL_GetWindowFlags(window);
  if(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
  {
    SDL_SetWindowFullscreen(window, 0);
  }
  else
  {
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
}

internal void
update_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      game.state = state_quit;
    }
    else if(event.type == SDL_KEYDOWN)
    {
      if(!event.key.repeat)
      {
        SDL_Scancode key = event.key.keysym.scancode;

        b32 alt_key_was_down = event.key.keysym.mod & KMOD_ALT;
        if((key == SDL_SCANCODE_F4) && alt_key_was_down)
        {
          game.state = state_quit;
        }
        else if((key == SDL_SCANCODE_RETURN) && alt_key_was_down)
        {
          SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
          if(window)
          {
            toggle_fullscreen(window);
          }
        }
        else
        {
          player_keypress(key);
        }
      }
    }
  }
}

internal void
update_camera()
{
  game.camera.x = tile_mul(player.x) - (game.camera.w / 2);
  game.camera.y = (tile_mul(player.y) + (player.h / 2)) - (game.camera.h / 2);

  if(game.camera.x < 0)
  {
    game.camera.x = 0;
  }

  if(game.camera.y < 0)
  {
    game.camera.y = 0;
  }

  if(game.camera.x >= LEVEL_PIXEL_WIDTH - game.camera.w)
  {
    game.camera.x = LEVEL_PIXEL_WIDTH - game.camera.w;
  }

  if(game.camera.y >= LEVEL_PIXEL_HEIGHT - game.camera.h)
  {
    game.camera.y = LEVEL_PIXEL_HEIGHT - game.camera.h;
  }
}

internal void
run_game()
{
  add_player();

  generate_level();

  add_monster(monster_slime, 16, 54);
  add_monster(monster_skeleton, 17, 54);

  // add_item(id_rune_helmet, 12, 57);

  add_item(id_rune_chestplate, 13, 57);
  add_item(id_red_chestplate, 14, 57);
  add_item(id_iron_sword, 15, 57);
  add_item(id_red_sword, 16, 57);

  // add_item(id_rune_platelegs, 14, 57);
  // add_item(id_rune_boots, 15, 57);
  add_item(id_rune_shield, 17, 57);
  // add_item(id_rune_amulet, 17, 57);
  add_item(id_rune_ring, 18, 57);
  // add_item(id_rune_ring, 18, 58);
  // add_item(id_lesser_health_potion, 19, 57);
  // add_item(id_iron_sword, 12, 58);

  u32 frames_per_second = 144;
  r32 target_seconds_per_frame = 1.0f / (r32)frames_per_second;
  u64 old_counter = SDL_GetPerformanceCounter();
  r32 old_dt = SDL_GetPerformanceCounter();

  while(game.state)
  {
    // NOTE(rami):
    // i32 w = 0;
    // i32 h = 0;
    // SDL_GetWindowSize(game.window, &w, &h);

    // if(w != 1280 || h != 720)
    // {
    //   SDL_SetWindowSize(game.window, 1280, 720);
    // }

    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    r32 new_dt = SDL_GetPerformanceCounter();
    game.dt = (r32)(new_dt - old_dt) / (r32)game.perf_count_frequency;
    old_dt = new_dt;
    // printf("\ndt: %f\n", game.dt);

    // NOTE(rami): Pop up text
  #if 0
    for(i32 i = POP_UP_TEXT_COUNT - 1; i > -1; --i)
    {
      if(pop_up_text[i].active)
      {
        printf("\npop_up_text[%d]\n", i);
        printf("str: %s\n", pop_up_text[i].str);
        printf("x: %.02f, y: %.02f\n", pop_up_text[i].x, pop_up_text[i].y);
        printf("change: %.02f\n", pop_up_text[i].change);
        printf("speed: %.02f\n", pop_up_text[i].speed);
        printf("duration_time: %dms\n", pop_up_text[i].duration_time);
        printf("start_time: %dms\n", pop_up_text[i].start_time);
      }
    }
  #endif

    // NOTE(rami): Inventory
  #if 0
    for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; --i)
    {
      if(inventory.slot[i].id)
      {
        printf("\nInventory.slots[%d]\n", i);
        printf("id %d\n", inventory.slot[i].id);
        printf("unique_id %d\n", inventory.slot[i].unique_id);
        printf("x: %d, y: %d\n", inventory.slot[i].x, inventory.slot[i].y);
        printf("in_inventory %d\n", inventory.slot[i].in_inventory);
        printf("equipped %d\n", inventory.slot[i].equipped);
      }
    }
  #endif

    // NOTE(rami): Item
  #if 0
    for(i32 i = ITEM_COUNT - 1; i > -1; --i)
    {
      if(item[i].id)
      {
        printf("\nitem[%d]\n", i);
        printf("id %d\n", item[i].id);
        printf("unique_id %d\n", item[i].unique_id);
        printf("x: %d, y: %d\n", item[i].x, item[i].y);
        printf("in_inventory %d\n", item[i].in_inventory);
        printf("is_equipped %d\n", item[i].equipped);
      }
    }
  #endif

    // NOTE(rami): Player
  #if 0
    printf("\nPlayer\n");
    printf("frame_start.x, y: %d, %d\n", player.sprite.frame_start.x,
                                         player.sprite.frame_start.y);
    printf("frame_current.x, y: %d, %d\n", player.sprite.frame_current.x,
                                           player.sprite.frame_current.y);
    printf("frame_count: %d\n", player.sprite.frame_count);
    printf("frame_delay: %d\n", player.sprite.frame_delay);
    printf("frame_last_changed: %d\n", player.sprite.frame_last_changed);
    printf("new_x, new_y: %d, %d\n", player.new_x, player.new_y);
    printf("x, y: %d, %d\n", player.x, player.y);
    printf("w, h: %d, %d\n", player.w, player.h);
    printf("name: %s\n", player.name);
    printf("max_hp: %d\n", player.max_hp);
    printf("hp: %d\n", player.hp);
    printf("damage: %d\n", player.damage);
    printf("armor: %d\n", player.armor);
    printf("speed: %d\n", player.speed);
    printf("level: %d\n", player.level);
    printf("money: %d\n", player.money);
    printf("fov: %d\n", player.fov);
    printf("brightness: %d\n", player.brightness);
  #endif

    // NOTE(rami): Monster
  #if 0
    for(i32 i = MONSTER_COUNT - 1; i > -1; --i)
    {
      if(monster[i].type)
      {
        printf("\nmonster[%d]\n", i);
        printf("type: %d\n", monster[i].type);
        printf("ai: %d\n", monster[i].ai);

        printf("frame_start.x, y: %d, %d\n", monster[i].sprite.frame_start.x,
                                             monster[i].sprite.frame_start.y);
        printf("frame_current.x, y: %d, %d\n", monster[i].sprite.frame_current.x,
                                               monster[i].sprite.frame_current.y);
        printf("frame_count : %d\n", monster[i].sprite.frame_count);
        printf("frame_delay: %d\n", monster[i].sprite.frame_delay);
        printf("frame_last_changed: %d\n", monster[i].sprite.frame_last_changed);

        printf("x, y: %d, %d\n", monster[i].x, monster[i].y);
        printf("w, h: %d, %d\n", monster[i].w, monster[i].h);
        printf("in_combat: %d\n", monster[i].in_combat);
        printf("max_hp: %d\n", monster[i].max_hp);
        printf("hp: %d\n", monster[i].hp);
        printf("damage: %d\n", monster[i].damage);
        printf("armor: %d\n", monster[i].armor);
        printf("speed: %d\n", monster[i].speed);
        printf("level: %d\n", monster[i].level);
      }
    }
  #endif

    update_events();

    if(game.turn_changed)
    {
      update_player();
      update_monsters();
      update_lighting();
      update_camera();

      game.turn_changed = 0;
    }

    update_pop_up_text();

    render_tilemap();
    render_item();
    render_player();
    render_monsters();
    render_ui();
    render_pop_up_text();

    if(inventory.open)
    {
      render_inventory();
    }

    u64 work_counter_elapsed = SDL_GetPerformanceCounter() - old_counter;
    r32 ms_for_work = (1000.0f * (r32)work_counter_elapsed) / (r32)game.perf_count_frequency;
    // printf("ms_for_work: %.02f\n", ms_for_work);

    if(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
    {
      u32 time_to_delay = ((target_seconds_per_frame - get_seconds_elapsed(old_counter,
                          SDL_GetPerformanceCounter())) * 1000) - 1;
      if(time_to_delay > 0)
      {
        SDL_Delay(time_to_delay);
      }

      while(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter())
            < target_seconds_per_frame)
      {
      }
    }
    else
    {
      // NOTE(rami): We're right on the schedule or late
      // NOTE(rami): Valgrind will trigger this!
      // return;
    }

    u64 new_counter = SDL_GetPerformanceCounter();
    u64 elapsed_counter = new_counter - old_counter;
    SDL_RenderPresent(game.renderer);

    r32 ms_per_frame = (1000.0f * (r32)elapsed_counter) / (r32)game.perf_count_frequency;
    r32 frames_per_second = (r32)game.perf_count_frequency / (r32)elapsed_counter;
    old_counter = new_counter;
    // printf("ms_per_frame: %.02f\n", ms_per_frame);
    // printf("frames_per_second: %.02f\n", frames_per_second);
    // printf("time_elapsed: %d\n", game.time_elapsed);
  }
}