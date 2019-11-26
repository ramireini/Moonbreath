#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <time.h>
#include <stdint.h>

#include "types.h"
#include "util.c"
#include "dungeon.c"
#include "fov.c"
#include "render.c"
#include "item.c"
#include "ui.c"
#include "pathfind.c"
// #include "conf.c" // TODO(rami): Work on conf when we need it again
#include "assets.c"
#include "pop_text.c"
#include "monster.c"
#include "player.c"

// NOTE(rami): Two Steps
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(rami): Place the torches, wall grates and wall small grates.
// TODO(rami): Next up: Banners and wall with stuff growing on it.

// TODO(rami): Game tileset is getting unwieldy,
// need to start stacking it.

// TODO(rami): More items!!

// TODO(rami): When a monster or monsters come into view have a message saying something like
// "You see a Slime."
// With multiple enemies:
// "You see a Slime and a Skeleton" on the same line to save message space.
// Or have them in the single version for every enemy.

// TODO(rami): When you see an item, "You found a weapon.", "You found a scroll."

// TODO(rami): After the ground work for the dungeon level layouts is done
// we can focus more on adding monsters, items, gold etc. to the levels.

internal void
resize_window(u32 w, u32 h)
{
    SDL_SetWindowSize(game.window, w, h);
    game.window_size = V2u(w, h);
    game.console_size.w = game.window_size.w;
    game.camera = V4s(0, 0, game.window_size.w, game.window_size.h - game.console_size.h);
}

