#include <SDL2/include/SDL.h>
#include <SDL2/include/SDL_image.h>
#include <SDL2/include/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "types.h"
#include "memory.c"
#include "name.c"
#include "fov.c"
#include "dungeon.c"
#include "pathfind.c"
#include "assets.c"
#include "editor.c"
#include "item.c"
#include "entity.c"
#include "ui.c"
#include "debug.c"
#include "config.c"

internal u32
tile_div(u32 value)
{
    u32 result = 0;
    
    if(value)
    {
        result = value / TILE_SIZE;
    }
    
    return(result);
}

internal u32
tile_mul(u32 value)
{
    u32 result = 0;
    
    if(value)
    {
        result = value * TILE_SIZE;
    }
    
    return(result);
}

internal String32
get_game_session_time_string(u32 time)
{
    String32 result = {0};
    
    if(time < 10)
    {
        sprintf(result.s, "%c%u", '0', time);
    }
    else
    {
        sprintf(result.s, "%u", time);
    }
    
    return(result);
}

internal void
update_game_session_time(GameTimer *timer)
{
    if(timer->start > timer->end + 1000)
    {
        timer->end = timer->start;
        ++timer->seconds;
        
        if(timer->seconds >= 60)
        {
            timer->seconds = 0;
            ++timer->minutes;
            
            if(timer->minutes >= 60)
            {
                timer->minutes = 0;
                ++timer->hours;
            }
        }
    }
}

internal void
render_game_mouse_highlight(Game *game,
                            Input *input,
                            ExamineMode *examine,
                            u32 inventory_flags,
                            u32 ui_flags,
                            Assets *assets)
{
    if(windows_are_closed(examine, inventory_flags, ui_flags) &&
       input->mouse_pos.x > 0 &&
       input->mouse_pos.y > 0 &&
       input->mouse_pos.x < game->window_size.w - 1 && // -1 is so the rectangle doesn't stay on the right edge of the screen if the mouse goes past it
       input->mouse_pos.y < game->window_size.h - assets->stat_and_log_window_h)
    {
        v2u mouse_tile_pos =
        {
            tile_div(input->mouse_pos.x),
            tile_div(input->mouse_pos.y)
        };
        
        v2u camera_offset =
        {
            tile_div(game->camera.rect.x),
            tile_div(game->camera.rect.y)
        };
        
        input->mouse_tile_pos.x = mouse_tile_pos.x + camera_offset.x;
        input->mouse_tile_pos.y = mouse_tile_pos.y + camera_offset.y;
        
        v4u dest = get_game_dest(game->camera.rect, input->mouse_tile_pos);
        render_outline_rect(game->renderer, dest, Color_Yellow);
    }
}

internal void
update_examine_mode(ExamineMode *examine,
                    Input *input,
                    Entity *player,
                    EntityState *entity_state,
                    ItemState *item_state,
                    u32 *inventory_flags,
                    Dungeon *dungeon,
                    UI *ui)
{
    DungeonPassages *passages = &dungeon->passages;
    
    if(is_set(examine->flags, ExamineFlag_Open))
    {
        if(examine->type == ExamineType_Entity)
        {
            assert(is_enemy_entity_valid(examine->entity));
            
            // Begin entity spell examine
            char pressed = get_pressed_keyboard_char(input, KeyboardCharType_Alphabet);
            if(pressed)
            {
                Owner *owner = get_owner_from_letter(ui->temp_owners, pressed);
                if(owner->type)
                {
                    examine->type = ExamineType_Spell;
                    examine->spell = owner->spell;
                }
            }
        }
        else if(!examine->type)
        {
            for(u32 index = GameKey_Up; index <= GameKey_DownRight; ++index)
            {
                if(input->game_keys[index].is_down)
                {
                    b32 can_move = true;
                    for(u32 second_index = GameKey_Up; second_index <= GameKey_DownRight; ++second_index)
                    {
                        if(second_index != index &&
                           examine->key_pressed[second_index])
                        {
                            can_move = false;
                            break;
                        }
                    }
                    
                    if(can_move)
                    {
                        if(examine->key_pressed_start[index])
                        {
                            u32 hold_time = get_sdl_ticks_difference(examine->key_pressed_start[index]);
                            if(hold_time >= examine->hold_time_for_move)
                            {
                                examine->key_pressed[index] = true;
                            }
                        }
                        else
                        {
                            examine->key_pressed_start[index] = SDL_GetTicks();
                        }
                    }
                }
                else
                {
                    examine->key_pressed[index] = false;
                    examine->key_pressed_start[index] = 0;
                }
            }
            
            // The code below this line can only run if we don't have an examine type so that they're
            // not executed when we have a multiple pickup/examine window open.
            assert(!examine->type);
            
            if(was_pressed(&input->GameKey_IteratePassages))
            {
                DungeonPassageType passage_search_type = DungeonPassageType_None;
                
                if(input->Key_Shift.is_down)
                {
                    passage_search_type = DungeonPassageType_Up;
                }
                else
                {
                    passage_search_type = DungeonPassageType_Down;
                }
                
                //printf("\nExamine Mode: Searching for a passage going %s.\n", passage_search_type == DungeonPassageType_Up ? "Up" : "Down");
                
                // Attempt to find the passage
                for(u32 passage_index = 0; passage_index < MAX_DUNGEON_PASSAGE_COUNT; ++passage_index)
                {
                    DungeonPassage *passage = &passages->array[passage_index];
                    if(passage->type == passage_search_type)
                    {
                        //if(is_tile_seen(dungeon->tiles, passage->pos)) // For debugging
                        if(has_tile_been_seen(dungeon->tiles, passage->pos))
                        {
                            if((passage_index > examine->selected_passage) ||
                               is_dungeon_passage_last(&dungeon->passages, passage_search_type, examine->selected_passage))
                            {
                                //printf("Examine Mode: Selected %s passage[%u].\n", passage_search_type == DungeonPassageType_Up ? "Up" : "Down", passage_index);
                                
                                examine->selected_passage = passage_index;
                                examine->pos = passage->pos;
                                break;
                            }
                        }
                    }
                }
            }
            else if(was_pressed(&input->GameKey_AutoExplore) &&
                    can_player_auto_explore(&examine->flags, entity_state, dungeon, ui))
            {
                b32 can_pathfind = false;
                
                // Pathfind to passage
                for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                {
                    DungeonPassage *passage = &passages->array[index];
                    if(passage->type && is_v2u_equal(passage->pos, examine->pos))
                    {
                        can_pathfind = true;
                        break;
                    }
                }
                
                // Start manual player pathfind
                if(!can_pathfind &&
                   is_dungeon_pos_traversable(dungeon->tiles, examine->pos) &&
                   has_tile_been_seen(dungeon->tiles, examine->pos))
                {
                    // This only wants that the position is traversable and that the tile has been
                    // seen. Even if there's something on the tile like a dungeon trap or an entity.
                    can_pathfind = true;
                }
                
                if(can_pathfind)
                {
                    unset(examine->flags, ExamineFlag_Open);
                    init_entity_pathfind(player, dungeon, &player->pathfind_map, examine->pos);
                }
            }
            else if(was_pressed(&input->GameKey_Yes))
            {
                if(get_dungeon_pos_examine_source_count(dungeon, entity_state, item_state, examine->pos) > 1 &&
                   has_tile_been_seen(dungeon->tiles, examine->pos))
                {
                    // Examine multiple
                    unset(examine->flags, ExamineFlag_Open);
                    set(*inventory_flags, InventoryFlag_MultipleExamine);
                    set_view_at_start_and_reset_move(&item_state->examine_window.view);
                    return;
                }
                
                for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
                {
                    Entity *enemy = &entity_state->entities[enemy_index];
                    
                    if(is_enemy_entity_valid_in_level(enemy, dungeon->level) &&
                       has_tile_been_seen(dungeon->tiles, examine->pos) &&
                       is_v2u_equal(enemy->pos, examine->pos))
                    {
                        examine->type = ExamineType_Entity;
                        examine->entity = enemy;
                        return;
                    }
                }
                
                if(has_tile_been_seen(dungeon->tiles, examine->pos))
                {
                    Item *item = get_dungeon_pos_item(item_state, dungeon->level, examine->pos, 0);
                    if(item)
                    {
                        examine->item = item;
                        examine->type = ExamineType_Item;
                        return;
                    }
                    
                    DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, examine->pos);
                    if(trap)
                    {
                        examine->type = ExamineType_Trap;
                        examine->trap = trap;
                        return;
                    }
                    
                    examine->type = ExamineType_Tile;
                    examine->tile_id = get_dungeon_pos_tile_id(dungeon->tiles, examine->pos);
                }
            }
        }
    }
}

