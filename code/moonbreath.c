#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

#include "types.h"
#include "util.c"
#include "name.c"
#include "fov.c"
#include "dungeon.c"
#include "pathfind.c"
#include "assets.c"
#include "item.c"
#include "entity.c"
#include "ui.c"
#include "debug.c"
#include "config.c"

// TODO(rami):
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it?
// Simplify it?
// Pull things into reusable functions? (Compression Oriented)

// Adjust array and #define sizes.
// Log window.
// Level overview.
// Don't check NULL pointers that are never NULL.
// Do we need potion stacking?
// If you equip a two-handed weapon, unequip shield.

internal v2u
get_direction_pos(v2u pos, Direction direction)
{
    v2u result = {0};
    
    switch(direction)
    {
        case Direction_None: result = pos; break;
        
        case Direction_Up: result = make_v2u(pos.x, pos.y - 1); break;
        case Direction_Down: result = make_v2u(pos.x, pos.y + 1); break;
        case Direction_Left: result = make_v2u(pos.x - 1, pos.y); break;
        case Direction_Right: result = make_v2u(pos.x + 1, pos.y); break;
        
        case Direction_UpLeft: result = make_v2u(pos.x - 1, pos.y - 1); break;
        case Direction_UpRight: result = make_v2u(pos.x + 1, pos.y - 1); break;
        case Direction_DownLeft: result = make_v2u(pos.x - 1, pos.y + 1); break;
        case Direction_DownRight: result = make_v2u(pos.x + 1, pos.y + 1); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal Direction
get_random_direction(RandomState *random)
{
    Direction result = random_number(random, Direction_Up, Direction_DownRight);
    return(result);
}

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
game_dest(GameState *game, v2u pos)
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

internal void
render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped)
{
    SDL_SetTextureColorMod(texture, 127, 127, 127);
    SDL_RenderCopyEx(renderer, texture, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, is_flipped);
    SDL_SetTextureColorMod(texture, 255, 255, 255);
}

internal void
render_tilemap(GameState *game, Dungeon *dungeon, Assets *assets)
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
    
    // If the dungeon w/h is less than the camera w/h then
    // we can clamp the render area to the dungeon w/h.
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
            v2u tile_pos = v2u_from_index(tile_id(dungeon->tiles, render_pos), tileset_tile_width);
            
            v4u src = tile_rect(tile_pos);
            v4u dest = tile_rect(render_pos);
            
            if(tile_is_seen(dungeon->tiles, render_pos))
            {
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                RemainsSource src = get_tile_remains_src(dungeon, render_pos, tileset_tile_width);
                if(src.found)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src.rect, (SDL_Rect *)&dest);
                }
            }
            else if(tile_has_been_seen(dungeon->tiles, render_pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, false);
                
                RemainsSource src = get_tile_remains_src(dungeon, render_pos, tileset_tile_width);
                if(src.found)
                {
                    render_texture_half_color(game->renderer, assets->tileset.tex, src.rect, dest, false);
                }
            }
        }
    }
    
    SDL_SetRenderTarget(game->renderer, 0);
    
    v4u src = {game->camera.x, game->camera.y, game->camera.w, game->camera.h};
    v4u dest = {0, 0, game->camera.w, game->camera.h};
    SDL_RenderCopy(game->renderer, assets->tilemap.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
}

internal void
update_camera(GameState *game, Dungeon *dungeon, Entity *player)
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
    
    // if statement is so dungeons smaller than the camera work properly.
    if(tile_mul(dungeon->w) >= game->camera.w)
    {
        s32 camera_max_x = (s32)(tile_mul(dungeon->w) - game->camera.w);
        s32 camera_max_y = (s32)(tile_mul(dungeon->h) - game->camera.h);
        
        // Clamp the camera if we get close enough to the map right/bottom edge.
        if(game->camera.x >= camera_max_x)
        {
            game->camera.x = camera_max_x;
        }
        
        if(game->camera.y >= camera_max_y)
        {
            game->camera.y = camera_max_y;
        }
    }
    
