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
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(rami): Adjust array and #define sizes.

// TODO(rami): Would be cool to have a log that you could open and it would
// show you more messages than the UI area at the bottom.

// TODO(rami): An overview mode where you can do things like view the rest
// of the map and choose a position your character will go to.

// TODO(rami): Get rid of checking for NULL on pointers that never are NULL.

// TODO(rami):
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
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest);
                
                RemainsSource src = get_tile_remains_src(dungeon, render_pos, tileset_tile_width);
                if(src.found)
                {
                    render_texture_half_color(game->renderer, assets->tileset.tex, src.rect, dest);
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
was_pressed(InputState *state)
{
    b32 result = false;
    
    if(state->ended_down)
    {
#if MOONBREATH_SLOW
        if(is_fkey_active[FKey_F3])
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
process_input_event(InputState *state, b32 is_down)
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
            
            if(event.key.repeat == 0)
            {
                for(u32 index = 0; index < Key_Count; ++index)
                {
                    if(key_code == SDLK_p)
                    {
                        game->mode = GameMode_Quit;
                    }
                    else if(key_code == game->keybinds[index])
                    {
                        process_input_event(&input->keyboard[index], is_down);
                    }
                    
#if MOONBREATH_SLOW
                    else if(key_code == SDLK_F1)
                    {
                        process_input_event(&input->fkeys[FKey_F1], is_down);
                    }
                    else if(key_code == SDLK_F2)
                    {
                        process_input_event(&input->fkeys[FKey_F2], is_down);
                    }
                    else if(key_code == SDLK_F3)
                    {
                        process_input_event(&input->fkeys[FKey_F3], is_down);
                    }
                    else if(key_code == SDLK_F4)
                    {
                        process_input_event(&input->fkeys[FKey_F4], is_down);
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
        
        if(is_inside_rect(rect, input->mouse_pos))
        {
            render_text(game, "%sNew Game", 100, 340, assets->fonts[FontName_ClassicOutlined], 0, start_color(Color_Yellow));
            
            if(was_pressed(&input->mouse[Button_Left]))
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
            // Set randomized tiles for potions and scrolls.
            b32 is_potion_color_set[Potion_Count] = {0};
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                while(!consumable_data->potion_tiles[index].x &&
                      !consumable_data->potion_tiles[index].y)
                {
                    u32 potion_index = random_number(&game->random, 0, Potion_Count - 1);
                    if(!is_potion_color_set[potion_index])
                    {
                        switch(potion_index)
                        {
                            case 0: consumable_data->potion_tiles[index] = make_v2u(8, 1); break;
                            case 1: consumable_data->potion_tiles[index] = make_v2u(8, 2); break;
                            case 2: consumable_data->potion_tiles[index] = make_v2u(8, 3); break;
                            case 3: consumable_data->potion_tiles[index] = make_v2u(8, 4); break;
                            case 4: consumable_data->potion_tiles[index] = make_v2u(8, 5); break;
                            case 5: consumable_data->potion_tiles[index] = make_v2u(8, 6); break;
                            case 6: consumable_data->potion_tiles[index] = make_v2u(8, 7); break;
                            case 7: consumable_data->potion_tiles[index] = make_v2u(8, 8); break;
                            case 8: consumable_data->potion_tiles[index] = make_v2u(8, 9); break;
                            case 9: consumable_data->potion_tiles[index] = make_v2u(8, 10); break;
                            case 10: consumable_data->potion_tiles[index] = make_v2u(8, 11); break;
                            case 11: consumable_data->potion_tiles[index] = make_v2u(8, 12); break;
                            case 12: consumable_data->potion_tiles[index] = make_v2u(8, 13); break;
                            case 13: consumable_data->potion_tiles[index] = make_v2u(8, 14); break;
                            
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
                while(!consumable_data->scroll_tiles[index].x &&
                      !consumable_data->scroll_tiles[index].y)
                {
                    u32 scroll_index = random_number(&game->random, 0, Scroll_Count - 1);
                    if(!is_scroll_color_set[scroll_index])
                    {
                        switch(scroll_index)
                        {
                            case 0: consumable_data->scroll_tiles[index] = make_v2u(9, 1); break;
                            case 1: consumable_data->scroll_tiles[index] = make_v2u(9, 2); break;
                            case 2: consumable_data->scroll_tiles[index] = make_v2u(9, 3); break;
                            case 3: consumable_data->scroll_tiles[index] = make_v2u(9, 4); break;
                            case 4: consumable_data->scroll_tiles[index] = make_v2u(9, 5); break;
                            
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
                       consumable_data->potion_tiles[index].x,
                       consumable_data->potion_tiles[index].y);
            }
            
            printf("\nRandomized Scroll Tiles\n");
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                printf("[%u]: %u, %u\n", index,
                       consumable_data->scroll_tiles[index].x,
                       consumable_data->scroll_tiles[index].y);
            }
#endif
            
            // Set potion spawn chances.
            consumable_data->potion_spawn_chances[Potion_Might] = 50;
            consumable_data->potion_spawn_chances[Potion_Wisdom] = 50;
            consumable_data->potion_spawn_chances[Potion_Agility] = 50;
            consumable_data->potion_spawn_chances[Potion_Fortitude] = 30;
            consumable_data->potion_spawn_chances[Potion_Resistance] = 20;
            consumable_data->potion_spawn_chances[Potion_Healing] = 30;
            consumable_data->potion_spawn_chances[Potion_Focus] = 30;
            consumable_data->potion_spawn_chances[Potion_Curing] = 40;
            consumable_data->potion_spawn_chances[Potion_Flight] = 10;
            consumable_data->potion_spawn_chances[Potion_Decay] = 50;
            consumable_data->potion_spawn_chances[Potion_Weakness] = 30;
            consumable_data->potion_spawn_chances[Potion_Wounding] = 30;
            consumable_data->potion_spawn_chances[Potion_Venom] = 25;
            consumable_data->potion_spawn_chances[Potion_Confusion] = 25;
            
            // Set scroll spawn chances.
            consumable_data->scroll_spawn_chances[Scroll_Identify] = 30;
            consumable_data->scroll_spawn_chances[Scroll_EnchantWeapon] = 20;
            consumable_data->scroll_spawn_chances[Scroll_EnchantArmor] = 20;
            consumable_data->scroll_spawn_chances[Scroll_MagicMapping] = 10;
            consumable_data->scroll_spawn_chances[Scroll_Teleportation] = 25;
            
            // Set enemy levels.
            enemy_levels[EntityID_Dummy] = 0;
            
            // TODO(rami): Unused
            enemy_levels[EntityID_FrostShards] = 0;
            enemy_levels[EntityID_GreenMamba] = 0;
            
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
            
            enemy_levels[EntityID_GoblinWarrior] = 5;
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
            
            create_dungeon(game, dungeon, player, log, entities, items, consumable_data, enemy_levels);
            add_player_entity(game, player, items, inventory);
            update_fov(dungeon, player);
            
#if 0
            // Identify all items
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
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
        
        // TODO(rami): Inline?
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
    
    // All data below is required to be initialized to zero.
    GameState game = {0};
    Assets assets = {0};
    Entity entities[MAX_ENTITY_COUNT] = {0};
    Entity *player = &entities[0];
    u32 enemy_levels[EntityID_Count] = {0};
    Dungeon dungeon = {0};
    dungeon.tiles.array = calloc(1, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(Tile));
    Inventory inventory = {0};
    Item items[MAX_ITEM_COUNT] = {0};
    ConsumableData consumable_data = {0};
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
                            u64 seed = 13852174;
#endif
                            printf("Seed: %lu\n", seed);
                            
                            game.random = set_random_seed(seed);
                            game.mode = GameMode_Game;
                            //game.mode = GameMode_MainMenu;
                            
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
                            
                            for(u32 index = 0; index < FKey_Count; ++index)
                            {
                                old_input->fkeys[index].has_been_up = true;
                            }
#if MOONBREATH_SLOW
                            f32 actual_fps = 0.0f;
                            f32 actual_seconds_per_frame = 0.0f;
                            f32 work_seconds_per_frame = 0.0f;
                            
                            DebugState debug_state = {0};
                            //debug_state.selected_group_index = 1;
                            
                            DebugGroup *debug_variables = add_debug_group(&debug_state, "Variables", 25, 25, assets.fonts[FontName_ClassicOutlined]);
                            add_debug_float32(debug_variables, "FPS", &actual_fps);
                            add_debug_float32(debug_variables, "Frame MS", &actual_seconds_per_frame);
                            add_debug_float32(debug_variables, "Work MS", &work_seconds_per_frame);
                            add_debug_float32(debug_variables, "Frame DT", &new_input->frame_dt);
                            add_debug_uint32(debug_variables, "Mouse X", &new_input->mouse_pos.x);
                            add_debug_uint32(debug_variables, "Mouse Y", &new_input->mouse_pos.y);
                            
                            v2u mouse_final = {0};
                            add_debug_uint32(debug_variables, "Mouse Tile X", &mouse_final.x);
                            add_debug_uint32(debug_variables, "Mouse Tile Y", &mouse_final.y);
                            
                            add_debug_uint32(debug_variables, "Player Tile X", &player->pos.x);
                            add_debug_uint32(debug_variables, "Player Tile Y", &player->pos.y);
                            add_debug_bool32(debug_variables, "Debug Fov", &is_fkey_active[FKey_F1]);
                            add_debug_bool32(debug_variables, "Debug Traversable", &is_fkey_active[FKey_F2]);
                            add_debug_bool32(debug_variables, "Debug Has Been Up", &is_fkey_active[FKey_F3]);
                            
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
                            
                            while(game.mode)
                            {
#if MOONBREATH_SLOW
                                array_debug(items);
#endif
                                
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
                                
                                for(u32 index = 0; index < FKey_Count; ++index)
                                {
                                    new_input->fkeys[index].ended_down = old_input->fkeys[index].ended_down;
                                    new_input->fkeys[index].has_been_up = old_input->fkeys[index].has_been_up;
                                }
                                
                                process_events(&game, new_input);
                                
                                u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                process_input_event(&new_input->mouse[Button_Left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                process_input_event(&new_input->mouse[Button_Middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                process_input_event(&new_input->mouse[Button_Right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                process_input_event(&new_input->mouse[Button_Extended1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                process_input_event(&new_input->mouse[Button_Extended2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->frame_dt = (end_dt - last_dt) / (f32)performance_frequency;
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
