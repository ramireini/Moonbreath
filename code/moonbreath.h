typedef struct
{
    b32 is_open;
    b32 is_key_held[Key_Count];
    u32 key_hold_start[Key_Count];
    v2u pos;
    
    // For dungeon passages.
    b32 start_up_passages_from_first;
    u32 up_passage_index;
    
    b32 start_down_passages_from_first;
    u32 down_passage_index;
} Examine;

typedef struct
{
    b32 is_open;
    
    Item *item;
    Entity *entity;
    TileID tile_id;
} Inspect;

typedef struct
{
    b32 is_initialized;
    b32 show_item_ground_outline;
    
    Random random;
    GameMode mode;
    Examine examine;
    Inspect inspect;
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
            u32 Key_IteratePassages;
            u32 Key_Examine;
            u32 Key_Log;
            
            u32 Key_Wait;
            u32 Key_Yes;
            u32 Key_No;
        };
    };
} GameState;

internal b32 is_window_1920x1080(v2u window_size);
internal b32 is_window_1280x720(v2u window_size);
internal v4u get_tile_rect(v2u tile);
internal v4u get_game_dest(GameState *game, v2u pos);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal Direction get_random_direction(Random *random);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal b32 was_pressed(InputState *state);