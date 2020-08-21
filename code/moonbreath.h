typedef struct
{
    b32 is_initialized;
    
    RandomState random;
    GameMode mode;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    u32 keybinds[Key_Count];
    b32 show_item_ground_outline;
} GameState;

internal v4u tile_rect(v2u tile);
internal v4u game_dest(GameState *game, v2u pos);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal Direction get_random_direction(RandomState *random);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal b32 was_pressed(InputState *state, b32 *fkey_active);