#if 0
    printf("camera.x: %d\n", game->camera.x);
    printf("camera.y: %d\n", game->camera.y);
    printf("camera.w: %u\n", game->camera.w);
    printf("camera.h: %u\n\n", game->camera.h);
    
    printf("camera.x2: %d\n", game->camera.x + game->camera.w);
    printf("camera.y2: %d\n\n", game->camera.y + game->camera.h);
    
    printf("camera_tile_offset_x: %u\n", tile_div(game->camera.x));
    printf("camera_tile_offset_y: %u\n\n", tile_div(game->camera.y));
#endif
}

internal b32
was_pressed(InputState *state, b32 *fkey_active)
{
    b32 result = false;
    
    if(state->ended_down)
    {
#if MOONBREATH_SLOW
        if(fkey_active[3])
        {
            result = true;
        }
#endif
        
        if(state->has_been_up)
        {
            state->has_been_up = false;
            result = true;
        }
    }
    
    return(result);
}

internal void
process_input(InputState *state, b32 is_down)
{
    if(state->ended_down != is_down)
    {
        state->ended_down = is_down;
        
        if(!state->ended_down)
        {
            state->has_been_up = true;
        }
    }
}

internal void
process_events(GameState *game, GameInput *input)
{
    SDL_Event event = {0};
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game->mode = GameMode_Quit;
        }
        else if(event.type == SDL_KEYUP ||
                event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key_code = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(!event.key.repeat)
            {
                for(u32 index = 0; index < Key_Count; ++index)
                {
                    if(key_code == SDLK_p)
                    {
                        game->mode = GameMode_Quit;
                    }
                    else if(key_code == game->keybinds[index])
                    {
                        process_input(&input->keyboard[index], is_down);
                    }
                    
#if MOONBREATH_SLOW
                    else if(key_code == SDLK_F1)
                    {
                        process_input(&input->fkeys[1], is_down);
                    }
                    else if(key_code == SDLK_F2)
                    {
                        process_input(&input->fkeys[2], is_down);
                    }
                    else if(key_code == SDLK_F3)
                    {
                        process_input(&input->fkeys[3], is_down);
                    }
                    else if(key_code == SDLK_F4)
                    {
                        process_input(&input->fkeys[4], is_down);
                    }
                    else if(key_code == SDLK_F5)
                    {
                        process_input(&input->fkeys[5], is_down);
                    }
#endif
                }
            }
        }
    }
}

