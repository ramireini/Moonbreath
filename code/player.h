typedef struct
{
    char name[16];
    
    v2u pos;
    v2u new_pos;
    u32 w, h;
    
    u32 max_hp;
    u32 hp;
    
    u32 strength;
    u32 defence;
    u32 move_speed;
    
    u32 level;
    u32 money;
    u32 fov;
    
    v2u tile;
    b32 tile_flip;
    
    v2u head_wear_pos;
    v2u body_wear_pos;
    v2u legs_wear_pos;
    v2u feet_wear_pos;
    v2u amulet_wear_pos;
    v2u ring_wear_pos;
    v2u first_hand_wear_pos;
    v2u second_hand_wear_pos;
} player_t;

internal b32 heal_player(u32 amount);