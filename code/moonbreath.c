#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

#include "types.h"
#include "util.c"
#include "name.c"
#include "dungeon.c"
#include "fov.c"
#include "assets.c"
#include "item.c"
#include "ui.c"
#include "pathfind.c"
#include "entity.c"
#include "debug.c"
// #include "conf.c" // TODO(rami): Work on conf when we need it again

//NOTE(rami):
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(Rami):
// 1. We want to have tiers from 1 to 10 of enemies, where each tier has a certain
// number of sensible enemies.

// 2. We want to have different specs for dungeons so that the dungeon can change
// nicely as you play, and we might want even bigger control of it through the specs.
// Like having the tiles change depending on how deep you are to create atmosphere.

// 3. We want to have the art for the items and all their tiers done and their stats.

// TODO(rami): Need to make it so that when you equip a two-handed weapon,
// a worn shield will be unequipped.

// TODO(rami): About fountains and corpses.
// The reason why you'd drink from a fountain or consume a corpse would be
// because you get something for it, otherwise it's just stuff that's useless.
// If we have the concept of food then these two things become useful.

// Another thing you could use corpses for could be sacrificing them for some gain
// on different altars, perhaps the different altars are to appease different
// deities?

// TODO(rami): Enemy corpses which you can pick up and eat if you want.
// Can have various effects depending on what was eaten.

// TODO(rami): More enemies and make sure distribution of them is good.
// TODO(rami): Give enemies stats.

// TODO(rami): Make sure distribution of items is good.
// TODO(rami): Give items stats.

// TODO(rami): After the ground work for the dungeon level layouts is done
// we can focus more on adding monsters, items, gold etc. to the levels.

// TODO(rami): We might want to keep resize_window
// if we want to be able to resize the game without restarting it.
#if 0
internal void
resize_window(game_state_t *game, u32 w, u32 h)
{
    SDL_SetWindowSize(game->window, w, h);
    game->window_size = V2u(w, h);
    game->console_size.w = game->window_size.w;
    game->camera = V4s(0, 0,
                       game->window_size.w,
                       game->window_size.h - game->console_size.h);
}
#endif

internal v4u
tile_rect(v2u pos)
{
    v4u result =
    {
        tile_mul(pos.x),
        tile_mul(pos.y),
        32,
        32
    };
    
    return(result);
}

internal v4u
game_dest(game_state_t *game, v2u pos)
{
    v4u result =
    {
        tile_mul(pos.x) - game->camera.x,
        tile_mul(pos.y) - game->camera.y,
        32,
        32
    };
    
    return(result);
}

internal b32
is_input_valid(input_state_t *state)
{
    b32 result = false;
    
#if MOONBREATH_SLOW
    if(debug_has_been_up)
    {
        if(state->is_down)
        {
            result = true;
        }
    }
    else
#endif
    {
        if(state->is_down && state->has_been_up)
        {
            state->has_been_up = false;
            result = true;
        }
    }
    
    return(result);
}