internal void
update_and_render_game(GameState *game,
                       GameInput *input,
                       Dungeon *dungeon,
                       Entity *player,
                       Entity *entities,
                       String128 *log,
                       Item *items,
                       Inventory *inventory,
                       Assets *assets,
                       ItemInfo *item_info,
                       u32 *enemy_levels)
{
    if(game->mode == GameMode_MainMenu)
    {
        set_render_color(game, Color_Cyan);
        v4u rect = {50, 300, 200, 100};
        SDL_RenderFillRect(game->renderer, (SDL_Rect *)&rect);
        
        if(is_inside_rect(rect, input->mouse_pos))
        {
            render_text(game, "%sNew Game", 100, 340, assets->fonts[FontName_ClassicOutlined], 0, start_color(Color_Yellow));
            
            if(was_pressed(&input->mouse[Button_Left], input->fkey_active))
            {
                game->mode = GameMode_Game;
            }
        }
        else
        {
            render_text(game, "New Game", 100, 340, assets->fonts[FontName_ClassicOutlined], 0);
        }
    }
    else if(game->mode == GameMode_Game)
    {
        // TODO(rami): When we go back to the main menu
        // if the player wins or dies, we need to set game.is_initialized to false.
        if(!game->is_initialized)
        {
            b32 is_potion_color_set[Potion_Count] = {0};
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                while(!item_info->potion_tiles[index].x &&
                      !item_info->potion_tiles[index].y)
                {
                    u32 potion_index = random_number(&game->random, 0, Potion_Count - 1);
                    if(!is_potion_color_set[potion_index])
                    {
                        switch(potion_index)
                        {
                            case 0: item_info->potion_tiles[index] = make_v2u(8, 1); break;
                            case 1: item_info->potion_tiles[index] = make_v2u(8, 2); break;
                            case 2: item_info->potion_tiles[index] = make_v2u(8, 3); break;
                            case 3: item_info->potion_tiles[index] = make_v2u(8, 4); break;
                            case 4: item_info->potion_tiles[index] = make_v2u(8, 5); break;
                            case 5: item_info->potion_tiles[index] = make_v2u(8, 6); break;
                            case 6: item_info->potion_tiles[index] = make_v2u(8, 7); break;
                            case 7: item_info->potion_tiles[index] = make_v2u(8, 8); break;
                            case 8: item_info->potion_tiles[index] = make_v2u(8, 9); break;
                            case 9: item_info->potion_tiles[index] = make_v2u(8, 10); break;
                            case 10: item_info->potion_tiles[index] = make_v2u(8, 11); break;
                            case 11: item_info->potion_tiles[index] = make_v2u(8, 12); break;
                            case 12: item_info->potion_tiles[index] = make_v2u(8, 13); break;
                            case 13: item_info->potion_tiles[index] = make_v2u(8, 14); break;
                            
                            invalid_default_case;
                        }
                        
                        is_potion_color_set[potion_index] = true;
                        break;
                    }
                }
            }
            
            b32 is_scroll_color_set[Scroll_Count] = {0};
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                while(!item_info->scroll_tiles[index].x &&
                      !item_info->scroll_tiles[index].y)
                {
                    u32 scroll_index = random_number(&game->random, 0, Scroll_Count - 1);
                    if(!is_scroll_color_set[scroll_index])
                    {
                        switch(scroll_index)
                        {
                            case 0: item_info->scroll_tiles[index] = make_v2u(9, 1); break;
                            case 1: item_info->scroll_tiles[index] = make_v2u(9, 2); break;
                            case 2: item_info->scroll_tiles[index] = make_v2u(9, 3); break;
                            case 3: item_info->scroll_tiles[index] = make_v2u(9, 4); break;
                            case 4: item_info->scroll_tiles[index] = make_v2u(9, 5); break;
                            
                            invalid_default_case;
                        }
                        
                        is_scroll_color_set[scroll_index] = true;
                        break;
                    }
                }
            }
            
#if 0
            // Print randomized potion and scroll tiles.
            printf("\nRandomized Potion Tiles\n");
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                printf("[%u]: %u, %u\n", index,
                       item_info->potion_tiles[index].x,
                       item_info->potion_tiles[index].y);
            }
            
            printf("\nRandomized Scroll Tiles\n");
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                printf("[%u]: %u, %u\n", index,
                       item_info->scroll_tiles[index].x,
                       item_info->scroll_tiles[index].y);
            }