internal String128
get_os_path(char *path)
{
    assert(path);
    
    u32 index = 0;
    String128 result = {0};
    
    for(char *c = path; *c; ++c)
    {
        assert(index < array_count(result.s));
        
#if MOONBREATH_WINDOWS
        if(c[0] == '/')
        {
            result.s[index++] = '\\';
            result.s[index++] = '\\';
        }
#else
        if(c[0] == '\\')
        {
            result.s[index++] = '/';
        }
#endif
        else
        {
            result.s[index++] = c[0];
        }
    }
    
    return(result);
}

internal void
init_view_scrolling_data(View *view, u32 entry_size, f32 shared_step_multiplier)
{
    assert(view);
    assert(entry_size);
    assert(shared_step_multiplier);
    
    view->entry_size = entry_size;
    view->move.size = entry_size;
    view->shared_step_multiplier = shared_step_multiplier;
}

internal Texture
load_texture(SDL_Renderer *renderer, char *path, v4u *color_key)
{
    Texture result = {0};
    
    SDL_Surface *loaded_surf = IMG_Load(path);
    if(loaded_surf)
    {
        result.w = loaded_surf->w;
        result.h = loaded_surf->h;
        
        if(color_key)
        {
            // Store the rgb color into formatted_key in the color format of the surface.
            // All pixels with the color of formatted_key will be transparent.
            u32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
            SDL_SetColorKey(loaded_surf, 1, formatted_key);
        }
        
        SDL_Texture *new_tex = SDL_CreateTextureFromSurface(renderer, loaded_surf);
        if(new_tex)
        {
            result.tex = new_tex;
        }
        else
        {
            printf("Error: Texture could not be created from a surface.\n");
        }
    }
    else
    {
        printf("Error: Image could not be loaded: \"%s\".\n", path);
    }
    
    SDL_FreeSurface(loaded_surf);
    return(result);
}

internal void
set_render_color(SDL_Renderer *renderer, Color color)
{
    assert(renderer);
    assert(color);
    
    v4u value = get_color_value(color);
    SDL_SetRenderDrawColor(renderer, value.r, value.g, value.b, value.a);
}

internal b32
is_diagonal(Direction direction)
{
    assert(direction);
    
    b32 result = (direction == Direction_UpLeft ||
                  direction == Direction_UpRight ||
                  direction == Direction_DownLeft ||
                  direction == Direction_DownRight);
    
    return(result);
}

