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

internal void
update_examine_mode(Examine *examine,
                    Input *input,
                    Entity *player,
                    EntityState *entities,
                    ItemState *items,
                    Inventory *inventory,
                    Dungeon *dungeon,
                    UI *ui)
{
    DungeonPassages *passages = &dungeon->passages;
    
    if(is_set(examine->flags, ExamineFlag_Open))
    {
        if(examine->type == ExamineType_Entity)
        {
            char pressed = get_pressed_alphabet_char(input);
            if(pressed)
            {
                if(is_set(examine->flags, ExamineFlag_ReadyForKeypress))
                {
                    Spell *spell = &examine->entity->e.spells[(pressed - 'a')];
                    if(spell->id)
                    {
                        examine->type = ExamineType_Spell;
                        examine->spell = spell;
                    }
                }
                else
                {
                    set(examine->flags, ExamineFlag_ReadyForKeypress);
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
                            u32 hold_duration = get_sdl_ticks_difference(examine->key_pressed_start[index]);
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
                for(u32 index = 0; index < passages->count; ++index)
                {
                    Passage *passage = &passages->array[index];
                    if(passage->type)
                    {
                        printf("Passage[%u]: %s\n", index, passage->type == PassageType_Up ? "Up" : "Down");
                        printf("Pos: %u, %u\n\n", passage->pos.x, passage->pos.y);
                    }
                }
#endif
                
                // Set which passage type we want to find
                DungeonPassageType passage_search_type = DungeonPassageType_None;
                
                if(input->Key_Shift.is_down)
                {
                    passage_search_type = DungeonPassageType_Up;
                }
                else
                {
                    passage_search_type = DungeonPassageType_Down;
                }
                
                //printf("\nExamine Mode: Searching for a passage going %s.\n", passage_search_type == PassageType_Up ? "Up" : "Down");

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
                                //printf("Examine Mode: Selected %s passage[%u].\n", passage_search_type == PassageType_Up ? "Up" : "Down", passage_index);
                                
                                examine->selected_passage = passage_index;
                                examine->pos = passage->pos;
                                
                                    break;
                            }
                        }
                    }
                }
            }
            else if(can_player_pathfind(examine, input, inventory, ui))
            {
                    // Pathfind to passage
                    for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
                    {
                        DungeonPassage *passage = &passages->array[index];
                        if(passage->type && is_v2u_equal(passage->pos, examine->pos))
                        {
                            unset(examine->flags, ExamineFlag_Open);
                            make_entity_pathfind(player, items, dungeon, &entities->player_pathfind_map, examine->pos);
                            return;
                        }
                    }
                    
                    // Pathfind to tile
                    if(is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, examine->pos) &&
                       has_tile_been_seen(dungeon->tiles, examine->pos))
                    {
                        unset(examine->flags, ExamineFlag_Open);
                        make_entity_pathfind(player, items, dungeon, &entities->player_pathfind_map, examine->pos);
                        
                        return;
                    }
            }
            else if(was_pressed(&input->GameKey_Yes) && !examine->type)
            {
                if(get_dungeon_pos_examine_source_count(dungeon, entities, items, examine->pos) > 1)
                {
                    // Examine multiple
                    unset(examine->flags, ExamineFlag_Open);
                    set(inventory->flags, InventoryFlag_MultipleExamine);
                    set_view_at_start(&inventory->examine_view);
                    
                    return;
                }
                else
                {
                    unset(examine->flags, ExamineFlag_CameraFollow);
                    
                    Item *item = get_dungeon_pos_item(items, dungeon->level, examine->pos, 0);
                    if(item)
                    {
                        examine->item = item;
                        examine->type = ExamineType_Item;
                        return;
                    }
                    
                    Entity *entity = get_dungeon_pos_entity(entities, dungeon->level, examine->pos, true);
                    if(entity)
                    {
                        examine->type = ExamineType_Entity;
                        examine->entity = entity;
                        return;
                    }
                    
                    DungeonTrap *trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, examine->pos);
                    if(trap)
                    {
                            examine->type = ExamineType_Trap;
                            examine->trap = trap;
                            return;
                        }
                    
                    examine->type = ExamineType_Tile;
                    examine->tile = get_dungeon_pos_tile(dungeon->tiles, examine->pos);
                    return;
                    }
                }
            }
        }
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
set_render_color(Game *game, Color color)
{
    v4u value = get_color_value(color);
    SDL_SetRenderDrawColor(game->renderer,
                           value.r,
                           value.g,
                           value.b,
                           value.a);
}

