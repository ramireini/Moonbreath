typedef enum
{
    ExamineFlag_Open = (1 << 1),
    ExamineFlag_CameraOnExamine = (1 << 2),
    ExamineFlag_ReadyForKeypress = (1 << 3),
} ExamineFlag;

typedef enum
{
    ExamineType_None,
    
    ExamineType_Spell,
    ExamineType_Entity,
    ExamineType_Item,
    ExamineType_Trap,
    ExamineType_Tile,
    
    ExamineType_Count
} ExamineType;

typedef struct
{
    u32 flags;
    ExamineType type;
    
    b32 key_pressed[GameKey_Count];
    u32 key_pressed_start[GameKey_Count];
    u32 key_hold_duration;
    v2u pos;
    
    // Not in a union because we want to be able to do things like go from an entity spell examine window
    // back to the entity examine window.
        Item *item;
        Spell *spell;
        Entity *entity;
        DungeonTrap *trap;
        DungeonTileID tile;
    
    u32 selected_passage;
} Examine;

typedef struct
{
    MemoryArena *arena;
    memory_size used;
} TemporaryMemory;

typedef struct
{
    b32 is_set;
    
    void *storage; // Required to be initialized to zero
    memory_size size;
    memory_size used;
} GameMemory;

struct Game
{
    b32 is_set;
    
    MemoryArena memory_arena;
    DebugState debug;
    
    GameMode mode;
    Random random;
    Examine examine;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Key keybinds[GameKey_Count];
    
    b32 should_update;
    f32 passed_time;
    
    b32 show_item_ground_outline;
    };

internal void render_outline_rect(SDL_Renderer *renderer, v4u rect, Color color);
internal void render_fill_rect(SDL_Renderer *renderer, v4u rect, Color color, b32 blend);
internal void render_window(Game *game, v4u rect, u32 border_size);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal char get_pressed_keyboard_char(Input *input);
internal char get_pressed_alphabet_char(Input *input);
internal char *get_direction_string(Direction direction);
internal u32 tile_div(u32 value);
internal u32 tile_mul(u32 value);
internal u32 get_sdl_ticks_difference(u32 start_time);
internal b32 is_diagonal(Direction direction);
internal b32 is_direction_vertical(Direction direction);
internal b32 is_chance_valid(u32 chance);
internal b32 was_pressed(InputState *state);
internal b32 is_value_in_range(s32 value, s32 start, s32 end);
internal v4u get_pos_tile_rect(v2u pos);
internal v4u get_pos_tile_mul_rect(v2u pos);
internal v4u get_dungeon_tile_rect(v2u tile);
internal v4u get_game_dest(v4s camera, v2u pos);
internal v4u render_game_dest_tile(Game *game, SDL_Texture *texture, v4u tile_src, v2u pos, b32 flipped);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal String8 get_printable_key(Input *input, Key key);
internal Direction get_random_direction(Random *random);
internal Direction get_direction_moved_from(v2u old_pos, v2u new_pos);
internal Texture load_texture(SDL_Renderer *renderer, char *path, v4u *color_key);