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
// Don't check NULL pointers that are never NULL.
// Do we need potion stacking?
// If you equip a two-handed weapon, unequip shield.
// Single depth water for dungeons, half movement speed while in water.
// Traps for dungeons.

/*
Examination mode:
- Cycle passages
- Actually examining things

Pathfind:
- Show path travelled by pathfind?

Log:
- Full log window

Art:
- Need a couple sessions doing this
*/

internal void
update_and_render_examine_mode(GameState *game, GameInput *input, Assets *assets)
{
    ExamineMode *examine = &game->examine;
    
    if(examine->is_enabled)
    {
        for(u32 index = Key_Up; index <= Key_DownRight; ++index)
        {
            if(input->keyboard[index].ended_down)
            {
                b32 can_move = true;
                for(u32 second_index = Key_Up; second_index <= Key_DownRight; ++second_index)
                {
                    if(second_index != index &&
                       examine->is_key_held[second_index])
                    {
                        can_move = false;
                        break;
                    }
                }
                
                if(can_move)
                {
                    if(examine->key_hold_start[index])
                    {
                        u32 hold_time_ms = SDL_GetTicks() - examine->key_hold_start[index];
                        if(hold_time_ms >= 400)
                        {
                            examine->is_key_held[index] = true;
                        }
                    }
                    else
                    {
                        examine->key_hold_start[index] = SDL_GetTicks();
                    }
                }
            }
            else
            {
                examine->is_key_held[index] = false;
                examine->key_hold_start[index] = 0;
            }
        }
        
        if(was_pressed(&input->Key_CyclePassages))
        {
            // TODO(rami): Cycle available passages,
            // pathfind to one if we want to do that.
        }
        
        v4u src = get_tile_rect(make_v2u(1, 16));
        v4u dest = get_game_dest(game, examine->pos);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    }
}

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
get_random_direction(Random *random)
{
    Direction result = random_number(random, Direction_Up, Direction_DownRight);
    return(result);
}

internal v4u
get_tile_rect(v2u pos)
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
get_game_dest(GameState *game, v2u pos)
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
    if(tile_mul(dungeon->width) < game->camera.w)
    {
        render_area.w = dungeon->width - 1;
    }
    
    if(tile_mul(dungeon->height) < game->camera.h)
    {
        render_area.h = dungeon->height - 1;
    }
    
#if 0
    printf("render_area.x: %d\n", render_area.x);
    printf("render_area.y: %d\n", render_area.y);
    printf("render_area.w: %d\n", render_area.w);
    printf("render_area.h: %d\n\n", render_area.h);