internal void
render_tilemap(game_state_t *game, dungeon_t *dungeon, assets_t *assets)
{
    SDL_SetRenderTarget(game->renderer, assets->tilemap.tex);
    SDL_RenderClear(game->renderer);
    
    v4u render_area =
    {
        tile_div(game->camera.x),
        tile_div(game->camera.y),
        tile_div(game->camera.x + game->camera.w),
        tile_div(game->camera.y + game->camera.h)
    };
    
    // NOTE(rami): If the dungeon w/h is less than
    // the w/h of the camera we can clamp the render area
    // to the w/h of the dungeon.
    if(tile_mul(dungeon->w) < game->camera.w)
    {
        render_area.w = dungeon->w - 1;
    }
    
    if(tile_mul(dungeon->h) < game->camera.h)
    {
        render_area.h = dungeon->h - 1;
    }
    
#if 0
    printf("render_area.x: %d\n", render_area.x);
    printf("render_area.y: %d\n", render_area.y);
    printf("render_area.w: %d\n", render_area.w);
    printf("render_area.h: %d\n\n", render_area.h);
#endif
    
    u32 tileset_tile_width = tile_div(assets->tileset.w);
    
    for(u32 y = render_area.y; y <= render_area.h; ++y)
    {
        for(u32 x = render_area.x; x <= render_area.w; ++x)
        {
            v2u render_pos = {x, y};
            v2u tile_pos = v2u_from_index(tile_value(dungeon->tiles, render_pos),
                                          tileset_tile_width);
            
            v4u src = tile_rect(tile_pos);
            v4u dest = tile_rect(render_pos);
            
            if(is_seen(dungeon, render_pos))
            {
                SDL_SetTextureColorMod(assets->tileset.tex, 255, 255, 255);
                SDL_RenderCopy(game->renderer, assets->tileset.tex,
                               (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                v4u_bool_t src = remains_src(dungeon, render_pos, tileset_tile_width);
                if(src.success)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex,
                                   (SDL_Rect *)&src.rect, (SDL_Rect *)&dest);
                }
            }
            else if(has_been_seen(dungeon, render_pos))
            {
                SDL_SetTextureColorMod(assets->tileset.tex, 127, 127, 127);
                SDL_RenderCopy(game->renderer, assets->tileset.tex,
                               (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                v4u_bool_t src = remains_src(dungeon, render_pos, tileset_tile_width);
                if(src.success)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex,
                                   (SDL_Rect *)&src.rect, (SDL_Rect *)&dest);
                }
            }
        }
    }
    
    SDL_SetRenderTarget(game->renderer, 0);
    
    v4u src = {game->camera.x, game->camera.y, game->camera.w, game->camera.h};
    v4u dest = {0, 0, game->camera.w, game->camera.h};
    SDL_RenderCopy(game->renderer,
                   assets->tilemap.tex,
                   (SDL_Rect *)&src,
                   (SDL_Rect *)&dest);
}

internal void
update_camera(game_state_t *game, dungeon_t *dungeon, entity_t *player)
{
    game->camera.x = tile_mul(player->pos.x) - (game->camera.w / 2);
    
    if(game->window_size.w == 1280 &&
       game->window_size.h == 720)
    {
        game->camera.y = tile_mul(player->pos.y) - (game->camera.h / 2) + 16;
    }
    else if(game->window_size.w == 1920 &&
            game->window_size.h == 1080)
    {
        game->camera.y = tile_mul(player->pos.y) - (game->camera.h / 2) + 4;
    }
    
    if(game->camera.x < 0)
    {
        game->camera.x = 0;
    }
    
    if(game->camera.y < 0)
    {
        game->camera.y = 0;
    }
    
    // NOTE(rami): if statement is so that dungeons smaller than
    // the size of the camera will be rendered properly.
    if(tile_mul(dungeon->w) >= game->camera.w)
    {
        if(game->camera.x >= (s32)(tile_mul(dungeon->w) - game->camera.w))
        {
            game->camera.x = tile_mul(dungeon->w) - game->camera.w;
        }
        
        if(game->camera.y >= (s32)(tile_mul(dungeon->h) - game->camera.h))
        {
            game->camera.y = tile_mul(dungeon->h) - game->camera.h;
        }
    }
    
#if 0
    printf("camera.x1: %d\n", game->camera.x);
    printf("camera.y1: %d\n", game->camera.y);
    printf("camera.x2: %d\n", game->camera.x + game->camera.w);
    printf("camera.y2: %d\n", game->camera.y + game->camera.h);
    
    printf("camera.w: %u\n", game->camera.w);
    printf("camera.h: %u\n", game->camera.h);
    
    printf("camera_offset.x: %u\n", tile_div(game->camera.x));
    printf("camera_offset.y: %u\n\n", tile_div(game->camera.y));
#endif
}

internal void
process_input(input_state_t *state, b32 is_down)
{
    if(state->is_down != is_down)
    {
        state->is_down = is_down;
        if(!state->is_down)
        {
            state->has_been_up = true;
        }
    }
}

