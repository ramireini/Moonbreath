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

/* TODO(rami):

- A way to view items in player range or a certain dungeon level.
- Dungeon traps
- Items art
- Enemies art

*/

internal void
update_examine_mode(Game *game,
                    Input *input,
                    Entity *player,
                    EntityState *entities,
                    ItemState *items,
                    Inventory *inventory,
                    Dungeon *dungeon)
{
    Examine *examine = &game->examine;
    
    if(is_set(game->examine.flags, ExamineFlags_Open))
    {
        if(examine->type == ExamineType_Entity)
        {
            char pressed = get_pressed_alphabet_char(input);
            if(pressed)
            {
                if(is_set(game->examine.flags, ExamineFlags_ReadyForKeypress))
                {
                    Spell *spell = &examine->entity->e.spells[(pressed - 'a')];
                    if(spell->id)
                    {
                        examine->type = ExamineType_EntitySpell;
                        examine->spell = spell;
                    }
                }
                else
                {
                    set(game->examine.flags, ExamineFlags_ReadyForKeypress);
                }
            }
        }
        else
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
                            u32 hold_duration = SDL_GetTicks() - examine->key_pressed_start[index];
                            if(hold_duration >= examine->key_hold_duration)
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
            
            if(was_pressed(&input->GameKey_IteratePassages))
            {
                
                #if 0
                for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                {
                    Passage *passage = &dungeon->passages[index];
                    if(passage->type)
                    {
                        printf("Passage[%u]: %s\n", index, passage->type == PassageType_Up ? "Up" : "Down");
                        printf("Pos: %u, %u\n\n", passage->pos.x, passage->pos.y);
                    }
                }
                #endif
                
                PassageType passage_search_type = PassageType_None;
                u32 *examine_passage_index = &game->examine.passage_index;
                
                // Set which passage type we want to find.
                if(input->Key_Shift.is_down)
                {
                    passage_search_type = PassageType_Up;
                }
                else
                {
                    passage_search_type = PassageType_Down;
                }
                
                // Attempt to find the passage.
                for(u32 passage_index = 0; passage_index < MAX_DUNGEON_PASSAGE_COUNT; ++passage_index)
                {
                    Passage *passage = &dungeon->passages[passage_index];
                    
                    if(has_tile_been_seen(dungeon->tiles, passage->pos) &&
                           passage->type == passage_search_type &&
                           *examine_passage_index != passage_index)
                    {
                            //printf("Went to %s passage[%u]: %u, %u\n", passage->type == PassageType_Up ? "up" : "down", passage_index, passage->pos.x, passage->pos.y);
                            
                            *examine_passage_index = passage_index;
                            examine->pos = passage->pos;
                            break;
                    }
                }
            }
            else if(was_pressed(&input->GameKey_AutoExplore))
            {
                // Pathfind to passage
                for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                {
                    Passage *passage = &dungeon->passages[index];
                    if(passage->type && equal_v2u(passage->pos, examine->pos))
                    {
                        unset(game->examine.flags, ExamineFlags_Open);
                        start_entity_pathfind(player, dungeon, items, &entities->player_pathfind, examine->pos);
                        return;
                    }
                }
                
                // Pathfind to tile
                if(is_tile_traversable_and_has_been_seen(dungeon->tiles, examine->pos))
                {
                    unset(game->examine.flags, ExamineFlags_Open);
                    start_entity_pathfind(player, dungeon, items, &entities->player_pathfind, examine->pos);
                    return;
                }
            }
            else if(was_pressed(&input->GameKey_Yes))
            {
                if(get_pos_item_count(items, examine->pos) > 1)
                {
                    unset(game->examine.flags, ExamineFlags_Open);
                    set(inventory->flags, InventoryFlags_MultipleExamine);
                    
                    set_view_at_start(&inventory->examine_view);
                    return;
                }
                else
                {
                // Examine item
                    examine->item = get_item_on_pos(items, examine->pos, 0);
                    if(examine->item)
                    {
                        examine->type = ExamineType_Item;
                        return;
                    }
                    }
                
                    // Examine entity
                    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
                    {
                    Entity *entity = &entities->array[index];
                        if(is_entity_valid_and_not_player(entity->type) &&
                           equal_v2u(examine->pos, entity->pos))
                        {
                            examine->type = ExamineType_Entity;
                            examine->entity = entity;
                            return;
                        }
                    }
                
                    // Examine tile
                    examine->type = ExamineType_Tile;
                examine->tile_id = get_pos_tile_id(dungeon->tiles, examine->pos);
                return;
                }
            }
        }
    }

