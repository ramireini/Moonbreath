#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

#include "types.h"
#include "random.c"
#include "util.c"
#include "dungeon.c"
#include "fov.c"
#include "item.c"
#include "assets.c"
#include "ui.c"
#include "pathfind.c"
// #include "conf.c" // TODO(rami): Work on conf when we need it again
#include "pop_text.c"
#include "entity.c"
#include "debug.c"

//NOTE(rami): Two Steps
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(rami): Need to make it so that when you equip a two-handed weapon,
// a worn shield will be unequipped.

// TODO(rami): Art and placing of status effects like being poisoned.

// TODO(rami): About fountains and corpses.
// The reason why you'd drink from a fountain or consume a corpse would be
// because you get something for it, otherwise it's just stuff that's useless.
// If we have the concept of food then these two things become useful.

// Another thing you could use corpses for could be sacrificing them for some gain
// on different altars, perhaps the different altars are to appease different
// deities?

// TODO(rami): Make different dungeon layouts that we can generate between.
// Figure out a way of just generally placing items
// on a level. Later we probably want item tiers so that certain items
// have a better chance to spawn on certain levels depending on their tier.
// After we have nice dungeon layouts, item tiers and item distribution
// we can test all of that together.
// Then we do the same for enemies, make sure we have a bunch of enemies
// have tiers and distribute them as well plus test everything at once.

// TODO(rami): Enemy corpses which you can pick up and eat if you want.
// Can have various effects depending on what was eaten.

// TODO(rami): More enemies and make sure distribution of them is good.
// TODO(rami): Give enemies stats.

// TODO(rami): Make sure distribution of items is good.
// TODO(rami): Give items stats.

// TODO(rami): After the ground work for the dungeon level layouts is done
// we can focus more on adding monsters, items, gold etc. to the levels.