internal char *
get_direction_string(Direction direction)
{
    char *result = 0;
    
    switch(direction)
    {
        case Direction_None: result = "None"; break;
        
        case Direction_Up: result = "Up"; break;
        case Direction_Down: result = "Down"; break;
        case Direction_Left: result = "Left"; break;
        case Direction_Right: result = "Right"; break;
        
        case Direction_UpLeft: result = "Up Left"; break;
        case Direction_UpRight: result = "Up Right"; break;
        case Direction_DownLeft: result = "Down Left"; break;
        case Direction_DownRight: result = "Down Right"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal Direction
get_direction_between_positions(v2u a_pos, v2u b_pos)
{
    Direction result = Direction_None;
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        if(is_v2u_equal(a_pos, get_direction_pos(b_pos, direction)))
        {
            result = direction;
            break;
        }
    }
    
    return(result);
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

internal b32
is_direction_vertical(Direction direction)
{
    b32 result = (direction == Direction_Up || direction == Direction_Down);
    return(result);
}

internal Direction
get_random_direction(Random *random)
{
    assert(random);
    
    Direction result = get_random(random, Direction_Up, Direction_DownRight);
    return(result);
}

internal v4u
get_pos_tile_rect(v2u pos)
{
    v4u result =
    {
        pos.x,
        pos.y,
        TILE_SIZE,
        TILE_SIZE
    };
    
    return(result);
}

internal v4u
get_pos_tile_mul_rect(v2u pos)
{
    v4u result =
    {
        tile_mul(pos.x),
        tile_mul(pos.y),
        TILE_SIZE,
        TILE_SIZE
    };
    
    return(result);
}

internal v4u
get_game_dest(v4s camera, v2u pos)
{
    assert(!is_v4s_zero(camera));
    
    v4u result = get_pos_tile_mul_rect(pos);
    result.x -= camera.x;
    result.y -= camera.y;
    
    return(result);
}

internal void
render_texture(SDL_Renderer *renderer, SDL_Texture *texture, v4u *src, v4u *dest, b32 flipped, b32 half_color)
{
    assert(renderer);
    assert(texture);
    assert(dest);
    
    if(half_color) SDL_SetTextureColorMod(texture, 127, 127, 127);
    
    SDL_RenderCopyEx(renderer, texture, (SDL_Rect *)src, (SDL_Rect *)dest, 0, 0, flipped);
    
    if(half_color) SDL_SetTextureColorMod(texture, 255, 255, 255);
}

internal v4u
render_game_dest_tile(Game *game, SDL_Texture *texture, v4u src, v2u pos, b32 flipped)
{
    assert(game);
    assert(texture);
    assert(!is_v4u_zero(src));
    
    v4u dest = get_game_dest(game->camera.rect, pos);
    render_texture(game->renderer, texture, &src, &dest, flipped, false);
    
    return(dest);
}

internal void
render_dungeon(Game *game, Dungeon *dungeon, Assets *assets)
{
    SDL_Renderer *renderer = game->renderer;
    GameCamera camera = game->camera;
    
    // Clear tilemap
    SDL_SetRenderTarget(renderer, assets->tilemap.tex);
    SDL_RenderClear(renderer);
    
    v4u render_area =
    {
        tile_div(camera.rect.x),
        tile_div(camera.rect.y),
        tile_div(camera.rect.x + camera.rect.w),
        tile_div(camera.rect.y + camera.rect.h)
    };
    
    // If the dungeon w/h is less than the camera w/h then
    // we can clamp the render area to the dungeon w/h.
    if(tile_mul(dungeon->size.w) < camera.rect.w)
    {
        render_area.w = dungeon->size.w - 1;
    }
    
    if(tile_mul(dungeon->size.h) < camera.rect.h)
    {
        render_area.h = dungeon->size.h - 1;
    }
    // print_v4u(render_area);
    
    // Render to tilemap
    for(u32 y = render_area.y; y < render_area.h; ++y)
    {
        for(u32 x = render_area.x; x < render_area.w; ++x)
        {
            v2u tile_pos = {x, y};
            
            //printf("%u, %u: %u\n", x, y, get_dungeon_pos_tile_id(dungeon->tiles, tile_pos));
            assert(get_dungeon_pos_tile_id(dungeon->tiles, tile_pos));
            
#if MOONBREATH_SLOW
            // All tiles are seen in the editor.
            if(game->editor.is_open)
            {
                set_tile_is_seen_and_has_been_seen(dungeon->tiles, tile_pos, true);
            }
#endif
            
            b32 render_tile = false;
            b32 half_color = false;
            v2u tileset_pos = {0};
            DungeonTile *tile = get_dungeon_pos_tile(dungeon->tiles, tile_pos);
            
            if(is_tile_seen(dungeon->tiles, tile_pos))
            { 
                render_tile = true;
                
                tile->seen_id = tile->id;
                tileset_pos = get_dungeon_tileset_pos_from_tile_id(tile->id);
            }
            else if(has_tile_been_seen(dungeon->tiles, tile_pos))
            {
                render_tile = true;
                half_color = true;
                
                tileset_pos = get_dungeon_tileset_pos_from_tile_id(tile->seen_id);
            }
            
            if(render_tile)
            {
                v4u tile_src = get_dungeon_tile_rect(tileset_pos);
                v4u tile_dest = get_dungeon_tile_rect(tile_pos);
                
                DungeonRemainsInfo remains = get_dungeon_pos_remains_info(dungeon->tiles, tile_pos);
                DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, tile_pos);
                
                render_texture(renderer, assets->tileset.tex, &tile_src, &tile_dest, false, half_color);
                if(remains.tile_id)
                {
                    assert(!is_v4u_zero(remains.src));
                    render_texture(renderer, assets->tileset.tex, &remains.src, &tile_dest, false, half_color);
                }
                else if(trap)
                {
                    render_texture(renderer, assets->tileset.tex, &trap->tile_src, &tile_dest, false, half_color);
                }
            }
        }
    }
    
    SDL_SetRenderTarget(renderer, 0);
    
    // Render tilemap to screen
    v4u tilemap_src = {camera.rect.x, camera.rect.y, camera.rect.w, camera.rect.h};
    v4u tilemap_dest = {0, 0, camera.rect.w, camera.rect.h};
    render_texture(renderer, assets->tilemap.tex, &tilemap_src, &tilemap_dest, false, false);
}

internal u32
get_sdl_ticks_difference(u32 start_time)
{
    u32 result = (SDL_GetTicks() - start_time);
    return(result);
}

internal b32
is_value_in_range(s32 value, s32 start, s32 end)
{
    b32 result = ((value > start) && (value < end));
    return(result);
}

internal b32
is_chance_valid(u32 chance)
{
    b32 result = (chance >= 1 && chance <= 100);
    return(result);
}

internal s32
get_index(s32 value)
{
    s32 result = (value - 1);
    return(result);
}

internal b32
is_window_size(v2u window_size, u32 width, u32 height)
{
    b32 result = (window_size.w == width &&
                  window_size.h == height);
    
    return(result);
}

internal void
update_camera(Game *game, v2u player_pos, v2u dungeon_size)
{
    ExamineMode *examine = &game->examine;
    v2u camera_pos = {0};
    
#if MOONBREATH_SLOW
    EditorMode *editor = &game->editor;
    if(editor->is_open)
    {
        camera_pos = editor->pos;
    }
    
    else
#endif
    
    if(is_set(examine->flags, ExamineFlag_Open) &&
       is_set(examine->flags, ExamineFlag_CameraOnExaminePos))
    {
        camera_pos = examine->pos;
    }
    else
    {
        camera_pos = player_pos;
    }
    
    // TODO(Rami): Game resolution should be amount of tiles horizontally and vertically
    // ultimately stored in the config.
    
    // TODO(Rami): There's a bug where we have a black hozizontal bar at the bottom of the
    // window except when our position gets close enough ot the bottom of the dungeon.
    // Make sure this doesn't replicate with the new system.
    
    GameCamera *camera = &game->camera;
    camera->rect.x = tile_mul(camera_pos.x) - (camera->rect.w / 2);
    
    if(is_window_size(game->window_size, 1920, 1080))
    {
        camera->rect.y = tile_mul(camera_pos.y) - (camera->rect.h / 2) + 4;
    }
    else if(is_window_size(game->window_size, 1280, 720))
    {
        camera->rect.y = tile_mul(camera_pos.y) - (camera->rect.h / 2) + 16;
    }
    
    if(camera->rect.x < 0) camera->rect.x = 0;
    if(camera->rect.y < 0) camera->rect.y = 0;
    
    // Make dungeons smaller than camera work correctly
    if(tile_mul(dungeon_size.w) >= camera->rect.w)
    {
        s32 camera_max_x = (s32)(tile_mul(dungeon_size.w) - camera->rect.w);
        s32 camera_max_y = (s32)(tile_mul(dungeon_size.h) - camera->rect.h);
        
        // Clamp the camera if we get close enough to the map right/bottom edge
        if(camera->rect.x >= camera_max_x)
        {
            camera->rect.x = camera_max_x;
        }
        
        if(camera->rect.y >= camera_max_y)
        {
            camera->rect.y = camera_max_y;
        }
    }
    
#if 0
    print_v4s("game->camera", game->camera);
    
    printf("camera.x2: %d\n", camera->x + camera->w);
    printf("camera.y2: %d\n\n", camera->y + camera->h);
    
    printf("camera_tile_offset_x: %u\n", tile_div(camera->x));
    printf("camera_tile_offset_y: %u\n\n", tile_div(camera->y));
#endif
    
}

internal void
update_input(InputState *state, b32 is_down)
{
    assert(state);
    
    if(state->is_down != is_down)
    {
        state->repeat = false;
        state->is_down = is_down;
        
        if(!state->is_down)
        {
            state->has_been_up = true;
            state->toggled = !state->toggled; // For key states like caps lock.
        }
    }
}

internal void
reset_input_state(InputState *state)
{
    assert(state);
    zero_struct(*state);
}

internal Key
get_key_from_keycode(SDL_Keycode keycode)
{
    Key result = 0;
    
    switch(keycode)
    {
        case SDLK_a: result = Key_A; break;
        case SDLK_b: result = Key_B; break;
        case SDLK_c: result = Key_C; break;
        case SDLK_d: result = Key_D; break;
        case SDLK_e: result = Key_E; break;
        case SDLK_f: result = Key_F; break;
        case SDLK_g: result = Key_G; break;
        case SDLK_h: result = Key_H; break;
        case SDLK_i: result = Key_I; break;
        case SDLK_j: result = Key_J; break;
        case SDLK_k: result = Key_K; break;
        case SDLK_l: result = Key_L; break;
        case SDLK_m: result = Key_M; break;
        case SDLK_n: result = Key_N; break;
        case SDLK_o: result = Key_O; break;
        case SDLK_p: result = Key_P; break;
        case SDLK_q: result = Key_Q; break;
        case SDLK_r: result = Key_R; break;
        case SDLK_s: result = Key_S; break;
        case SDLK_t: result = Key_T; break;
        case SDLK_u: result = Key_U; break;
        case SDLK_v: result = Key_V; break;
        case SDLK_w: result = Key_W; break;
        case SDLK_x: result = Key_X; break;
        case SDLK_y: result = Key_Y; break;
        case SDLK_z: result = Key_Z; break;
        
        case SDLK_0: result = Key_0; break;
        case SDLK_1: result = Key_1; break;
        case SDLK_2: result = Key_2; break;
        case SDLK_3: result = Key_3; break;
        case SDLK_4: result = Key_4; break;
        case SDLK_5: result = Key_5; break;
        case SDLK_6: result = Key_6; break;
        case SDLK_7: result = Key_7; break;
        case SDLK_8: result = Key_8; break;
        case SDLK_9: result = Key_9; break;
        
        case SDLK_QUOTE: result = Key_Quote; break;
        case SDLK_SPACE: result = Key_Space; break;
        case SDLK_PLUS: result = Key_Plus; break;
        case SDLK_MINUS: result = Key_Minus; break;
        case SDLK_COMMA: result = Key_Comma; break;
        case SDLK_PERIOD: result = Key_Period; break;
        case SDLK_LESS: result = Key_LessThan; break;
        case SDLK_GREATER: result = Key_GreaterThan; break;
        
        case SDLK_LSHIFT: result = Key_Shift; break;
        case SDLK_RSHIFT: result = Key_Shift; break;
        
        case SDLK_LCTRL: result = Key_Control; break;
        case SDLK_RCTRL: result = Key_Control; break;
        
        case SDLK_LALT: result = Key_Alt; break;
        case SDLK_RALT: result = Key_Alt; break;
        
        case SDLK_PAGEUP: result = Key_PageUp; break;
        case SDLK_PAGEDOWN: result = Key_PageDown; break;
        
        case SDLK_DELETE: result = Key_Del; break;
        case SDLK_HOME: result = Key_Home; break;
        case SDLK_END: result = Key_End; break;
        case SDLK_RETURN: result = Key_Enter; break;
        case SDLK_ESCAPE: result = Key_Escape; break;
        case SDLK_BACKSPACE: result = Key_Backspace; break;
        
        case SDLK_LEFT: result = Key_ArrowLeft; break;
        case SDLK_RIGHT: result = Key_ArrowRight; break;
        
        // Don't use invalid_default_case.
    }
    
    return(result);
}

internal void
update_mouse_and_keyboard(Game *game, Input *input)
{
    // Update mouse
    u32 mouse_state = SDL_GetMouseState(&input->mouse_pos.x, &input->mouse_pos.y);
    update_input(&input->Button_Left, mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
    update_input(&input->Button_Middle, mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
    update_input(&input->Button_Right, mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
    update_input(&input->Button_Extended1, mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
    update_input(&input->Button_Extended2, mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
    
    // Reset scrolling
    reset_input_state(&input->Button_ScrollUp);
    reset_input_state(&input->Button_ScrollDown);
    
    // Update keyboard
    SDL_Event event = {0};
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game->mode = GameMode_Quit;
        }
        else if(event.type == SDL_MOUSEWHEEL)
        {
            if(event.wheel.y > 0)
            {
                input->Button_ScrollUp.is_down = true;
            }
            else if(event.wheel.y < 0)
            {
                input->Button_ScrollDown.is_down = true;
            }
        }
        else if(event.type == SDL_KEYUP ||
                event.type == SDL_KEYDOWN)
        {
            SDL_Keycode keycode = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(event.key.repeat)
            {
                switch(keycode)
                {
                    case SDLK_a: input->Key_A.repeat = true; break;
                    case SDLK_b: input->Key_B.repeat = true; break;
                    case SDLK_c: input->Key_C.repeat = true; break;
                    case SDLK_d: input->Key_D.repeat = true; break;
                    case SDLK_e: input->Key_E.repeat = true; break;
                    case SDLK_f: input->Key_F.repeat = true; break;
                    case SDLK_g: input->Key_G.repeat = true; break;
                    case SDLK_h: input->Key_H.repeat = true; break;
                    case SDLK_i: input->Key_I.repeat = true; break;
                    case SDLK_j: input->Key_J.repeat = true; break;
                    case SDLK_k: input->Key_K.repeat = true; break;
                    case SDLK_l: input->Key_L.repeat = true; break;
                    case SDLK_m: input->Key_M.repeat = true; break;
                    case SDLK_n: input->Key_N.repeat = true; break;
                    case SDLK_o: input->Key_O.repeat = true; break;
                    case SDLK_p: input->Key_P.repeat = true; break;
                    case SDLK_q: input->Key_Q.repeat = true; break;
                    case SDLK_r: input->Key_R.repeat = true; break;
                    case SDLK_s: input->Key_S.repeat = true; break;
                    case SDLK_t: input->Key_T.repeat = true; break;
                    case SDLK_u: input->Key_U.repeat = true; break;
                    case SDLK_v: input->Key_V.repeat = true; break;
                    case SDLK_w: input->Key_W.repeat = true; break;
                    case SDLK_x: input->Key_X.repeat = true; break;
                    case SDLK_y: input->Key_Y.repeat = true; break;
                    case SDLK_z: input->Key_Z.repeat = true; break;
                    
                    case SDLK_QUOTE: input->Key_Quote.repeat = true; break;
                    case SDLK_SPACE: input->Key_Space.repeat = true; break;
                    case SDLK_DELETE: input->Key_Del.repeat = true; break;
                    case SDLK_BACKSPACE: input->Key_Backspace.repeat = true; break;
                    
                    case SDLK_LEFT: input->Key_ArrowLeft.repeat = true; break;
                    case SDLK_RIGHT: input->Key_ArrowRight.repeat = true; break;
                }
            }
            else
            {
                // Update game key input
                for(u32 index = 0; index < GameKey_Count; ++index)
                {
                    assert(game->keybinds[index]);
                    
                    Key key = get_key_from_keycode(keycode);
                    if(key && key == game->keybinds[index])
                    {
                        update_input(&input->game_keys[index], is_down);
                        break;
                    }
                }
                
                // Update general key input
                switch(keycode)
                {
                    case SDLK_a: update_input(&input->Key_A, is_down); break;
                    case SDLK_b: update_input(&input->Key_B, is_down); break;
                    case SDLK_c: update_input(&input->Key_C, is_down); break;
                    case SDLK_d: update_input(&input->Key_D, is_down); break;
                    case SDLK_e: update_input(&input->Key_E, is_down); break;
                    case SDLK_f: update_input(&input->Key_F, is_down); break;
                    case SDLK_g: update_input(&input->Key_G, is_down); break;
                    case SDLK_h: update_input(&input->Key_H, is_down); break;
                    case SDLK_i: update_input(&input->Key_I, is_down); break;
                    case SDLK_j: update_input(&input->Key_J, is_down); break;
                    case SDLK_k: update_input(&input->Key_K, is_down); break;
                    case SDLK_l: update_input(&input->Key_L, is_down); break;
                    case SDLK_m: update_input(&input->Key_M, is_down); break;
                    case SDLK_n: update_input(&input->Key_N, is_down); break;
                    case SDLK_o: update_input(&input->Key_O, is_down); break;
                    case SDLK_p: update_input(&input->Key_P, is_down); break;
                    case SDLK_q: update_input(&input->Key_Q, is_down); break;
                    case SDLK_r: update_input(&input->Key_R, is_down); break;
                    case SDLK_s: update_input(&input->Key_S, is_down); break;
                    case SDLK_t: update_input(&input->Key_T, is_down); break;
                    case SDLK_u: update_input(&input->Key_U, is_down); break;
                    case SDLK_v: update_input(&input->Key_V, is_down); break;
                    case SDLK_w: update_input(&input->Key_W, is_down); break;
                    case SDLK_x: update_input(&input->Key_X, is_down); break;
                    case SDLK_y: update_input(&input->Key_Y, is_down); break;
                    case SDLK_z: update_input(&input->Key_Z, is_down); break;
                    
                    case SDLK_0: update_input(&input->Key_0, is_down); break;
                    case SDLK_1: update_input(&input->Key_1, is_down); break;
                    case SDLK_2: update_input(&input->Key_2, is_down); break;
                    case SDLK_3: update_input(&input->Key_3, is_down); break;
                    case SDLK_4: update_input(&input->Key_4, is_down); break;
                    case SDLK_5: update_input(&input->Key_5, is_down); break;
                    case SDLK_6: update_input(&input->Key_6, is_down); break;
                    case SDLK_7: update_input(&input->Key_7, is_down); break;
                    case SDLK_8: update_input(&input->Key_8, is_down); break;
                    case SDLK_9: update_input(&input->Key_9, is_down); break;
                    
                    case SDLK_QUOTE: update_input(&input->Key_Quote, is_down); break;
                    case SDLK_SPACE: update_input(&input->Key_Space, is_down); break;
                    case SDLK_PLUS: update_input(&input->Key_Plus, is_down); break;
                    case SDLK_MINUS: update_input(&input->Key_Minus, is_down); break;
                    case SDLK_COMMA: update_input(&input->Key_Comma, is_down); break;
                    case SDLK_PERIOD: update_input(&input->Key_Period, is_down); break;
                    case SDLK_LESS: update_input(&input->Key_LessThan, is_down); break;
                    case SDLK_GREATER: update_input(&input->Key_GreaterThan, is_down); break;
                    
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT: update_input(&input->Key_Shift, is_down); break;
                    
                    case SDLK_LCTRL:
                    case SDLK_RCTRL: update_input(&input->Key_Control, is_down); break;
                    
                    case SDLK_LALT:
                    case SDLK_RALT: update_input(&input->Key_Alt, is_down); break;
                    
                    case SDLK_PAGEUP: update_input(&input->Key_PageUp, is_down); break;
                    case SDLK_PAGEDOWN: update_input(&input->Key_PageDown, is_down); break;
                    
                    case SDLK_DELETE: update_input(&input->Key_Del, is_down); break;
                    case SDLK_HOME: update_input(&input->Key_Home, is_down); break;
                    case SDLK_END: update_input(&input->Key_End, is_down); break;
                    case SDLK_RETURN: update_input(&input->Key_Enter, is_down); break;
                    case SDLK_ESCAPE: update_input(&input->Key_Escape, is_down); break;
                    case SDLK_BACKSPACE: update_input(&input->Key_Backspace, is_down); break;
                    
                    case SDLK_LEFT: update_input(&input->Key_ArrowLeft, is_down); break;
                    case SDLK_RIGHT: update_input(&input->Key_ArrowRight, is_down); break;
                    case SDLK_CAPSLOCK: update_input(&input->Key_CapsLock, is_down); break;
                    
#if MOONBREATH_SLOW
                    case SDLK_F1: update_input(&input->fkeys[1], is_down); break;
                    case SDLK_F2: update_input(&input->fkeys[2], is_down); break;
                    case SDLK_F3: update_input(&input->fkeys[3], is_down); break;
                    case SDLK_F4: update_input(&input->fkeys[4], is_down); break;
                    case SDLK_F5: update_input(&input->fkeys[5], is_down); break;
                    case SDLK_F6: update_input(&input->fkeys[6], is_down); break;
                    case SDLK_F7: update_input(&input->fkeys[7], is_down); break;
                    case SDLK_F8: update_input(&input->fkeys[8], is_down); break;
                    case SDLK_F9: update_input(&input->fkeys[9], is_down); break;
                    case SDLK_F10: update_input(&input->fkeys[10], is_down); break;
                    case SDLK_F11: update_input(&input->fkeys[11], is_down); break;
                    case SDLK_F12: update_input(&input->fkeys[12], is_down); break;
#endif
                    
                }
            }
        }
    }
}

internal void
render_window(Game *game, v4u rect, u32 border_size)
{
    // Window border
    render_fill_rect(game->renderer, rect, Color_WindowBorder, false);
    
    // Window background
    v4u background_rect = get_border_adjusted_rect(rect, border_size);
    render_fill_rect(game->renderer, background_rect, Color_WindowBackground, false);
    
    // Window shadow
    render_outline_rect(game->renderer, background_rect, Color_WindowShadow);
}

internal void
render_outline_rect(SDL_Renderer *renderer, v4u rect, Color color)
{
    assert(renderer);
    assert(!is_v4u_zero(rect));
    assert(color);
    
    set_render_color(renderer, color);
    SDL_RenderDrawRect(renderer, (SDL_Rect *)&rect);
}

internal void
render_fill_rect(SDL_Renderer *renderer, v4u rect, Color color, b32 blend)
{
    assert(renderer);
    assert(!is_v4u_zero(rect));
    assert(color);
    
    if(blend) SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    set_render_color(renderer, color);
    SDL_RenderFillRect(renderer, (SDL_Rect *)&rect);
    
    if(blend) SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

internal void
set_render_clip_rect(SDL_Renderer *renderer, v4u *clip_rect)
{
    SDL_RenderSetClipRect(renderer, (SDL_Rect *)clip_rect);
}

internal char
get_char(char c, b32 uppercase)
{
    assert(is_alpha(c));
    if(uppercase)
    {
        c = get_uppercase(c);
    }
    
    assert(is_alpha(c));
    return(c);
}

internal String8
get_printable_key(Input *input, Key key)
{
    assert(input);
    
    String8 result = {0};
    
    b32 uppercase = (input->Key_Shift.is_down || input->Key_CapsLock.toggled);
    switch(key)
    {
        case Key_A: result.s[0] = get_char('a', uppercase); break;
        case Key_B: result.s[0] = get_char('b', uppercase); break;
        case Key_C: result.s[0] = get_char('c', uppercase); break;
        case Key_D: result.s[0] = get_char('d', uppercase); break;
        case Key_E: result.s[0] = get_char('e', uppercase); break;
        case Key_F: result.s[0] = get_char('f', uppercase); break;
        case Key_G: result.s[0] = get_char('g', uppercase); break;
        case Key_H: result.s[0] = get_char('h', uppercase); break;
        case Key_I: result.s[0] = get_char('i', uppercase); break;
        case Key_J: result.s[0] = get_char('j', uppercase); break;
        case Key_K: result.s[0] = get_char('k', uppercase); break;
        case Key_L: result.s[0] = get_char('l', uppercase); break;
        case Key_M: result.s[0] = get_char('m', uppercase); break;
        case Key_N: result.s[0] = get_char('n', uppercase); break;
        case Key_O: result.s[0] = get_char('o', uppercase); break;
        case Key_P: result.s[0] = get_char('p', uppercase); break;
        case Key_Q: result.s[0] = get_char('q', uppercase); break;
        case Key_R: result.s[0] = get_char('r', uppercase); break;
        case Key_S: result.s[0] = get_char('s', uppercase); break;
        case Key_T: result.s[0] = get_char('t', uppercase); break;
        case Key_U: result.s[0] = get_char('u', uppercase); break;
        case Key_V: result.s[0] = get_char('v', uppercase); break;
        case Key_W: result.s[0] = get_char('w', uppercase); break;
        case Key_X: result.s[0] = get_char('x', uppercase); break;
        case Key_Y: result.s[0] = get_char('y', uppercase); break;
        case Key_Z: result.s[0] = get_char('z', uppercase); break;
        
        case Key_0:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '=';
            }
            else
            {
                result.s[0] = '0';
            }
        } break;
        
        case Key_1:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '!';
            }
            else
            {
                result.s[0] = '1';
            }
        } break;
        
        case Key_2:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '"';
            }
            else
            {
                result.s[0] = '2';
            }
        } break;
        
        case Key_3:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '#';
            }
            else
            {
                result.s[0] = '3';
            }
        } break;
        
        case Key_4:
        {
            result.s[0] = '4';
        } break;
        
        case Key_5:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '%';
            }
            else
            {
                result.s[0] = '5';
            }
        } break;
        
        case Key_6:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '&';
            }
            else
            {
                result.s[0] = '6';
            }
        } break;
        
        case Key_7:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '/';
            }
            else
            {
                result.s[0] = '7';
            }
        } break;
        
        case Key_8:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '(';
            }
            else
            {
                result.s[0] = '8';
            }
        } break;
        
        case Key_9:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = ')';
            }
            else
            {
                result.s[0] = '9';
            }
        } break;
        
        case Key_Quote: result.s[0] = '\''; break;
        case Key_Space: result.s[0] = ' '; break;
        
        case Key_Plus:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '?';
            }
            else
            {
                result.s[0] = '+';
            }
        } break;
        
        case Key_Minus:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '_';
            }
            else
            {
                result.s[0] = '-';
            }
        } break;
        
        case Key_Comma:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = ';';
            }
            else
            {
                result.s[0] = ',';
            }
        } break;
        
        case Key_Period:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = ':';
            }
            else
            {
                result.s[0] = '.';
            }
        } break;
        
        case Key_LessThan:
        {
            if(input->Key_Shift.is_down)
            {
                result.s[0] = '>';
            }
            else
            {
                result.s[0] = '<';
            }
        } break;
        
        case Key_Escape: strcpy(result.s, "ESC"); break;
    }
    
    assert(result.s[0]);
    return(result);
}