internal Direction
get_direction_moved_from(v2u old_pos, v2u new_pos)
{
    Direction result = Direction_None;
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        if(equal_v2u(old_pos, get_direction_pos(new_pos, direction)))
        {
            result = direction;
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
get_game_dest(Game *game, v2u pos)
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
render_tilemap(Game *game, Dungeon *dungeon, Assets *assets)
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
    print_v4u("render_area", render_area);
#endif
    
    for(u32 y = render_area.y; y <= render_area.h; ++y)
    {
        for(u32 x = render_area.x; x <= render_area.w; ++x)
        {
            v2u render_pos = {x, y};
            
            v4u src = get_tile_rect(get_tile_tileset_pos(dungeon->tiles, render_pos));
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

internal b32
is_window_1920x1080(v2u window_size)
{
    b32 result = (window_size.w == 1920 &&
                  window_size.h == 1080);
    
    return(result);
}

internal b32
is_window_1280x720(v2u window_size)
{
    b32 result = (window_size.w == 1280 &&
                  window_size.h == 720);
    
    return(result);
}

internal void
update_camera(Game *game, Entity *player, Dungeon *dungeon)
{
    v2u camera_follow_pos = {0};
    if(is_set(game->examine.flags, ExamineFlags_Open))
    {
        camera_follow_pos = game->examine.pos;
    }
    else
    {
        camera_follow_pos = player->pos;
    }
    
    game->camera.x = tile_mul(camera_follow_pos.x) - (game->camera.w / 2);
    
    if(is_window_1920x1080(game->window_size))
    {
        game->camera.y = tile_mul(camera_follow_pos.y) - (game->camera.h / 2) + 4;
    }
    else if(is_window_1280x720(game->window_size))
    {
        game->camera.y = tile_mul(camera_follow_pos.y) - (game->camera.h / 2) + 16;
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
    print_v4s("game->camera", game->camera);
    
    printf("camera.x2: %d\n", game->camera.x + game->camera.w);
    printf("camera.y2: %d\n\n", game->camera.y + game->camera.h);
    
    printf("camera_tile_offset_x: %u\n", tile_div(game->camera.x));
    printf("camera_tile_offset_y: %u\n\n", tile_div(game->camera.y));
#endif
}

internal void
update_input(InputState *state, b32 is_down)
{
    if(state->is_down != is_down)
    {
        state->repeat = false;
        state->is_down = is_down;
        
        if(!state->is_down)
        {
            state->has_been_up = true;
        }
    }
}

internal void
reset_input_state(InputState *state)
{
    state->repeat = false;
    state->is_down = false;
    state->has_been_up = true;
}

internal Key
get_key_from_keycode(SDL_Keycode keycode)
{
    Key result = 0;
    
    switch(keycode)
    {
        case SDLK_w: result = Key_W; break;
        case SDLK_s: result = Key_S; break;
        case SDLK_a: result = Key_A; break;
        case SDLK_d: result = Key_D; break;
        
        case SDLK_q: result = Key_Q; break;
        case SDLK_e: result = Key_E; break;
        case SDLK_z: result = Key_Z; break;
        case SDLK_c: result = Key_C; break;
        
        case SDLK_i: result = Key_I; break;
        case SDLK_COMMA: result = Key_Comma; break;
        case SDLK_u: result = Key_U; break;
        case SDLK_p: result = Key_P; break;
        case SDLK_LESS: result = Key_LessThan; break;
        case SDLK_o: result = Key_O; break;
        case SDLK_l: result = Key_L; break;
        
        case SDLK_ESCAPE: result = Key_Escape; break;
        case SDLK_v: result = Key_V; break;
        case SDLK_h: result = Key_H; break;
        case SDLK_j: result = Key_J; break;
    }
    
    return(result);
}

internal void
update_events(Game *game, Input *input)
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
    render_fill_rect(game, rect, Color_WindowBorder);
    
    // Window background
    v4u background_rect = get_border_adjusted_rect(rect, border_size);
    render_fill_rect(game, background_rect, Color_WindowBackground);
    
    // Window accent
    render_draw_rect(game, background_rect, Color_WindowAccent);
}

internal void
render_draw_rect(Game *game, v4u rect, Color color)
{
    set_render_color(game, color);
    SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&rect);
}

internal void
render_fill_rect(Game *game, v4u rect, Color color)
{
    set_render_color(game, color);
    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&rect);
}

internal char
get_char(char c, b32 is_shift_down)
{
    char result = 0;
    
    if(is_shift_down)
    {
        result = c - 32;
    }
    else
    {
        result = c;
    }
    
    assert(result);
    return(result);
}

internal PrintableKey
get_printable_key(Input *input, Key key)
{
    PrintableKey result = {0};
    
    switch(key)
    {
        case Key_A: result.c = get_char('a', input->Key_Shift.is_down); break;
        case Key_B: result.c = get_char('b', input->Key_Shift.is_down); break;
        case Key_C: result.c = get_char('c', input->Key_Shift.is_down); break;
        case Key_D: result.c = get_char('d', input->Key_Shift.is_down); break;
        case Key_E: result.c = get_char('e', input->Key_Shift.is_down); break;
        case Key_F: result.c = get_char('f', input->Key_Shift.is_down); break;
        case Key_G: result.c = get_char('g', input->Key_Shift.is_down); break;
        case Key_H: result.c = get_char('h', input->Key_Shift.is_down); break;
        case Key_I: result.c = get_char('i', input->Key_Shift.is_down); break;
        case Key_J: result.c = get_char('j', input->Key_Shift.is_down); break;
        case Key_K: result.c = get_char('k', input->Key_Shift.is_down); break;
        case Key_L: result.c = get_char('l', input->Key_Shift.is_down); break;
        case Key_M: result.c = get_char('m', input->Key_Shift.is_down); break;
        case Key_N: result.c = get_char('n', input->Key_Shift.is_down); break;
        case Key_O: result.c = get_char('o', input->Key_Shift.is_down); break;
        case Key_P: result.c = get_char('p', input->Key_Shift.is_down); break;
        case Key_Q: result.c = get_char('q', input->Key_Shift.is_down); break;
        case Key_R: result.c = get_char('r', input->Key_Shift.is_down); break;
        case Key_S: result.c = get_char('s', input->Key_Shift.is_down); break;
        case Key_T: result.c = get_char('t', input->Key_Shift.is_down); break;
        case Key_U: result.c = get_char('u', input->Key_Shift.is_down); break;
        case Key_V: result.c = get_char('v', input->Key_Shift.is_down); break;
        case Key_W: result.c = get_char('w', input->Key_Shift.is_down); break;
        case Key_X: result.c = get_char('x', input->Key_Shift.is_down); break;
        case Key_Y: result.c = get_char('y', input->Key_Shift.is_down); break;
        case Key_Z: result.c = get_char('z', input->Key_Shift.is_down); break;
        
        case Key_0:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '=';
            }
            else
            {
                result.c = '0';
            }
        } break;
        
        case Key_1:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '!';
            }
            else
            {
                result.c = '1';
            }
        } break;
        
        case Key_2:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '"';
            }
            else
            {
                result.c = '2';
            }
        } break;
        
        case Key_3:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '#';
            }
            else
            {
                result.c = '3';
            }
        } break;
        
        case Key_4:
        {
            result.c = '4';
        } break;
        
        case Key_5:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '%';
            }
            else
            {
                result.c = '5';
            }
        } break;
        
        case Key_6:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '&';
            }
            else
            {
                result.c = '6';
            }
        } break;
        
        case Key_7:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '/';
            }
            else
            {
                result.c = '7';
            }
        } break;
        
        case Key_8:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '(';
            }
            else
            {
                result.c = '8';
            }
        } break;
        
        case Key_9:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = ')';
            }
            else
            {
                result.c = '9';
            }
        } break;
        
        case Key_Space: result.c = ' '; break;
        
        case Key_Plus:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '?';
            }
            else
            {
                result.c = '+';
            }
        } break;
        
        case Key_Minus:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = '_';
            }
            else
            {
                result.c = '-';
            }
        } break;
        
        case Key_Comma:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = ';';
            }
            else
            {
                result.c = ',';
            }
        } break;
        
        case Key_Period:
        {
            if(input->Key_Shift.is_down)
            {
                result.c = ':';
            }
            else
            {
                result.c = '.';
            }
        } break;
        
        case Key_Escape: result.str = "ESC"; break;
    }
    
    assert(result.c || result.str);
    return(result);
}