internal u32
tile_div(u32 value)
{
    u32 result = value / 32;
    return(result);
}

internal u32
tile_mul(u32 value)
{
    u32 result = value * 32;
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
get_direction_moved_from(v2u old_pos, v2u new_pos)
{
    Direction result = Direction_None;
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        if(is_v2u_equal(old_pos, get_direction_pos(new_pos, direction)))
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
    Direction result = get_random_number(random, Direction_Up, Direction_DownRight);
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
render_dungeon(Game *game, u32 player_flags, Dungeon *dungeon, Assets *assets)
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
    if(tile_mul(dungeon->size.w) < game->camera.w)
    {
        render_area.w = dungeon->size.w - 1;
    }
    
    if(tile_mul(dungeon->size.h) < game->camera.h)
    {
        render_area.h = dungeon->size.h - 1;
    }
    
    //print_v4u(render_area);
    
    for(u32 y = render_area.y; y < render_area.h; ++y)
    {
        for(u32 x = render_area.x; x < render_area.w; ++x)
        {
            v2u tile_pos = {x, y};
            
            assert(get_dungeon_pos_tile(dungeon->tiles, tile_pos));
            
            v2u tileset_pos = get_dungeon_tileset_pos(dungeon->tiles, tile_pos);
            v4u tile_src = get_dungeon_tile_rect(tileset_pos);
            v4u tile_dest = get_dungeon_tile_rect(tile_pos);
            
            DungeonTrap *trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, tile_pos);
            
            b32 has_remains = false;
                v4u remains_src = {0};
                
                v2u remains_tile_pos = {0};
            DungeonTileID remains = get_dungeon_pos_remains(dungeon->tiles, tile_pos);
                
                if(remains)
                {
                    has_remains = true;
                    
                    switch(remains)
                    {
                        case DungeonTileID_RedBloodGroundSmall1: remains_tile_pos = make_v2u(33, 0); break;
                        case DungeonTileID_RedBloodGroundSmall2: remains_tile_pos = make_v2u(34, 0); break;
                        case DungeonTileID_RedBloodGroundSmall3: remains_tile_pos = make_v2u(35, 0); break;
                        
                        case DungeonTileID_RedBloodGroundMedium1: remains_tile_pos = make_v2u(36, 0); break;
                        case DungeonTileID_RedBloodGroundMedium2: remains_tile_pos = make_v2u(37, 0); break;
                        
                        case DungeonTileID_RedBloodGroundLarge1: remains_tile_pos = make_v2u(38, 0); break;
                        case DungeonTileID_RedBloodGroundLarge2: remains_tile_pos = make_v2u(39, 0); break;
                        
                        case DungeonTileID_RedBloodWallUp1: remains_tile_pos = make_v2u(40, 0); break;
                        case DungeonTileID_RedBloodWallUp2: remains_tile_pos = make_v2u(41, 0); break;
                        case DungeonTileID_RedBloodWallUp3: remains_tile_pos = make_v2u(42, 0); break;
                        
                        case DungeonTileID_RedBloodWallDown1: remains_tile_pos = make_v2u(43, 0); break;
                        case DungeonTileID_RedBloodWallDown2: remains_tile_pos = make_v2u(44, 0); break;
                        case DungeonTileID_RedBloodWallDown3: remains_tile_pos = make_v2u(45, 0); break;
                        
                        case DungeonTileID_RedBloodWallLeft1: remains_tile_pos = make_v2u(46, 0); break;
                        case DungeonTileID_RedBloodWallLeft2: remains_tile_pos = make_v2u(47, 0); break;
                        case DungeonTileID_RedBloodWallLeft3: remains_tile_pos = make_v2u(48, 0); break;
                        
                        case DungeonTileID_RedBloodWallRight1: remains_tile_pos = make_v2u(49, 0); break;
                        case DungeonTileID_RedBloodWallRight2: remains_tile_pos = make_v2u(50, 0); break;
                        case DungeonTileID_RedBloodWallRight3: remains_tile_pos = make_v2u(51, 0); break;
                        
                        //
                        
                        case DungeonTileID_GreenBloodGroundSmall1: remains_tile_pos = make_v2u(33, 1); break;
                        case DungeonTileID_GreenBloodGroundSmall2: remains_tile_pos = make_v2u(34, 1); break;
                        case DungeonTileID_GreenBloodGroundSmall3: remains_tile_pos = make_v2u(35, 1); break;
                        
                        case DungeonTileID_GreenBloodGroundMedium1: remains_tile_pos = make_v2u(36, 1); break;
                        case DungeonTileID_GreenBloodGroundMedium2: remains_tile_pos = make_v2u(37, 1); break;
                        
                        case DungeonTileID_GreenBloodGroundLarge1: remains_tile_pos = make_v2u(38, 1); break;
                        case DungeonTileID_GreenBloodGroundLarge2: remains_tile_pos = make_v2u(39, 1); break;
                        
                        case DungeonTileID_GreenBloodWallUp1: remains_tile_pos = make_v2u(40, 1); break;
                        case DungeonTileID_GreenBloodWallUp2: remains_tile_pos = make_v2u(41, 1); break;
                        case DungeonTileID_GreenBloodWallUp3: remains_tile_pos = make_v2u(42, 1); break;
                        
                        case DungeonTileID_GreenBloodWallDown1: remains_tile_pos = make_v2u(43, 1); break;
                        case DungeonTileID_GreenBloodWallDown2: remains_tile_pos = make_v2u(44, 1); break;
                        case DungeonTileID_GreenBloodWallDown3: remains_tile_pos = make_v2u(45, 1); break;
                        
                        case DungeonTileID_GreenBloodWallLeft1: remains_tile_pos = make_v2u(46, 1); break;
                        case DungeonTileID_GreenBloodWallLeft2: remains_tile_pos = make_v2u(47, 1); break;
                        case DungeonTileID_GreenBloodWallLeft3: remains_tile_pos = make_v2u(48, 1); break;
                        
                        case DungeonTileID_GreenBloodWallRight1: remains_tile_pos = make_v2u(49, 1); break;
                        case DungeonTileID_GreenBloodWallRight2: remains_tile_pos = make_v2u(59, 1); break;
                        case DungeonTileID_GreenBloodWallRight3: remains_tile_pos = make_v2u(51, 1); break;
                        
                        invalid_default_case;
                    }
                    
                remains_src = get_dungeon_tile_rect(remains_tile_pos);
                }
            
            if(is_tile_seen(dungeon->tiles, tile_pos))
            {
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&tile_src, (SDL_Rect *)&tile_dest);
                if(has_remains)
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&remains_src, (SDL_Rect *)&tile_dest);
                }
                else if(trap)
                {
                    set_flag_if_player_is_not_pathfinding(player_flags, &trap->flags, DungeonTrapFlag_HasBeenSeen);
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&trap->tile_src, (SDL_Rect *)&tile_dest);
                }
            }
            else if(has_tile_been_seen(dungeon->tiles, tile_pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, tile_src, tile_dest, false);
                if(has_remains)
                {
                    render_texture_half_color(game->renderer, assets->tileset.tex, remains_src, tile_dest, false);
                }
                else if(trap)
                {
                    render_texture_half_color(game->renderer, assets->tileset.tex, trap->tile_src, tile_dest, false);
                }
            }
        }
    }
    
    SDL_SetRenderTarget(game->renderer, 0);
    
    v4u src = {game->camera.x, game->camera.y, game->camera.w, game->camera.h};
    v4u dest = {0, 0, game->camera.w, game->camera.h};
    SDL_RenderCopy(game->renderer, assets->tilemap.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
}