internal void
toggle_fullscreen()
{
    u32 flags = SDL_GetWindowFlags(game.window);
    if(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_SetWindowFullscreen(game.window, 0);
        resize_window(1280, 720);
        SDL_SetWindowPosition(game.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    else
    {
        SDL_SetWindowFullscreen(game.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        
        v2u window_size = {0};
        SDL_GetWindowSize(game.window, (s32 *)&window_size.x, (s32 *)&window_size.y);
        resize_window(window_size.x, window_size.y);
    }
}

internal void
update_camera()
{
#if 0
    printf("camera.x1: %d\n", game.camera.x);
    printf("camera.y1: %d\n", game.camera.y);
    printf("camera.x2: %d\n", game.camera.x + game.camera.w);
    printf("camera.y2: %d\n\n", game.camera.y + game.camera.h);
#endif
    
    game.camera.x = tile_mul(player.pos.x) - (game.camera.w * 0.5);
    
    // NOTE(rami): This gives us 24 pixels from the top and bottom
    // initially when the camera is not locked to an edge which seems to be
    // the closest we can get to 32 pixels.
    game.camera.y = tile_mul(player.pos.y) - (game.camera.h * 0.5) + (player.h * 0.5);
    
    if(game.camera.x < 0)
    {
        game.camera.x = 0;
    }
    
    if(game.camera.y < 0)
    {
        game.camera.y = 0;
    }
    
    if(game.camera.x >= (s32)(tile_mul(MAX_DUNGEON_WIDTH) - game.camera.w))
    {
        game.camera.x = tile_mul(MAX_DUNGEON_WIDTH) - game.camera.w;
    }
    
    if(game.camera.y >= (s32)(tile_mul(MAX_DUNGEON_HEIGHT) - game.camera.h))
    {
        game.camera.y = tile_mul(MAX_DUNGEON_HEIGHT) - game.camera.h;
    }
}

internal void
update_input(input_state_t *new_state, b32 is_down)
{
    if(new_state->is_down != is_down)
    {
        new_state->is_down = is_down;
        if(!new_state->is_down)
        {
            new_state->has_been_up = true;
        }
    }
}

internal void
process_events(input_state_t *keyboard)
{
    SDL_Event event = {0};
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game.state = state_quit;
        }
        else if(event.type == SDL_KEYUP ||
                event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key_code = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(!event.key.repeat)
            {
                switch(key_code)
                {
                    case SDLK_q: game.state = state_quit; break;
                    
                    case SDLK_F1: update_input(&keyboard[key_fov_toggle], is_down); break;
                    
                    case SDLK_w: update_input(&keyboard[key_move_up], is_down); break;
                    case SDLK_s: update_input(&keyboard[key_move_down], is_down); break;
                    case SDLK_a: update_input(&keyboard[key_move_left], is_down); break;
                    case SDLK_d: update_input(&keyboard[key_move_right], is_down); break;
                    case SDLK_i: update_input(&keyboard[key_inventory], is_down); break;
                    case SDLK_COMMA: update_input(&keyboard[key_pick_up], is_down); break;
                    case SDLK_PERIOD: update_input(&keyboard[key_drop], is_down); break;
                    case SDLK_e: update_input(&keyboard[key_equip], is_down); break;
                    case SDLK_c: update_input(&keyboard[key_consume], is_down); break;
                    case SDLK_m: update_input(&keyboard[key_move], is_down); break;
                    case SDLK_UP: update_input(&keyboard[key_ascend], is_down); break;
                    case SDLK_DOWN: update_input(&keyboard[key_descend], is_down); break;
                }
                
                if(is_down)
                {
                    b32 alt_key_was_down = (event.key.keysym.mod & KMOD_ALT);
                    if((key_code == SDLK_RETURN) && alt_key_was_down)
                    {
                        SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                        if(window)
                        {
                            toggle_fullscreen();
                        }
                    }
                }
            }
        }
    }
}

internal b32
set_window_icon()
{
    b32 result = false;
    
    SDL_Surface *icon = IMG_Load("data/images/icon.png");
    if(icon)
    {
        SDL_SetWindowIcon(game.window, icon);
        SDL_FreeSurface(icon);
        result = true;
    }
    else
    {
        printf("ERROR: Failed to load window icon\n");
    }
    
    return(result);
}

internal b32
set_fonts()
{
    b32 result = true;
    
    fonts[font_classic] = create_bmp_font("data/fonts/classic16x16.png", 16, 16, 14, 8, 13);
    fonts[font_classic_outlined] = create_bmp_font("data/fonts/classic_outlined16x16.png", 16, 16, 14, 8, 13);
    fonts[font_alkhemikal] = create_ttf_font("data/fonts/alkhemikal.ttf", 16, 6);
    fonts[font_monaco] = create_ttf_font("data/fonts/monaco.ttf", 16, 6);
    fonts[font_dos_vga] = create_ttf_font("data/fonts/dos_vga.ttf", 16, 6);
    
    for(u32 i = 0; i < font_total; ++i)
    {
        if(!fonts[i] || !fonts[i]->success)
        {
            result = false;
            printf("ERROR: Font %u could not be loaded\n", i);
        }
    }
    
    return(result);
}

internal b32
set_textures()
{
    b32 result = true;
    
    textures[tex_tilemap].tex = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tile_mul(MAX_DUNGEON_WIDTH), tile_mul(MAX_DUNGEON_HEIGHT));
    textures[tex_tilemap].w = tile_mul(MAX_DUNGEON_WIDTH);
    textures[tex_tilemap].h = tile_mul(MAX_DUNGEON_HEIGHT);
    
    textures[tex_game_tileset] = load_texture("data/images/game_tileset.png", 0);
    textures[tex_item_tileset] = load_texture("data/images/item_tileset.png", 0);
    textures[tex_wearable_item_tileset] = load_texture("data/images/wearable_item_tileset.png", 0);
    textures[tex_sprite_sheet] = load_texture("data/images/sprite_sheet.png", 0);
    textures[tex_inventory_win] = load_texture("data/images/inventory_win.png", 0);
    textures[tex_inventory_item_win] = load_texture("data/images/inventory_item_win.png", 0);
    textures[tex_inventory_selected_item] = load_texture("data/images/inventory_selected_item.png", 0);
    textures[tex_interface_bottom_win] = load_texture("data/images/interface_bottom_win.png", 0);
    textures[tex_health_bar_outside] = load_texture("data/images/health_bar_outside.png", 0);
    textures[tex_health_bar_inside] = load_texture("data/images/health_bar_inside.png", 0);
    textures[tex_player_parts] = load_texture("data/images/player_parts.png", 0);
    
    for(u32 i = 0; i < tex_total; ++i)
    {
        if(!textures[i].tex)
        {
            result = false;
            printf("ERROR: Texture %u could not be created\n", i);
        }
    }
    
    return(result);
}