internal char
get_pressed_keyboard_char(Input *input, KeyboardCharType type)
{
    assert(input);
    assert(type);
    
    Key key_end = Key_None;
    switch(type)
    {
        case KeyboardCharType_Printable:
        {
            key_end = Key_PrintableEnd - 1;
        } break;
        
        case KeyboardCharType_Alphabet:
        {
            key_end = Key_Z;
        } break;
        
        invalid_default_case;
    }
    
    for(Key key = Key_A; key <= key_end; ++key)
    {
        if(was_pressed(&input->keyboard[key]))
        {
            String8 printable_key = get_printable_key(input, key);
            return(printable_key.s[0]);
        }
    }
    
    return(0);
}

internal void
update_and_render_game(Game *game,
                       Input *input,
                       EntityState *entity_state,
                       DungeonState *dungeon_state,
                       ItemState *item_state,
                       Inventory *inventory,
                       Assets *assets,
                       UI *ui)
{
    assert(game);
    assert(input);
    assert(entity_state);
    assert(dungeon_state);
    assert(item_state);
    assert(inventory);
    assert(assets);
    assert(ui);
    
    Random *random = &game->random;
    Entity *player = get_player_entity();
    
    if(game->mode == GameMode_MainMenu)
    {
        v4u rect = {50, 300, 200, 100};
        render_fill_rect(game->renderer, rect, Color_Cyan, false);
        
        v2u pos = {100, 340};
        if(is_pos_inside_rect(rect, input->mouse_pos))
        {
            render_string(game->renderer, "%sNew Game", &pos, ui->font, start_color(Color_LightYellow));
            if(was_pressed(&input->Button_Left)) game->mode = GameMode_Playing;
        }
        else
        {
            render_string(game->renderer, "New Game", &pos, ui->font);
        }
    }
    else if(game->mode == GameMode_Playing)
    {
        game->timer.start = SDL_GetTicks();
        
        if(!game->is_set)
        {
            b32 potion_adjective_taken[16] = {0};
            char *potion_adjectives[16] =
            {
                "Bubbling ",
                "Sparkling ",
                "Glowing ",
                "Soupy ",
                "Glittering ",
                "Radiant ",
                "Elastic ",
                "Flowing ",
                "Lucent ",
                "Turbulent ",
                "Dim ",
                "Shadowy ",
                "Frothy ",
                "Dubious ",
                "Muddy ",
                "Watery "
            };
            
            b32 potion_color_set[Potion_Count] = {0};
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                ItemInfo *info = &item_state->potion_info[index];
                
                while(is_v2u_zero(info->tile_pos))
                {
                    assert_loop_count();
                    
                    u32 potion_index = get_random(random, 0, Potion_Count - 1);
                    if(!potion_color_set[potion_index])
                    {
                        potion_color_set[potion_index] = true;
                        
                        // Add a random not already taken adjective to the potion depiction
                        for(;;)
                        {
                            assert_loop_count();
                            
                            u32 adjective_index = get_random(random, 0, 15);
                            if(!potion_adjective_taken[adjective_index])
                            {
                                potion_adjective_taken[adjective_index] = true;
                                strcpy(info->depiction.s, potion_adjectives[adjective_index]);
                                
                                break;
                            }
                        }
                        
                        switch(potion_index)
                        {
                            case 0:
                            {
                                info->tile_pos = make_v2u(10, 2);
                                strcat(info->depiction.s, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile_pos = make_v2u(10, 3);
                                strcat(info->depiction.s, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile_pos = make_v2u(10, 4);
                                strcat(info->depiction.s, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile_pos = make_v2u(10, 5);
                                strcat(info->depiction.s, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile_pos = make_v2u(10, 6);
                                strcat(info->depiction.s, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile_pos = make_v2u(10, 7);
                                strcat(info->depiction.s, "Purple ");
                            } break;
                            
                            case 6:
                            {
                                info->tile_pos = make_v2u(10, 8);
                                strcat(info->depiction.s, "Green ");
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        break;
                    }
                }
            }
            
            b32 scroll_color_set[Scroll_Count] = {0};
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                ItemInfo *info = &item_state->scroll_info[index];
                
                while(is_v2u_zero(info->tile_pos))
                {
                    assert_loop_count();
                    
                    u32 scroll_index = get_random(random, 0, Scroll_Count - 1);
                    if(!scroll_color_set[scroll_index])
                    {
                        scroll_color_set[scroll_index] = true;
                        
                        switch(scroll_index)
                        {
                            case 0:
                            {
                                info->tile_pos = make_v2u(11, 2);
                                strcpy(info->depiction.s, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile_pos = make_v2u(11, 3);
                                strcpy(info->depiction.s, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile_pos = make_v2u(11, 4);
                                strcpy(info->depiction.s, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile_pos = make_v2u(11, 5);
                                strcpy(info->depiction.s, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile_pos = make_v2u(11, 6);
                                strcpy(info->depiction.s, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile_pos = make_v2u(11, 7);
                                strcpy(info->depiction.s, "Purple ");
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        break;
                    }
                }
            }
            
            item_state->potion_info[Potion_Healing].value_range = make_v2u(30, 50);
            
            item_state->ration_info.is_known = true; // Auto set identified on creation.
            item_state->ration_info.value_range = make_v2u(10, 20);
            item_state->ration_info.tile_pos = make_v2u(12, 2);
            
#if 0
            // Print randomized potion tiles
            printf("\nRandomized Potion Tiles\n");
            
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                ItemInfo *info = &item_state->potion_info[index];
                
                printf("[%u]: %u, %u\n", index, info->tile_pos.x, info->tile_pos.y);
            }
            
            // Print randomized scroll tiles
            printf("\nRandomized Scroll Tiles\n");
            
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                ItemInfo *info = &item_state->scroll_info[index];
                printf("[%u]: %u, %u\n", index, info->tile_pos.x, info->tile_pos.y);
            }
#endif
            
#if MOONBREATH_SLOW
            // Init editor
            EditorMode *editor = &game->editor;
            if(!editor->is_set)
            {
                editor->is_set = true;
                
                u32 editor_x = 2;
                u32 editor_y = 2;
                editor->font = &assets->fonts[FontName_DosVga];
                
                EditorGroup *tile_group = add_editor_group(editor, "Tiles", editor_x, editor_y, 10, false);
                {
                    add_editor_source_tile(tile_group, EditorSourceType_Wall, DungeonTileID_StoneWall1);
                    add_editor_source_tile(tile_group, EditorSourceType_Water, DungeonTileID_Water1);
                    add_editor_source_tile(tile_group, EditorSourceType_DungeonTile, DungeonTileID_StoneDoorClosed);
                }
                
                EditorGroup *trap_group = add_editor_group(editor, "Traps", editor_x, editor_y + 3, 10, false);
                {
                    for(DungeonTrapType type = DungeonTrapType_None + 1; type < DungeonTrapType_Count; ++type)
                    {
                        add_editor_source_trap(trap_group, type);
                    }
                }
                
                EditorGroup *entity_group = add_editor_group(editor, "Entities", editor_x, editor_y + 6, 10, true);
                {
                    for(EntityID id = EntityID_EnemyStart + 1; id < EntityID_EnemyEnd; ++id)
                    {
                        add_editor_source_entity(entity_group, id);
                    }
                }
                
                EditorGroup *item_group = add_editor_group(editor, "Items", editor_x, editor_y + 13, 0, true);
                {
                    for(ItemID id = ItemID_WeaponStart + 1; id < ItemID_WeaponEnd; ++id)
                    {
                        for(ItemRarity rarity = ItemRarity_None + 1; rarity < ItemRarity_Count; ++rarity)
                        {
                            add_editor_source_item(item_group, item_state, id, rarity);
                        }
                    }
                    add_editor_newline(item_group, 2);
                    
                    for(ItemID id = ItemID_ArmorStart + 1; id < ItemID_ArmorEnd; ++id)
                    {
                        add_editor_source_item(item_group, item_state, id, ItemRarity_Common);
                    }
                    add_editor_newline(item_group, 2);
                    
                    for(ItemID id = ItemID_PotionStart + 1; id < ItemID_PotionEnd; ++id)
                    {
                        add_editor_source_item(item_group, item_state, id, ItemRarity_Common);
                    }
                    add_editor_newline(item_group, 2);
                    
                    for(ItemID id = ItemID_ScrollStart + 1; id < ItemID_ScrollEnd; ++id)
                    {
                        add_editor_source_item(item_group, item_state, id, ItemRarity_Common);
                    }
                    add_editor_newline(item_group, 2);
                }
            }
#endif
            
            // TODO(rami): Let the player set their name or generate a random one.
            log_add("%sWelcome, %s!", ui, start_color(Color_LightYellow), player->name.s);
            log_add("%sFind and destroy the underworld portal,", ui, start_color(Color_LightYellow));
            log_add("%swhich is located somewhere in the depths.", ui, start_color(Color_LightYellow));
            
            create_dungeon(game, entity_state, item_state, inventory, dungeon_state, ui, 1, make_v2u(0, 0));
            
            game->is_set = true;
        }
        
        ExamineMode *examine = &game->examine;
        Dungeon *dungeon = get_dungeon_from_level(dungeon_state, dungeon_state->current_level);
        
#if 0
        printf("Used Game Memory: %lu/%lu\n", game->memory_arena.used, game->memory_arena.size);
        printf("Used Debug Memory: %lu/%lu\n\n", game->debug.memory_arena.used, game->debug.memory_arena.size);
#endif
        
#if 0
        for(u32 index = 0; index < MAX_FONT_METRICS_COUNT; ++index)
        {
            Font *font = &assets->fonts[FontName_DosVga];
            char c = index + FIRST_FONT_GLYPH;
            
            printf("%c advance: %u\n", c, font->metrics[index].advance);
        }
        printf("\n\n");
#endif
        
#if 0
        printf("\nDungeon Level %u Items:\n", dungeon->level);
        
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            
            if(is_item_valid_and_not_in_inventory(item, dungeon->level))
            {
                printf("%s at %u, %u\n", item->name.s, item->pos.x, item->pos.y);
            }
        }
#endif
        
#if 0
        for(u32 index = 0; index < MAX_DUNGEON_LEVEL; ++index)
        {
            Dungeon *dungeon = &dungeons->levels[index];
            
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
                    if(is_dungeon_pos_occupied(dungeon->tiles, make_v2u(x, y)))
                    {
                        printf("Dungeon Level %u (Index %u): %u, %u is occupied\n", index + 1, index, x, y);
                    }
                }
            }
        }
#endif
        
#if 0
        for(u32 index = 0; index < MAX_OWNER_COUNT; ++index)
        {
            Owner *owner = &inventory->item_owners[index];
            
            if(owner->type)
            {
                printf("owner->type: %u\n", owner->type);
                printf("owner->item: %p\n", owner->item);
                printf("owner->c: %c\n\n", owner->c);
            }
        }
#endif
        
#if 0
        for(u32 index = 0; index < MAX_DEBUG_TREE_COUNT; ++index)
        {
            DebugTree *tree = &game->debug.trees[index];
            if(tree->is_set)
            {
                printf("Debug Tree Index: %u\n\n", index);
            }
        }
#endif
        
#if 0
        for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
        {
            DungeonTrap *trap = &dungeon->traps.array[index];
            if(trap->type)
            {
                printf("Index: %u\n", index);
                printf("Name: %s\n\n", trap->name.s);
            }
        }
        
        printf("\n");
#endif
        
#if 0
        for(u32 index = 0; index < MAX_ENTITY_GHOST_COUNT; ++index)
        {
            EntityGhost *ghost = &entity_state->ghosts[index];
            if(ghost->set)
            {
                printf("Index: %u\n", index);
                printf("Pos: %u, %u\n", ghost->pos.x, ghost->pos.y);
            }
        }
        
        printf("\n");
#endif
        
#if MOONBREATH_SLOW
        // Make sure inventory items are valid.
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *item = inventory->slots[index];
            if(item)
            {
                assert(is_valid_inventory_item(item));
            }
        }
        
        // Make sure dungeon passages are valid.
        for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
        {
            DungeonPassage *passage = &dungeon->passages.array[index];
            if(passage->type)
            {
                assert(!is_v2u_zero(passage->pos));
                assert(is_dungeon_level_valid(passage->dungeon_level));
                
                if(passage->type == DungeonPassageType_Up &&
                   passage->dungeon_level > 1 &&
                   is_v2u_zero(passage->dest_pos))
                {
                    print_v2u(passage->pos);
                    print_v2u(passage->dest_pos);
                    printf("passage->dungeon_level: %u\n\n", passage->dungeon_level);
                    
                    assert(0);
                }
            }
        }
#endif
        
        update_game_session_time(&game->timer);
        update_examine_mode(examine, input, player, entity_state, item_state, &inventory->flags, dungeon, ui);
        update_entities(game, input, entity_state, item_state, inventory, dungeon_state, assets, ui);
        update_camera(game, player->pos, dungeon->size);
        
        render_dungeon(game, dungeon, assets);
        render_items(game, player, item_state, dungeon, assets);
        render_entities(game, entity_state, inventory, dungeon, assets);
        render_ui(game, input->mouse_pos, entity_state, item_state, inventory, dungeon, assets, ui);
        
#if MOONBREATH_SLOW
        render_editor_mode(game->renderer, game->camera, input, &game->editor, assets->tileset);
        update_and_render_debug_state(game, input, entity_state, item_state, dungeon, ui);
#endif
        
        render_game_mouse_highlight(game, input, examine, inventory->flags, ui->flags, assets);
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
    f32 result = ((f32)(new_counter - old_counter) / (f32)performance_frequency);
    return(result);
}

int main(int argc, char *args[])
{
    u32 result = 0;
    
    GameMemory memory = {0};
    memory.size = megabytes(256);
    memory.storage = calloc(1, memory.size);
    
    if(memory.size && memory.storage)
    {
        Game *game = memory.storage;
        game->examine.hold_time_for_move = 400;
        
        init_memory_arena(&game->memory,
                          memory.storage + sizeof(Game),
                          megabytes(128));
        
        EntityState *entity_state = push_memory_struct(&game->memory, EntityState);
        Entity *player = get_player_entity();
        
        DungeonState *dungeon_state = push_memory_struct(&game->memory, DungeonState);
        for(u32 index = 0; index < MAX_DUNGEON_LEVEL; ++index)
        {
            dungeon_state->levels[index].tiles.array = push_memory(&game->memory, MAX_DUNGEON_AREA * sizeof(DungeonTile));
        }
        
        Assets *assets = push_memory_struct(&game->memory, Assets);
        UI *ui = push_memory_struct(&game->memory, UI);
        init_all_owner_letters(ui->temp_owners);
        
        ui->window_offset = 12;
        ui->short_log_view.end = 9;
        ui->default_view_step_multiplier = 0.14f;
        
        ItemState *item_state = push_memory_struct(&game->memory, ItemState);
        init_mark(&item_state->temp_mark, 24);
        init_view_scrolling_data(&item_state->pickup_window.view, 32, ui->default_view_step_multiplier);
        init_view_scrolling_data(&item_state->examine_window.view, 32, ui->default_view_step_multiplier);
        
        Inventory *inventory = push_memory_struct(&game->memory, Inventory);
        init_all_owner_letters(inventory->item_owners);
        init_view_scrolling_data(&inventory->window.view, 32, ui->default_view_step_multiplier);
        init_view_scrolling_data(&inventory->interact_window.view, 32, ui->default_view_step_multiplier);
        
#if 0
        // Config Example
        
        ConfigValue config_uint = get_config_uint(&config, "config_uint");
        if(config_uint.is_valid)
        {
            printf("config_uint: %u\n", config_uint.uint);
        }
        
        ConfigValue config_bool = get_config_bool(&config, "config_bool");
        if(config_bool.is_valid)
        {
            printf("config_bool: %u\n", config_bool.boolean);
        }
        
        ConfigValue config_string = get_config_string(&config, "config_string");
        if(config_string.is_valid)
        {
            printf("config_string: %s\n", config_string.string);
        }
        
        ConfigValue config_char = get_config_string(&config, "config_char");
        if(config_char.is_valid)
        {
            printf("config_char: %s\n", config_char.string);
        }
#endif
        
        // TODO(rami): Config: Check success
        Config config = get_config(&game->memory, get_os_path("data/config.txt").s);
        
        ConfigValue show_item_ground_outline = get_config_bool(&config, "show_item_ground_outline");
        if(show_item_ground_outline.boolean)
        {
            game->show_item_ground_outline = show_item_ground_outline.boolean;
        }
        
        ConfigValue window_size = get_config_uint(&config, "window_size");
        assert(window_size.uint == 1 || window_size.uint == 2);
        
        if(window_size.uint == 1)
        {
            game->window_size = make_v2u(1920, 1080);
        }
        else if(window_size.uint == 2)
        {
            game->window_size = make_v2u(1280, 720);
        }
        
        game->window_size = make_v2u(1920, 1080);
        
#if 0
        for(u32 index = 0; index < GameKey_Count; ++index)
        {
            char *token_name = 0;
            
            switch(index)
            {
                case GameKey_Up: token_name = "key_up"; break;
                case GameKey_Down: token_name = "key_down"; break;
                case GameKey_Left: token_name = "key_left"; break;
                case GameKey_Right: token_name = "key_right"; break;
                
                case GameKey_UpLeft: token_name = "key_up_left"; break;
                case GameKey_UpRight: token_name = "key_up_right"; break;
                case GameKey_DownLeft: token_name = "key_down_left"; break;
                case GameKey_DownRight: token_name = "key_down_right"; break;
                
                case GameKey_OpenInventory: token_name = "key_open_inventory"; break;
                case GameKey_Pickup: token_name = "key_pickup"; break;
                case GameKey_UsePassage: token_name = "key_use_passage"; break;
                case GameKey_AutoExplore: token_name = "key_auto_explore"; break;
                case GameKey_IteratePassages: token_name = "key_iterate_passages"; break;
                case GameKey_Examine: token_name = "key_examine"; break;
                case GameKey_Log: token_name = "key_log"; break;
                
                case GameKey_Back: token_name = "key_back"; break;
                case GameKey_Wait: token_name = "key_wait"; break;
                case GameKey_Yes: token_name = "key_yes"; break;
                case GameKey_No: token_name = "key_no"; break;
                
                invalid_default_case;
            }
            
            ConfigValue value = get_config_string(&config, token_name);
            if(value.is_valid)
            {
                game.keybinds[index] = value.string[0];
            }
            else
            {
                printf("Error: Value could not be loaded for game.keybinds[%u].\n", index);
                assert(0);
            }
        }
        
#else
        game->keybinds[GameKey_Up] = Key_W;
        game->keybinds[GameKey_Down] = Key_S;
        game->keybinds[GameKey_Left] = Key_A;
        game->keybinds[GameKey_Right] = Key_D;
        
        game->keybinds[GameKey_UpLeft] = Key_Q;
        game->keybinds[GameKey_UpRight] = Key_E;
        game->keybinds[GameKey_DownLeft] = Key_Z;
        game->keybinds[GameKey_DownRight] = Key_C;
        
        game->keybinds[GameKey_OpenInventory] = Key_I;
        game->keybinds[GameKey_Pickup] = Key_Comma;
        game->keybinds[GameKey_UsePassage] = Key_U;
        game->keybinds[GameKey_AutoExplore] = Key_P;
        game->keybinds[GameKey_IteratePassages] = Key_LessThan;
        
        game->keybinds[GameKey_Examine] = Key_O;
        game->keybinds[GameKey_Status] = Key_Y;
        game->keybinds[GameKey_Log] = Key_L;
        
        game->keybinds[GameKey_Back] = Key_Escape;
        game->keybinds[GameKey_Wait] = Key_V;
        game->keybinds[GameKey_Rest] = Key_B;
        game->keybinds[GameKey_Yes] = Key_H;
        game->keybinds[GameKey_No] = Key_J;
#endif
        
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            u32 window_flags = SDL_WINDOW_SHOWN;
            game->window = SDL_CreateWindow("Moonbreath",
                                            SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED,
                                            game->window_size.w,
                                            game->window_size.h,
                                            window_flags);
            
            if(game->window)
            {
                u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
                game->renderer = SDL_CreateRenderer(game->window, -1, renderer_flags);
                
                if(game->renderer)
                {
                    u32 image_flags = IMG_INIT_PNG;
                    
                    if(IMG_Init(image_flags) & image_flags)
                    {
                        if(!TTF_Init())
                        {
                            if(init_assets(game, assets))
                            {
                                ui->font = &assets->fonts[FontName_DosVga];
                                ui->window_scroll_start_y = game->window_size.h - assets->stat_and_log_window_h;
                                init_view_scrolling_data(&ui->full_log.view, get_font_newline(ui->font->size), ui->default_view_step_multiplier);
                                
                                //u64 seed = time(0);
                                u64 seed = 9560064343;
                                printf("Seed: %lu\n", seed);
                                game->random = set_random_seed(seed);
                                
                                game->mode = GameMode_Playing;
                                //game->mode = GameMode_MainMenu;
                                game->camera.rect = make_v4s(0, 0, game->window_size.w, ui->window_scroll_start_y);
                                
                                u32 target_fps = 60;
                                f32 target_seconds_per_frame = 1.0f / (f32)target_fps;
                                
                                u64 performance_frequency = SDL_GetPerformanceFrequency();
                                u64 last_counter = SDL_GetPerformanceCounter();
                                f32 last_dt = (f32)SDL_GetPerformanceCounter();
                                
                                Input input[2] = {0};
                                Input *new_input = &input[0];
                                Input *old_input = &input[1];
                                
                                for(u32 index = 0; index < Button_Count; ++index)
                                {
                                    old_input->mouse_buttons[index].has_been_up = true;
                                }
                                
                                for(u32 index = Key_None + 1; index < Key_Count; ++index)
                                {
                                    old_input->keyboard[index].has_been_up = true;
                                }
                                
                                for(u32 index = 0; index < GameKey_Count; ++index)
                                {
                                    old_input->game_keys[index].has_been_up = true;
                                }
                                
                                for(u32 index = 1; index < array_count(new_input->fkeys); ++index)
                                {
                                    old_input->fkeys[index].has_been_up = true;
                                }
                                
#if MOONBREATH_SLOW
                                f32 fps = 0.0f;
                                f32 full_ms_per_frame = 0.0f;
                                f32 work_ms_per_frame = 0.0f;
                                
                                // Init debug
                                DebugState *debug = &game->debug;
                                
                                debug->font = &assets->fonts[FontName_DosVga];
                                debug->text_offset.x = get_font_newline(debug->font->size) * 2;
                                debug->text_offset.y = get_font_newline(debug->font->size);
                                
                                debug->memory_size = megabytes(1);
                                init_memory_arena(&debug->memory_arena,
                                                  memory.storage + memory.size - debug->memory_size,
                                                  debug->memory_size);
                                
                                DebugTree *var_tree = add_debug_tree(debug, 50, 25);
                                DebugTree *color_tree = add_debug_tree(debug, 300, 25);
                                
                                start_debug_group(debug, var_tree, "Variables", false);
                                {
                                    add_debug_variable(var_tree, "Frame MS", full_ms_per_frame, DebugVarType_F32);
                                    add_debug_variable(var_tree, "Work MS", work_ms_per_frame, DebugVarType_F32);
                                    add_debug_variable(var_tree, "Frame DT", new_input->frame_dt, DebugVarType_F32);
                                    add_debug_newline(debug, var_tree);
                                    
                                    add_debug_variable(var_tree, "Mouse", new_input->mouse_pos, DebugVarType_V2U);
                                    add_debug_variable(var_tree, "Mouse Tile", new_input->mouse_tile_pos, DebugVarType_V2U);
                                    add_debug_variable(var_tree, "Player Tile", player->pos, DebugVarType_V2U);
                                    add_debug_newline(debug, var_tree);
                                    
                                    add_debug_variable(var_tree, "Traverse All", debug_toggles[DebugToggleType_TraverseAll], DebugVarType_B32);
                                    add_debug_variable(var_tree, "Skip Has Been Up", debug_toggles[DebugToggleType_SkipHasBeenUp], DebugVarType_B32);
                                    add_debug_variable(var_tree, "Entity Hit Test", debug_toggles[DebugToggleType_EntityHitTest], DebugVarType_B32);
                                }
                                end_debug_group(var_tree);
                                
                                start_debug_group(debug, color_tree, "Colors", false);
                                {
                                    add_debug_string(color_tree, get_color_string(Color_White), Color_White);
                                    add_debug_string(color_tree, get_color_string(Color_LightGray), Color_LightGray);
                                    add_debug_string(color_tree, get_color_string(Color_DarkGray), Color_DarkGray);
                                    add_debug_string(color_tree, get_color_string(Color_LightRed), Color_LightRed);
                                    add_debug_string(color_tree, get_color_string(Color_DarkRed), Color_DarkRed);
                                    add_debug_string(color_tree, get_color_string(Color_Green), Color_Green);
                                    add_debug_string(color_tree, get_color_string(Color_LightGreen), Color_LightGreen);
                                    add_debug_string(color_tree, get_color_string(Color_DarkGreen), Color_DarkGreen);
                                    add_debug_string(color_tree, get_color_string(Color_LightBlue), Color_LightBlue);
                                    add_debug_string(color_tree, get_color_string(Color_DarkBlue), Color_DarkBlue);
                                    add_debug_string(color_tree, get_color_string(Color_LightBrown), Color_LightBrown);
                                    add_debug_string(color_tree, get_color_string(Color_DarkBrown), Color_DarkBrown);
                                    add_debug_string(color_tree, get_color_string(Color_Cyan), Color_Cyan);
                                    add_debug_string(color_tree, get_color_string(Color_LightYellow), Color_LightYellow);
                                    add_debug_string(color_tree, get_color_string(Color_Purple), Color_Purple);
                                    add_debug_string(color_tree, get_color_string(Color_Orange), Color_Orange);
                                }
                                end_debug_group(color_tree);
#endif
                                
                                while(game->mode)
                                {
                                    set_render_color(game->renderer, Color_Black);
                                    SDL_RenderClear(game->renderer);
                                    
                                    for(u32 index = 0; index < Button_Count; ++index)
                                    {
                                        new_input->mouse_buttons[index] = old_input->mouse_buttons[index];
                                    }
                                    
                                    for(u32 index = Key_None + 1; index < Key_Count; ++index)
                                    {
                                        new_input->keyboard[index] = old_input->keyboard[index];
                                    }
                                    
                                    for(u32 index = 0; index < GameKey_Count; ++index)
                                    {
                                        new_input->game_keys[index] = old_input->game_keys[index];
                                    }
                                    
                                    for(u32 index = 1; index < array_count(new_input->fkeys); ++index)
                                    {
                                        new_input->fkeys[index] = old_input->fkeys[index];
                                    }
                                    
                                    update_mouse_and_keyboard(game, new_input);
                                    
                                    f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                    new_input->frame_dt = ((end_dt - last_dt) / (f32)performance_frequency);
                                    last_dt = end_dt;
                                    
                                    update_and_render_game(game, new_input, entity_state, dungeon_state, item_state, inventory, assets, ui);
                                    
#if MOONBREATH_SLOW
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
                                        //printf("Missed frame rate: %.01f\n", work_ms_per_frame);
                                    }
                                    
                                    u64 end_counter = SDL_GetPerformanceCounter();
                                    u64 elapsed_counter = end_counter - last_counter;
                                    last_counter = end_counter;
                                    
#if MOONBREATH_SLOW
                                    fps = (f32)performance_frequency / (f32)elapsed_counter;
                                    full_ms_per_frame = get_ms_from_elapsed(elapsed_counter, performance_frequency);
#endif
                                    
                                    Input *temp = new_input;
                                    new_input = old_input;
                                    old_input = temp;
                                    
                                    SDL_RenderPresent(game->renderer);
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
        
        // Cleanup and exit the game
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        
        free_assets(assets);
        free(memory.storage);
        
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    else
    {
        // TODO(rami): Logging
        printf("ERROR: Could not allocate game memory.\n");
    }
    
    return(result);
}