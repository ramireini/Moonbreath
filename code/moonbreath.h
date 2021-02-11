typedef enum
{
    ExamineFlags_Open = (1 << 1),
    ExamineFlags_ReadyForKeypress = (1 << 2),
} ExamineFlags;

typedef enum
{
    ExamineType_None,
    
    ExamineType_Item,
    ExamineType_Entity,
    ExamineType_EntitySpell,
    ExamineType_Tile
} ExamineType;

typedef struct
{
    u32 flags;
    
    b32 is_key_pressed[GameKey_Count];
    u32 key_pressed_start[GameKey_Count];
    v2u pos;
    
    ExamineType type;
    Item *item;
    Entity *entity;
    Spell *spell;
    TileID tile_id;
    
    // For dungeon passages
    u32 passage_index;
} Examine;

typedef struct
{
    b32 is_initialized;
    b32 show_item_ground_outline;
    
    u32 keybinds[GameKey_Count];
    
    Random random;
    GameMode mode;
    Examine examine;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    // Player input
    b32 should_update;
    f32 action_count;
    } Game;

internal void add_player_starting_item(Game *game, Entity *player, Item *items, ItemInfo *item_info, Inventory *inventory, UI *ui, ItemID item_id, u32 x, u32 y);
internal void render_draw_rect(Game *game, v4u rect, Color color);
internal void render_fill_rect(Game *game, v4u rect, Color color);
internal void render_window(Game *game, v4u rect, u32 border_size);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal b32 was_pressed(InputState *state);
internal v4u get_tile_rect(v2u tile);
internal v4u get_game_dest(Game *game, v2u pos);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal Direction get_random_direction(Random *random);
internal Direction get_direction_moved_from(v2u old_pos, v2u new_pos);