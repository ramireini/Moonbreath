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
#include "config_parser.c"

// TODO(Rami):
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(Rami): Get rid of checking for NULL on pointers that never are NULL.

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

internal b32
is_input_valid(InputState *state)
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
render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest)
{
    SDL_SetTextureColorMod(texture, 127, 127, 127);
    SDL_RenderCopy(renderer, texture, (SDL_Rect *)&src, (SDL_Rect *)&dest);
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
    
    // If the dungeon w/h is less than
    // the w/h of the camera we can clamp the render area
    // to the w/h of the dungeon.
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
    
    u32 tileset_tile_width = tile_div(assets->tileset.w);
    
    for(u32 y = render_area.y; y <= render_area.h; ++y)
    {
        for(u32 x = render_area.x; x <= render_area.w; ++x)
        {
            v2u render_pos = {x, y};
            v2u tile_pos = v2u_from_index(get_tile_id(dungeon->tiles, render_pos), tileset_tile_width);
            
            v4u src = tile_rect(tile_pos);
            v4u dest = tile_rect(render_pos);
            
            if(tile_is_seen(dungeon->tiles, render_pos))
            {
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                v4u_b32 remains_src = get_tile_remains_src(dungeon, render_pos, tileset_tile_width);
                if(remains_src.success)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&remains_src.rect, (SDL_Rect *)&dest);
                }
            }
            else if(tile_has_been_seen(dungeon->tiles, render_pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest);
                
                v4u_b32 remains_src = get_tile_remains_src(dungeon, render_pos, tileset_tile_width);
                if(remains_src.success)
                {
                    render_texture_half_color(game->renderer, assets->tileset.tex, remains_src.rect, dest);
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
    
    // if statement is so that dungeons smaller than
    // the size of the camera will be rendered properly.
    if(tile_mul(dungeon->width) >= game->camera.w)
    {
        if(game->camera.x >= (s32)(tile_mul(dungeon->width) - game->camera.w))
        {
            game->camera.x = tile_mul(dungeon->width) - game->camera.w;
        }
        
        if(game->camera.y >= (s32)(tile_mul(dungeon->height) - game->camera.h))
        {
            game->camera.y = tile_mul(dungeon->height) - game->camera.h;
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
process_input(InputState *state, b32 is_down)
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
process_events(GameState *game, InputState *keyboard)
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
                for(u32 key_index = 0; key_index < Key_Count; ++key_index)
                {
                    if(key_code == SDLK_p)
                    {
                        game->mode = GameMode_Quit;
                    }
                    else if(key_code == game->keybinds[key_index])
                    {
                        process_input(&keyboard[key_index], is_down);
                    }
#if MOONBREATH_SLOW
                    else if(key_code == SDLK_F1)
                    {
                        process_input(&keyboard[Key_ToggleFov], is_down);
                    }
                    else if(key_code == SDLK_F2)
                    {
                        process_input(&keyboard[Key_ToggleTraversable], is_down);
                    }
                    else if(key_code == SDLK_F3)
                    {
                        process_input(&keyboard[Key_ToggleHasBeenUp], is_down);
                    }
                    else if(key_code == SDLK_F4)
                    {
                        process_input(&keyboard[Key_ToggleIdentify], is_down);
                    }
#endif
                }
            }
        }
    }
}

internal u32
window_refresh_rate(GameState *game)
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
array_debug(Item *items)
{
#if 0
    for(u32 index = MAX_ITEMS - 1; index < MAX_ITEMS; --index)
    {
        Item *item = &items[index];
        if(item->id)
        {
            printf("\n\nitems[%u]\n", index);
            
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
                case ItemType_Weapon:
                {
                    printf("damage: %d\n", item->w.damage);
                    printf("accuracy: %d\n", item->w.accuracy);
                    printf("attack_speed: %.01f\n", item->w.attack_speed);
                } break;
                
                case ItemType_Armor:
                {
                    printf("defence: %d\n", item->a.defence);
                    printf("weight: %d\n", item->a.weight);
                } break;
                
                case ItemType_Potion:
                case ItemType_Scroll:
                {
                    printf("duration: %u\n", item->c.duration);
                    printf("value: %u\n", item->c.value);
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
                       ConsumableData *consumable_data,
                       u32 *enemy_levels)
{
    if(game->mode == GameMode_MainMenu)
    {
        set_render_color(game, Color_Cyan);
        v4u rect = {50, 300, 200, 100};
        SDL_RenderFillRect(game->renderer, (SDL_Rect *)&rect);
        
        if(is_inside_rectangle(input->mouse_pos, rect))
        {
            render_text(game, "%sNew Game", 100, 340, assets->fonts[FontName_ClassicOutlined], Color_White, start_color(Color_Yellow));
            
            if(is_input_valid(&input->Button_Left))
            {
                game->mode = GameMode_Game;
            }
        }
        else
        {
            render_text(game, "New Game", 100, 340, assets->fonts[FontName_ClassicOutlined], Color_White);
        }
    }
    else if(game->mode == GameMode_Game)
    {
        
        // TODO(rami): When we go back to the main menu
        // if the player wins or dies, we need to set game.is_initialized to false.
        if(!game->is_initialized)
        {
#if 0
            printf("\nPotion Count: %u\n", ItemID_PotionCount);
            printf("Scroll Count: %u\n", ItemID_ScrollCount);
            printf("Consumable Count: %u\n\n", Consumable_Count);
#endif
            
            // Randomize consumable colors
            assert(Consumable_Count == (ItemID_PotionCount + ItemID_ScrollCount));
            b32 is_color_used[Consumable_Count] = {0};
            
            for(u32 index = 0; index < Consumable_Count; ++index)
            {
                while(!consumable_data->tiles[index].x &&
                      !consumable_data->tiles[index].y)
                {
                    u32 color_index;
                    if(index < ItemID_PotionCount)
                    {
                        color_index = random_number(&game->random, 0, ItemID_PotionCount - 1);
                    }
                    else
                    {
                        color_index = random_number(&game->random, ItemID_PotionCount, Consumable_Count - 1);
                    }
                    
                    if(!is_color_used[color_index])
                    {
                        switch(color_index)
                        {
                            case 0: consumable_data->tiles[index] = V2u(8, 1); break;
                            case 1: consumable_data->tiles[index] = V2u(8, 2); break;
                            case 2: consumable_data->tiles[index] = V2u(8, 3); break;
                            case 3: consumable_data->tiles[index] = V2u(8, 4); break;
                            case 4: consumable_data->tiles[index] = V2u(8, 5); break;
                            case 5: consumable_data->tiles[index] = V2u(8, 6); break;
                            case 6: consumable_data->tiles[index] = V2u(8, 7); break;
                            case 7: consumable_data->tiles[index] = V2u(8, 8); break;
                            case 8: consumable_data->tiles[index] = V2u(8, 9); break;
                            case 9: consumable_data->tiles[index] = V2u(8, 10); break;
                            case 10: consumable_data->tiles[index] = V2u(8, 11); break;
                            case 11: consumable_data->tiles[index] = V2u(8, 12); break;
                            case 12: consumable_data->tiles[index] = V2u(8, 13); break;
                            case 13: consumable_data->tiles[index] = V2u(8, 14); break;
                            
                            case 14: consumable_data->tiles[index] = V2u(9, 1); break;
                            case 15: consumable_data->tiles[index] = V2u(9, 2); break;
                            case 16: consumable_data->tiles[index] = V2u(9, 3); break;
                            case 17: consumable_data->tiles[index] = V2u(9, 4); break;
                            case 18: consumable_data->tiles[index] = V2u(9, 5); break;
                            case 19: consumable_data->tiles[index] = V2u(9, 6); break;
                            
                            invalid_default_case;
                        }
                        
                        is_color_used[color_index] = true;
                        break;
                    }
                }
            }
#if 0
            // Print consumable tiles
            for(u32 index = 0; index < Consumable_Count; ++index)
            {
                printf("[%u]: %u, %u\n", index,
                       consumable_data->tiles[index].x,
                       consumable_data->tiles[index].y);
            }
#endif
            
            enemy_levels[EntityID_Skeleton] = 1;
            enemy_levels[EntityID_CaveBat] = 1;
            enemy_levels[EntityID_Slime] = 1;
            enemy_levels[EntityID_Rat] = 1;
            enemy_levels[EntityID_Snail] = 1;
            
            enemy_levels[EntityID_Dog] = 2;
            enemy_levels[EntityID_GiantSlime] = 2;
            enemy_levels[EntityID_SkeletonWarrior] = 2;
            enemy_levels[EntityID_Goblin] = 2;
            enemy_levels[EntityID_Python] = 2;
            
            enemy_levels[EntityID_OrcWarrior] = 3;
            enemy_levels[EntityID_Assassin] = 3;
            enemy_levels[EntityID_Kobold] = 3;
            enemy_levels[EntityID_Ghoul] = 3;
            enemy_levels[EntityID_Centaur] = 3;
            
            enemy_levels[EntityID_Imp] = 4;
            enemy_levels[EntityID_FloatingEye] = 4;
            enemy_levels[EntityID_UndeadElfWarrior] = 4;
            enemy_levels[EntityID_Viper] = 4;
            enemy_levels[EntityID_FrostWalker] = 4;
            
            enemy_levels[EntityID_DwarwenWarrior] = 5;
            enemy_levels[EntityID_Minotaur] = 5;
            enemy_levels[EntityID_Tormentor] = 5;
            enemy_levels[EntityID_Treant] = 5;
            
            enemy_levels[EntityID_Devourer] = 6;
            enemy_levels[EntityID_Wolf] = 6;
            enemy_levels[EntityID_CentaurWarrior] = 6;
            
            enemy_levels[EntityID_BrimstoneImp] = 7;
            enemy_levels[EntityID_Spectre] = 7;
            enemy_levels[EntityID_FlyingSkull] = 7;
            
            enemy_levels[EntityID_Hellhound] = 8;
            enemy_levels[EntityID_BlackKnight] = 8;
            enemy_levels[EntityID_GiantDemon] = 8;
            
            enemy_levels[EntityID_CursedBlackKnight] = 9;
            enemy_levels[EntityID_ScarletKingsnake] = 9;
            
            enemy_levels[EntityID_Griffin] = 10;
            enemy_levels[EntityID_Ogre] = 10;
            enemy_levels[EntityID_Cyclops] = 10;
            
            // TODO(Rami): ?
            enemy_levels[EntityID_FrostShards] = 0;
            enemy_levels[EntityID_GreenMamba] = 0;
            
            create_dungeon(game, dungeon, player, log, entities, items, consumable_data, enemy_levels);
            add_player_entity(game, player, items, inventory);
            update_fov(dungeon, player);
            
#if 0
            // Identify all items
            for(u32 index = 0; index < MAX_ITEMS; ++index)
            {
                Item *item = &items[index];
                if(item->id)
                {
                    item->is_identified = true;
                }
            }
#endif
            
            game->is_initialized = true;
        }
        
        update_entities(game, input, player, entities, dungeon, items, consumable_data, log, inventory, enemy_levels);
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
    
    // TODO(rami): Adjust array and #define sizes.
    // Everything has to be initialized to zero.
    GameState game = {0};
    Assets assets = {0};
    Entity entities[MAX_ENTITIES] = {0};
    Entity *player = &entities[0];
    u32 enemy_levels[EntityID_Count] = {0};
    Dungeon dungeon = {0};
    dungeon.tiles.array = calloc(1, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(Tile));
    Inventory inventory = {0};
    Item items[MAX_ITEMS] = {0};
    ConsumableData consumable_data = {0};
    String128 log[MAX_LOG_ENTRIES] = {0};
    
    Config config = parse_config_file("data/config.txt");
    
    if(1)
    {
        game.window_size = V2u(1280, 720);
    }
    else
    {
        game.window_size = V2u(1920, 1080);
    }
    
    game.keybinds[Key_Up] = 'w';
    game.keybinds[Key_Down] = 's';
    game.keybinds[Key_Left] = 'a';
    game.keybinds[Key_Right] = 'd';
    
    game.keybinds[Key_UpLeft] = 'q';
    game.keybinds[Key_UpRight] = 'e';
    game.keybinds[Key_DownLeft] = 'z';
    game.keybinds[Key_DownRight] = 'c';
    
    game.keybinds[Key_Inventory] = 'i';
    game.keybinds[Key_EquipOrConsumeItem] = 'n';
    game.keybinds[Key_PickupOrDropItem] = ',';
    game.keybinds[Key_IdentifyOrEnchantItem] = '.';
    game.keybinds[Key_MoveItem] = 'm';
    game.keybinds[Key_AscendOrDescend] = 'b';
    game.keybinds[Key_Wait] = 'v';
    game.keybinds[Key_Yes] = 'h';
    game.keybinds[Key_No] = 'j';
    
    game.show_ground_item_outline = get_config_value_bool32(&config, "show_ground_item_outline");
    
    // TODO(Rami): Do we want ""'s around characters, might make the config clearer.
    // TODO(Rami): For the config, remove later.
    /*
// 1 = 1920 x 1080
// 2 = 1280 x 720
window_size_option = 1

    key_up = "w"
        key_down = "s"
        key_left = "a"
        key_right = "d"
        
        key_up_left = "q"
        key_up_right = "e"
        key_down_left = "z"
        key_down_right = "c"
        
        key_inventory = "i"
        key_equip_or_consume_item = "n"
        key_pickup_or_drop_item = ","
    key_identify_or_enchant_item = "."
        key_move_item = "m"
        key_ascend_or_descend = "b"
        key_wait = "v"
        key_yes = "h"
        key_no = "j"
        
        show_ground_item_outline = true
 ground_item_outline_style = 1

        draw_mouse_tile_outline = false
mouse_tile_outline_style = 1
        */
    
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
                            u64 seed = 1587001145;
#endif
                            printf("Seed: %lu\n", seed);
                            
                            game.random = set_random_seed(seed);
                            game.mode = GameMode_Game;
                            //game.mode = GameMode_MainMenu;
                            
                            game.camera = V4s(0, 0,
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
                            
#if MOONBREATH_SLOW
                            f32 actual_fps = 0.0f;
                            f32 actual_seconds_per_frame = 0.0f;
                            f32 work_seconds_per_frame = 0.0f;
                            
                            DebugState debug_state = {0};
                            debug_state.selected_group_index = 1;
                            
                            DebugGroup *debug_variables = add_debug_group(&debug_state, "Variables", 25, 25, assets.fonts[FontName_ClassicOutlined]);
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
                            
                            DebugGroup *debug_colors = add_debug_group(&debug_state, "Colors", 150, 25, assets.fonts[FontName_ClassicOutlined]);
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
                            
                            for(u32 index = 0; index < Button_Count; ++index)
                            {
                                old_input->mouse[index].has_been_up = true;
                            }
                            
                            for(u32 index = 0; index < Key_Count; ++index)
                            {
                                old_input->keyboard[index].has_been_up = true;
                            }
                            
                            if(game.mode == GameMode_Quit)
                            {
                                assert(0);
                            }
                            
                            while(game.mode)
                            {
#if MOONBREATH_SLOW
                                array_debug(items);
#endif
                                
                                set_render_color(&game, Color_Black);
                                SDL_RenderClear(game.renderer);
                                
                                for(u32 index = 0; index < Button_Count; ++index)
                                {
                                    new_input->mouse[index].is_down = old_input->mouse[index].is_down;
                                    new_input->mouse[index].has_been_up = old_input->mouse[index].has_been_up;
                                }
                                
                                for(u32 index = 0; index < Key_Count; ++index)
                                {
                                    new_input->keyboard[index].is_down = old_input->keyboard[index].is_down;
                                    new_input->keyboard[index].has_been_up = old_input->keyboard[index].has_been_up;
                                }
                                
                                process_events(&game, new_input->keyboard);
                                
                                u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                process_input(&new_input->Button_Left, mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                process_input(&new_input->Button_Middle, mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                process_input(&new_input->Button_Right, mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                process_input(&new_input->Button_X1, mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                process_input(&new_input->Button_X2, mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->dt = (end_dt - last_dt) / (f32)performance_frequency;
                                last_dt = end_dt;
                                
                                update_and_render_game(&game, new_input, &dungeon, player, entities, log, items, &inventory, &assets, &consumable_data, enemy_levels);
                                
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
                                
                                // Logical result
                                mouse_final.x = selection.x + camera_offset.x;
                                mouse_final.y = selection.y + camera_offset.y;
                                
                                if(selection.y < tile_div(game.camera.h))
                                {
                                    set_render_color(&game, Color_Yellow);
                                    SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&rect);
                                }
#endif
                                
#if MOONBREATH_SLOW
                                u64 work_counter = SDL_GetPerformanceCounter();
                                u64 work_elapsed_counter = work_counter - last_counter;
                                work_seconds_per_frame = (1000.0f * (f32)work_elapsed_counter) / (f32)performance_frequency;
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
                                actual_fps = (f32)performance_frequency / (f32)elapsed_counter;
                                actual_seconds_per_frame = (1000.0f * (f32)elapsed_counter) / (f32)performance_frequency;
                                
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
    free(dungeon.tiles.array);
    
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return(result);
}
