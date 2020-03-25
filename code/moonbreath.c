#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

#include "types.h"
#include "util.c"
#include "dungeon.c"
#include "fov.c"
#include "render.c"
#include "item.c"
#include "assets.c"
#include "ui.c"
#include "pathfind.c"
// #include "conf.c" // TODO(rami): Work on conf when we need it again
#include "pop_text.c"
#include "monster.c"
#include "player.c"
#include "debug.c"

// NOTE(rami): Two Steps
// Write the fastest, simplest way what you need, make it actually work.
// Can you clean it? Simplify it? Pull things into reusable functions? (Compression Oriented)

// TODO(rami): A quiver inventory slot for stuff like rocks?
// This could be used in the future for ranged things.

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

// TODO(rami): Prefab rooms?

// TODO(rami): Enemy corpses which you can pick up and eat if you want.
// Can have various effects depending on what was eaten.

// TODO(rami): Tiles with blood.

// TODO(rami): An idea of sub areas, so you have an enterance to the sub area and
// how it is generated is dependant on the current dungeon level.

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

internal void
update_camera()
{
    game.camera.x = tile_mul(player.pos.x) - (game.camera.w / 2);
    
    if(game.window_size.w == 1280 &&
       game.window_size.h == 720)
    {
        game.camera.y = tile_mul(player.pos.y) - (game.camera.h / 2) + 16;
    }
    else if(game.window_size.w == 1920 &&
            game.window_size.h == 1080)
    {
        game.camera.y = tile_mul(player.pos.y) - (game.camera.h / 2) + 4;
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
update_input(input_state_t *state, b32 is_down)
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
                    update_input(&keyboard[key_move_up], is_down);
                }
                else if(key_code == game.keybinds[key_move_down])
                {
                    update_input(&keyboard[key_move_down], is_down);
                }
                else if(key_code == game.keybinds[key_move_left])
                {
                    update_input(&keyboard[key_move_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_right])
                {
                    update_input(&keyboard[key_move_right], is_down);
                }
                else if(key_code == game.keybinds[key_move_up_left])
                {
                    update_input(&keyboard[key_move_up_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_up_right])
                {
                    update_input(&keyboard[key_move_up_right], is_down);
                }
                else if(key_code == game.keybinds[key_move_down_left])
                {
                    update_input(&keyboard[key_move_down_left], is_down);
                }
                else if(key_code == game.keybinds[key_move_down_right])
                {
                    update_input(&keyboard[key_move_down_right], is_down);
                }
                else if(key_code == game.keybinds[key_inventory])
                {
                    update_input(&keyboard[key_inventory], is_down);
                }
                else if(key_code == game.keybinds[key_pick_up])
                {
                    update_input(&keyboard[key_pick_up], is_down);
                }
                else if(key_code == game.keybinds[key_drop])
                {
                    update_input(&keyboard[key_drop], is_down);
                }
                else if(key_code == game.keybinds[key_equip])
                {
                    update_input(&keyboard[key_equip], is_down);
                }
                else if(key_code == game.keybinds[key_consume])
                {
                    update_input(&keyboard[key_consume], is_down);
                }
                else if(key_code == game.keybinds[key_move])
                {
                    update_input(&keyboard[key_move], is_down);
                }
                else if(key_code == game.keybinds[key_ascend])
                {
                    update_input(&keyboard[key_ascend], is_down);
                }
                else if(key_code == game.keybinds[key_descend])
                {
                    update_input(&keyboard[key_descend], is_down);
                }
                
#if MOONBREATH_SLOW
                else if(key_code == SDLK_F1)
                {
                    update_input(&keyboard[key_debug_fov], is_down);
                }
                else if(key_code == SDLK_F2)
                {
                    update_input(&keyboard[key_debug_player_traversable_check], is_down);
                }
                else if(key_code == SDLK_F3)
                {
                    update_input(&keyboard[key_debug_has_been_up_check], is_down);
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
    textures.health_bar_outside = V4u(558, 0, 204, 24);
    textures.health_bar_inside = V4u(558, 28, 200, 20);
    
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
    textures.inventory_selected_slot = V4u(558, 52, 32, 32);
    textures.inventory_equipped_slot = V4u(594, 52, 32, 32);
    textures.item_window = V4u(302, 0, 252, 338);
    
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

internal void
set_game_data()
{
#if 1
    u64 seed = 16371218;
#else
    u64 seed = time(0);
#endif
    
    srand(seed);
    printf("Random Seed: %lu\n\n", seed);
    
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
    game.keybinds[key_pick_up] = ',';
    game.keybinds[key_drop] = '.';
    game.keybinds[key_equip] = 'b';
    game.keybinds[key_consume] = 'n';
    game.keybinds[key_move] = 'm';
    game.keybinds[key_ascend] = 'y';
    game.keybinds[key_descend] = 'u';
    
    inventory.w = 8;
    inventory.h = 4;
    
    dungeon.level = 1;
    
    strcpy(player.name, "Name");
    player.w = 32;
    player.h = 32;
    player.max_hp = 10;
    player.hp = 5;
    player.strength = 10;
    player.intelligence = 10;
    player.dexterity = 10;
    player.evasion = 10;
    player.move_speed = 1;
    player.fov = 7;
    
    { // Set Monster Info
        for(u32 monster_index = 0;
            monster_index < array_count(monsters);
            ++monster_index)
        {
            monsters[monster_index].unique_id = monster_index + 1;
        }
        
        u32 monster_info_index = 0;
        
        monster_info_index = add_monster_info(monster_info_index, "Baby Slime", 32, 32, 1, 4, 1, 1, 4, 1, 1, 1, 0);
        monster_info_index = add_monster_info(monster_info_index, "Slime", 32, 32, 1, 4, 1, 1, 4, 1, 1, 1, 0);
        monster_info_index = add_monster_info(monster_info_index, "Cave Bat", 32, 32, 1, 4, 1, 1, 14, 1, 1, 3, 0);
        monster_info_index = add_monster_info(monster_info_index, "Python", 32, 32, 1, 4, 1, 1, 10, 1, 1, 4, 0);
        monster_info_index = add_monster_info(monster_info_index, "Skeleton", 32, 32, 1, 4, 1, 1, 6, 1, 1, 5, 0);
        monster_info_index = add_monster_info(monster_info_index, "Armored Skeleton", 32, 32, 1, 4, 1, 1, 6, 1, 1, 6, 0);
        monster_info_index = add_monster_info(monster_info_index, "Orc Warrior", 32, 32, 1, 4, 1, 1, 8, 1, 1, 7, 0);
        monster_info_index = add_monster_info(monster_info_index, "Kobold", 32, 32, 1, 4, 1, 1, 8, 1, 1, 8, 0);
        monster_info_index = add_monster_info(monster_info_index, "Ogre", 32, 32, 1, 4, 1, 1, 2, 1, 1, 9, 0);
        
        // TODO(rami): Fix Treant and Wolf art.
        monster_info_index = add_monster_info(monster_info_index, "Tormentor", 32, 32, 1, 4, 1, 1, 0, 1, 1, 10, 0);
        monster_info_index = add_monster_info(monster_info_index, "Imp", 32, 32, 1, 4, 1, 1, 0, 1, 1, 11, 0);
        monster_info_index = add_monster_info(monster_info_index, "Giant Demon", 32, 32, 1, 4, 1, 1, 0, 1, 1, 12, 0);
        monster_info_index = add_monster_info(monster_info_index, "Hellhound", 32, 32, 1, 4, 1, 1, 0, 1, 1, 13, 0);
        monster_info_index = add_monster_info(monster_info_index, "Undead Elf Warrior", 32, 32, 1, 4, 1, 1, 0, 1, 1, 14, 0);
        monster_info_index = add_monster_info(monster_info_index, "Dark Thief", 32, 32, 1, 4, 1, 1, 0, 1, 1, 15, 0);
        monster_info_index = add_monster_info(monster_info_index, "Goblin", 32, 32, 1, 4, 1, 1, 0, 1, 1, 16, 0);
        monster_info_index = add_monster_info(monster_info_index, "Viper", 32, 32, 1, 4, 1, 1, 0, 1, 1, 17, 0);
        monster_info_index = add_monster_info(monster_info_index, "Scarlet Kingsnake", 32, 32, 1, 4, 1, 1, 1, 1, 1, 18, 0);
        monster_info_index = add_monster_info(monster_info_index, "Dog", 32, 32, 1, 4, 1, 1, 1, 1, 1, 19, 0);
        monster_info_index = add_monster_info(monster_info_index, "Green Mamba", 32, 32, 1, 4, 1, 1, 1, 1, 1, 20, 0);
        monster_info_index = add_monster_info(monster_info_index, "Wolf", 32, 32, 1, 4, 1, 1, 1, 1, 1, 21, 0);
        monster_info_index = add_monster_info(monster_info_index, "Goblin Warrior", 32, 32, 1, 4, 1, 1, 1, 1, 1, 22, 0);
        monster_info_index = add_monster_info(monster_info_index, "Floating Eye", 32, 32, 1, 4, 1, 1, 1, 1, 1, 23, 0);
        monster_info_index = add_monster_info(monster_info_index, "Devourer", 32, 32, 1, 4, 1, 1, 1, 1, 1, 24, 0);
        monster_info_index = add_monster_info(monster_info_index, "Ghoul", 32, 32, 1, 4, 1, 1, 1, 1, 1, 25, 0);
        monster_info_index = add_monster_info(monster_info_index, "Cyclops", 32, 32, 1, 4, 1, 1, 1, 1, 1, 26, 0);
        monster_info_index = add_monster_info(monster_info_index, "Dwarven Warrior", 32, 32, 1, 4, 1, 1, 1, 1, 1, 27, 0);
        monster_info_index = add_monster_info(monster_info_index, "Black Knight", 32, 32, 1, 4, 1, 1, 1, 1, 1, 28, 0);
        monster_info_index = add_monster_info(monster_info_index, "Cursed Black Knight", 32, 32, 1, 4, 1, 1, 1, 1, 1, 29, 0);
        monster_info_index = add_monster_info(monster_info_index, "Treant", 32, 32, 1, 4, 1, 1, 1, 1, 1, 30, 0);
        monster_info_index = add_monster_info(monster_info_index, "Minotaur", 32, 32, 1, 4, 1, 1, 1, 1, 1, 31, 0);
        monster_info_index = add_monster_info(monster_info_index, "Centaur Warrior", 32, 32, 1, 4, 1, 1, 1, 1, 1, 32, 0);
        monster_info_index = add_monster_info(monster_info_index, "Centaur", 32, 32, 1, 4, 1, 1, 1, 1, 1, 33, 0);
        monster_info_index = add_monster_info(monster_info_index, "Frost Shards", 32, 32, 1, 4, 1, 1, 1, 1, 1, 34, 0);
        monster_info_index = add_monster_info(monster_info_index, "Frost Walker", 32, 32, 1, 4, 1, 1, 1, 1, 1, 35, 0);
        monster_info_index = add_monster_info(monster_info_index, "Griffin", 32, 32, 1, 4, 1, 1, 1, 1, 1, 36, 0);
        monster_info_index = add_monster_info(monster_info_index, "Spectre", 32, 32, 1, 4, 1, 1, 1, 1, 1, 37, 0);
        monster_info_index = add_monster_info(monster_info_index, "Flying Skull", 32, 32, 1, 4, 1, 1, 1, 1, 1, 38, 0);
        monster_info_index = add_monster_info(monster_info_index, "Brimstone Imp", 32, 32, 1, 4, 1, 1, 1, 1, 1, 39, 0);
    }
    
    { // Set Item Info
        for(u32 item_index = 0;
            item_index < array_count(items);
            ++item_index)
        {
            items[item_index].unique_id = item_index + 1;
        }
        
        u32 item_info_index = 0;
        
        // Head
        item_info_index = add_item_info(item_info_index, "Steel Visage", slot_head, "", 0, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Demonic Greathelm", slot_head, "", 0, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Crusaders Helmet", slot_head, "", 0, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Haniara's Mask", slot_head, "", 0, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Hood of Shadows", slot_head, "", 0, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Hood of Swiftness", slot_head, "", 0, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Hardleather Helmet", slot_head, "", 0, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Kings Crown", slot_head, "", 0, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Watchers Gaze", slot_head, "", 0, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Body
        item_info_index = add_item_info(item_info_index, "Imperial Platebody", slot_body, "", 1, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Knight's Chestguard", slot_body, "", 1, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Engraved Chestpiece", slot_body, "", 1, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Steel Chainmail", slot_body, "", 1, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Mithril Chainmail", slot_body, "", 1, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Sturdy Leather Shirt", slot_body, "", 1, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Green Leather Vest", slot_body, "", 1, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Fine Clotch Shirt", slot_body, "", 1, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Holy Garb", slot_body, "", 1, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Legs
        item_info_index = add_item_info(item_info_index, "Leather Trousers", slot_legs, "", 2, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Hardened Protectors", slot_legs, "", 2, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Savage Guardians", slot_legs, "", 2, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Unwavering Platelegs", slot_legs, "", 2, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Threaded Greaves", slot_legs, "", 2, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Hunters Pants", slot_legs, "", 2, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Bronze Platelegs", slot_legs, "", 2, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Fine Legwraps", slot_legs, "", 2, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Moonlight Trousers", slot_legs, "", 2, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Feet
        item_info_index = add_item_info(item_info_index, "Rugged Boots", slot_feet, "", 3, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Challenger's Threads", slot_feet, "", 3, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Mithril Deflectors", slot_feet, "", 3, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Infernal Greaves", slot_feet, "", 3, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Aspiring Boots", slot_feet, "", 3, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Soldier's Boots", slot_feet, "", 3, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Sturdy Walkers", slot_feet, "", 3, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Steps of Discipline", slot_feet, "", 3, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Irontoe Boots", slot_feet, "", 3, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // First Hand
        item_info_index = add_item_info(item_info_index, "Ceremonial Dagger", slot_first_hand, "Weapon 1", 4, 2, type_weapon, 1, 2, 1, 1, 1, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Katana", slot_first_hand, "Weapon 2", 4, 3, type_weapon, 8, 9, 9, 9, 9, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Broadsword", slot_first_hand, "", 4, 1, type_weapon, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Battle Edge", slot_first_hand, "", 4, 4, type_weapon, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Jungle Cleaver", slot_first_hand, "", 4, 5, type_weapon, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Piercing Advance", slot_first_hand, "Move forward and never stop. You'll die if you hesitate.", 4, 6, type_weapon, 2, 4, 1, 1, 1, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Raging Skullcleaver", slot_first_hand, "", 4, 7, type_weapon, 3, 4, 1, 1, 1, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Heavens Reflection", slot_first_hand, "", 4, 8, type_weapon, 3, 4, 1, 1, 1, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Evil Intent", slot_first_hand, "", 4, 9, type_weapon, 3, 4, 1, 1, 1, effect_none, "", 0);
        
        // Second Hand
        item_info_index = add_item_info(item_info_index, "Soldier's Heater", slot_second_hand, "", 5 ,1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ironwood Buckler", slot_second_hand, "", 5, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Wall of Honor", slot_second_hand, "", 5, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Crystal Shield", slot_second_hand, "", 5, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Knight's Kite Shield", slot_second_hand, "", 5, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Jaded Aegis", slot_second_hand, "", 5, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Glacier", slot_second_hand, "", 5, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Radiant Crest", slot_second_hand, "", 5, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Adamantite Barrier", slot_second_hand, "", 5, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Amulet
        item_info_index = add_item_info(item_info_index, "Brave Pendant", slot_amulet, "", 6, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Dark Heart", slot_amulet, "", 6, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Last Echo", slot_amulet, "", 6, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Majestic Pendant", slot_amulet, "", 6, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Holy Grace", slot_amulet, "", 6, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Pendant of Thorns", slot_amulet, "", 6, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Soul Siphon", slot_amulet, "", 6, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Calm Gem Necklace", slot_amulet, "", 6, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Pure Gem Necklace", slot_amulet, "", 6, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Ring
        item_info_index = add_item_info(item_info_index, "Ring of Protection", slot_ring, "", 7, 1, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Obsidian Ring", slot_ring, "", 7, 2, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Scorching Ring", slot_ring, "", 7, 3, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ring of Fortitude", slot_ring, "", 7, 4, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ring of Sight", slot_ring, "", 7, 5, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ring of Avarice", slot_ring, "", 7, 6, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ring of Pain", slot_ring, "", 7, 7, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Rusty Ring", slot_ring, "", 7, 8, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        item_info_index = add_item_info(item_info_index, "Ring of Thorns", slot_ring, "", 7, 9, type_armor, 0, 0, 0, 0, 0, effect_none, "", 0);
        
        // Other
        item_info_index = add_item_info(item_info_index, "Small Health Potion", slot_none, "Heal 1", 8, 1, type_consumable, 0, 0, 0, 0, 0, effect_heal, "Restores 2 health", 2);
        item_info_index = add_item_info(item_info_index, "Medium Health Potion", slot_none, "Heal 2", 8, 2, type_consumable, 0, 0, 0, 0, 0, effect_heal, "Restores 4 health", 4);
        item_info_index = add_item_info(item_info_index, "Large Health Potion", slot_none, "Heal 3", 8, 3, type_consumable, 0, 0, 0, 0, 0, effect_heal, "Restores 6 health", 6);
    }
}

internal u32
initialize_game()
{
    b32 result = false;
    
    set_game_data();
    
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
                                    
                                    game.camera = V4s(0,
                                                      0,
                                                      game.window_size.w - 32*12,
                                                      game.window_size.h - textures.log_window.h);
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
    
    // NOTE(rami): Inventory
#if 0
    for(s32 i = (inventory.w * inventory.h) - 1; i > -1; --i)
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
    
#if 0
    // NOTE(rami): Item Info
    for(s32 item_info_index = array_count(item_information) - 1;
        item_info_index > -1;
        --item_info_index)
    {
        item_info_t *item_info = &item_info[info_index];
        if(item_info->id)
        {
            printf("\nitem_info[%u]\n", item_info_index);
            printf("id: %u\n", item_info->id);
            printf("name: %s\n", item_info->name);
            printf("slot: %u\n", item_info->slot);
            printf("DESCRIPTION SKIPPED\n");
            printf("tile: %u, %u\n", item_info->tile.x, item_info->tile.y);
            printf("info->type: %u\n", item_info->type);
            
            if(item_info->type == type_weapon || item_info->type == type_armor)
            {
                printf("min_damage: %u\n", item_info->stats.min_damage);
                printf("max_damage: %u\n", item_info->stats.max_damage);
                
                printf("strength: %u\n", item_info->stats.strength);
                printf("defence: %u\n", item_info->stats.defence);
                printf("vitality: %u\n", item_info->stats.vitality);
            }
            else if(item_info->type == type_consumable)
            {
                printf("effect: %u\n", item_info->consumable.effect);
                printf("effect_text: %s\n", item_info->consumable.effect_text);
                printf("effect_amount: %u\n", item_info->consumable.effect_amount);
            }
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
    printf("move_speed: %u\n", player.move_speed);
    printf("level: %u\n", player.level);
    printf("money: %u\n", player.money);
    printf("fov: %u\n", player.fov);
#endif
    
    // NOTE(rami): Monster
#if 0
    for(s32 i = array_count(monsters) - 1; i > -1; --i)
    {
        monster_t *monster = &monsters[i];
        if(monster->id)
        {
            printf("\nmonster[%u]\n", i);
            printf("id: %u\n", monster->id);
            printf("unique_id: %u\n", monster->unique_id);
            printf("ai: %u\n", monster->ai);
            printf("hp: %u\n", monster->hp);
            printf("max_hp: %u\n", monster->max_hp);
            printf("pos: %u, %u\n", monster->pos.x, monster->pos.y);
            printf("new_pos: %u, %u\n", monster->new_pos.x, monster->new_pos.y);
            printf("tile_flipped: %u\n", monster->tile_flipped);
            printf("in_combat: %u\n", monster->in_combat);
            printf("has_attacked: %u\n", monster->has_attacked);
        }
    }
#endif
    
#if 0
    // NOTE(rami): Monster Info
    for(s32 info_index = array_count(monster_info) - 1;
        info_index > -1;
        --info_index)
    {
        monster_info_t *monster_info = &monster_info[info_index];
        if(info->id)
        {
            printf("\nmonster_info[%u]\n", monster_info_index);
            printf("name: %s\n", monster_info->name);
            printf("w, h: %u, %u\n", monster_info->w, monster_info->h);
            printf("level: %u\n", monster_info->level);
            printf("max_hp: %u\n", monster_info->max_hp);
            printf("damage: %u\n", monster_info->damage);
            printf("armor: %u\n", monster_info->armor);
            printf("attack_speed: %u\n", monster_info->attack_speed);
            printf("move_speed: %u\n", monster_info->move_speed);
            printf("tile: %u, %u\n", monster_info->tile.x, monster_info->tile.y);
        }
    }
#endif
}

#endif

internal void
run_game()
{
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
    
    debug_group_t *debug_variables = add_debug_group(&debug_state, "Variables", 25, 25, color_white, fonts[font_classic_outlined]);
    add_debug_float32(debug_variables, "FPS", &actual_fps, color_white);
    add_debug_float32(debug_variables, "Frame MS", &actual_seconds_per_frame, color_white);
    add_debug_float32(debug_variables, "Work MS", &work_seconds_per_frame, color_white);
    add_debug_float32(debug_variables, "Frame DT", &game.dt, color_white);
    add_debug_uint32(debug_variables, "Mouse X", &new_input->mouse_pos.x, color_white);
    add_debug_uint32(debug_variables, "Mouse Y", &new_input->mouse_pos.y, color_white);
    add_debug_uint32(debug_variables, "Player X", &player.pos.x, color_white);
    add_debug_uint32(debug_variables, "Player Y", &player.pos.y, color_white);
    add_debug_bool32(debug_variables, "Fov", &debug_fov, color_white);
    add_debug_bool32(debug_variables, "Player Traversable", &debug_player_traversable, color_white);
    add_debug_bool32(debug_variables, "Has Been Up", &debug_has_been_up, color_white);
    
    debug_group_t *debug_colors = add_debug_group(&debug_state, "Colors", 150, 25, color_white, fonts[font_classic_outlined]);
    add_debug_text(debug_colors, "Gray", color_gray);
    add_debug_text(debug_colors, "White", color_white);
    add_debug_text(debug_colors, "Red", color_red);
    add_debug_text(debug_colors, "Dark Red", color_dark_red);
    add_debug_text(debug_colors, "Green", color_green);
    add_debug_text(debug_colors, "Blue", color_blue);
    add_debug_text(debug_colors, "Yellow", color_yellow);
    add_debug_text(debug_colors, "Orange", color_orange);
    add_debug_text(debug_colors, "Brown", color_brown);
    add_debug_text(debug_colors, "Light Brown", color_light_brown);
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
        update_input(&new_input->mouse[button_left], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
        update_input(&new_input->mouse[button_middle], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
        update_input(&new_input->mouse[button_right], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
        update_input(&new_input->mouse[button_x1], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
        update_input(&new_input->mouse[button_x2], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
        
        f32 end_dt = (f32)SDL_GetPerformanceCounter();
        game.dt = (end_dt - last_dt) / (f32)perf_count_frequency;
        last_dt = end_dt;
        
        if(game.state == state_main_menu)
        {
            set_render_color(color_blue);
            v4u rect = {50, 300, 200, 100};
            SDL_RenderFillRect(game.renderer, (SDL_Rect *)&rect);
            
            if(is_in_rectangle(new_input->mouse_pos, rect))
            {
                render_text("New Game", 100, 340, color_yellow, fonts[font_classic_outlined], 0);
                
                if(is_input_valid(&new_input->mouse[button_left]))
                {
                    game.state = state_in_game;
                }
            }
            else
            {
                render_text("New Game", 100, 340, color_white, fonts[font_classic_outlined], 0);
            }
        }
        else if(game.state == state_in_game)
        {
            // TODO(rami): When we go back to the main menu
            // if the player wins or dies, we need to set game.is_initialized to false.
            if(!game.initialized)
            {
                generate_dungeon();
                update_fov();
                
#if 0
                // Head
                add_item(item_steel_visage, player.pos.x, player.pos.y + 2);
                add_item(item_demonic_greathelm, player.pos.x + 1, player.pos.y + 2);
                add_item(item_crusaders_helmet, player.pos.x + 2, player.pos.y + 2);
                add_item(item_haniaras_mask, player.pos.x + 3, player.pos.y + 2);
                add_item(item_hood_of_shadows, player.pos.x + 4, player.pos.y + 2);
                add_item(item_hood_of_swiftness, player.pos.x + 5, player.pos.y + 2);
                add_item(item_hardleather_helmet, player.pos.x + 6, player.pos.y + 2);
                add_item(item_kings_crown, player.pos.x + 7, player.pos.y + 2);
                add_item(item_watchers_gaze, player.pos.x + 8, player.pos.y + 2);
                
                // Body
                add_item(item_imperial_platebody, player.pos.x, player.pos.y + 4);
                add_item(item_knights_chestguard, player.pos.x + 1, player.pos.y + 4);
                add_item(item_engraved_chestpiece, player.pos.x + 2, player.pos.y + 4);
                add_item(item_steel_chainmain, player.pos.x + 3, player.pos.y + 4);
                add_item(item_mithril_chainmail, player.pos.x + 4, player.pos.y + 4);
                add_item(item_sturdy_leather_shirt, player.pos.x + 5, player.pos.y + 4);
                add_item(item_green_leather_vest, player.pos.x + 6, player.pos.y + 4);
                add_item(item_fine_clotch_shirt, player.pos.x + 7, player.pos.y + 4);
                add_item(item_holy_garb, player.pos.x + 8, player.pos.y + 4);
                
                // Legs
                add_item(item_leather_trousers, player.pos.x, player.pos.y + 6);
                add_item(item_hardened_protectors, player.pos.x + 1, player.pos.y + 6);
                add_item(item_savage_guardians, player.pos.x + 2, player.pos.y + 6);
                add_item(item_unwavering_platelegs, player.pos.x + 3, player.pos.y + 6);
                add_item(item_threaded_graves, player.pos.x + 4, player.pos.y + 6);
                add_item(item_hunters_pants, player.pos.x + 5, player.pos.y + 6);
                add_item(item_bronze_platelegs, player.pos.x + 6, player.pos.y + 6);
                add_item(item_fine_legwraps, player.pos.x + 7, player.pos.y + 6);
                add_item(item_moonlight_trousers, player.pos.x + 8, player.pos.y + 6);
                
                // Feet
                add_item(item_rugged_boots, player.pos.x, player.pos.y + 8);
                add_item(item_challengers_threads, player.pos.x + 1, player.pos.y + 8);
                add_item(item_mithril_deflectors, player.pos.x + 2, player.pos.y + 8);
                add_item(item_infernal_greaves, player.pos.x + 3, player.pos.y + 8);
                add_item(item_aspiring_boots, player.pos.x + 4, player.pos.y + 8);
                add_item(item_soldiers_boots, player.pos.x + 5, player.pos.y + 8);
                add_item(item_sturdy_walkers, player.pos.x + 6, player.pos.y + 8);
                add_item(item_steps_of_discipline, player.pos.x + 7, player.pos.y + 8);
                add_item(item_irontoe_boots, player.pos.x + 8, player.pos.y + 8);
                
                // First hand
                add_item(item_ceremonial_dagger, player.pos.x, player.pos.y + 10);
                add_item(item_katana, player.pos.x + 1, player.pos.y + 10);
                add_item(item_broadsword, player.pos.x + 2, player.pos.y + 10);
                add_item(item_battle_edge, player.pos.x + 3, player.pos.y + 10);
                add_item(item_jungle_cleaver, player.pos.x + 4, player.pos.y + 10);
                add_item(item_piercing_advance, player.pos.x + 5, player.pos.y + 10);
                add_item(item_raging_skullcleaver, player.pos.x + 6, player.pos.y + 10);
                add_item(item_heavens_reflection, player.pos.x + 7, player.pos.y + 10);
                add_item(item_evil_intent, player.pos.x + 8, player.pos.y + 10);
                
                // Second hand
                add_item(item_soldiers_heater, player.pos.x, player.pos.y + 12);
                add_item(item_ironwood_buckler, player.pos.x + 1, player.pos.y + 12);
                add_item(item_wall_of_honor, player.pos.x + 2, player.pos.y + 12);
                add_item(item_crystal_shield, player.pos.x + 3, player.pos.y + 12);
                add_item(item_knights_kite_shield, player.pos.x + 4, player.pos.y + 12);
                add_item(item_jaded_aegis, player.pos.x + 5, player.pos.y + 12);
                add_item(item_glacier, player.pos.x + 6, player.pos.y + 12);
                add_item(item_radiant_crest, player.pos.x + 7, player.pos.y + 12);
                add_item(item_adamantite_barrier, player.pos.x + 8, player.pos.y + 12);
                
                // Amulet
                add_item(item_brave_pendant, player.pos.x, player.pos.y + 14);
                add_item(item_dark_heart, player.pos.x + 1, player.pos.y + 14);
                add_item(item_last_echo, player.pos.x + 2, player.pos.y + 14);
                add_item(item_majestic_pendant, player.pos.x + 3, player.pos.y + 14);
                add_item(item_holy_grace, player.pos.x + 4, player.pos.y + 14);
                add_item(item_pendant_of_thorns, player.pos.x + 5, player.pos.y + 14);
                add_item(item_soul_siphon, player.pos.x + 6, player.pos.y + 14);
                add_item(item_calm_gem_necklace, player.pos.x + 7, player.pos.y + 14);
                add_item(item_pure_gem_necklace, player.pos.x + 8, player.pos.y + 14);
                
                // Ring
                add_item(item_ring_of_protection, player.pos.x, player.pos.y + 16);
                add_item(item_obsidian_ring, player.pos.x + 1, player.pos.y + 16);
                add_item(item_scorching_ring, player.pos.x + 2, player.pos.y + 16);
                add_item(item_ring_of_fortitude, player.pos.x + 3, player.pos.y + 16);
                add_item(item_ring_of_sight, player.pos.x + 4, player.pos.y + 16);
                add_item(item_ring_of_avarice, player.pos.x + 5, player.pos.y + 16);
                add_item(item_ring_of_pain, player.pos.x + 6, player.pos.y + 16);
                add_item(item_rusty_ring, player.pos.x + 7, player.pos.y + 16);
                add_item(item_ring_of_thorns, player.pos.x + 8, player.pos.y + 16);
                
                // Other
                add_item(item_small_health_potion, player.pos.x, player.pos.y + 18);
                add_item(item_medium_health_potion, player.pos.x + 1, player.pos.y + 18);
                add_item(item_large_health_potion, player.pos.x + 2, player.pos.y + 18);
#endif
                
                add_monster(monster_baby_slime, player.pos.x + 1, player.pos.y);
#if 0
                add_monster(monster_baby_slime, player.pos.x + 10, player.pos.y + 1);
                add_monster(monster_slime, player.pos.x + 11, player.pos.y + 1);
                add_monster(monster_cave_bat, player.pos.x + 12, player.pos.y + 1);
                add_monster(monster_python, player.pos.x + 13, player.pos.y + 1);
                add_monster(monster_skeleton, player.pos.x + 14, player.pos.y + 1);
                add_monster(monster_armored_skeleton, player.pos.x + 15, player.pos.y + 1);
                add_monster(monster_orc_warrior, player.pos.x + 16, player.pos.y + 1);
                add_monster(monster_kobold, player.pos.x + 17, player.pos.y + 1);
                add_monster(monster_ogre, player.pos.x + 18, player.pos.y + 1);
                add_monster(monster_test_1, player.pos.x + 19, player.pos.y + 1);
                add_monster(monster_test_2, player.pos.x + 20, player.pos.y + 1);
                add_monster(monster_test_3, player.pos.x + 21, player.pos.y + 1);
                add_monster(monster_test_4, player.pos.x + 22, player.pos.y + 1);
                add_monster(monster_test_5, player.pos.x + 23, player.pos.y + 1);
                add_monster(monster_test_6, player.pos.x + 24, player.pos.y + 1);
                add_monster(monster_test_7, player.pos.x + 25, player.pos.y + 1);
                add_monster(monster_test_8, player.pos.x + 26, player.pos.y + 1);
                add_monster(monster_test_9, player.pos.x + 27, player.pos.y + 1);
                add_monster(monster_test_10, player.pos.x + 28, player.pos.y + 1);
                add_monster(monster_test_11, player.pos.x + 29, player.pos.y + 1);
                add_monster(monster_test_12, player.pos.x + 30, player.pos.y + 1);
                add_monster(monster_test_13, player.pos.x + 31, player.pos.y + 1);
                add_monster(monster_test_14, player.pos.x + 32, player.pos.y + 1);
                add_monster(monster_test_15, player.pos.x + 33, player.pos.y + 1);
                add_monster(monster_test_16, player.pos.x + 34, player.pos.y + 1);
                add_monster(monster_test_17, player.pos.x + 35, player.pos.y + 1);
                add_monster(monster_test_18, player.pos.x + 36, player.pos.y + 1);
                add_monster(monster_test_19, player.pos.x + 37, player.pos.y + 1);
                add_monster(monster_test_20, player.pos.x + 38, player.pos.y + 1);
                add_monster(monster_test_21, player.pos.x + 39, player.pos.y + 1);
                add_monster(monster_test_22, player.pos.x + 40, player.pos.y + 1);
                add_monster(monster_test_23, player.pos.x + 41, player.pos.y + 1);
                add_monster(monster_test_24, player.pos.x + 42, player.pos.y + 1);
                add_monster(monster_test_25, player.pos.x + 43, player.pos.y + 1);
                add_monster(monster_test_26, player.pos.x + 44, player.pos.y + 1);
                add_monster(monster_test_27, player.pos.x + 45, player.pos.y + 1);
                add_monster(monster_test_28, player.pos.x + 46, player.pos.y + 1);
                add_monster(monster_test_29, player.pos.x + 47, player.pos.y + 1);
                add_monster(monster_test_30, player.pos.x + 48, player.pos.y + 1);
#endif
                
                game.initialized = true;
            }
            
            if(game.state == state_in_game)
            {
                if(process_player_input(new_input->keyboard))
                {
                    update_player(new_input->keyboard);
                    update_pathfind_map(pathfind_map, dungeon.w, dungeon.h);
                    update_monsters();
                    update_fov();
                }
                
                update_camera();
                
                render_tilemap();
                render_items();
                render_monsters();
                render_player();
                render_ui();
                update_and_render_pop_text();
            }
            
#if 1
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
            
        }
        
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
    u32 result = initialize_game();
    if(result)
    {
        run_game();
    }
    
    exit_game();
    return(result);
}