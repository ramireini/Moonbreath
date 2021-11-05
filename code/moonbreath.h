typedef enum
{
    ExamineFlag_Open = (1 << 1),
    ExamineFlag_CameraOnExaminePos = (1 << 2),
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
    u32 hold_time_for_move;
    v2u pos;
    
    // Not in a union because we want to be able to do things like go from an entity spell examine window
    // back to the entity examine window.
    Item *item;
    Spell *spell;
    Entity *entity;
    DungeonTrap *trap;
    DungeonTileID tile_id;
    
    u32 selected_passage;
} ExamineMode;

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

typedef struct
{
    u32 seconds;
    u32 minutes;
    u32 hours;
    
    u32 start;
    u32 end;
} GameTimer;

typedef struct
{
    v4s rect;
} GameCamera;

struct Game
{
    b32 is_set;
    
#if MOONBREATH_SLOW
    // TODO(rami): Final: Things like Debug and Editor have to be #ifdef'd out of the final build.
    DebugState debug;
    EditorMode editor;
#endif
    
    b32 should_update;
    f32 passed_time;
    f32 time;
    
    Random random;
    GameMode mode;
    GameTimer timer;
    GameCamera camera;
    ExamineMode examine;
    MemoryArena memory;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Key keybinds[GameKey_Count];
    
    b32 show_item_ground_outline;
};

// TODO(rami): Goal with keys is to let you bind anything to anything as long as you don't have more
// than one key bound to a gamekey. Make sure we are taking into account all the SDLK keys and that they
// are present in get_key_from_keycode(). Fkeys included, since someone could use them seriously and not
// just for debugging as we do.

internal void set_render_clip_rect(SDL_Renderer *renderer, v4u *clip_rect);
internal void render_texture(SDL_Renderer *renderer, SDL_Texture *texture, v4u *src, v4u *dest, b32 flipped, b32 half_color);
internal void render_outline_rect(SDL_Renderer *renderer, v4u rect, Color color);
internal void render_fill_rect(SDL_Renderer *renderer, v4u rect, Color color, b32 blend);
internal void render_window(Game *game, v4u rect, u32 border_size);
internal char get_pressed_keyboard_char(Input *input, KeyboardCharType type, b32 is_repeat_valid);
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
internal v4u get_game_dest(v4s camera, v2u pos);
internal v4u render_game_dest_tile(Game *game, SDL_Texture *texture, v4u tile_src, v2u pos, b32 flipped);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal String8 get_printable_key(Input *input, Key key);
internal String32 get_game_session_time_string(u32 time);
internal String128 get_os_path(char *path);
internal Direction get_random_direction(Random *random);
internal Direction get_direction_between_positions(v2u a_pos, v2u b_pos);
internal Texture load_texture(SDL_Renderer *renderer, char *path, v4u *color_key);