internal char
get_pressed_keyboard_char(Input *input)
{
    char result = 0;
    
    for(Key key = Key_A; key < Key_Shift; ++key)
    {
        if(was_pressed(&input->keyboard[key]))
        {
            result = get_printable_key(input, key).c;
            break;
        }
    }
    
    return(result);
}

internal char
get_pressed_alphabet_char(Input *input)
{
    char result = 0;
    
    for(Key key = Key_A; key <= Key_Z; ++key)
    {
        if(was_pressed(&input->keyboard[key]))
        {
            result = get_printable_key(input, key).c;
            break;
        }
    }
    
    return(result);
}

internal void
update_and_render_game(Game *game,
                       Input *input,
                       Dungeon *dungeon,
                       Entity *player,
                       EntityState *entities,
                       ItemState *items,
                       Inventory *inventory,
                       Assets *assets,
                       UI *ui)
{
    if(game->mode == GameMode_MainMenu)
    {
        v4u rect = {50, 300, 200, 100};
        render_fill_rect(game, rect, Color_Cyan);
        
        if(is_inside_rect(rect, input->mouse_pos))
        {
            render_text(game, "%sNew Game", 100, 340, &assets->fonts[FontName_DosVga], 0, start_color(Color_Yellow));
            
            if(was_pressed(&input->Button_Left))
            {
                game->mode = GameMode_Playing;
            }
        }
        else
        {
            render_text(game, "New Game", 100, 340, &assets->fonts[FontName_DosVga], 0);
        }
    }
    else if(game->mode == GameMode_Playing)
    {
        // TODO(rami): Set game is_initialized to false when going back to main menu.
        if(!game->is_initialized)
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
                ConsumableInfo *info = &items->potion_info[index];
                
                while(is_zero_v2u(info->tile))
                {
                    u32 potion_index = random_number(&game->random, 0, Potion_Count - 1);
                    if(!potion_color_set[potion_index])
                    {
                        potion_color_set[potion_index] = true;
                        
                        // Add a random not already taken adjective to the potion depiction.
                        for(;;)
                        {
                            u32 adjective_index = random_number(&game->random, 0, 15);
                            if(!potion_adjective_taken[adjective_index])
                            {
                                potion_adjective_taken[adjective_index] = true;
                                strcpy(info->depiction, potion_adjectives[adjective_index]);
                                
                                break;
                            }
                        }
                        
                        switch(potion_index)
                        {
                            case 0:
                            {
                                info->tile = make_v2u(10, 2);
                                strcat(info->depiction, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile = make_v2u(10, 3);
                                strcat(info->depiction, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile = make_v2u(10, 4);
                                strcat(info->depiction, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile = make_v2u(10, 5);
                                strcat(info->depiction, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile = make_v2u(10, 6);
                                strcat(info->depiction, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile = make_v2u(10, 7);
                                strcat(info->depiction, "Purple ");
                            } break;
                            
                            case 6:
                            {
                                info->tile = make_v2u(10, 8);
                                strcat(info->depiction, "Green ");
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
                ConsumableInfo *info = &items->scroll_info[index];
                
                while(is_zero_v2u(info->tile))
                {
                    u32 scroll_index = random_number(&game->random, 0, Scroll_Count - 1);
                    if(!scroll_color_set[scroll_index])
                    {
                        scroll_color_set[scroll_index] = true;
                        
                        switch(scroll_index)
                        {
                            case 0:
                            {
                                info->tile = make_v2u(11, 2);
                                strcpy(info->depiction, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile = make_v2u(11, 3);
                                strcpy(info->depiction, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile = make_v2u(11, 4);
                                strcpy(info->depiction, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile = make_v2u(11, 5);
                                strcpy(info->depiction, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile = make_v2u(11, 6);
                                strcpy(info->depiction, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile = make_v2u(11, 7);
                                strcpy(info->depiction, "Purple ");
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        break;
                    }
                }
            }
            
            items->potion_healing_range = make_v2u(20, 40);
            items->ration_healing_range = make_v2u(10, 20);
            
#if 0
            // Print randomized potion and scroll tiles.
            printf("\nRandomized Potion Tiles\n");
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                ConsumableInfo *info = &items->potion_info[index];
                
                printf("[%u]: %u, %u\n", index, info->tile.x, info->tile.y);
            }
            
            printf("\nRandomized Scroll Tiles\n");
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                ConsumableInfo *info = &items->scroll_info[index];
                printf("[%u]: %u, %u\n", index, info->tile.x, info->tile.y);
            }
#endif
            // Set entity spell chances
            entities->spell_chances[SpellID_DarkBolt] = 40;
            entities->spell_chances[SpellID_LesserHeal] = 30;
            entities->spell_chances[SpellID_Bolster] = 20;
            
            // Set entity levels
            entities->levels[EntityID_SkeletonWarrior] = 1;
            entities->levels[EntityID_SkeletonArcher] = 1;
            entities->levels[EntityID_SkeletonMage] = 1;
            entities->levels[EntityID_Bat] = 1;
            entities->levels[EntityID_Rat] = 1;
            
            entities->levels[EntityID_KoboldWarrior] = 2;
            entities->levels[EntityID_KoboldShaman] = 2;
            entities->levels[EntityID_Snail] = 2;
            entities->levels[EntityID_Slime] = 2;
            entities->levels[EntityID_Dog] = 2;
            
            entities->levels[EntityID_OrcWarrior] = 3;
            entities->levels[EntityID_OrcArcher] = 3;
            entities->levels[EntityID_OrcShaman] = 3;
            entities->levels[EntityID_Python] = 3;
            entities->levels[EntityID_Shade] = 3;
            
            entities->levels[EntityID_ElfKnight] = 4;
            entities->levels[EntityID_ElfArbalest] = 4;
            entities->levels[EntityID_ElfMage] = 4;
            entities->levels[EntityID_GiantSlime] = 4;
            entities->levels[EntityID_Spectre] = 4;
            
            entities->levels[EntityID_OrcSorcerer] = 5;
            entities->levels[EntityID_OrcAssassin] = 5;
            entities->levels[EntityID_Minotaur] = 5;
            entities->levels[EntityID_Treant] = 5;
            entities->levels[EntityID_Viper] = 5;
            
            entities->levels[EntityID_CentaurWarrior] = 6;
            entities->levels[EntityID_CentaurSpearman] = 6;
            entities->levels[EntityID_CentaurArcher] = 6;
            entities->levels[EntityID_CursedSkull] = 6;
            entities->levels[EntityID_Wolf] = 6;
            
            entities->levels[EntityID_OgreWarrior] = 7;
            entities->levels[EntityID_OgreArcher] = 7;
            entities->levels[EntityID_OgreMage] = 7;
            entities->levels[EntityID_Cyclops] = 7;
            entities->levels[EntityID_ShadowWalker] = 7;
            
            entities->levels[EntityID_DwarwenWarrior] = 8;
            entities->levels[EntityID_DwarwenSorcerer] = 8;
            entities->levels[EntityID_DwarwenPriest] = 8;
            entities->levels[EntityID_ScarletSnake] = 8;
            entities->levels[EntityID_Lich] = 8;
            
            entities->levels[EntityID_AbyssalFiend] = 9;
            entities->levels[EntityID_BloodTroll] = 9;
            entities->levels[EntityID_IronGolem] = 9;
            entities->levels[EntityID_Griffin] = 9;
            entities->levels[EntityID_Imp] = 9;
            
            entities->levels[EntityID_BlackKnight] = 10;
            entities->levels[EntityID_GiantDemon] = 10;
            entities->levels[EntityID_Hellhound] = 10;
            entities->levels[EntityID_AbyssalHexmaster] = 10;
            entities->levels[EntityID_Mahjarrat] = 10;
            
            add_player_entity(&game->random, player);
            create_dungeon(game, player, entities, dungeon, items, inventory, ui);
            update_fov(player, dungeon);
            
            ui->font = &assets->fonts[FontName_DosVga];
            ui->font_newline = get_font_newline(ui->font->size);
            
            log_add(ui, "%sWelcome, %s!", start_color(Color_Yellow), player->name);
            log_add(ui, "%sFind and destroy the underworld portal, ", start_color(Color_Yellow));
            log_add(ui, "%swhich is located somewhere in the depths.", start_color(Color_Yellow));
            
            game->is_initialized = true;
        }
        
        update_examine_mode(game, input, player, entities, items, inventory, dungeon);
        update_entities(game, input, player, entities, items, inventory, dungeon, assets, ui);
        update_camera(game, player, dungeon);
        
        render_tilemap(game, dungeon, assets);
        render_items(game, player, items, dungeon, assets);
        render_entities(game, entities, inventory, dungeon, assets);
        render_ui(game, input, player, items, inventory, dungeon, assets, ui);
        
#if MOONBREATH_SLOW
        if(other_windows_are_closed(game, inventory, ui))
        {
        // Render cursor rect on mouse tile.
        v2u tile_pos =
        {
            tile_div(input->mouse_pos.x),
            tile_div(input->mouse_pos.y)
        };
        
        v2u camera_offset =
        {
            tile_div(game->camera.x),
            tile_div(game->camera.y)
        };
        
        input->mouse_tile_pos.x = tile_pos.x + camera_offset.x;
        input->mouse_tile_pos.y = tile_pos.y + camera_offset.y;
        
        if(tile_pos.y < tile_div(game->camera.h))
        {
            v4u dest = get_game_dest(game, input->mouse_tile_pos);
            SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
            }
        }
    #endif
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

internal void *
add_to_game_memory(GameMemory *memory, u32 add_size)
{
    void *result = 0;
    
    if((memory->used + add_size) <= memory->size)
    {
        result = memory->storage + memory->used;
        memory->used += add_size;
    }
    else
    {
        assert(!"No space in game memory storage.");
    }
    
    printf("Used Game Memory: %u/%u (%u added)\n", memory->used, memory->size, add_size);
    
    return(result);
}

int main(int argc, char *argv[])
{
    u32 result = 0;
    
    GameMemory memory = {0};
    memory.size = megabytes(4);
    memory.storage = calloc(1, memory.size);
    
    if(memory.storage)
    {
        Game *game = add_to_game_memory(&memory, sizeof(Game));
        game->examine.key_hold_duration = 400;
        
        EntityState *entities = add_to_game_memory(&memory, sizeof(EntityState));
        Entity *player = &entities->array[0];
        
        Dungeon *dungeon = add_to_game_memory(&memory, sizeof(Dungeon));
        dungeon->tiles.array = add_to_game_memory(&memory, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(Tile));
        
        ItemState *items = add_to_game_memory(&memory, sizeof(ItemState));
        
        Inventory *inventory = add_to_game_memory(&memory, sizeof(Inventory));
        inventory->entry_size = 32;
        
        Assets *assets = add_to_game_memory(&memory, sizeof(Assets));
        
        UI *ui = add_to_game_memory(&memory, sizeof(UI));
        ui->window_offset = 12;
        ui->short_log_view.end = 9;
        ui->mark.render_duration = 800;
        ui->mark.view.end = 24;
        
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
        
        // TODO(rami): Need to check success on everything.
        Config config = get_config("data/config.txt");
        
        ConfigValue show_item_ground_outline = get_config_bool(&config, "show_item_ground_outline");
        if(show_item_ground_outline.boolean)
        {
            game->show_item_ground_outline = show_item_ground_outline.boolean;
        }
        
        ConfigValue window_size = get_config_uint(&config, "window_size");
        if(window_size.uint)
        {
            if(window_size.uint == 1)
            {
                game->window_size = make_v2u(1920, 1080);
            }
            else if(window_size.uint == 2)
            {
                game->window_size = make_v2u(1280, 720);
            }
        }
        else
        {
            assert(0);
        }
        
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
                case GameKey_AscendDescend: token_name = "key_ascend_descend"; break;
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
        game->keybinds[GameKey_AscendDescend] = Key_U;
        game->keybinds[GameKey_AutoExplore] = Key_P;
        game->keybinds[GameKey_IteratePassages] = Key_LessThan;
        game->keybinds[GameKey_Examine] = Key_O;
        game->keybinds[GameKey_Log] = Key_L;
        
        game->keybinds[GameKey_Back] = Key_Escape;
        game->keybinds[GameKey_Wait] = Key_V;
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
                            if(initialize_assets(game, assets))
                            {
                                
#if 0
                                u64 seed = time(0);
#else
                                u64 seed = 1602811425;
#endif
                                printf("Seed: %lu\n", seed);
                                
                                game->random = set_random_seed(seed);
                                
                                game->mode = GameMode_Playing;
                                //game->mode = GameMode_MainMenu;
                                
                                game->camera = make_v4s(0, 0,
                                                        game->window_size.w,
                                                        game->window_size.h - assets->stat_and_log_window_h);
                                
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
                                    old_input->mouse[index].has_been_up = true;
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
                                
                                DebugState debug_state = {0};
                                
                                DebugGroup *debug_vars = add_debug_group(&debug_state, "Variables", 25, 25, &assets->fonts[FontName_DosVga]);
                                add_debug_float32(debug_vars, "FPS", &fps);
                                add_debug_float32(debug_vars, "Frame MS", &full_ms_per_frame);
                                add_debug_float32(debug_vars, "Work MS", &work_ms_per_frame);
                                add_debug_float32(debug_vars, "Frame DT", &new_input->frame_dt);
                                add_debug_newline(debug_vars);
                                
                                add_debug_v2u(debug_vars, "Mouse", &new_input->mouse_pos);
                                add_debug_v2u(debug_vars, "Mouse Tile", &new_input->mouse_tile_pos);
                                add_debug_v2u(debug_vars, "Player Tile", &player->pos);
                                add_debug_newline(debug_vars);
                                
                                add_debug_bool32(debug_vars, "FOV Toggle", &fkey_active[1]);
                                add_debug_bool32(debug_vars, "Traversable Toggle", &fkey_active[2]);
                                add_debug_bool32(debug_vars, "Has Been Up Toggle", &fkey_active[3]);
                                add_debug_bool32(debug_vars, "Hit Test Toggle", &fkey_active[4]);
                                
                                DebugGroup *debug_colors = add_debug_group(&debug_state, "Colors", 125, 25, &assets->fonts[FontName_DosVga]);
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
                                
                                while(game->mode)
                                {
                                    set_render_color(game, Color_Black);
                                    SDL_RenderClear(game->renderer);
                                    
                                    for(u32 index = 0; index < Button_Count; ++index)
                                    {
                                        new_input->mouse[index] = old_input->mouse[index];
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
                                    
                                    update_events(game, new_input);
                                    
                                    f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                    new_input->frame_dt = ((end_dt - last_dt) / (f32)performance_frequency);
                                    last_dt = end_dt;
                                    
                                    update_and_render_game(game,
                                                           new_input,
                                                           dungeon,
                                                           player,
                                                           entities,
                                                           items,
                                                           inventory,
                                                           assets,
                                                           ui);
                                    
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
                                    
                                    update_and_render_debug_state(game, &debug_state, new_input);
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
        
        // Cleanup and exit the game.
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