#include "types.h"
#include "util.c"
#include "fov.c"
#include "sprite.c"
#include "render.c"
#include "ui.c"
#include "level_gen.c"
#include "pathfind.c"
// TODO(rami): Work on conf when we need it again
// #include "conf.c"
#include "assets.c"
#include "pop_up_text.c"
#include "monster.c"
#include "item.c"
#include "player.c"

/*
Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
   What can you pull out as reusable?
*/

/*
- Item window might need to be a little wider for item names
- Animation dilemma
  - Monster art is okay but needs to be adjusted to fit the sprite flip
- Shadows for font glyphs, could possibly make it look way better
- Monsters need to actually use sprite flip
  */

internal void
resize_window(u32 w, u32 h)
{
    SDL_SetWindowSize(game.window, w, h);
    game.window_size = V2u(w, h);
    game.console_size.w = game.window_size.w;
    game.camera = V4i(0, 0,
                      game.window_size.w,
                      game.window_size.h - game.console_size.h);
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
        SDL_GetWindowSize(game.window, &window_size.x, &window_size.y);
        resize_window(window_size.x, window_size.y);
    }
}

internal void
update_camera()
{
    game.camera.x = tile_mul(player.pos.x) - (game.camera.w / 2);
    game.camera.y = (tile_mul(player.pos.y) + (player.size.h / 2)) - (game.camera.h / 2);
    
    if(game.camera.x < 0)
    {
        game.camera.x = 0;
    }
    
    if(game.camera.y < 0)
    {
        game.camera.y = 0;
    }
    
    if(game.camera.x >= LEVEL_WIDTH_PIXELS - game.camera.w)
    {
        game.camera.x = LEVEL_WIDTH_PIXELS - game.camera.w;
    }
    
    if(game.camera.y >= LEVEL_HEIGHT_PIXELS - game.camera.h)
    {
        game.camera.y = LEVEL_HEIGHT_PIXELS - game.camera.h;
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
#if MOONBREATH_DEBUG
            // TODO(rami): Debug
            if(1)
#else
                if(!event.key.repeat)
#endif
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
                        toggle_fullscreen();
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

internal u32
set_window_icon()
{
    u32 result = 0;
    
    SDL_Surface *icon = IMG_Load("data/images/icon.png");
    if(icon)
    {
        SDL_SetWindowIcon(game.window, icon);
        SDL_FreeSurface(icon);
        result = 1;
    }
    else
    {
        printf("Failed to load window icon\n");
    }
    
    return(result);
}

internal u32
set_fonts()
{
    u32 result = 1;
    
    font[font_classic] = create_bmp_font("data/fonts/classic16x16.png",
                                         16, 16, 14, 8, 12);
    
    font[font_cursive] = create_ttf_font("data/fonts/alkhemikal.ttf", 16, 4);
    
    font[font_misc] = create_ttf_font("data/fonts/monaco.ttf",
                                      16, 4);
    
    for(u32 i = 0; i < font_total; ++i)
    {
        if(!font[i].success)
        {
            result = 0;
            printf("Font atlas %u failed\n", i);
        }
    }
    
    return(result);
}

internal u32
set_textures()
{
    u32 result = 1;
    
    texture[tex_tilemap] = SDL_CreateTexture(game.renderer,
                                             SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET,
                                             LEVEL_WIDTH_PIXELS, LEVEL_HEIGHT_PIXELS);
    texture[tex_game_tileset] = load_texture("data/images/game_tileset.png", 0);
    texture[tex_item_tileset] = load_texture("data/images/item_tileset.png", 0);
    texture[tex_wearable_item_tileset] = load_texture("data/images/wearable_item_tileset.png", 0);
    texture[tex_sprite_sheet] = load_texture("data/images/sprite_sheet.png", 0);
    texture[tex_inventory_win] = load_texture("data/images/inventory_win.png", 0);
    texture[tex_inventory_item_win] = load_texture("data/images/inventory_item_win.png", 0);
    texture[tex_inventory_selected_item] = load_texture("data/images/inventory_selected_item.png", 0);
    texture[tex_interface_bottom_win] = load_texture("data/images/interface_bottom_win.png", 0);
    texture[tex_health_bar_outside] = load_texture("data/images/health_bar_outside.png", 0);
    texture[tex_health_bar_inside] = load_texture("data/images/health_bar_inside.png", 0);
    texture[tex_player_parts] = load_texture("data/images/player_parts.png", 0);
    
    for(u32 i = 0; i < tex_total; ++i)
    {
        if(!texture[i])
        {
            result = 0;
            printf("Texture %u failed", i);
        }
    }
    
    return(result);
}

internal void
set_game_data()
{
    // TODO(rami):
    // srand(time(0));
    srand(1553293671);
    printf("Random Seed: %lu\n\n", time(0));
    
    game.state = state_running;
    game.window_size = V2u(1280, 720);
    game.console_size = V2u(game.window_size.w, 160);
    game.camera = V4i(0, 0,
                      game.window_size.w,
                      game.window_size.h - game.console_size.h);
    game.turn_changed = 1;
    game.perf_count_frequency = (f32)SDL_GetPerformanceFrequency();
    
    level.current_level = 1;
    
    // TODO(rami): Do this for all monsters
    monster_spawn_chance[0][0] = 84;
    monster_spawn_chance[0][1] = 100;
    
    monster_spawn_chance[1][0] = 100;
    monster_spawn_chance[1][1] = 100;
    
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        item[i].unique_id = i + 1;
    }
    
    for(u32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        strcpy(console_message[i].msg, CONSOLE_MESSAGE_EMPTY);
        console_message[i].color = color_black;
    }
    
    // TODO(rami): Since we know the item we are setting the information for,
    // we could skip all the things that item doesn't care about because
    // the item array is initialized to zero
    
    item_info_t *info = &item_info[0];
    info->id = 1;
    strcpy(info->name, "Lesser Health Potion");
    info->category = category_consumable;
    info->slot = slot_none;
    info->tile = V2u(8, 0);
    strcpy(info->use, "Restores 2 health");
    info->heal_amount = 2;
    info->damage = 0;
    info->armor = 0;
    strcpy(info->description, "");
    
    info = &item_info[1];
    info->id = 2;
    strcpy(info->name, "Iron Sword");
    info->category = category_weapon;
    info->slot = slot_first_hand;
    info->tile = V2u(4, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 2;
    info->armor = 0;
    strcpy(info->description, "");
    
    info = &item_info[2];
    info->id = 3;
    strcpy(info->name, "Rune Helmet");
    info->category = category_armor;
    info->slot = slot_head;
    info->tile = V2u(0, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[3];
    info->id = 4;
    strcpy(info->name, "Rune Chestplate");
    info->category = category_armor;
    info->slot = slot_body;
    info->tile = V2u(1, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[4];
    info->id = 5;
    strcpy(info->name, "Rune Platelegs");
    info->category = category_armor;
    info->slot = slot_legs;
    info->tile = V2u(2, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[5];
    info->id = 6;
    strcpy(info->name, "Rune Boots");
    info->category = category_armor;
    info->slot = slot_feet;
    info->tile = V2u(3, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[6];
    info->id = 7;
    strcpy(info->name, "Rune Shield");
    info->category = category_armor;
    info->slot = slot_second_hand;
    info->tile = V2u(5, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[7];
    info->id = 8;
    strcpy(info->name, "Rune Amulet");
    info->category = category_armor;
    info->slot = slot_amulet;
    info->tile = V2u(6, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[8];
    info->id = 9;
    strcpy(info->name, "Rune Ring");
    info->category = category_armor;
    info->slot = slot_ring;
    info->tile = V2u(7, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[9];
    info->id = 10;
    strcpy(info->name, "Red Chestplate");
    info->category = category_armor;
    info->slot = slot_body;
    info->tile = V2u(1, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 3;
    strcpy(info->description, "");
    
    info = &item_info[10];
    info->id = 11;
    strcpy(info->name, "Red Sword");
    info->category = category_weapon;
    info->slot = slot_first_hand;
    info->tile = V2u(4, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 3;
    info->armor = 0;
    strcpy(info->description, "");
}

internal u32
init_game()
{
    u32 init_result = 0;
    
    set_game_data();
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        u32 window_flags = 0;
        game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       game.window_size.w, game.window_size.h,
                                       window_flags);
        if(game.window)
        {
            printf("Monitor refresh rate: %dHz\n", get_window_refresh_rate());
            
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
                                    init_result = 1;
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
                        printf("SDL TTF library could not initialize: %s\n", SDL_GetError());
                    }
                }
                else
                {
                    printf("SLD image library could not initialize: %s\n", SDL_GetError());
                }
            }
            else
            {
                printf("SDL could not create a renderer: %s\n", SDL_GetError());
            }
        }
        else
        {
            printf("SDL could not create window: %s\n", SDL_GetError());
        }
    }
    else
    {
        printf("SDL could not initialize: %s\n", SDL_GetError());
    }
    
    return(init_result);
}

internal void
run_game()
{
    add_player();
    
    generate_level();
    
    //add_monster(monster_skeleton, 52, 29);
    
    /*add_item(id_rune_helmet, 49, 30);
    add_item(id_rune_amulet, 50, 30);
    add_item(id_rune_chestplate, 51, 30);
    add_item(id_rune_platelegs, 52, 30);
    add_item(id_rune_boots, 53, 30);
    add_item(id_iron_sword, 54, 30);
    add_item(id_iron_sword, 55, 30);
    add_item(id_rune_shield, 56, 30);
    add_item(id_rune_ring, 49, 31);
    
    add_item(id_red_chestplate, 50, 31);
    add_item(id_red_sword, 51, 31);
    add_item(id_lesser_health_potion, 52, 31);*/
    
    // TODO(rami):
    u32 frames_per_second = 144;
    f32 target_seconds_per_frame = 1.0f / (f32)frames_per_second;
    u64 old_counter = SDL_GetPerformanceCounter();
    f32 old_dt = SDL_GetPerformanceCounter();
    
    while(game.state)
    {
        // TODO(rami):
        /*printf("item_being_moved_unique_id: %u\n", inventory.item_being_moved_unique_id);
        printf("item_being_moved_src.x: %u\n", inventory.item_being_moved_src.x);
        printf("item_being_moved_src.y: %u\n\n", inventory.item_being_moved_src.y);
        
        printf("pos.x: %u\n", inventory.pos.x);
        printf("pos.y: %u\n", inventory.pos.y);*/
        
        SDL_SetRenderDrawColor(game.renderer,
                               0, 0, 0, 255);
        SDL_RenderClear(game.renderer);
        
        f32 new_dt = SDL_GetPerformanceCounter();
        game.dt = (f32)(new_dt - old_dt) / game.perf_count_frequency;
        old_dt = new_dt;
        // NOTE(rami):
        // printf("\ndt: %f\n", game.dt);
        
        // NOTE(rami): Pop up text
#if 0
        for(i32 i = POP_UP_TEXT_COUNT - 1; i > -1; --i)
        {
            if(pop_up_text[i].active)
            {
                printf("\npop_up_text[%u]\n", i);
                printf("str: %s\n", pop_up_text[i].str);
                printf("x: %u, y: %u\n", pop_up_text[i].pos.x, pop_up_text[i].pos.y);
                printf("change: %.02f\n", pop_up_text[i].change);
                printf("speed: %.02f\n", pop_up_text[i].speed);
                printf("duration_time: %ums\n", pop_up_text[i].duration_time);
                printf("start_time: %ums\n", pop_up_text[i].start_time);
            }
        }
#endif
        
        // NOTE(rami): Inventory
#if 0
        for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; --i)
        {
            if(inventory.slot[i].id)
            {
                printf("\nInventory.slots[%u]\n", i);
                printf("id %u\n", inventory.slot[i].id);
                printf("unique_id %u\n", inventory.slot[i].unique_id);
                printf("x: %u, y: %u\n", inventory.slot[i].x, inventory.slot[i].y);
                printf("in_inventory %u\n", inventory.slot[i].in_inventory);
                printf("equipped %u\n", inventory.slot[i].equipped);
            }
        }
#endif
        
        // NOTE(rami): Item
#if 0
        for(i32 i = ITEM_COUNT - 1; i > -1; --i)
        {
            if(item[i].id)
            {
                printf("\nitem[%u]\n", i);
                printf("id %u\n", item[i].id);
                printf("unique_id %u\n", item[i].unique_id);
                printf("x: %u, y: %u\n", item[i].pos.x, item[i].pos.y);
                printf("in_inventory %u\n", item[i].in_inventory);
                printf("is_equipped %u\n", item[i].equipped);
            }
        }
#endif
        
        // NOTE(rami): Player
#if 0
        printf("\nPlayer\n");
        printf("frame_start.x, y: %u, %u\n", player.sprite.start_frame.x,
               player.sprite.start_frame.y);
        printf("frame_current.x, y: %u, %u\n", player.sprite.current_frame.x,
               player.sprite.current_frame.y);
        printf("frame_count: %u\n", player.sprite.frame_count);
        printf("frame_delay: %u\n", player.sprite.frame_duration);
        printf("frame_last_changed: %u\n", player.sprite.frame_last_changed);
        printf("new_x, new_y: %u, %u\n", player.new_pos.x, player.new_pos.y);
        printf("x, y: %u, %u\n", player.pos.x, player.pos.y);
        printf("w, h: %u, %u\n", player.size.w, player.size.h);
        printf("name: %s\n", player.name);
        printf("max_hp: %u\n", player.max_hp);
        printf("hp: %u\n", player.hp);
        printf("damage: %u\n", player.damage);
        printf("armor: %u\n", player.armor);
        printf("speed: %u\n", player.speed);
        printf("level: %u\n", player.level);
        printf("money: %u\n", player.money);
        printf("fov: %u\n", player.fov);
        printf("sprite_flip: %u\n", player.sprite_flip);
#endif
        
        // NOTE(rami): Monster
#if 0
        for(i32 i = MONSTER_COUNT - 1; i > -1; --i)
        {
            if(monster[i].type)
            {
                printf("\nmonster[%u]\n", i);
                printf("type: %u\n", monster[i].type);
                printf("ai: %u\n", monster[i].ai);
                
                printf("start_frame.x, y: %u, %u\n", monster[i].sprite.start_frame.x,
                       monster[i].sprite.start_frame.y);
                printf("current_frame.x, y: %u, %u\n", monster[i].sprite.current_frame.x,
                       monster[i].sprite.current_frame.y);
                printf("frame_count: %u\n", monster[i].sprite.frame_count);
                printf("frame_duration: %u\n", monster[i].sprite.frame_duration);
                printf("frame_last_changed: %u\n", monster[i].sprite.frame_last_changed);
                
                printf("x, y: %u, %u\n", monster[i].pos.x, monster[i].pos.y);
                printf("w, h: %u, %u\n", monster[i].size.w, monster[i].size.h);
                printf("in_combat: %u\n", monster[i].in_combat);
                printf("max_hp: %u\n", monster[i].max_hp);
                printf("hp: %u\n", monster[i].hp);
                printf("damage: %u\n", monster[i].damage);
                printf("armor: %u\n", monster[i].armor);
                printf("speed: %u\n", monster[i].speed);
                printf("level: %u\n", monster[i].level);
            }
        }
#endif
        
        update_events();
        
        if(game.turn_changed)
        {
            update_player();
            update_monsters();
            update_fov();
            
            game.turn_changed = 0;
        }
        
        update_camera();
        update_pop_up_text();
        
        render_tilemap();
        render_items();
        render_player();
        render_monsters();
        render_ui();
        render_pop_up_text();
        
        if(inventory.open)
        {
            render_inventory();
        }
        
        u64 work_counter_elapsed = SDL_GetPerformanceCounter() - old_counter;
        f32 ms_for_work = (1000.0f * (f32)work_counter_elapsed) / game.perf_count_frequency;
        // printf("ms_for_work: %.02f\n", ms_for_work);
        
        if(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
        {
            u32 time_to_delay =
                ((target_seconds_per_frame - get_seconds_elapsed(old_counter,SDL_GetPerformanceCounter())) * 1000) - 1;
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
        
        f32 ms_per_frame = (1000.0f * (f32)elapsed_counter) / game.perf_count_frequency;
        f32 frames_per_second = game.perf_count_frequency / (f32)elapsed_counter;
        old_counter = new_counter;
        // TODO(rami):
        // printf("ms_per_frame: %.02f\n", ms_per_frame);
        // printf("frames_per_second: %.02f\n", frames_per_second);
        // printf("time_elapsed: %u\n", game.time_elapsed);
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

i32 main(int argc, char *argv[])
{
    if(init_game())
    {
        run_game();
        exit_game();
        return(EXIT_SUCCESS);
    }
    
    exit_game();
    return(EXIT_FAILURE);
}