#endif
            
            enemy_levels[EntityID_Dummy] = 0;
            
            enemy_levels[EntityID_SkeletonWarrior] = 1;
            enemy_levels[EntityID_SkeletonArcher] = 1;
            enemy_levels[EntityID_SkeletonMage] = 1;
            enemy_levels[EntityID_Bat] = 1;
            enemy_levels[EntityID_Rat] = 1;
            
            enemy_levels[EntityID_KoboldWarrior] = 2;
            enemy_levels[EntityID_KoboldShaman] = 2;
            enemy_levels[EntityID_Snail] = 2;
            enemy_levels[EntityID_Slime] = 2;
            enemy_levels[EntityID_Dog] = 2;
            
            enemy_levels[EntityID_OrcWarrior] = 3;
            enemy_levels[EntityID_OrcArcher] = 3;
            enemy_levels[EntityID_OrcShaman] = 3;
            enemy_levels[EntityID_Python] = 3;
            enemy_levels[EntityID_Shade] = 3;
            
            enemy_levels[EntityID_ElfKnight] = 4;
            enemy_levels[EntityID_ElfArbalest] = 4;
            enemy_levels[EntityID_ElfMage] = 4;
            enemy_levels[EntityID_GiantSlime] = 4;
            enemy_levels[EntityID_Spectre] = 4;
            
            enemy_levels[EntityID_OrcSorcerer] = 5;
            enemy_levels[EntityID_OrcAssassin] = 5;
            enemy_levels[EntityID_Minotaur] = 5;
            enemy_levels[EntityID_Treant] = 5;
            enemy_levels[EntityID_Viper] = 5;
            
            enemy_levels[EntityID_CentaurWarrior] = 6;
            enemy_levels[EntityID_CentaurSpearman] = 6;
            enemy_levels[EntityID_CentaurArcher] = 6;
            enemy_levels[EntityID_CursedSkull] = 6;
            enemy_levels[EntityID_Wolf] = 6;
            
            enemy_levels[EntityID_OgreWarrior] = 7;
            enemy_levels[EntityID_OgreArcher] = 7;
            enemy_levels[EntityID_OgreMage] = 7;
            enemy_levels[EntityID_Cyclops] = 7;
            enemy_levels[EntityID_ShadowWalker] = 7;
            
            enemy_levels[EntityID_DwarwenWarrior] = 8;
            enemy_levels[EntityID_DwarwenSorcerer] = 8;
            enemy_levels[EntityID_DwarwenPriest] = 8;
            enemy_levels[EntityID_ScarletSnake] = 8;
            enemy_levels[EntityID_Lich] = 8;
            
            enemy_levels[EntityID_AbyssalFiend] = 9;
            enemy_levels[EntityID_BloodTroll] = 9;
            enemy_levels[EntityID_IronGolem] = 9;
            enemy_levels[EntityID_Griffin] = 9;
            enemy_levels[EntityID_Imp] = 9;
            
            enemy_levels[EntityID_BlackKnight] = 10;
            enemy_levels[EntityID_GiantDemon] = 10;
            enemy_levels[EntityID_Hellhound] = 10;
            enemy_levels[EntityID_AbyssalHexmaster] = 10;
            enemy_levels[EntityID_Mahjarrat] = 10;
            
            add_player_entity(&game->random, player, items, inventory);
            create_dungeon(&game->random, dungeon, player, log, entities, items, inventory, item_info, enemy_levels);
            update_fov(dungeon, player, input->fkey_active);
            
            game->is_initialized = true;
        }
        
        // TODO(rami): Bundle?
        update_entities(game, input, player, entities, dungeon, items, item_info, log, inventory, enemy_levels);
        update_camera(game, dungeon, player);
        
        render_tilemap(game, dungeon, assets);
        render_items(game, dungeon, items, assets);
        render_entities(game, dungeon, player, entities, inventory, assets);
        render_ui(game, dungeon, player, log, inventory, assets);
    }
}

