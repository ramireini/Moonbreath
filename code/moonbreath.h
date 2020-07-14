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
    b32 show_ground_item_outline;
} GameState;

internal Direction get_random_direction(RandomState *random);
internal v4u tile_rect(v2u tile);
internal v4u game_dest(GameState *game, v2u pos);
internal b32 was_pressed(InputState *state);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest);