typedef struct
{
    b32 is_initialized;
    
    game_state state;
    random_state_t random;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    u32 keybinds[key_count];
} game_state_t;

internal v4u tile_rect(v2u tile);
internal v4u game_dest(game_state_t *game, v2u pos);
internal b32 is_input_valid(input_state_t *state);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest);