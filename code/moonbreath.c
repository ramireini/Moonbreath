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
/*
 - Way to list all seen items that are currently on the floor of the current dungeon level.

Examination mode:
- Resistances
- Status effects

Pathfind:
- Do pathfind work in an infinite loop or with a pass every frame?

Items:
- Pick up messages for consumable items should say the stack count if it's more than one.
- Same for the inspect window from inventory and examine mode.

Art:
- Items
- Enemies
*/

internal void
update_examine_mode(Game *game,
                    Dungeon *dungeon,
                    Entity *player,
                    Entity *entities,
                    Item *items,
                    Input *input)
{
    Examine *examine = &game->examine;
    
    if(is_set(game->examine.flags, ExamineFlags_Open))
    {
        if(examine->type == ExamineType_Entity)
        {
            char pressed_char = get_pressed_alphabet_char(input);
            if(pressed_char)
            {
                if(is_set(game->examine.flags, ExamineFlags_ReadyForKeypress))
                {
                    Spell *spell = &examine->entity->e.spells[(pressed_char - 'a')];
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
                if(input->game_keys[index].ended_down)
                {
                    b32 can_move = true;
                    for(u32 second_index = GameKey_Up; second_index <= GameKey_DownRight; ++second_index)
                    {
                        if(second_index != index &&
                           examine->is_key_pressed[second_index])
                        {
                            can_move = false;
                            break;
                        }
                    }
                    
                    if(can_move)
                    {
                        if(examine->key_pressed_start[index])
                        {
                            u32 hold_time_ms = SDL_GetTicks() - examine->key_pressed_start[index];
                            if(hold_time_ms >= 400)
                            {
                                examine->is_key_pressed[index] = true;
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
                    examine->is_key_pressed[index] = false;
                    examine->key_pressed_start[index] = 0;
                }
            }
            
            if(was_pressed(&input->GameKey_IteratePassages))
            {
                b32 *start_passages_from_first = false;
                u32 *passage_index = 0;
                PassageType to_find_type = PassageType_None;
                
                if(was_pressed(&input->Key_Shift))
                {
                    start_passages_from_first = &game->examine.start_up_passages_from_first;
                    passage_index = &game->examine.up_passage_index;
                    to_find_type = PassageType_Up;
                }
                else
                {
                    start_passages_from_first = &game->examine.start_down_passages_from_first;
                    passage_index = &game->examine.down_passage_index;
                    to_find_type = PassageType_Down;
                }
                
                for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                {
                    Passage *passage = &dungeon->passages[index];
                    if(is_passage_type_and_has_been_seen(dungeon->tiles, passage, to_find_type) &&
                       (*passage_index < index || *start_passages_from_first))
                    {
                        b32 is_last_valid_set = false;
                        u32 last_valid_index = 0;
                        for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                        {
                            Passage *passage = &dungeon->passages[index];
                            if(is_passage_type_and_has_been_seen(dungeon->tiles, passage, to_find_type))
                            {
                                is_last_valid_set = true;
                                last_valid_index = index;
                            }
                        }
                        
                        if(is_last_valid_set && index == last_valid_index)
                        {
                            *start_passages_from_first = true;
                        }
                        else
                        {
                            *start_passages_from_first = false;
                            *passage_index = index;
                        }
                        
                        examine->pos = passage->pos;
                        break;
                    }
                }
            }
            else if(was_pressed(&input->GameKey_AutoExplore))
            {
                for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                {
                    Passage *passage = &dungeon->passages[index];
                    if(passage->type && equal_v2u(passage->pos, examine->pos))
                    {
                        unset(game->examine.flags, ExamineFlags_Open);
                        start_entity_pathfind(player, dungeon, items, examine->pos);
                        
                        return;
                    }
                }
                
                if(is_tile_traversable_and_has_been_seen(dungeon->tiles, examine->pos))
                {
                    unset(game->examine.flags, ExamineFlags_Open);
                    start_entity_pathfind(player, dungeon, items, examine->pos);
                }
            }
            else if(was_pressed(&input->GameKey_Yes))
            {
#if !MOONBREATH_SLOW
                if(has_tile_been_seen(dungeon->tiles, examine->pos))
#endif
                {
                    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                    {
                        Item *item = &items[index];
                        if(is_item_valid_and_not_in_inventory(item) &&
                           equal_v2u(examine->pos, item->pos))
                        {
                            examine->type = ExamineType_Item;
                            examine->item = item;
                            return;
                        }
                    }
                    
                    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
                    {
                        Entity *entity = &entities[index];
                        if(is_entity_valid_and_not_player(entity->type) &&
                           equal_v2u(examine->pos, entity->pos))
                        {
                            examine->type = ExamineType_Entity;
                            examine->entity = entity;
                            return;
                        }
                    }
                    
                    examine->type = ExamineType_Tile;
                    examine->tile_id = get_pos_tile_id(dungeon->tiles, examine->pos);
                }
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
update_camera(Game *game, Dungeon *dungeon, Entity *player)
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
    if(state->ended_down != is_down)
    {
        state->repeat = false;
        state->ended_down = is_down;
        
        if(!state->ended_down)
        {
            state->has_been_up = true;
        }
    }
}

internal void
update_events(Game *game, Input *input)
{
    u32 mouse_state = SDL_GetMouseState(&input->mouse_pos.x, &input->mouse_pos.y);
    update_input(&input->Button_Left, mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
    update_input(&input->Button_Middle, mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
    update_input(&input->Button_Right, mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
    update_input(&input->Button_Extended1, mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
    update_input(&input->Button_Extended2, mouse_state & SDL_BUTTON(SDL_BUTTON_X2));
    
    input->Button_ScrollUp.ended_down = false;
    input->Button_ScrollUp.has_been_up = true;
    
    input->Button_ScrollDown.ended_down = false;
    input->Button_ScrollDown.has_been_up = true;
    
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
                input->Button_ScrollUp.ended_down = true;
            }
            else if(event.wheel.y < 0)
            {
                input->Button_ScrollDown.ended_down = true;
            }
        }
        else if(event.type == SDL_KEYUP ||
                event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key = event.key.keysym.sym;
            b32 is_down = (event.key.state == SDL_PRESSED);
            
            if(event.key.repeat)
            {
                switch(key)
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
                    case SDLK_BACKSPACE: input->Key_Backspace.repeat = true; break;
                    
                    case SDLK_LEFT: input->Key_ArrowLeft.repeat = true; break;
                    case SDLK_RIGHT: input->Key_ArrowRight.repeat = true; break;
                }
            }
            else
            {
                for(u32 index = 0; index < GameKey_Count; ++index)
                {
                    if(key == game->keybinds[index])
                    {
                        update_input(&input->game_keys[index], is_down);
                        break;
                    }
                }
                
                switch(key)
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
                    
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT: update_input(&input->Key_Shift, is_down); break;
                    
                    case SDLK_LCTRL:
                    case SDLK_RCTRL: update_input(&input->Key_Control, is_down); break;
                    
                    case SDLK_LALT:
                    case SDLK_RALT: update_input(&input->Key_Alt, is_down); break;
                    
                    case SDLK_PAGEUP: update_input(&input->Key_PageUp, is_down); break;
                    case SDLK_PAGEDOWN: update_input(&input->Key_PageDown, is_down); break;
                    
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

internal void
update_and_render_game(Game *game,
                       Input *input,
                       Dungeon *dungeon,
                       Entity *player,
                       Entity *entities,
                       UI *ui,
                       Item *items,
                       Inventory *inventory,
                       Assets *assets,
                       ItemInfo *item_info,
                       u32 *entity_levels)
{
    if(game->mode == GameMode_MainMenu)
    {
        v4u rect = {50, 300, 200, 100};
        render_fill_rect(game, rect, Color_Cyan);
        
        if(is_inside_rect(rect, input->mouse_pos))
        {
            render_text(game, "%sNew Game", 100, 340, assets->fonts[FontName_DosVga], 0, start_color(Color_Yellow));
            
            if(was_pressed(&input->Button_Left))
            {
                game->mode = GameMode_Playing;
            }
        }
        else
        {
            render_text(game, "New Game", 100, 340, assets->fonts[FontName_DosVga], 0);
        }
    }
    else if(game->mode == GameMode_Playing)
    {
        // TODO(rami): Set game is_initialized to false when going back to main menu.
        if(!game->is_initialized)
        {
            b32 is_potion_adjective_taken[16] = {0};
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
            
            b32 is_potion_color_set[Potion_Count] = {0};
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                ConsumableInfo *potion_info = &item_info->potion[index];
                
                while(is_zero_v2u(potion_info->tile))
                {
                    u32 potion_index = random_number(&game->random, 0, Potion_Count - 1);
                    if(!is_potion_color_set[potion_index])
                    {
                        is_potion_color_set[potion_index] = true;
                        
                        // Add a random not already taken adjective to the potion depiction.
                        for(;;)
                        {
                            u32 adjective_index = random_number(&game->random, 0, 15);
                            if(!is_potion_adjective_taken[adjective_index])
                            {
                                is_potion_adjective_taken[adjective_index] = true;
                                strcpy(potion_info->depiction, potion_adjectives[adjective_index]);
                                
                                break;
                            }
                        }
                        
                        switch(potion_index)
                        {
                            case 0:
                            {
                                potion_info->tile = make_v2u(10, 2);
                                strcat(potion_info->depiction, "Red ");
                            } break;
                            
                            case 1:
                            {
                                potion_info->tile = make_v2u(10, 3);
                                strcat(potion_info->depiction, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                potion_info->tile = make_v2u(10, 4);
                                strcat(potion_info->depiction, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                potion_info->tile = make_v2u(10, 5);
                                strcat(potion_info->depiction, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                potion_info->tile = make_v2u(10, 6);
                                strcat(potion_info->depiction, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                potion_info->tile = make_v2u(10, 7);
                                strcat(potion_info->depiction, "Purple ");
                            } break;
                            
                            case 6:
                            {
                                potion_info->tile = make_v2u(10, 8);
                                strcat(potion_info->depiction, "Green ");
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        break;
                    }
                }
            }
            
            b32 is_scroll_color_set[Scroll_Count] = {0};
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                ConsumableInfo *scroll_info = &item_info->scroll[index];
                
                while(is_zero_v2u(scroll_info->tile))
                {
                    u32 scroll_index = random_number(&game->random, 0, Scroll_Count - 1);
                    if(!is_scroll_color_set[scroll_index])
                    {
                        is_scroll_color_set[scroll_index] = true;
                        
                        switch(scroll_index)
                        {
                            case 0:
                            {
                                scroll_info->tile = make_v2u(11, 2);
                                strcpy(scroll_info->depiction, "Red ");
                            } break;
                            
                            case 1:
                            {
                                scroll_info->tile = make_v2u(11, 3);
                                strcpy(scroll_info->depiction, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                scroll_info->tile = make_v2u(11, 4);
                                strcpy(scroll_info->depiction, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                scroll_info->tile = make_v2u(11, 5);
                                strcpy(scroll_info->depiction, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                scroll_info->tile = make_v2u(11, 6);
                                strcpy(scroll_info->depiction, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                scroll_info->tile = make_v2u(11, 7);
                                strcpy(scroll_info->depiction, "Purple ");
                            } break;
                            
                            invalid_default_case;
                        }
                        
                        break;
                    }
                }
            }
            
            item_info->potion_healing_range = make_v2u(20, 40);
            item_info->ration_healing_range = make_v2u(10, 20);
            
#if 0
            // Print randomized potion and scroll tiles.
            printf("\nRandomized Potion Tiles\n");
            for(u32 index = 0; index < Potion_Count; ++index)
            {
                Info *potion_info = &item_info->potion[index];
                
                printf("[%u]: %u, %u\n", index,
                       potion_info->tile.x,
                       potion_info->tile.y);
            }
            
            printf("\nRandomized Scroll Tiles\n");
            for(u32 index = 0; index < Scroll_Count; ++index)
            {
                Info *scroll_info = &item_info->scroll[index];
                
                printf("[%u]: %u, %u\n", index,
                       scroll_info->tile.x,
                       scroll_info->tile.y);
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
            
            add_player_entity(&game->random, player);
            create_dungeon(game, dungeon, player, ui, entities, items, inventory, item_info, entity_levels);
            update_fov(dungeon, player);
            
            ui->font = assets->fonts[FontName_DosVga];
            ui->font_newline = get_font_newline(ui->font->size);
            
            log_add(ui, "%sWelcome, %s!", start_color(Color_Yellow), player->name);
            log_add(ui, "%sFind and destroy the underworld portal, ", start_color(Color_Yellow));
            log_add(ui, "%swhich is located somewhere in the depths.", start_color(Color_Yellow));
            
            game->is_initialized = true;
        }
        
        update_examine_mode(game, dungeon, player, entities, items, input);
        update_entities(game, input, player, entities, dungeon, items, item_info, ui, assets, inventory, entity_levels);
        update_camera(game, dungeon, player);
        
        render_tilemap(game, dungeon, assets);
        render_items(game, player, dungeon, items, assets);
        render_entities(game, dungeon, entities, inventory, assets);
        render_ui(game, input, dungeon, player, items, inventory, item_info, assets, ui);
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

int main(int argc, char *argv[])
{
    u32 result = 0;
    
    // All game data is required to be initialized to zero
    Game game = {0};
    Assets assets = {0};
    Entity *entities = calloc(1, MAX_ENTITY_COUNT * sizeof(Entity));
    Entity *player = &entities[0];
    u32 entity_levels[EntityID_Count] = {0};
    Dungeon dungeon = {0};
    dungeon.tiles.array = calloc(1, (MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE) * sizeof(Tile));
    Inventory inventory = {0};
    inventory.entry_size = 32;
    Item items[MAX_ITEM_COUNT] = {0};
    ItemInfo item_info = {0};
    UI ui = {0};
    ui.window_offset = 12;
    ui.short_log_view.end = 9;
    ui.mark.view.end = 24;
    
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
        game.show_item_ground_outline = show_item_ground_outline.boolean;
    }
    
    ConfigValue window_size = get_config_uint(&config, "window_size");
    if(window_size.uint)
    {
        if(window_size.uint == 1)
        {
            game.window_size = make_v2u(1920, 1080);
        }
        else if(window_size.uint == 2)
        {
            game.window_size = make_v2u(1280, 720);
        }
    }
    else
    {
        assert(0);
    }
    
#if 1
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
    
    game.keybinds[GameKey_Up] = 'w';
    game.keybinds[GameKey_Down] = 's';
    game.keybinds[GameKey_Left] = 'a';
    game.keybinds[GameKey_Right] = 'd';
    
    game.keybinds[GameKey_UpLeft] = 'q';
    game.keybinds[GameKey_UpRight] = 'e';
    game.keybinds[GameKey_DownLeft] = 'z';
    game.keybinds[GameKey_DownRight] = 'c';
    
    game.keybinds[GameKey_OpenInventory] = 'i';
    game.keybinds[GameKey_Pickup] = ',';
    game.keybinds[GameKey_AscendDescend] = 'u';
    game.keybinds[GameKey_AutoExplore] = 'p';
    game.keybinds[GameKey_IteratePassages] = '<';
    game.keybinds[GameKey_Examine] = 'o';
    game.keybinds[GameKey_Log] = 'l';
    
    game.keybinds[GameKey_Back] = '-';
    game.keybinds[GameKey_Wait] = 'v';
    game.keybinds[GameKey_Yes] = 'h';
    game.keybinds[GameKey_No] = 'j';
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
                            u64 seed = 1602811425;
#endif
                            printf("Seed: %lu\n", seed);
                            
                            game.random = set_random_seed(seed);
                            
#if 1
                            game.mode = GameMode_Playing;
#else
                            game.mode = GameMode_MainMenu;
#endif
                            
                            game.camera = make_v4s(0, 0,
                                                   game.window_size.w,
                                                   game.window_size.h - assets.stat_and_log_window_h);
                            
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
                            
                            for(u32 index = 0; index < Key_Count; ++index)
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
                            
                            add_debug_bool32(debug_vars, "FOV Toggle", &fkey_active[1]);
                            add_debug_bool32(debug_vars, "Traversable Toggle", &fkey_active[2]);
                            add_debug_bool32(debug_vars, "Has Been Up Toggle", &fkey_active[3]);
                            add_debug_bool32(debug_vars, "Hit Test Toggle", &fkey_active[4]);
                            
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
                                    new_input->mouse[index] = old_input->mouse[index];
                                }
                                
                                for(u32 index = 0; index < Key_Count; ++index)
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
                                
                                update_events(&game, new_input);
                                
                                f32 end_dt = (f32)SDL_GetPerformanceCounter();
                                new_input->frame_dt = ((end_dt - last_dt) / (f32)performance_frequency);
                                last_dt = end_dt;
                                
                                update_and_render_game(&game,
                                                       new_input,
                                                       &dungeon,
                                                       player,
                                                       entities,
                                                       &ui,
                                                       items,
                                                       &inventory,
                                                       &assets,
                                                       &item_info,
                                                       entity_levels);
                                
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
                                    v4u dest = get_game_dest(&game, new_input->mouse_tile_pos);
                                    SDL_RenderCopy(game.renderer, assets.ui.tex, (SDL_Rect *)&assets.yellow_outline_src, (SDL_Rect *)&dest);
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
                                    //printf("Missed frame rate: %.01f\n", work_ms_per_frame);
                                }
                                
                                u64 end_counter = SDL_GetPerformanceCounter();
                                u64 elapsed_counter = end_counter - last_counter;
                                last_counter = end_counter;
                                
#if MOONBREATH_SLOW
                                fps = (f32)performance_frequency / (f32)elapsed_counter;
                                full_ms_per_frame = get_ms_from_elapsed(elapsed_counter, performance_frequency);
                                
                                update_and_render_debug_state(&game, &debug_state, new_input);
#endif
                                
                                Input *temp = new_input;
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