internal void
set_game_data()
{
#if 1
    u64 seed = 16674218;
#else
    u64 seed = time(0);
#endif
    
    srand(seed);
    printf("Random Seed: %lu\n\n", seed);
    
    game.state = state_running;
    game.window_size = V2u(1280, 720);
    game.console_size = V2u(game.window_size.w, 160);
    game.camera = V4s(0, 0, game.window_size.w, game.window_size.h - game.console_size.h);
    
    dungeon.level = 1;
    dungeon.w = 64;
    dungeon.h = 64;
    
    for(u32 i = 0; i < array_count(items); ++i)
    {
        items[i].unique_id = i + 1;
    }
    
    set_monster_spawn_chances();
    set_item_info_data();
}

internal u32
initialize_game()
{
    b32 result = false;
    
    set_game_data();
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        u32 window_flags = 0;
        game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       game.window_size.w, game.window_size.h,
                                       window_flags);
        if(game.window)
        {
            printf("Monitor refresh rate: %dHz\n\n", get_window_refresh_rate());
            
            u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
            game.renderer = SDL_CreateRenderer(game.window, -1, renderer_flags);
            if(game.renderer)
            {
                u32 img_flags = IMG_INIT_PNG;
                if(IMG_Init(img_flags) & img_flags)
                {
                    if(!TTF_Init())
                    {
                        if(set_window_icon())
                        {
                            if(set_fonts())
                            {
                                if(set_textures())
                                {
                                    result = true;
                                }
                                else
                                {
                                    // NOTE(rami): Texture failed
                                }
                            }
                            else
                            {
                                // NOTE(rami): Font failed
                            }
                        }
                        else
                        {
                            // NOTE(rami): Window icon failed
                        }
                    }
                    else
                    {
                        printf("ERROR: SDL TTF library could not initialize: %s\n", SDL_GetError());
                    }
                }
                else
                {
                    printf("ERROR: SLD image library could not initialize: %s\n", SDL_GetError());
                }
            }
            else
            {
                printf("ERROR: SDL could not create a renderer: %s\n", SDL_GetError());
            }
        }
        else
        {
            printf("ERROR: SDL could not create window: %s\n", SDL_GetError());
        }
    }
    else
    {
        printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
    }
    
    return(result);
}

