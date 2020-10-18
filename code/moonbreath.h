typedef struct
{
    b32 is_enabled;
    b32 is_key_held[Key_Count];
    u32 key_hold_start[Key_Count];
    v2u pos;
} ExamineMode;

typedef struct
{
    b32 is_initialized;
    b32 show_item_ground_outline;
    
    Random random;
    GameMode mode;
    ExamineMode examine;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    union
    {
        u32 keybinds[Key_Count];
        struct
        {
            u32 Key_Up;
            u32 Key_Down;
            u32 Key_Left;
            u32 Key_Right;
            
            u32 Key_UpLeft;
            u32 Key_UpRight;
            u32 Key_DownLeft;
            u32 Key_DownRight;
            
            u32 Key_InventoryOpen;
            u32 Key_InventoryAction;
            u32 Key_InventoryMove;
            
            u32 Key_PickupDrop;
            u32 Key_AscendDescend;
            u32 Key_AutoExplore;
            u32 Key_CyclePassages;
            u32 Key_Examine;
            u32 Key_Log;
            
            u32 Key_Wait;
            u32 Key_Yes;
            u32 Key_No;
        };
    };
} GameState;

internal v4u get_tile_rect(v2u tile);
internal v4u get_game_dest(GameState *game, v2u pos);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal Direction get_random_direction(Random *random);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal b32 was_pressed(InputState *state);