internal u32
get_sdl_ticks_difference(u32 start_time)
{
    u32 result = (SDL_GetTicks() - start_time);
    return(result);
}

internal b32
is_zero(u32 value)
{
    b32 result = ((s32)value <= 0);
    return(result);
}

internal b32
is_value_in_range(s32 value, s32 start, s32 end)
{
    b32 result = ((value > start) && (value < end));
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
}

internal void
update_camera(Game *game, v2u player_pos, v2u dungeon_size)
{
    Examine *examine = &game->examine;
    
    v2u camera_follow_pos = {0};
    
    if(is_set(examine->flags, ExamineFlag_Open) &&
           is_set(examine->flags, ExamineFlag_CameraFollow))
    {
        camera_follow_pos = examine->pos;
    }
    else
    {
        camera_follow_pos = player_pos;
    }
    
    game->camera.x = tile_mul(camera_follow_pos.x) - (game->camera.w / 2);
    
    if(is_window_size(game->window_size, 1920, 1080))
    {
        game->camera.y = tile_mul(camera_follow_pos.y) - (game->camera.h / 2) + 4;
    }
    else if(is_window_size(game->window_size, 1280, 720))
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
    
    // if statement is so dungeons smaller than the camera work properly
    if(tile_mul(dungeon_size.w) >= game->camera.w)
    {
        s32 camera_max_x = (s32)(tile_mul(dungeon_size.w) - game->camera.w);
        s32 camera_max_y = (s32)(tile_mul(dungeon_size.h) - game->camera.h);
        
        // Clamp the camera if we get close enough to the map right/bottom edge
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
render_rect(Game *game, v4u rect, u32 border_size)
{
    // Window border
    render_fill_rect(game, rect, Color_WindowBorder);
    
    // Window background
    v4u background_rect = get_border_adjusted_rect(rect, border_size);
    render_fill_rect(game, background_rect, Color_WindowBackground);
    
    // Window accent
    render_outline_rect(game, background_rect, Color_WindowAccent);
}

internal void
render_outline_rect(Game *game, v4u rect, Color color)
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

internal String8
get_printable_key(Input *input, Key key)
{
    String8 result = {0};
    
    switch(key)
    {
        case Key_A: result.s[0] = get_char('a', input->Key_Shift.is_down); break;
        case Key_B: result.s[0] = get_char('b', input->Key_Shift.is_down); break;
        case Key_C: result.s[0] = get_char('c', input->Key_Shift.is_down); break;
        case Key_D: result.s[0] = get_char('d', input->Key_Shift.is_down); break;
        case Key_E: result.s[0] = get_char('e', input->Key_Shift.is_down); break;
        case Key_F: result.s[0] = get_char('f', input->Key_Shift.is_down); break;
        case Key_G: result.s[0] = get_char('g', input->Key_Shift.is_down); break;
        case Key_H: result.s[0] = get_char('h', input->Key_Shift.is_down); break;
        case Key_I: result.s[0] = get_char('i', input->Key_Shift.is_down); break;
        case Key_J: result.s[0] = get_char('j', input->Key_Shift.is_down); break;
        case Key_K: result.s[0] = get_char('k', input->Key_Shift.is_down); break;
        case Key_L: result.s[0] = get_char('l', input->Key_Shift.is_down); break;
        case Key_M: result.s[0] = get_char('m', input->Key_Shift.is_down); break;
        case Key_N: result.s[0] = get_char('n', input->Key_Shift.is_down); break;
        case Key_O: result.s[0] = get_char('o', input->Key_Shift.is_down); break;
        case Key_P: result.s[0] = get_char('p', input->Key_Shift.is_down); break;
        case Key_Q: result.s[0] = get_char('q', input->Key_Shift.is_down); break;
        case Key_R: result.s[0] = get_char('r', input->Key_Shift.is_down); break;
        case Key_S: result.s[0] = get_char('s', input->Key_Shift.is_down); break;
        case Key_T: result.s[0] = get_char('t', input->Key_Shift.is_down); break;
        case Key_U: result.s[0] = get_char('u', input->Key_Shift.is_down); break;
        case Key_V: result.s[0] = get_char('v', input->Key_Shift.is_down); break;
        case Key_W: result.s[0] = get_char('w', input->Key_Shift.is_down); break;
        case Key_X: result.s[0] = get_char('x', input->Key_Shift.is_down); break;
        case Key_Y: result.s[0] = get_char('y', input->Key_Shift.is_down); break;
        case Key_Z: result.s[0] = get_char('z', input->Key_Shift.is_down); break;
        
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
get_pressed_keyboard_char(Input *input)
{
    char result = 0;
    
    for(Key key = Key_A; key < Key_Shift; ++key)
    {
        if(was_pressed(&input->keyboard[key]))
        {
            result = get_printable_key(input, key).s[0];
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
            result = get_printable_key(input, key).s[0];
            break;
        }
    }
    
    return(result);
}

internal void
update_and_render_game(Game *game,
                       Input *input,
                       Entity *player,
                       EntityState *entities,
                       Dungeons *dungeons,
                       ItemState *items,
                       Inventory *inventory,
                       Assets *assets,
                       UI *ui)
{
    if(game->mode == GameMode_MainMenu)
    {
        v4u rect = {50, 300, 200, 100};
        render_fill_rect(game, rect, Color_Cyan);
        
        if(is_pos_inside_rect(rect, input->mouse_pos))
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
                ConsumableInfo *info = &items->potion_info[index];
                
                while(is_v4u_zero(info->tile_src))
                {
                    u32 potion_index = get_random_number(&game->random, 0, Potion_Count - 1);
                    if(!potion_color_set[potion_index])
                    {
                        potion_color_set[potion_index] = true;
                        
                        // Add a random not already taken adjective to the potion depiction
                        for(;;)
                        {
                            u32 adjective_index = get_random_number(&game->random, 0, 15);
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
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 2));
                                strcat(info->depiction.s, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 3));
                                strcat(info->depiction.s, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 4));
                                strcat(info->depiction.s, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 5));
                                strcat(info->depiction.s, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 6));
                                strcat(info->depiction.s, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 7));
                                strcat(info->depiction.s, "Purple ");
                            } break;
                            
                            case 6:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(10, 8));
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
                ConsumableInfo *info = &items->scroll_info[index];
                
                while(is_v4u_zero(info->tile_src))
                {
                    u32 scroll_index = get_random_number(&game->random, 0, Scroll_Count - 1);
                    if(!scroll_color_set[scroll_index])
                    {
                        scroll_color_set[scroll_index] = true;
                        
                        switch(scroll_index)
                        {
                            case 0:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 2));
                                strcpy(info->depiction.s, "Red ");
                            } break;
                            
                            case 1:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 3));
                                strcpy(info->depiction.s, "Blue ");
                            } break;
                            
                            case 2:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 4));
                                strcpy(info->depiction.s, "Cyan ");
                            } break;
                            
                            case 3:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 5));
                                strcpy(info->depiction.s, "Yellow ");
                            } break;
                            
                            case 4:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 6));
                                strcpy(info->depiction.s, "Brown ");
                            } break;
                            
                            case 5:
                            {
                                info->tile_src = get_dungeon_tile_rect(make_v2u(11, 7));
                                strcpy(info->depiction.s, "Purple ");
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
            
            add_player_entity(player);
            
            // Create all dungeon levels
            //for(u32 dungeon_level = 1; dungeon_level <= MAX_DUNGEON_LEVELS; ++dungeon_level)
                for(u32 dungeon_level = 1; dungeon_level <= 1; ++dungeon_level)
            {
                Dungeon *created_dungeon = create_dungeon(game, player, entities, dungeons, items, inventory, ui, dungeon_level);
                
                if(dungeon_level == 1)
                {
                    dungeons->current_level = dungeon_level;
                    
                    // Place Player
                    DungeonPassage *passage = get_dungeon_passage_from_type(&created_dungeon->passages, DungeonPassageType_Up);
                    if(passage)
                    {
                        move_entity(&game->random, player, created_dungeon->tiles, ui, passage->pos);
                    }
                    
                    player->dungeon_level = dungeon_level;
                    add_player_starting_item(game, player, items, inventory, ui, dungeon_level, ItemID_Sword, player->pos.x, player->pos.y);
                    add_player_starting_item(game, player, items, inventory, ui, dungeon_level, ItemID_MightPotion, player->pos.x, player->pos.y);
                    
                    update_fov(player, created_dungeon);
                }
            }
            
            ui->font = &assets->fonts[FontName_DosVga];
            ui->font_newline = get_font_newline(ui->font->size);
            
            log_add(ui, "%sWelcome, %s!", start_color(Color_Yellow), player->name);
            log_add(ui, "%sFind and destroy the underworld portal, ", start_color(Color_Yellow));
            log_add(ui, "%swhich is located somewhere in the depths.", start_color(Color_Yellow));
            
            game->is_set = true;
        }
        
        Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
        
        #if 0
        print_dungeon_items(items, dungeon);
        print_all_dungeon_level_occupancies(dungeons);
        #endif
        
        #if 0
        printf("Used Game Memory: %lu/%lu\n", game->memory_arena.used, game->memory_arena.size);
        printf("Used Debug Memory: %lu/%lu\n\n", game->debug.memory_arena.used, game->debug.memory_arena.size);
        #endif
        
        update_examine_mode(&game->examine, input, player, entities, items, inventory, dungeon, ui);
        update_entities(game, input, entities, items, inventory, dungeons, assets, ui);
        update_camera(game, player->pos, dungeon->size);
        
        render_dungeon(game, player->flags, dungeon, assets);
        render_items(game, player, items, dungeon, assets);
        render_entities(game, entities, inventory, dungeon, assets);
        render_ui(game, input, entities, items, inventory, dungeon, assets, ui);
        