// TODO(rami):
// We might want to keep resize_window if we want to be able to
// resize the game without restarting it.
#if 0
internal void
resize_window(u32 w, u32 h)
{
    SDL_SetWindowSize(game.window, w, h);
    game.window_size = V2u(w, h);
    game.console_size.w = game.window_size.w;
    game.camera = V4s(0, 0, game.window_size.w, game.window_size.h - game.console_size.h);
}
#endif

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
render_tilemap()
{
    SDL_SetRenderTarget(game.renderer, textures.tilemap.tex);
    SDL_RenderClear(game.renderer);
    
    v4u render_area =
    {
        tile_div(game.camera.x),
        tile_div(game.camera.y),
        tile_div(game.camera.x + game.camera.w),
        tile_div(game.camera.y + game.camera.h)
    };
    
    // NOTE(rami): If the dungeon w/h is less than
    // the w/h of the camera we can clamp the render area
    // to the w/h of the dungeon.
    if(tile_mul(dungeon.w) < game.camera.w)
    {
        render_area.w = dungeon.w - 1;
    }
    
    if(tile_mul(dungeon.h) < game.camera.h)
    {
        render_area.h = dungeon.h - 1;
    }
    
#if 0
    printf("render_area.x: %d\n", render_area.x);
    printf("render_area.y: %d\n", render_area.y);
    printf("render_area.w: %d\n", render_area.w);
    printf("render_area.h: %d\n\n", render_area.h);
#endif
    
    u32 tileset_tile_width = tile_div(textures.tileset.w);
    
    for(u32 y = render_area.y; y <= render_area.h; ++y)
    {
        for(u32 x = render_area.x; x <= render_area.w; ++x)
        {
            v2u tilemap = {x, y};
            v2u tile = v2u_from_index(get_dungeon_tile(tilemap), tileset_tile_width);
            
            v4u src = {tile_mul(tile.x), tile_mul(tile.y), 32, 32};
            v4u dest = {tile_mul(tilemap.x), tile_mul(tilemap.y), 32, 32};
            
            if(is_seen(tilemap))
            {
                SDL_SetTextureColorMod(textures.tileset.tex, 255, 255, 255);
                SDL_RenderCopy(game.renderer, textures.tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                u32 blood_value = get_dungeon_tile_blood(tilemap);
                if(blood_value)
                {
                    v2u blood_tile = v2u_from_index(blood_value, tileset_tile_width);
                    
                    v4u blood_src =
                    {
                        tile_mul(blood_tile.x),
                        tile_mul(blood_tile.y),
                        32,
                        32
                    };
                    
                    SDL_RenderCopy(game.renderer, textures.tileset.tex, (SDL_Rect *)&blood_src, (SDL_Rect *)&dest);
                }
            }
            else if(has_been_seen(tilemap))
            {
                SDL_SetTextureColorMod(textures.tileset.tex, 85, 85, 85);
                SDL_RenderCopy(game.renderer, textures.tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                u32 blood_value = get_dungeon_tile_blood(tilemap);
                if(blood_value)
                {
                    v2u blood_tile = v2u_from_index(blood_value, tileset_tile_width);
                    
                    v4u blood_src =
                    {
                        tile_mul(blood_tile.x),
                        tile_mul(blood_tile.y),
                        32,
                        32
                    };
                    
                    SDL_RenderCopy(game.renderer, textures.tileset.tex, (SDL_Rect *)&blood_src, (SDL_Rect *)&dest);
                }
            }
        }
    }
    
    SDL_SetRenderTarget(game.renderer, 0);
    
    v4u src = {game.camera.x, game.camera.y, game.camera.w, game.camera.h};
    v4u dest = {0, 0, game.camera.w, game.camera.h};
    SDL_RenderCopy(game.renderer, textures.tilemap.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
}

internal void
update_camera()
{
    game.camera.x = tile_mul(player->pos.x) - (game.camera.w / 2);
    
    if(game.window_size.w == 1280 &&
       game.window_size.h == 720)
    {
        game.camera.y = tile_mul(player->pos.y) - (game.camera.h / 2) + 16;
    }
    else if(game.window_size.w == 1920 &&
            game.window_size.h == 1080)
    {
        game.camera.y = tile_mul(player->pos.y) - (game.camera.h / 2) + 4;
    }
    
    if(game.camera.x < 0)
    {
        game.camera.x = 0;
    }
    
    if(game.camera.y < 0)
    {
        game.camera.y = 0;
    }
    
    // NOTE(rami): if statement is so that dungeons smaller than
    // the size of the camera will be rendered properly.
    if(tile_mul(dungeon.w) >= game.camera.w)
    {
        if(game.camera.x >= (s32)(tile_mul(dungeon.w) - game.camera.w))
        {
            game.camera.x = tile_mul(dungeon.w) - game.camera.w;
        }
        
        if(game.camera.y >= (s32)(tile_mul(dungeon.h) - game.camera.h))
        {
            game.camera.y = tile_mul(dungeon.h) - game.camera.h;
        }
    }
    
#if 0
    printf("camera.x1: %d\n", game.camera.x);
    printf("camera.y1: %d\n", game.camera.y);
    printf("camera.x2: %d\n", game.camera.x + game.camera.w);
    printf("camera.y2: %d\n", game.camera.y + game.camera.h);
    
    printf("camera.w: %u\n", game.camera.w);
    printf("camera.h: %u\n", game.camera.h);
    
    printf("camera_offset.x: %u\n", tile_div(game.camera.x));
    printf("camera_offset.y: %u\n\n", tile_div(game.camera.y));
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
process_events(input_state_t *keyboard)
{
    SDL_Event event = {0};
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game.state = state_exit;
        }
        else if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key_code = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(!event.key.repeat)
            {
                if(key_code == SDLK_p)
                {
                    game.state = state_exit;
                }
                else if(key_code == game.keybinds[key_move_up])
                {
                    process_input(&keyboard[key_move_up], is_down);
                }
                else if(key_code == game.keybinds[key_move_down])
                {
                    process_input(&keyboard[key_move_down], is_down);
                }
                else if(key_code == game.keybinds[key_move_left])
                {
                    process_input(&keyboard[key_move_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_right])
                {
                    process_input(&keyboard[key_move_right], is_down);
                }
                else if(key_code == game.keybinds[key_move_up_left])
                {
                    process_input(&keyboard[key_move_up_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_up_right])
                {
                    process_input(&keyboard[key_move_up_right], is_down);
                }
                else if(key_code == game.keybinds[key_move_down_left])
                {
                    process_input(&keyboard[key_move_down_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_down_right])
                {
                    process_input(&keyboard[key_move_down_right], is_down);
                }
                else if(key_code == game.keybinds[key_inventory])
                {
                    process_input(&keyboard[key_inventory], is_down);
                }
                else if(key_code == game.keybinds[key_pick_up_item])
                {
                    process_input(&keyboard[key_pick_up_item], is_down);
                }
                else if(key_code == game.keybinds[key_drop_item])
                {
                    process_input(&keyboard[key_drop_item], is_down);
                }
                else if(key_code == game.keybinds[key_equip_item])
                {
                    process_input(&keyboard[key_equip_item], is_down);
                }
                else if(key_code == game.keybinds[key_consume_item])
                {
                    process_input(&keyboard[key_consume_item], is_down);
                }
                else if(key_code == game.keybinds[key_move_item])
                {
                    process_input(&keyboard[key_move_item], is_down);
                }
                else if(key_code == game.keybinds[key_ascend])
                {
                    process_input(&keyboard[key_ascend], is_down);
                }
                else if(key_code == game.keybinds[key_descend])
                {
                    process_input(&keyboard[key_descend], is_down);
                }
                else if(key_code == game.keybinds[key_wait])
                {
                    process_input(&keyboard[key_wait], is_down);
                }
                
#if MOONBREATH_SLOW
                else if(key_code == SDLK_F1)
                {
                    process_input(&keyboard[key_debug_fov], is_down);
                }
                else if(key_code == SDLK_F2)
                {
                    process_input(&keyboard[key_debug_player_traversable_check], is_down);
                }
                else if(key_code == SDLK_F3)
                {
                    process_input(&keyboard[key_debug_has_been_up_check], is_down);
                }
#endif
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
    
    return(result);
}

internal b32
set_fonts()
{
    b32 result = true;
    
    fonts[font_classic] = create_bmp_font("data/fonts/classic16x16.png", 16, 14, 8, 13);
    fonts[font_classic_outlined] = create_bmp_font("data/fonts/classic_outlined16x16.png", 16, 14, 8, 13);
    fonts[font_alkhemikal] = create_ttf_font("data/fonts/alkhemikal.ttf", 16);
    fonts[font_monaco] = create_ttf_font("data/fonts/monaco.ttf", 16);
    fonts[font_dos_vga] = create_ttf_font("data/fonts/dos_vga.ttf", 16);
    
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
    
    textures.tilemap.w = tile_mul(MAX_DUNGEON_SIZE);
    textures.tilemap.h = tile_mul(MAX_DUNGEON_SIZE);
    textures.tilemap.tex = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textures.tilemap.w, textures.tilemap.h);
    
    textures.tileset = load_texture("data/images/tileset.png", 0);
    textures.item_tileset = load_texture("data/images/item_tileset.png", 0);
    textures.wearable_item_tileset = load_texture("data/images/wearable_item_tileset.png", 0);
    textures.sprite_sheet = load_texture("data/images/sprite_sheet.png", 0);
    
    textures.ui = load_texture("data/images/ui.png", 0).tex;
    textures.health_bar_outside = V4u(1716, 0, 204, 16);
    textures.health_bar_inside = V4u(1718, 20, 200, 12);
    
    if(game.window_size.w == 1280 &&
       game.window_size.h == 720)
    {
        textures.log_window = V4u(0, 342, 1280, 176);
    }
    else if(game.window_size.w == 1920 &&
            game.window_size.h == 1080)
    {
        textures.log_window = V4u(0, 522, 1920, 176);
    }
    
    textures.inventory_window = V4u(0, 0, 298, 338);
    textures.inventory_selected_slot = V4u(1716, 36, 32, 32);
    textures.inventory_equipped_slot = V4u(1752, 36, 32, 32);
    textures.item_window = V4u(302, 0, 274, 338);
    
    if(!textures.tileset.tex ||
       !textures.item_tileset.tex ||
       !textures.wearable_item_tileset.tex ||
       !textures.sprite_sheet.tex ||
       !textures.ui)
    {
        result = false;
        printf("ERROR: A texture could not be created.\n");
    }
    
    return(result);
}

internal u32
get_window_refresh_rate()
{
    u32 result = 60;
    SDL_DisplayMode mode = {0};
    
    u32 display_index = SDL_GetWindowDisplayIndex(game.window);
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
            printf("change_in_pos.x: %.02f\n", pop_text->change_in_pos.x);
            printf("change_in_pos.y: %.02f\n", pop_text->change_in_pos.y);
            printf("speed: %.02f\n", pop_text->speed);
            printf("duration_time: %ums\n", pop_text->duration_time);
            printf("start_time: %ums\n", pop_text->start_time);
        }
    }
#endif
    
    // NOTE(rami): Item
#if 0
    for(s32 i = array_count(items) - 1;
        i > -1;
        --i)
    {
        item_t *item = &items[i];
        if(item->id)
        {
            printf("\nitems[%u]\n", i);
            
            printf("id: %u\n", item->id);
            
            printf("name: %s\n", item->name);
            printf("description: %s\n", item->description);
            printf("tile.x, tile.y: %u, %u\n", item->tile.x, item->tile.y);
            printf("pos.x, pos.y: %u, %u\n", item->pos.x, item->pos.y);
            printf("slot: %u\n", item->slot);
            printf("type: %u\n", item->type);
            printf("handedness: %u\n", item->handedness);
            printf("primary_damage_type: %u\n", item->primary_damage_type);
            printf("secondary_damage_type: %u\n", item->secondary_damage_type);
            
            printf("enchantment_level: %d\n", item->enchantment_level);
            
            switch(item->type)
            {
                case item_type_weapon:
                {
                    printf("damage: %d\n", item->w.damage);
                    printf("accuracy: %d\n", item->w.accuracy);
                } break;
                
                case item_type_armor:
                {
                    printf("defence: %d\n", item->a.defence);
                    printf("weight: %d\n", item->a.weight);
                } break;
                
                case item_type_consumable:
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
    
    // TODO(rami): Print entity
}
#endif

internal void
update_and_render_game(game_input_t *input, f32 dt)
{
    if(game.state == state_main_menu)
    {
        set_render_color(color_cyan);
        v4u rect = {50, 300, 200, 100};
        SDL_RenderFillRect(game.renderer, (SDL_Rect *)&rect);
        
        if(is_in_rectangle(input->mouse_pos, rect))
        {
            render_text("##D New Game", 100, 340, fonts[font_classic_outlined], 0);
            
            if(is_input_valid(&input->mouse[button_left]))
            {
                game.state = state_in_game;
            }
        }
        else
        {
            render_text("New Game", 100, 340, fonts[font_classic_outlined], 0);
        }
    }
    else if(game.state == state_in_game)
    {
        // TODO(rami): When we go back to the main menu
        // if the player wins or dies, we need to set game.is_initialized to false.
        if(!game.is_initialized)
        {
            inventory.w = 8;
            inventory.h = 4;
            
            dungeon.level = 1;
            
            add_player_entity();
            player = &entities[0];
            
            generate_dungeon();
            update_fov();
            
            add_enemy_entity(entity_id_baby_slime, player->pos.x, player->pos.y + 1);
            
            add_weapon_item(item_dagger, item_rarity_common, player->pos.x + 1, player->pos.y);
            add_weapon_item(item_dagger, item_rarity_magical, player->pos.x + 2, player->pos.y);
            add_weapon_item(item_dagger, item_rarity_mythical, player->pos.x + 3, player->pos.y);
            
#if 0
            add_consumable_item(item_potion_of_might, player->pos.x + 1, player->pos.y);
            add_consumable_item(item_potion_of_wisdom, player->pos.x + 2, player->pos.y);
            add_consumable_item(item_potion_of_fortitude, player->pos.x + 3, player->pos.y);
            add_consumable_item(item_potion_of_agility, player->pos.x + 4, player->pos.y);
            add_consumable_item(item_potion_of_clumsiness, player->pos.x + 5, player->pos.y);
            add_consumable_item(item_potion_of_haste, player->pos.x + 6, player->pos.y);
            add_consumable_item(item_potion_of_resistance, player->pos.x + 7, player->pos.y);
            add_consumable_item(item_potion_of_mana, player->pos.x + 8, player->pos.y);
            add_consumable_item(item_potion_of_healing, player->pos.x + 9, player->pos.y);
            add_consumable_item(item_potion_of_flight, player->pos.x + 10, player->pos.y);
            add_consumable_item(item_potion_of_knowledge, player->pos.x + 11, player->pos.y);
            add_consumable_item(item_potion_of_poison, player->pos.x + 12, player->pos.y);
            add_consumable_item(item_potion_of_curing, player->pos.x + 13, player->pos.y);
            add_consumable_item(item_potion_of_vulnerability, player->pos.x + 14, player->pos.y);
            
            add_consumable_item(item_scroll_of_identify, player->pos.x + 1, player->pos.y + 1);
            add_consumable_item(item_scroll_of_brand_weapon, player->pos.x + 2, player->pos.y + 1);
            add_consumable_item(item_scroll_of_enchant_weapon, player->pos.x + 3, player->pos.y + 1);
            add_consumable_item(item_scroll_of_enchant_armor, player->pos.x + 4, player->pos.y + 1);
            add_consumable_item(item_scroll_of_magic_mapping, player->pos.x + 5, player->pos.y + 1);
#endif
            
            game.is_initialized = true;
        }
        
        printf("damage: %u\n", player->damage);
        printf("accuracy: %u\n\n", player->accuracy);
        
        // TODO(rami): Run some dungeon generation testing.
        
        // TODO(rami): When we gen a dungeon, we need to update
        // the player fov everytime, somehow.
        
        update_entities(input->keyboard);
        update_camera();
        
        render_tilemap();
        render_items();
        render_entities();
        update_and_render_pop_text(dt);
        render_ui();
        
#if 1
        v2u selection =
        {
            tile_div(input->mouse_pos.x),
            tile_div(input->mouse_pos.y)
        };
        
        v2u camera_offset =
        {
            tile_div(game.camera.x),
            tile_div(game.camera.y)
        };
        
        v4u rect =
        {
            tile_mul(selection.x),
            tile_mul(selection.y),
            32,
            32
        };
        
        // Logical result is
        // selection_pos.x + camera_offset.x,
        // selection_pos.y + camera_offset.y.
        
        v2u mouse_final =
        {
            selection.x + camera_offset.x,
            selection.y + camera_offset.y
        };
        
        if(selection.y < tile_div(game.camera.h))
        {
            set_render_color(color_yellow);
            SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&rect);
        }
#endif
    }
}

internal void
exit_game()
{
    free_assets();
    
    if(game.renderer)
    {
        SDL_DestroyRenderer(game.renderer);
    }
    
    if(game.window)
    {
        SDL_DestroyWindow(game.window);
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int
main(int argc, char *argv[])
{
    u32 result = 0;
    
    // TODO(rami): The keybinds and resolution would come from a config file.
#if 0
    u64 seed = time(0);
#else
    u64 seed = 1587001144;
#endif
    printf("Seed: %lu\n", seed);
    
    set_random_seed(seed);
    game.state = state_in_game;
    
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
    
    game.keybinds[key_inventory] = 'i';
    game.keybinds[key_pick_up_item] = ',';
    game.keybinds[key_drop_item] = '.';
    game.keybinds[key_equip_item] = 'b';
    game.keybinds[key_consume_item] = 'n';
    game.keybinds[key_move_item] = 'm';
    
    // TODO(rami): I think it would be nice to make moving
    // up or down a single key, like <.
    game.keybinds[key_ascend] = 'y';
    game.keybinds[key_descend] = 'u';
    game.keybinds[key_wait] = 'v';
    
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
            printf("Monitor refresh rate: %uHz\n\n", get_window_refresh_rate());
            
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
                                    game.camera = V4s(0, 0, game.window_size.w, game.window_size.h - textures.log_window.h);
                                    
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
                                    
                                    debug_group_t *debug_variables = add_debug_group(&debug_state, "Variables", 25, 25, fonts[font_classic_outlined]);
                                    add_debug_float32(debug_variables, "FPS", &actual_fps);
                                    add_debug_float32(debug_variables, "Frame MS", &actual_seconds_per_frame);
                                    add_debug_float32(debug_variables, "Work MS", &work_seconds_per_frame);
                                    add_debug_float32(debug_variables, "Frame DT", &new_input->dt);
                                    add_debug_uint32(debug_variables, "Mouse X", &new_input->mouse_pos.x);
                                    add_debug_uint32(debug_variables, "Mouse Y", &new_input->mouse_pos.y);
                                    add_debug_uint32(debug_variables, "Player Tile X", &entities[0].pos.x);
                                    add_debug_uint32(debug_variables, "Player Tile Y", &entities[0].pos.y);
                                    add_debug_bool32(debug_variables, "Fov", &debug_fov);
                                    add_debug_bool32(debug_variables, "Player Traversable", &debug_player_traversable);
                                    add_debug_bool32(debug_variables, "Has Been Up", &debug_has_been_up);
                                    
                                    debug_group_t *debug_colors = add_debug_group(&debug_state, "Colors", 150, 25, fonts[font_classic_outlined]);
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
                                        array_debug();
#endif
                                        set_render_color(color_black);
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
                                        
                                        process_events(new_input->keyboard);
                                        
                                        u32 mouse_state = SDL_GetMouseState(&new_input->mouse_pos.x, &new_input->mouse_pos.y);
                                        process_input(&new_input->mouse[button_left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
                                        process_input(&new_input->mouse[button_middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
                                        process_input(&new_input->mouse[button_right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
                                        process_input(&new_input->mouse[button_x1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
                                        process_input(&new_input->mouse[button_x2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
                                        
                                        f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                        new_input->dt = (end_dt - last_dt) / (f32)perf_count_frequency;
                                        last_dt = end_dt;
                                        
                                        update_and_render_game(new_input, new_input->dt);
                                        
#if MOONBREATH_SLOW
                                        u64 work_counter = SDL_GetPerformanceCounter();
                                        u64 work_elapsed_counter = work_counter - last_counter;
                                        work_seconds_per_frame = (1000.0f * (f64)work_elapsed_counter) / (f64)perf_count_frequency;
#endif
                                        
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
                                            // NOTE(rami): Missed frame rate.
                                        }
                                        
                                        u64 end_counter = SDL_GetPerformanceCounter();
                                        u64 elapsed_counter = end_counter - last_counter;
                                        last_counter = end_counter;
                                        
#if MOONBREATH_SLOW
                                        actual_fps = (f64)perf_count_frequency / (f64)elapsed_counter;
                                        actual_seconds_per_frame = (1000.0f * (f64)elapsed_counter) / (f64)perf_count_frequency;
                                        
                                        update_and_render_debug_state(new_input, &debug_state);
#endif
                                        
                                        game_input_t *temp = new_input;
                                        new_input = old_input;
                                        old_input = temp;
                                        
                                        SDL_RenderPresent(game.renderer);
                                    }
                                }
                                else
                                {
                                    // TODO(rami): Logging
                                    printf("ERROR: set_textures(): %s\n", SDL_GetError());
                                }
                            }
                            else
                            {
                                // TODO(rami): Logging
                                printf("ERROR: set_fonts(): %s\n", SDL_GetError());
                            }
                        }
                        else
                        {
                            // TODO(rami): Logging
                            printf("ERROR: set_window_icon(): %s\n", SDL_GetError());
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
    
    exit_game();
    return(result);
}