internal void
process_events(game_state_t *game, input_state_t *keyboard)
{
    SDL_Event event = {0};
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game->state = game_state_exit;
        }
        else if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key_code = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(!event.key.repeat)
            {
                if(key_code == SDLK_p)
                {
                    game->state = game_state_exit;
                }
                else if(key_code == game->keybinds[key_move_up])
                {
                    process_input(&keyboard[key_move_up], is_down);
                }
                else if(key_code == game->keybinds[key_move_down])
                {
                    process_input(&keyboard[key_move_down], is_down);
                }
                else if(key_code == game->keybinds[key_move_left])
                {
                    process_input(&keyboard[key_move_left], is_down);
                }
                else if(key_code == game->keybinds[key_move_right])
                {
                    process_input(&keyboard[key_move_right], is_down);
                }
                else if(key_code == game->keybinds[key_move_up_left])
                {
                    process_input(&keyboard[key_move_up_left], is_down);
                }
                else if(key_code == game->keybinds[key_move_up_right])
                {
                    process_input(&keyboard[key_move_up_right], is_down);
                }
                else if(key_code == game->keybinds[key_move_down_left])
                {
                    process_input(&keyboard[key_move_down_left], is_down);
                }
                else if(key_code == game->keybinds[key_move_down_right])
                {
                    process_input(&keyboard[key_move_down_right], is_down);
                }
                else if(key_code == game->keybinds[key_toggle_inventory])
                {
                    process_input(&keyboard[key_toggle_inventory], is_down);
                }
                else if(key_code == game->keybinds[key_equip_or_consume_item])
                {
                    process_input(&keyboard[key_equip_or_consume_item], is_down);
                }
                else if(key_code == game->keybinds[key_pick_up_or_drop_item])
                {
                    process_input(&keyboard[key_pick_up_or_drop_item], is_down);
                }
                else if(key_code == game->keybinds[key_identify_item])
                {
                    process_input(&keyboard[key_identify_item], is_down);
                }
                else if(key_code == game->keybinds[key_move_item])
                {
                    process_input(&keyboard[key_move_item], is_down);
                }
                else if(key_code == game->keybinds[key_ascend_or_descend])
                {
                    process_input(&keyboard[key_ascend_or_descend], is_down);
                }
                else if(key_code == game->keybinds[key_wait])
                {
                    process_input(&keyboard[key_wait], is_down);
                }
                else if(key_code == game->keybinds[key_yes])
                {
                    process_input(&keyboard[key_yes], is_down);
                }
                else if(key_code == game->keybinds[key_no])
                {
                    process_input(&keyboard[key_no], is_down);
                }
#if MOONBREATH_SLOW
                else if(key_code == SDLK_F1)
                {
                    process_input(&keyboard[key_toggle_fov], is_down);
                }
                else if(key_code == SDLK_F2)
                {
                    process_input(&keyboard[key_toggle_traversable_check], is_down);
                }
                else if(key_code == SDLK_F3)
                {
                    process_input(&keyboard[key_toggle_has_been_up_check], is_down);
                }
                else if(key_code == SDLK_F4)
                {
                    process_input(&keyboard[key_toggle_identify], is_down);
                }
#endif
            }
        }
    }
}

internal u32
window_refresh_rate(game_state_t *game)
{
    u32 result = 60;
    SDL_DisplayMode mode = {0};
    
    u32 display_index = SDL_GetWindowDisplayIndex(game->window);
    if(!SDL_GetDesktopDisplayMode(display_index, &mode))
    {
        if(mode.refresh_rate)
        {
            result = mode.refresh_rate;
        }
    }
    
    return(result);
}