internal void
array_debug()
{
    // NOTE(rami): Pop text
#if 0
    for(s32 i = array_count(pop_texts) - 1; i > -1; --i)
    {
        pop_text_t *pop_text = &pop_texts[i];
        if(pop_texts[i].active)
        {
            printf("\npop_up_text[%u]\n", i);
            printf("str: %s\n", pop_text->str);
            printf("x: %u, y: %u\n", pop_text->pos.x, pop_text->pos.y);
            printf("change: %.02f\n", pop_text->change);
            printf("speed: %.02f\n", pop_text->speed);
            printf("duration_time: %ums\n", pop_text->duration_time);
            printf("start_time: %ums\n", pop_text->start_time);
        }
    }
#endif
    
    // NOTE(rami): Inventory
#if 0
    for(s32 i = array_count(inventory.slots) - 1; i > -1; --i)
    {
        item_t *inv_slot = &inventory.slots[i];
        if(inv_slot->id)
        {
            printf("\ninventory.slots[%u]\n", i);
            printf("id %u\n", inv_slot->id);
            printf("unique_id %u\n", inv_slot->unique_id);
            printf("x: %u, y: %u\n", inv_slot->pos.x, inv_slot->pos.y);
            printf("in_inventory %u\n", inv_slot->in_inventory);
            printf("is_equipped %u\n", inv_slot->is_equipped);
        }
    }
#endif
    
#if 0
    // NOTE(rami): Item Info
    for(s32 i = array_count(item_info) - 1; i > -1; --i)
    {
        item_info_t *info = &item_info[i];
        if(info->id)
        {
            printf("\nitem_info[%u]\n", i);
            printf("id: %u\n", info->id);
            printf("name: %s\n", info->name);
            printf("slot: %u\n", info->slot);
            printf("DESCRIPTION SKIPPED\n");
            printf("tile: %u, %u\n", info->tile.x, info->tile.y);
            
            if(info->type == type_weapon || info->type == type_armor)
            {
                printf("strength: %u\n", info->general.strength);
                printf("defence: %u\n", info->general.defence);
                printf("hp: %u\n", info->general.hp);
            }
            else if(item_info[i].type == type_consumable)
            {
                printf("effect: %u\n", info->consumable.effect);
                printf("effect_text: %s\n", info->consumable.effect_text);
                printf("effect_amount: %u\n", info->consumable.effect_amount);
            }
        }
    }
#endif
    
    // NOTE(rami): Item
#if 0
    for(s32 i = array_count(items) - 1; i > -1; --i)
    {
        item_t *item = &items[i];
        if(item->id)
        {
            printf("\nitems[%u]\n", i);
            printf("id %u\n", item->id);
            printf("unique_id %u\n", item->unique_id);
            printf("x: %u, y: %u\n", item->pos.x, item->pos.y);
            printf("in_inventory %u\n", item->in_inventory);
            printf("is_equipped %u\n", item->is_equipped);
        }
    }
#endif
    
    // NOTE(rami): Player
#if 0
    printf("\nPlayer\n");
    printf("new_x, new_y: %u, %u\n", player.new_pos.x, player.new_pos.y);
    printf("x, y: %u, %u\n", player.pos.x, player.pos.y);
    printf("w, h: %u, %u\n", player.w, player.h);
    printf("name: %s\n", player.name);
    printf("max_hp: %u\n", player.max_hp);
    printf("hp: %u\n", player.hp);
    printf("strength: %u\n", player.strength);
    printf("defence: %u\n", player.defence);
    printf("speed: %u\n", player.speed);
    printf("level: %u\n", player.level);
    printf("money: %u\n", player.money);
    printf("fov: %u\n", player.fov);
    printf("sprite_flip: %u\n", player.sprite_flip);
#endif
    
    // NOTE(rami): Monster
#if 0
    for(s32 i = array_count(monsters) - 1; i > -1; --i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            printf("\nmonster[%u]\n", i);
            printf("type: %u\n", monster->type);
            printf("ai: %u\n", monster->ai);
            
            printf("pos: %u, %u\n", monster->pos.x, monster->pos.y);
            printf("new_pos: %u, %u\n", monster->new_pos.x, monster->new_pos.y);
            printf("size: %u, %u\n", monster->w, monster->h);
            printf("in_combat: %u\n", monster->in_combat);
            printf("max_hp: %u\n", monster->max_hp);
            printf("hp: %u\n", monster->hp);
            printf("damage: %u\n", monster->damage);
            printf("armor: %u\n", monster->armor);
            printf("attack_speed: %u\n", monster->attack_speed);
            printf("move_speed: %u\n", monster->move_speed);
            printf("level: %u\n", monster->level);
        }
    }
#endif
}

