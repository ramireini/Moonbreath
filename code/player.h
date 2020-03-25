typedef struct
{
    char name[16];
    
    v2u pos;
    v2u new_pos;
    u32 w, h;
    
    u32 max_hp;
    u32 hp;
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    u32 defence;
    u32 evasion;
    u32 move_speed;
    
    u32 level;
    u32 money;
    u32 fov;
    
    v2u tile;
} player_t;

internal b32 heal_player(u32 amount);