int main(int argc, char *argv[])
{
    u32 result = 0;
    
    // All game data is required to be initialized to zero.
    GameState game = {0};
    Assets assets = {0};
    Entity *entities = calloc(1, MAX_ENTITY_COUNT * sizeof(Entity));
    Entity *player = &entities[0];
    u32 enemy_levels[EntityID_Count] = {0};
    Dungeon dungeon = {0};
    dungeon.tiles.array = calloc(1, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(Tile));
    Inventory inventory = {0};
    Item items[MAX_ITEM_COUNT] = {0};
    ItemInfo item_info = {0};
    String128 log[MAX_LOG_ENTRY_COUNT] = {0};
    
    Config config = get_config("data/config.txt");
    game.show_item_ground_outline = true;
    
    // TODO(rami): Need to check success on everything.
    ConfigValue window_size = config_uint(&config, "window_size");
    if(!window_size.success) {assert(0);}
    
    if(window_size.uint == 1)
    {
        game.window_size = make_v2u(1920, 1080);
    }
    else if(window_size.uint == 2)
    {
        game.window_size = make_v2u(1280, 720);
    }
    
    for(u32 index = 0; index < Key_Count; ++index)
    {
        char *token_name = 0;
        
        switch(index)
        {
            case Key_Up: token_name = "key_up"; break;
            case Key_Down: token_name = "key_down"; break;
            case Key_Left: token_name = "key_left"; break;
            case Key_Right: token_name = "key_right"; break;
            
            case Key_UpLeft: token_name = "key_up_left"; break;
            case Key_UpRight: token_name = "key_up_right"; break;
            case Key_DownLeft: token_name = "key_down_left"; break;
            case Key_DownRight: token_name = "key_down_right"; break;
            
            case Key_InventoryToggle: token_name = "key_inventory_toggle"; break;
            case Key_InventoryAction: token_name = "key_inventory_action"; break;
            case Key_InventoryMove: token_name = "key_inventory_move"; break;
            case Key_PickupOrDrop: token_name = "key_pickup_or_drop"; break;
            case Key_AscendOrDescend: token_name = "key_ascend_or_descend"; break;
            case Key_Wait: token_name = "key_wait"; break;
            case Key_Yes: token_name = "key_yes"; break;
            case Key_No: token_name = "key_no"; break;
            
            invalid_default_case;
        }
        
        ConfigValue value = config_char(&config, token_name);
        if(value.success)
        {
            game.keybinds[index] = value.c;
        }
        else
        {
            printf("ERROR: Could not get config chararacter for game.keybinds[%u].\n", index);
            assert(0);
        }
    }
    
#if 0
    game.keybinds[Key_Up] = 'w';
    game.keybinds[Key_Down] = 's';
    game.keybinds[Key_Left] = 'a';
    game.keybinds[Key_Right] = 'd';
    
    game.keybinds[Key_UpLeft] = 'q';
    game.keybinds[Key_UpRight] = 'e';
    game.keybinds[Key_DownLeft] = 'z';
    game.keybinds[Key_DownRight] = 'c';
    
    game.keybinds[Key_InventoryToggle] = 'i';
    game.keybinds[Key_InventoryAction] = 'n';
    game.keybinds[Key_InventoryMove] = 'm';
    game.keybinds[Key_PickupOrDrop] = ',';
    game.keybinds[Key_AscendOrDescend] = 'b';
    game.keybinds[Key_Wait] = 'v';
    game.keybinds[Key_Yes] = 'h';
    game.keybinds[Key_No] = 'j';
#endif
    
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
            u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
            game.renderer = SDL_CreateRenderer(game.window, -1, renderer_flags);
            if(game.renderer)
            {
                u32 image_flags = IMG_INIT_PNG;
                if(IMG_Init(image_flags) & image_flags)
                {
                    if(!TTF_Init())
                    {
                        if(initialize_assets(&game, &assets))
                        {
#if 1
                            u64 seed = time(0);
#else
                            u64 seed = 1599995411;
#endif
                            printf("Seed: %lu\n", seed);
                            
                            game.random = set_random_seed(seed);
                            
#if 1
                            game.mode = GameMode_Game;
#else
                            game.mode = GameMode_MainMenu;
#endif
                            
                            game.camera = make_v4s(0, 0,
                                                   game.window_size.w,
                                                   game.window_size.h - assets.log_window.h);
                            
                            u32 target_fps = 60;
                            f32 target_seconds_per_frame = 1.0f / (f32)target_fps;
                            
                            u64 performance_frequency = SDL_GetPerformanceFrequency();
                            u64 last_counter = SDL_GetPerformanceCounter();
                            f32 last_dt = (f32)SDL_GetPerformanceCounter();
                            
                            GameInput input[2] = {0};
                            GameInput *new_input = &input[0];
                            GameInput *old_input = &input[1];
                            
                            for(u32 index = 0; index < Button_Count; ++index)
                            {
                                old_input->mouse[index].has_been_up = true;
                            }
                            
                            for(u32 index = 0; index < Key_Count; ++index)
                            {
                                old_input->keyboard[index].has_been_up = true;
                            }
                            
                            for(u32 index = 1; index < array_count(new_input->fkeys); ++index)
                            {
                                old_input->fkeys[index].has_been_up = true;
                            }
#if MOONBREATH_SLOW
                            f32 fps = 0.0f;
                            f32 full_ms_per_frame = 0.0f;
                            f32 work_ms_per_frame = 0.0f;
                            
                            DebugState debug_state = {0};
                            
                            DebugGroup *debug_vars = debug_group(&debug_state, "Variables", 25, 25, assets.fonts[FontName_ClassicOutlined]);
                            debug_float32(debug_vars, "FPS", &fps);
                            debug_float32(debug_vars, "Frame MS", &full_ms_per_frame);
                            debug_float32(debug_vars, "Work MS", &work_ms_per_frame);
                            debug_float32(debug_vars, "Frame DT", &new_input->frame_dt);
                            
                            debug_uint32(debug_vars, "Mouse X", &new_input->mouse_pos.x);
                            debug_uint32(debug_vars, "Mouse Y", &new_input->mouse_pos.y);
                            debug_uint32(debug_vars, "Mouse Tile X", &new_input->mouse_tile_pos.x);
                            debug_uint32(debug_vars, "Mouse Tile Y", &new_input->mouse_tile_pos.y);
                            debug_uint32(debug_vars, "Player Tile X", &player->pos.x);
                            debug_uint32(debug_vars, "Player Tile Y", &player->pos.y);
                            
                            debug_bool32(debug_vars, "Fov Toggle", &input->fkey_active[1]);
                            debug_bool32(debug_vars, "Traversable Toggle", &input->fkey_active[2]);
                            debug_bool32(debug_vars, "Has Been Up Toggle", &input->fkey_active[3]);
                            debug_bool32(debug_vars, "Hit Test Toggle", &input->fkey_active[4]);
                            
                            DebugGroup *debug_colors = debug_group(&debug_state, "Colors", 150, 25, assets.fonts[FontName_ClassicOutlined]);
                            debug_text(debug_colors, "White");
                            debug_text(debug_colors, "%sLight Gray", start_color(Color_LightGray));
                            debug_text(debug_colors, "%sDark Gray", start_color(Color_DarkGray));
                            debug_text(debug_colors, "%sLight Red", start_color(Color_LightRed));
                            debug_text(debug_colors, "%sDark Red", start_color(Color_DarkRed));
                            debug_text(debug_colors, "%sLight Green", start_color(Color_LightGreen));
                            debug_text(debug_colors, "%sDark Green", start_color(Color_DarkGreen));
                            debug_text(debug_colors, "%sLight Blue", start_color(Color_LightBlue));
                            debug_text(debug_colors, "%sDark Blue", start_color(Color_DarkBlue));
                            debug_text(debug_colors, "%sLight Brown", start_color(Color_LightBrown));
                            debug_text(debug_colors, "%sDark Brown", start_color(Color_DarkBrown));
                            debug_text(debug_colors, "%sCyan", start_color(Color_Cyan));
                            debug_text(debug_colors, "%sYellow", start_color(Color_Yellow));
                            debug_text(debug_colors, "%sPurple", start_color(Color_Purple));
                            debug_text(debug_colors, "%sOrange", start_color(Color_Orange));
#endif
                            
                            while(game.mode)
                            {
                                set_render_color(&game, Color_Black);
                                SDL_RenderClear(game.renderer);
                                
                                for(u32 index = 0; index < Button_Count; ++index)
                                {
                                    new_input->mouse[index].ended_down = old_input->mouse[index].ended_down;
                                    new_input->mouse[index].has_been_up = old_input->mouse[index].has_been_up;
                                }
                                
                                for(u32 index = 0; index < Key_Count; ++index)
                                {
                                    new_input->keyboard[index].ended_down = old_input->keyboard[index].ended_down;
                                    new_input->keyboard[index].has_been_up = old_input->keyboard[index].has_been_up;
                                }
                                
                                for(u32 index = 1; index < array_count(new_input->fkeys); ++index)
                                {
                                    new_input->fkey_active[index] = old_input->fkey_active[index];
                                    new_input->fkeys[index].ended_down = old_input->fkeys[index].ended_down;
                                    new_input->fkeys[index].has_been_up = old_input->fkeys[index].has_been_up;
                                }
                                
                                process_events(&game, new_input);
                                
                                u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                process_input(&new_input->mouse[Button_Left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                process_input(&new_input->mouse[Button_Middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                process_input(&new_input->mouse[Button_Right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                process_input(&new_input->mouse[Button_Extended1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                process_input(&new_input->mouse[Button_Extended2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->frame_dt = (end_dt - last_dt) / (f32)performance_frequency;
                                last_dt = end_dt;
                                
                                update_and_render_game(&game, new_input, &dungeon, player, entities, log, items, &inventory, &assets, &item_info, enemy_levels);
                                
#if MOONBREATH_SLOW
                                if(was_pressed(&new_input->mouse[Button_Middle], new_input->fkey_active))
                                {
                                    for(u32 index = 0; index < dungeon.rooms.count; ++index)
                                    {
                                        if(is_inside_room(dungeon.rooms.array[index], player->pos))
                                        {
                                            printf("Room Index: %u\n", index);
                                            printf("room.x: %u\n", dungeon.rooms.array[index].x);
                                            printf("room.y: %u\n", dungeon.rooms.array[index].y);
                                            printf("room.w: %u\n", dungeon.rooms.array[index].w);
                                            printf("room.h: %u\n", dungeon.rooms.array[index].h);
                                            printf("enemy_count: %u\n", dungeon.rooms.enemy_count[index]);
                                            printf("item_count: %u\n\n", dungeon.rooms.item_count[index]);
                                            
                                            break;
                                        }
                                    }
                                }
                                
                                v2u tile_pos =
                                {
                                    tile_div(new_input->mouse_pos.x),
                                    tile_div(new_input->mouse_pos.y)
                                };
                                
                                v2u camera_offset =
                                {
                                    tile_div(game.camera.x),
                                    tile_div(game.camera.y)
                                };
                                
                                new_input->mouse_tile_pos.x = tile_pos.x + camera_offset.x;
                                new_input->mouse_tile_pos.y = tile_pos.y + camera_offset.y;
                                
                                if(tile_pos.y < tile_div(game.camera.h))
                                {
                                    set_render_color(&game, Color_Yellow);
                                    
                                    v4u rect = tile_rect(tile_pos);
                                    SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&rect);
                                }
                                
                                u64 work_counter = SDL_GetPerformanceCounter();
                                u64 work_elapsed_counter = work_counter - last_counter;
                                work_ms_per_frame = (1000.0f * (f32)work_elapsed_counter) / (f32)performance_frequency;
#endif
                                
                                if(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency) < target_seconds_per_frame)
                                {
                                    u32 time_to_delay =
                                        ((target_seconds_per_frame - seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency)) * 1000);
                                    SDL_Delay(time_to_delay);
                                    
                                    while(seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency) < target_seconds_per_frame)
                                    {
                                    }
                                }
                                else
                                {
                                    // Missed frame rate.
                                }
                                
                                u64 end_counter = SDL_GetPerformanceCounter();
                                u64 elapsed_counter = end_counter - last_counter;
                                last_counter = end_counter;
                                
#if MOONBREATH_SLOW
                                fps = (f32)performance_frequency / (f32)elapsed_counter;
                                full_ms_per_frame = (1000.0f * (f32)elapsed_counter) / (f32)performance_frequency;
                                
                                update_and_render_debug_state(&game, &debug_state, new_input);
#endif
                                
                                GameInput *temp = new_input;
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
    
    // Exit Game
    free_assets(&assets);
    free(entities);
    free(dungeon.tiles.array);
    
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return(result);
}