internal void
run_game()
{
    initialize_player();
    generate_dungeon();
    update_fov(); // NOTE(rami): This is so that we can see without moving initially.
    
#if 0
    //add_monster(monster_python, V2u(31, 29));
    add_monster(monster_slime, V2u(31, 30));
    add_monster(monster_skeleton, V2u(31, 31));
#endif
    
#if 0
    add_item(id_knights_greaves, V2u(player.pos.x, player.pos.y + 2));
    add_item(id_ring_of_protection, V2u(player.pos.x + 1, player.pos.y + 2));
    add_item(id_iron_sword, V2u(player.pos.x, player.pos.y + 3));
    add_item(id_iron_sword, V2u(player.pos.x + 1, player.pos.y + 3));
    add_item(id_royal_dagger, V2u(player.pos.x, player.pos.y + 4));
    add_item(id_lesser_health_potion, V2u(player.pos.x + 1, player.pos.y + 4));
#endif
    
    game_input_t input[2] = {0};
    game_input_t *new_input = &input[0];
    game_input_t *old_input = &input[1];
    
    for(int i = 0; i < key_count; ++i)
    {
        old_input->keyboard[i].has_been_up = true;
    }
    
    u32 frames_per_second = 60;
    f32 target_seconds_per_frame = 1.0f / (f32)frames_per_second;
    
    u64 perf_count_frequency = SDL_GetPerformanceFrequency();
    u64 last_counter = SDL_GetPerformanceCounter();
    f32 last_dt = (f32)SDL_GetPerformanceCounter();
    
    while(game.state)
    {
        set_render_color(color_black);
        SDL_RenderClear(game.renderer);
        
#if MOONBREATH_SLOW
        array_debug();
#endif
        
        memset(new_input, 0, sizeof(game_input_t));
        for(int i = 0; i < key_count; ++i)
        {
            new_input->keyboard[i].is_down = old_input->keyboard[i].is_down;
            new_input->keyboard[i].has_been_up = old_input->keyboard[i].has_been_up;
        }
        
        process_events(new_input->keyboard);
        
        u32 mouse_state = SDL_GetMouseState(&new_input->mouse_x, &new_input->mouse_y);
        update_input(&new_input->mouse[button_left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
        update_input(&new_input->mouse[button_middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
        update_input(&new_input->mouse[button_right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
        update_input(&new_input->mouse[button_x1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
        update_input(&new_input->mouse[button_x2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
        
        f32 end_dt = (f32)SDL_GetPerformanceCounter();
        game.dt = (end_dt - last_dt) / (f32)perf_count_frequency;
        last_dt = end_dt;
        
        if(process_player_input(new_input->keyboard))
        {
            update_player(new_input->keyboard);
            update_monsters();
            update_fov();
        }
        
        update_camera();
        update_pop_text();
        
        render_tilemap();
        render_items();
        render_monsters();
        render_player();
        render_ui();
        render_pop_text();
        
        game_input_t *temp = new_input;
        new_input = old_input;
        old_input = temp;
        
        if(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency) < target_seconds_per_frame)
        {
            u32 time_to_delay =
                ((target_seconds_per_frame - seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency)) * 1000) - 1;
            SDL_Delay(time_to_delay);
            
            while(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency) < target_seconds_per_frame)
            {
            }
        }
        else
        {
            // NOTE(rami): Valgrind will trigger this!
            assert(0, "Missed frate rate");
        }
        
        u64 end_counter = SDL_GetPerformanceCounter();
        u64 elapsed_counter = end_counter - last_counter;
        
        f64 ms_per_frame = (1000.0f * (f64)elapsed_counter) / (f64)perf_count_frequency;
        f64 fps = (f64)perf_count_frequency / (f64)elapsed_counter;
        last_counter = end_counter;
        
#if MOONBREATH_SLOW
        render_text("FPS: %.02f", V2u(25, 25), color_white, fonts[font_classic_outlined], fps);
        render_text("MS Per Frame: %.02f", V2u(25, 50), color_white, fonts[font_classic_outlined], ms_per_frame);
        render_text("DT Per Frame: %.02f", V2u(25, 75), color_white, fonts[font_classic_outlined], game.dt);
        render_text("Player Pos: %u, %u", V2u(25, 125), color_white, fonts[font_classic_outlined], player.pos.x, player.pos.y);
        
        // TODO(rami): Color Tests
#if 0
        render_text("Black", V2u(25, 200), color_black, fonts[font_classic_outlined]);
        render_text("Grey", V2u(25, 225), color_gray, fonts[font_classic_outlined]);
        render_text("White", V2u(25, 250), color_white, fonts[font_classic_outlined]);
        
        render_text("Red", V2u(25, 275), color_red, fonts[font_classic_outlined]);
        render_text("Dark Red", V2u(25, 300), color_dark_red, fonts[font_classic_outlined]);
        render_text("Green", V2u(25, 325), color_green, fonts[font_classic_outlined]);
        render_text("Blue", V2u(25, 350), color_blue, fonts[font_classic_outlined]);
        
        render_text("Yellow", V2u(25, 375), color_yellow, fonts[font_classic_outlined]);
        render_text("Orange", V2u(25, 400), color_orange, fonts[font_classic_outlined]);
        render_text("Brown", V2u(25, 425), color_brown, fonts[font_classic_outlined]);
        render_text("Light Brown", V2u(25, 450), color_light_brown, fonts[font_classic_outlined]);
#endif
#endif
        
        SDL_RenderPresent(game.renderer);
    }
}

internal void
exit_game()
{
    free_assets();
    
    if(game.renderer)
    {
        SDL_DestroyRenderer(game.renderer);
        game.renderer = 0;
    }
    
    if(game.window)
    {
        SDL_DestroyWindow(game.window);
        game.window = 0;
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int
main(int argc, char *argv[])
{
    if(initialize_game())
    {
        run_game();
        exit_game();
        return(EXIT_SUCCESS);
    }
    
    exit_game();
    return(EXIT_FAILURE);
}