#if MOONBREATH_SLOW
internal void
array_debug(item_t *items)
{
#if 0
    for(u32 i = MAX_ITEMS - 1;
        i < MAX_ITEMS;
        --i)
    {
        item_t *item = &items[i];
        if(item->id)
        {
            printf("\n\nitems[%u]\n", i);
            
            printf("id: %u\n", item->id);
            printf("name: %s\n", item->name);
            
            printf("description: %s\n", item->description);
            printf("pos.x, pos.y: %u, %u\n", item->pos.x, item->pos.y);
            printf("tile.x, tile.y: %u, %u\n", item->tile.x, item->tile.y);
            
            printf("rarity: %u\n", item->rarity);
            printf("slot: %u\n", item->slot);
            printf("handedness: %u\n", item->handedness);
            printf("primary_damage_type: %u\n", item->primary_damage_type);
            printf("secondary_damage_type: %u\n", item->secondary_damage_type);
            printf("enchantment_level: %d\n", item->enchantment_level);
            
            printf("type: %u\n", item->type);
            switch(item->type)
            {
                case item_type_weapon:
                {
                    printf("damage: %d\n", item->w.damage);
                    printf("accuracy: %d\n", item->w.accuracy);
                    printf("attack_speed: %.01f\n", item->w.attack_speed);
                } break;
                
                case item_type_armor:
                {
                    printf("defence: %d\n", item->a.defence);
                    printf("weight: %d\n", item->a.weight);
                } break;
                
                case item_type_potion:
                case item_type_scroll:
                {
                    printf("effect: %u\n", item->c.effect);
                    printf("effect_amount: %u\n", item->c.effect_amount);
                } break;
            }
            
            printf("in_inventory: %u\n", item->in_inventory);
            printf("in_identified: %u\n", item->is_identified);
            printf("is_equipped: %u\n", item->is_equipped);
            printf("is_cursed: %u\n", item->is_cursed);
        }
    }
#endif
}
#endif