#if MOONBREATH_SLOW
        // Render cursor rectangle
        if(other_windows_are_closed(&game->examine, inventory, ui))
        {
        // Render cursor rect on mouse tile
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

int main(int argc, char *argv[])
{
    u32 result = 0;
    
    GameMemory memory = {0};
    memory.size = megabytes(32);
    memory.storage = calloc(1, memory.size);
    
    if(memory.size && memory.storage)
    {
        Game *game = memory.storage;
        game->examine.key_hold_duration = 400;
        
        init_arena(&game->memory_arena,
                       memory.storage + sizeof(Game),
                       (megabytes(28)) - sizeof(Game));
        
        EntityState *entities = push_memory_struct(&game->memory_arena, EntityState);
        Entity *player = get_player_entity(entities);
        
        Dungeons *dungeons = push_memory_struct(&game->memory_arena, Dungeons);
        
        for(u32 index = 0; index < MAX_DUNGEON_LEVELS; ++index)
        {
            dungeons->levels[index].tiles.array = push_memory(&game->memory_arena, MAX_DUNGEON_SIZE_SQUARED * sizeof(DungeonTile));
        }
        
        ItemState *items = push_memory_struct(&game->memory_arena, ItemState);
        
        Inventory *inventory = push_memory_struct(&game->memory_arena, Inventory);
        inventory->entry_size = 32;
        
        Assets *assets = push_memory_struct(&game->memory_arena, Assets);
        
        UI *ui = push_memory_struct(&game->memory_arena, UI);
        ui->window_offset = 12;
        ui->short_log_view.end = 9;
        ui->mark.cursor_blink_duration = 800;
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
        
        // TODO(rami): Need to check success on everything
        Config config = get_config(&game->memory_arena, "data/config.txt");
        
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
                                printf("Seed: %lu\n\n", seed);
                                
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
                                
                                DebugState *debug = &game->debug;
                                
                                debug->font = &assets->fonts[FontName_DosVga];
                                debug->text_offset.x = get_font_newline(debug->font->size) * 2;
                                debug->text_offset.y = get_font_newline(debug->font->size);
                                
                                game->debug.memory_size = megabytes(1);
                                init_arena(&debug->memory_arena,
                                               memory.storage + memory.size - game->debug.memory_size,
                                               game->debug.memory_size);
                                
                                DebugTree *vars_tree = add_debug_tree(debug, 50, 25);
                                DebugTree *colors_tree = add_debug_tree(debug, 300, 25);
                                
                                start_debug_group(debug, vars_tree, "Variables", true);
                                {
                                    add_debug_variable(vars_tree, "Frame MS", full_ms_per_frame, DebugVariableType_F32);
                                    add_debug_variable(vars_tree, "Work MS", work_ms_per_frame, DebugVariableType_F32);
                                    add_debug_variable(vars_tree, "Frame DT", new_input->frame_dt, DebugVariableType_F32);
                                    add_debug_newline(debug, vars_tree);
                                    
                                    add_debug_variable(vars_tree, "Mouse", new_input->mouse_pos, DebugVariableType_V2U);
                                    add_debug_variable(vars_tree, "Mouse Tile", new_input->mouse_tile_pos, DebugVariableType_V2U);
                                    add_debug_variable(vars_tree, "Player Tile", player->pos, DebugVariableType_V2U);
                                    add_debug_newline(debug, vars_tree);
                                    
                                    add_debug_variable(vars_tree, "FOV Toggle", fkey_active[1], DebugVariableType_B32);
                                    add_debug_variable(vars_tree, "Traversable Toggle", fkey_active[2], DebugVariableType_B32);
                                    add_debug_variable(vars_tree, "Has Been Up Toggle", fkey_active[3], DebugVariableType_B32);
                                    add_debug_variable(vars_tree, "Hit Test Toggle", fkey_active[4], DebugVariableType_B32);
                                }
                                end_debug_group(vars_tree);
                                
                                start_debug_group(debug, colors_tree, "Colors", true);
                                {
                                    add_debug_text(colors_tree, "White", Color_White);
                                    add_debug_text(colors_tree, "Light Gray", Color_LightGray);
                                    add_debug_text(colors_tree, "Dark Gray", Color_DarkGray);
                                    add_debug_text(colors_tree, "Light Red", Color_LightRed);
                                    add_debug_text(colors_tree, "Dark Red", Color_DarkRed);
                                    add_debug_text(colors_tree, "Light Green", Color_LightGreen);
                                    add_debug_text(colors_tree, "Dark Green", Color_DarkGreen);
                                    add_debug_text(colors_tree, "Light Blue", Color_LightBlue);
                                    add_debug_text(colors_tree, "Dark Blue", Color_DarkBlue);
                                    add_debug_text(colors_tree, "Light Brown", Color_LightBrown);
                                    add_debug_text(colors_tree, "Dark Brown", Color_DarkBrown);
                                    add_debug_text(colors_tree, "Cyan", Color_Cyan);
                                    add_debug_text(colors_tree, "Yellow", Color_Yellow);
                                    add_debug_text(colors_tree, "Purple", Color_Purple);
                                    add_debug_text(colors_tree, "Orange", Color_Orange);
                                }
                                end_debug_group(colors_tree);
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
                                                               player,
                                                           entities,
                                                               dungeons,
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
                                    
                                    update_and_render_debug_state(game, new_input, entities, items, dungeons);
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