#endif
    
    for(u32 y = render_area.y; y <= render_area.h; ++y)
    {
        for(u32 x = render_area.x; x <= render_area.w; ++x)
        {
            v2u render_pos = {x, y};
            v4u src = get_tile_rect(get_dungeon_tile_pos(dungeon->tiles, render_pos));
            v4u dest = get_tile_rect(render_pos);
            
            if(is_tile_seen(dungeon->tiles, render_pos))
            {
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                RemainsSource src = get_tile_remains_src(dungeon, render_pos);
                if(src.found)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src.rect, (SDL_Rect *)&dest);
                }
            }
            else if(has_tile_been_seen(dungeon->tiles, render_pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, false);
                
                RemainsSource src = get_tile_remains_src(dungeon, render_pos);
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
    v2u camera_follow_pos = {0};
    if(game->examine.is_enabled)
    {
        camera_follow_pos = game->examine.pos;
    }
    else
    {
        camera_follow_pos = player->pos;
    }
    
    game->camera.x = tile_mul(camera_follow_pos.x) - (game->camera.w / 2);
    
    if(game->window_size.w == 1280 &&
       game->window_size.h == 720)
    {
        game->camera.y = tile_mul(camera_follow_pos.y) - (game->camera.h / 2) + 16;
    }
    else if(game->window_size.w == 1920 &&
            game->window_size.h == 1080)
    {
        game->camera.y = tile_mul(camera_follow_pos.y) - (game->camera.h / 2) + 4;
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
    if(tile_mul(dungeon->width) >= game->camera.w)
    {
        s32 camera_max_x = (s32)(tile_mul(dungeon->width) - game->camera.w);
        s32 camera_max_y = (s32)(tile_mul(dungeon->height) - game->camera.h);
        
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
                    if(key_code == SDLK_ESCAPE)
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
                    else if(key_code == SDLK_F6)
                    {
                        process_input(&input->fkeys[6], is_down);
                    }
                    else if(key_code == SDLK_F7)
                    {
                        process_input(&input->fkeys[7], is_down);
                    }
                    else if(key_code == SDLK_F8)
                    {
                        process_input(&input->fkeys[8], is_down);
                    }
                    else if(key_code == SDLK_F9)
                    {
                        process_input(&input->fkeys[9], is_down);
                    }
                    else if(key_code == SDLK_F10)
                    {
                        process_input(&input->fkeys[10], is_down);
                    }
                    else if(key_code == SDLK_F11)
                    {
                        process_input(&input->fkeys[11], is_down);
                    }
                    else if(key_code == SDLK_F12)
                    {
                        process_input(&input->fkeys[12], is_down);
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
                       u32 *entity_levels)
{
    if(game->mode == GameMode_MainMenu)
    {
        set_render_color(game, Color_Cyan);
        v4u rect = {50, 300, 200, 100};
        SDL_RenderFillRect(game->renderer, (SDL_Rect *)&rect);
        
        if(is_inside_rect(rect, input->mouse_pos))
        {
            render_text(game, "%sNew Game", 100, 340, assets->fonts[FontName_ClassicOutlined], 0, start_color(Color_Yellow));
            
            if(was_pressed(&input->Button_Left))
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
                            case 0: item_info->potion_tiles[index] = make_v2u(10, 2); break;
                            case 1: item_info->potion_tiles[index] = make_v2u(10, 3); break;
                            case 2: item_info->potion_tiles[index] = make_v2u(10, 4); break;
                            case 3: item_info->potion_tiles[index] = make_v2u(10, 5); break;
                            case 4: item_info->potion_tiles[index] = make_v2u(10, 6); break;
                            case 5: item_info->potion_tiles[index] = make_v2u(10, 7); break;
                            case 6: item_info->potion_tiles[index] = make_v2u(10, 8); break;
                            
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
                            case 0: item_info->scroll_tiles[index] = make_v2u(11, 2); break;
                            case 1: item_info->scroll_tiles[index] = make_v2u(11, 3); break;
                            case 2: item_info->scroll_tiles[index] = make_v2u(11, 4); break;
                            case 3: item_info->scroll_tiles[index] = make_v2u(11, 5); break;
                            case 4: item_info->scroll_tiles[index] = make_v2u(11, 6); break;
                            
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
            
            entity_levels[EntityID_SkeletonWarrior] = 1;
            entity_levels[EntityID_SkeletonArcher] = 1;
            entity_levels[EntityID_SkeletonMage] = 1;
            entity_levels[EntityID_Bat] = 1;
            entity_levels[EntityID_Rat] = 1;
            
            entity_levels[EntityID_KoboldWarrior] = 2;
            entity_levels[EntityID_KoboldShaman] = 2;
            entity_levels[EntityID_Snail] = 2;
            entity_levels[EntityID_Slime] = 2;
            entity_levels[EntityID_Dog] = 2;
            
            entity_levels[EntityID_OrcWarrior] = 3;
            entity_levels[EntityID_OrcArcher] = 3;
            entity_levels[EntityID_OrcShaman] = 3;
            entity_levels[EntityID_Python] = 3;
            entity_levels[EntityID_Shade] = 3;
            
            entity_levels[EntityID_ElfKnight] = 4;
            entity_levels[EntityID_ElfArbalest] = 4;
            entity_levels[EntityID_ElfMage] = 4;
            entity_levels[EntityID_GiantSlime] = 4;
            entity_levels[EntityID_Spectre] = 4;
            
            entity_levels[EntityID_OrcSorcerer] = 5;
            entity_levels[EntityID_OrcAssassin] = 5;
            entity_levels[EntityID_Minotaur] = 5;
            entity_levels[EntityID_Treant] = 5;
            entity_levels[EntityID_Viper] = 5;
            
            entity_levels[EntityID_CentaurWarrior] = 6;
            entity_levels[EntityID_CentaurSpearman] = 6;
            entity_levels[EntityID_CentaurArcher] = 6;
            entity_levels[EntityID_CursedSkull] = 6;
            entity_levels[EntityID_Wolf] = 6;
            
            entity_levels[EntityID_OgreWarrior] = 7;
            entity_levels[EntityID_OgreArcher] = 7;
            entity_levels[EntityID_OgreMage] = 7;
            entity_levels[EntityID_Cyclops] = 7;
            entity_levels[EntityID_ShadowWalker] = 7;
            
            entity_levels[EntityID_DwarwenWarrior] = 8;
            entity_levels[EntityID_DwarwenSorcerer] = 8;
            entity_levels[EntityID_DwarwenPriest] = 8;
            entity_levels[EntityID_ScarletSnake] = 8;
            entity_levels[EntityID_Lich] = 8;
            
            entity_levels[EntityID_AbyssalFiend] = 9;
            entity_levels[EntityID_BloodTroll] = 9;
            entity_levels[EntityID_IronGolem] = 9;
            entity_levels[EntityID_Griffin] = 9;
            entity_levels[EntityID_Imp] = 9;
            
            entity_levels[EntityID_BlackKnight] = 10;
            entity_levels[EntityID_GiantDemon] = 10;
            entity_levels[EntityID_Hellhound] = 10;
            entity_levels[EntityID_AbyssalHexmaster] = 10;
            entity_levels[EntityID_Mahjarrat] = 10;
            
            add_player_entity(&game->random, player, items, inventory);
            create_dungeon(&game->random, dungeon, player, log, entities, items, inventory, item_info, entity_levels);
            update_fov(dungeon, player);
            
            game->is_initialized = true;
        }
        
        // TODO(rami): Bundle?
        update_entities(game, input, player, entities, dungeon, items, item_info, log, inventory, entity_levels);
        update_camera(game, dungeon, player);
        
        render_tilemap(game, dungeon, assets);
        render_items(game, dungeon, items, assets);
        render_entities(game, dungeon, entities, inventory, assets);
        render_ui(game, dungeon, player, log, inventory, assets);
        
        update_and_render_examine_mode(game, input, assets);
    }
}

internal f32
get_ms_from_elapsed(u64 elapsed, u64 performance_frequency)
{
    f32 result = ((1000.0f * (f32)elapsed) / (f32)performance_frequency);
    return(result);
}

internal f32
get_seconds_elapsed(u64 old_counter, u64 new_counter, u64 performance_frequency)
{
    f32 result = (f32)(new_counter - old_counter) / (f32)performance_frequency;
    return(result);
}

int main(int argc, char *argv[])
{
    u32 result = 0;
    
    // All game data is required to be initialized to zero.
    GameState game = {0};
    Assets assets = {0};
    Entity *entities = calloc(1, MAX_ENTITY_COUNT * sizeof(Entity));
    Entity *player = &entities[0];
    u32 entity_levels[EntityID_Count] = {0};
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
    
#if 0
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
            
            case Key_InventoryOpen: token_name = "key_inventory_open"; break;
            case Key_InventoryAction: token_name = "key_inventory_action"; break;
            case Key_InventoryMove: token_name = "key_inventory_move"; break;
            
            case Key_PickupDrop: token_name = "key_pickup_drop"; break;
            case Key_AscendDescend: token_name = "key_ascend_descend"; break;
            
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
            printf("Error: Value could not be loaded for game.keybinds[%u].\n", index);
            assert(0);
        }
    }
#else
    game.Key_Up = 'w';
    game.Key_Down = 's';
    game.Key_Left = 'a';
    game.Key_Right = 'd';
    
    game.Key_UpLeft = 'q';
    game.Key_UpRight = 'e';
    game.Key_DownLeft = 'z';
    game.Key_DownRight = 'c';
    
    game.Key_InventoryOpen = 'i';
    game.Key_InventoryAction = 'n';
    game.Key_InventoryMove = 'm';
    
    game.Key_PickupDrop = ',';
    game.Key_AscendDescend = 'u';
    game.Key_AutoExplore = 'p';
    game.Key_CyclePassages = '<';
    game.Key_Examine = 'o';
    game.Key_Log = 'l';
    
    game.Key_Wait = 'v';
    game.Key_Yes = 'h';
    game.Key_No = 'j';
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
                            
#if 0
                            u64 seed = time(0);
#else
                            u64 seed = 1602812435;
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
                                                   game.window_size.h - assets.bottom_ui_window.h);
                            
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
                            
                            DebugGroup *debug_vars = add_debug_group(&debug_state, "Variables", 25, 25, assets.fonts[FontName_DosVga]);
                            add_debug_float32(debug_vars, "FPS", &fps);
                            add_debug_float32(debug_vars, "Frame MS", &full_ms_per_frame);
                            add_debug_float32(debug_vars, "Work MS", &work_ms_per_frame);
                            add_debug_float32(debug_vars, "Frame DT", &new_input->frame_dt);
                            add_debug_newline(debug_vars);
                            
                            add_debug_v2u(debug_vars, "Mouse", &new_input->mouse_pos);
                            add_debug_v2u(debug_vars, "Mouse Tile", &new_input->mouse_tile_pos);
                            add_debug_v2u(debug_vars, "Player Tile", &player->pos);
                            add_debug_newline(debug_vars);
                            
                            add_debug_bool32(debug_vars, "Fov", &fkey_active[1]);
                            add_debug_bool32(debug_vars, "All is Traversable", &fkey_active[2]);
                            add_debug_bool32(debug_vars, "Check Has Been Up", &fkey_active[3]);
                            add_debug_bool32(debug_vars, "Hit Test Enabled", &fkey_active[4]);
                            
                            DebugGroup *debug_colors = add_debug_group(&debug_state, "Colors", 125, 25, assets.fonts[FontName_DosVga]);
                            add_debug_text(debug_colors, "White");
                            add_debug_text(debug_colors, "%sLight Gray", start_color(Color_LightGray));
                            add_debug_text(debug_colors, "%sDark Gray", start_color(Color_DarkGray));
                            add_debug_text(debug_colors, "%sLight Red", start_color(Color_LightRed));
                            add_debug_text(debug_colors, "%sDark Red", start_color(Color_DarkRed));
                            add_debug_text(debug_colors, "%sLight Green", start_color(Color_LightGreen));
                            add_debug_text(debug_colors, "%sDark Green", start_color(Color_DarkGreen));
                            add_debug_text(debug_colors, "%sLight Blue", start_color(Color_LightBlue));
                            add_debug_text(debug_colors, "%sDark Blue", start_color(Color_DarkBlue));
                            add_debug_text(debug_colors, "%sLight Brown", start_color(Color_LightBrown));
                            add_debug_text(debug_colors, "%sDark Brown", start_color(Color_DarkBrown));
                            add_debug_text(debug_colors, "%sCyan", start_color(Color_Cyan));
                            add_debug_text(debug_colors, "%sYellow", start_color(Color_Yellow));
                            add_debug_text(debug_colors, "%sPurple", start_color(Color_Purple));
                            add_debug_text(debug_colors, "%sOrange", start_color(Color_Orange));
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
                                    new_input->fkeys[index].ended_down = old_input->fkeys[index].ended_down;
                                    new_input->fkeys[index].has_been_up = old_input->fkeys[index].has_been_up;
                                }
                                
                                process_events(&game, new_input);
                                
                                u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                process_input(&new_input->Button_Left, mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                process_input(&new_input->Button_Middle, mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                process_input(&new_input->Button_Right, mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                process_input(&new_input->Button_Extended1, mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                process_input(&new_input->Button_Extended2, mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->frame_dt = (end_dt - last_dt) / (f32)performance_frequency;
                                last_dt = end_dt;
                                
                                update_and_render_game(&game, new_input, &dungeon, player, entities, log, items, &inventory, &assets, &item_info, entity_levels);
                                
#if MOONBREATH_SLOW
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
                                    v4u src = get_tile_rect(make_v2u(1, 16));
                                    v4u dest = get_game_dest(&game, new_input->mouse_tile_pos);
                                    SDL_RenderCopy(game.renderer, assets.tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                                }
                                
                                u64 work_elapsed_counter = SDL_GetPerformanceCounter() - last_counter;
                                work_ms_per_frame = get_ms_from_elapsed(work_elapsed_counter, performance_frequency);
#endif
                                
                                if(get_seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency) < target_seconds_per_frame)
                                {
                                    u32 time_to_delay =
                                        ((target_seconds_per_frame - get_seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency)) * 1000);
                                    SDL_Delay(time_to_delay);
                                    
                                    while(get_seconds_elapsed(last_counter, SDL_GetPerformanceCounter(), performance_frequency) < target_seconds_per_frame)
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
                                full_ms_per_frame = get_ms_from_elapsed(elapsed_counter, performance_frequency);
                                
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
                        printf("ERROR: TTF_Init()\n");
                    }
                }
                else
                {
                    // TODO(rami): Logging
                    printf("ERROR: IMG_Init()\n");
                }
            }
            else
            {
                // TODO(rami): Logging
                printf("ERROR: SDL_CreateRenderer()\n");
            }
        }
        else
        {
            // TODO(rami): Logging
            printf("ERROR: SDL_CreateWindow()\n");
        }
    }
    else
    {
        // TODO(rami): Logging
        printf("ERROR: SDL_Init()\n");
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