int
main(int argc, char *argv[])
{
    u32 result = 0;
    
    // TODO(rami): Adjust array and #define sizes.
    // NOTE(Rami): Everything has to be initialized to zero.
    game_state_t game = {0};
    assets_t assets = {0};
    entity_t entities[MAX_ENTITIES] = {0};
    u32 enemy_levels[entity_count] = {0};
    entity_t *player = &entities[0];
    dungeon_t dungeon = {0};
    dungeon.tiles.array = calloc(1, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(tile_t));
    inventory_t inventory = {0};
    item_t items[MAX_ITEMS] = {0};
    string_t log[MAX_LOG_STRINGS] = {0};
    
    // TODO(rami): The keybinds and resolution would come from a config file.
    if(1)
    {
        game.window_size = V2u(1280, 720);
    }
    else
    {
        game.window_size = V2u(1920, 1080);
    }
    
    game.keybinds[key_move_up] = 'w';
    game.keybinds[key_move_down] = 's';
    game.keybinds[key_move_left] = 'a';
    game.keybinds[key_move_right] = 'd';
    
    game.keybinds[key_move_up_left] = 'q';
    game.keybinds[key_move_up_right] = 'e';
    game.keybinds[key_move_down_left] = 'z';
    game.keybinds[key_move_down_right] = 'c';
    
    game.keybinds[key_toggle_inventory] = 'i';
    game.keybinds[key_equip_or_consume_item] = 'n';
    game.keybinds[key_pick_up_or_drop_item] = ',';
    game.keybinds[key_identify_item] = '.';
    game.keybinds[key_move_item] = 'm';
    game.keybinds[key_ascend_or_descend] = 'b';
    game.keybinds[key_wait] = 'v';
    game.keybinds[key_yes] = 'h';
    game.keybinds[key_no] = 'j';
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        u32 window_flags = SDL_WINDOW_SHOWN;
        game.window = SDL_CreateWindow("Moonbreath",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       game.window_size.w,
                                       game.window_size.h,
                                       window_flags);
        if(game.window)
        {
            printf("Monitor refresh rate: %uHz\n\n", window_refresh_rate(&game));
            
            u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
            game.renderer = SDL_CreateRenderer(game.window, -1, renderer_flags);
            if(game.renderer)
            {
                u32 img_flags = IMG_INIT_PNG;
                if(IMG_Init(img_flags) & img_flags)
                {
                    if(!TTF_Init())
                    {
                        if(initialize_assets(&game, &assets))
                        {
#if 0
                            u64 seed = time(0);
#else
                            u64 seed = 1587001144;
#endif
                            printf("Seed: %lu\n", seed);
                            
                            game.random = set_random_seed(seed);
                            game.state = game_state_in_game;
                            
                            game.camera = V4s(0, 0,
                                              game.window_size.w,
                                              game.window_size.h - assets.log_window.h);
                            
                            u32 target_fps = 60;
                            f32 target_seconds_per_frame = 1.0f / (f32)target_fps;
                            
                            u64 perf_count_frequency = SDL_GetPerformanceFrequency();
                            u64 last_counter = SDL_GetPerformanceCounter();
                            f32 last_dt = (f32)SDL_GetPerformanceCounter();
                            
                            game_input_t input[2] = {0};
                            game_input_t *new_input = &input[0];
                            game_input_t *old_input = &input[1];
                            
#if MOONBREATH_SLOW
                            f32 actual_fps = 0.0f;
                            f32 actual_seconds_per_frame = 0.0f;
                            f32 work_seconds_per_frame = 0.0f;
                            
                            debug_state_t debug_state = {0};
                            debug_state.selected_group_index = 1;
                            
                            debug_group_t *debug_variables = add_debug_group(&debug_state, "Variables", 25, 25, assets.fonts[font_classic_outlined]);
                            add_debug_float32(debug_variables, "FPS", &actual_fps);
                            add_debug_float32(debug_variables, "Frame MS", &actual_seconds_per_frame);
                            add_debug_float32(debug_variables, "Work MS", &work_seconds_per_frame);
                            add_debug_float32(debug_variables, "Frame DT", &new_input->dt);
                            add_debug_uint32(debug_variables, "Mouse X", &new_input->mouse_pos.x);
                            add_debug_uint32(debug_variables, "Mouse Y", &new_input->mouse_pos.y);
                            
                            v2u mouse_final = {0};
                            add_debug_uint32(debug_variables, "Mouse Tile X", &mouse_final.x);
                            add_debug_uint32(debug_variables, "Mouse Tile Y", &mouse_final.y);
                            
                            add_debug_uint32(debug_variables, "Player Tile X", &player->pos.x);
                            add_debug_uint32(debug_variables, "Player Tile Y", &player->pos.y);
                            add_debug_bool32(debug_variables, "Debug Fov", &debug_fov);
                            add_debug_bool32(debug_variables, "Debug Traversable", &debug_traversable);
                            add_debug_bool32(debug_variables, "Debug Has Been Up", &debug_has_been_up);
                            
                            debug_group_t *debug_colors = add_debug_group(&debug_state, "Colors", 150, 25, assets.fonts[font_classic_outlined]);
                            add_debug_text(debug_colors, "White");
                            
                            add_debug_text(debug_colors, "##2 Light Gray");
                            add_debug_text(debug_colors, "##3 Dark Gray");
                            
                            add_debug_text(debug_colors, "##4 Light Red");
                            add_debug_text(debug_colors, "##5 Dark Red");
                            
                            add_debug_text(debug_colors, "##6 Light Green");
                            add_debug_text(debug_colors, "##7 Dark Green");
                            
                            add_debug_text(debug_colors, "##8 Light Blue");
                            add_debug_text(debug_colors, "##9 Dark Blue");
                            
                            add_debug_text(debug_colors, "##A Light Brown");
                            add_debug_text(debug_colors, "##B Dark Brown");
                            
                            add_debug_text(debug_colors, "##C Cyan");
                            add_debug_text(debug_colors, "##D Yellow");
                            add_debug_text(debug_colors, "##E Purple");
                            add_debug_text(debug_colors, "##F Orange");
#endif
                            
                            for(u32 button_index = 0;
                                button_index < button_count;
                                ++button_index)
                            {
                                old_input->mouse[button_index].has_been_up = true;
                            }
                            
                            for(u32 key_index = 0;
                                key_index < key_count;
                                ++key_index)
                            {
                                old_input->keyboard[key_index].has_been_up = true;
                            }
                            
                            while(game.state)
                            {
#if MOONBREATH_SLOW
                                array_debug(items);
#endif
                                set_render_color(&game, color_black);
                                SDL_RenderClear(game.renderer);
                                
                                memset(new_input, 0, sizeof(game_input_t));
                                
                                for(u32 button_index = 0;
                                    button_index < button_count;
                                    ++button_index)
                                {
                                    new_input->mouse[button_index].is_down = old_input->mouse[button_index].is_down;
                                    new_input->mouse[button_index].has_been_up = old_input->mouse[button_index].has_been_up;
                                }
                                
                                for(u32 key_index = 0;
                                    key_index < key_count;
                                    ++key_index)
                                {
                                    new_input->keyboard[key_index].is_down = old_input->keyboard[key_index].is_down;
                                    new_input->keyboard[key_index].has_been_up = old_input->keyboard[key_index].has_been_up;
                                }
                                
                                process_events(&game, new_input->keyboard);
                                
                                u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                process_input(&new_input->mouse[button_left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                process_input(&new_input->mouse[button_middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                process_input(&new_input->mouse[button_right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                process_input(&new_input->mouse[button_x1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                process_input(&new_input->mouse[button_x2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->dt = (end_dt - last_dt) / (f32)perf_count_frequency;
                                last_dt = end_dt;
                                
                                // NOTE(Rami): Update And Render Game
                                if(game.state == game_state_main_menu)
                                {
                                    set_render_color(&game, color_cyan);
                                    v4u rect = {50, 300, 200, 100};
                                    SDL_RenderFillRect(game.renderer, (SDL_Rect *)&rect);
                                    
                                    if(is_inside_rectangle(new_input->mouse_pos, rect))
                                    {
                                        render_text(&game, "##D New Game", 100, 340, assets.fonts[font_classic_outlined], color_white);
                                        
                                        if(is_input_valid(&new_input->mouse[button_left]))
                                        {
                                            game.state = game_state_in_game;
                                        }
                                    }
                                    else
                                    {
                                        render_text(&game, "New Game", 100, 340, assets.fonts[font_classic_outlined], color_white);
                                    }
                                }
                                else if(game.state == game_state_in_game)
                                {
                                    // TODO(rami): When we go back to the main menu
                                    // if the player wins or dies, we need to set game.is_initialized to false.
                                    if(!game.is_initialized)
                                    {
                                        enemy_levels[entity_skeleton] = 1; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_cave_bat] = 1; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_slime] = 1; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_rat] = 1; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_snail] = 1; // NOTE(Rami): Art is ok.
                                        
                                        enemy_levels[entity_dog] = 2; // NOTE(Rami): Ok.
                                        enemy_levels[entity_giant_slime] = 2; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_skeleton_warrior] = 2; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_goblin] = 2; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_python] = 2; // NOTE(Rami): Ok.
                                        
                                        enemy_levels[entity_orc_warrior] = 3; // NOTE(Rami): Art is ok.
                                        enemy_levels[entity_assassin] = 3;
                                        enemy_levels[entity_kobold] = 3;
                                        enemy_levels[entity_ghoul] = 3;
                                        enemy_levels[entity_centaur] = 3;
                                        
                                        enemy_levels[entity_imp] = 4;
                                        enemy_levels[entity_floating_eye] = 4;
                                        enemy_levels[entity_undead_elf_warrior] = 4;
                                        enemy_levels[entity_viper] = 4;
                                        enemy_levels[entity_frost_walker] = 4;
                                        
                                        enemy_levels[entity_dwarwen_warrior] = 5;
                                        enemy_levels[entity_minotaur] = 5;
                                        enemy_levels[entity_tormentor] = 5;
                                        enemy_levels[entity_treant] = 5;
                                        
                                        enemy_levels[entity_devourer] = 6;
                                        enemy_levels[entity_wolf] = 6;
                                        enemy_levels[entity_centaur_warrior] = 6;
                                        
                                        enemy_levels[entity_brimstone_imp] = 7;
                                        enemy_levels[entity_spectre] = 7;
                                        enemy_levels[entity_flying_skull] = 7;
                                        
                                        enemy_levels[entity_hellhound] = 8;
                                        enemy_levels[entity_black_knight] = 8;
                                        enemy_levels[entity_giant_demon] = 8;
                                        
                                        enemy_levels[entity_cursed_black_knight] = 9;
                                        enemy_levels[entity_scarlet_kingsnake] = 9;
                                        
                                        enemy_levels[entity_griffin] = 10;
                                        enemy_levels[entity_ogre] = 10;
                                        enemy_levels[entity_cyclops] = 10;
                                        
                                        // TODO(Rami): ?
                                        enemy_levels[entity_frost_shards] = 0;
                                        enemy_levels[entity_green_mamba] = 0;
                                        
                                        add_player_entity(player);
                                        create_dungeon(&game, &dungeon, player, entities, items, enemy_levels);
                                        update_fov(&dungeon, player);
                                        
                                        add_weapon_item(&game, items, item_dagger, item_rarity_common, player->pos.x - 1, player->pos.y - 1);
                                        add_weapon_item(&game, items, item_sword, item_rarity_common, player->pos.x - 1, player->pos.y);
                                        add_scroll_item(items, item_scroll_of_identify, player->pos.x - 1, player->pos.y + 1);
                                        //add_scroll_item(items, item_scroll_of_infuse_weapon, player->pos.x - 1, player->pos.y);
                                        //add_scroll_item(items, item_scroll_of_enchant_weapon, player->pos.x - 1, player->pos.y - 1);
                                        //add_scroll_item(items, item_scroll_of_enchant_armor, player->pos.x - 1, player->pos.y - 2);
                                        //add_scroll_item(items, item_scroll_of_magic_mapping, player->pos.x - 1, player->pos.y - 3);
                                        
                                        //add_potion_item(items, item_potion_of_healing, player->pos.x - 1, player->pos.y);
                                        
                                        //add_enemy_entity(entities, &dungeon, enemy_levels, entity_id_slime, player->pos.x - 1, player->pos.y);
#if 0
                                        add_weapon_item(&game, items, item_dagger, item_rarity_common, player->pos.x + 1, player->pos.y);
                                        add_weapon_item(&game, items, item_dagger, item_rarity_magical, player->pos.x + 1, player->pos.y + 1);
                                        add_weapon_item(&game, items, item_dagger, item_rarity_mythical, player->pos.x + 1, player->pos.y + 2);
                                        
                                        add_weapon_item(&game, items, item_sword, item_rarity_common, player->pos.x + 2, player->pos.y);
                                        add_weapon_item(&game, items, item_sword, item_rarity_magical, player->pos.x + 2, player->pos.y + 1);
                                        add_weapon_item(&game, items, item_sword, item_rarity_mythical, player->pos.x + 2, player->pos.y + 2);
                                        
                                        add_weapon_item(&game, items, item_scimitar, item_rarity_common, player->pos.x + 3, player->pos.y);
                                        add_weapon_item(&game, items, item_scimitar, item_rarity_magical, player->pos.x + 3, player->pos.y + 1);
                                        add_weapon_item(&game, items, item_scimitar, item_rarity_mythical, player->pos.x + 3, player->pos.y + 2);
                                        
                                        add_weapon_item(&game, items, item_club, item_rarity_common, player->pos.x + 4, player->pos.y);
                                        add_weapon_item(&game, items, item_club, item_rarity_magical, player->pos.x + 4, player->pos.y + 1);
                                        add_weapon_item(&game, items, item_club, item_rarity_mythical, player->pos.x + 4, player->pos.y + 2);
                                        
                                        add_weapon_item(&game, items, item_morningstar, item_rarity_common, player->pos.x + 5, player->pos.y);
                                        add_weapon_item(&game, items, item_morningstar, item_rarity_magical, player->pos.x + 5, player->pos.y + 1);
                                        add_weapon_item(&game, items, item_morningstar, item_rarity_mythical, player->pos.x + 5, player->pos.y + 2);
#endif
                                        
                                        game.is_initialized = true;
                                    }
                                    
                                    update_entities(&game, new_input->keyboard, player, entities, &dungeon, items, log, &inventory, enemy_levels);
                                    update_camera(&game, &dungeon, player);
                                    
                                    render_tilemap(&game, &dungeon, &assets);
                                    render_items(&game, &dungeon, items, &assets);
                                    render_entities(&game, &dungeon, player, entities, &inventory, &assets);
                                    render_ui(&game, &dungeon, player, log, &inventory, &assets);
                                    
#if 0
                                    // TODO(Rami): Find out which room index
                                    // the player is in.
                                    for(u32 room_index = 0;
                                        room_index < dungeon.rooms.count;
                                        ++room_index)
                                    {
                                        if(player->pos.x >= dungeon.rooms.array[room_index].x &&
                                           player->pos.x <= dungeon.rooms.array[room_index].x + dungeon.rooms.array[room_index].w - 1 &&
                                           player->pos.y >= dungeon.rooms.array[room_index].y &&
                                           player->pos.y <= dungeon.rooms.array[room_index].y + dungeon.rooms.array[room_index].h - 1)
                                        {
                                            if(is_tile_floor(dungeon.tiles, player->pos))
                                            {
                                                printf("Player in room index: %u\n", room_index);
                                                break;
                                            }
                                        }
                                    }
#endif
                                    
#if MOONBREATH_SLOW
                                    v2u selection =
                                    {
                                        tile_div(new_input->mouse_pos.x),
                                        tile_div(new_input->mouse_pos.y)
                                    };
                                    
                                    v2u camera_offset =
                                    {
                                        tile_div(game.camera.x),
                                        tile_div(game.camera.y)
                                    };
                                    
                                    v4u rect = tile_rect(selection);
                                    
                                    // NOTE(Rami): Logical result:
                                    mouse_final.x = selection.x + camera_offset.x;
                                    mouse_final.y = selection.y + camera_offset.y;
                                    
                                    if(selection.y < tile_div(game.camera.h))
                                    {
                                        set_render_color(&game, color_yellow);
                                        SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&rect);
                                    }
#endif
                                }
                                
#if MOONBREATH_SLOW
                                u64 work_counter = SDL_GetPerformanceCounter();
                                u64 work_elapsed_counter = work_counter - last_counter;
                                work_seconds_per_frame = (1000.0f * (f64)work_elapsed_counter) / (f64)perf_count_frequency;
#endif
                                
                                if(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency) < target_seconds_per_frame)
                                {
                                    u32 time_to_delay =
                                        ((target_seconds_per_frame - seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency)) * 1000);
                                    SDL_Delay(time_to_delay);
                                    
                                    while(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), perf_count_frequency) < target_seconds_per_frame)
                                    {
                                    }
                                }
                                else
                                {
                                    // NOTE(rami): Missed frame rate.
                                }
                                
                                u64 end_counter = SDL_GetPerformanceCounter();
                                u64 elapsed_counter = end_counter - last_counter;
                                last_counter = end_counter;
                                
#if MOONBREATH_SLOW
                                actual_fps = (f64)perf_count_frequency / (f64)elapsed_counter;
                                actual_seconds_per_frame = (1000.0f * (f64)elapsed_counter) / (f64)perf_count_frequency;
                                
                                update_and_render_debug_state(&game, new_input, &debug_state);
#endif
                                
                                game_input_t *temp = new_input;
                                new_input = old_input;
                                old_input = temp;
                                
                                SDL_RenderPresent(game.renderer);
                            }
                        }
                    }
                    else
                    {
                        // TODO(rami): Logging
                        printf("ERROR: TTF_Init(): %s\n", SDL_GetError());
                    }
                }
                else
                {
                    // TODO(rami): Logging
                    printf("ERROR: IMG_Init(): %s\n",SDL_GetError());
                }
            }
            else
            {
                // TODO(rami): Logging
                printf("ERROR: SDL_CreateRenderer(): %s\n", SDL_GetError());
            }
        }
        else
        {
            // TODO(rami): Logging
            printf("ERROR: SDL_CreateWindow(): %s\n", SDL_GetError());
        }
    }
    else
    {
        // TODO(rami): Logging
        printf("ERROR: SDL_Init(): %s\n", SDL_GetError());
    }
    
    // NOTE(Rami): Exit Game
    
    free_assets(&assets);
    free(dungeon.tiles.array);
    
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return(result);
}
