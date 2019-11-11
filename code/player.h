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
    
    sprite_t sprite;
    b32 sprite_flip;
    
    v2u head_ap;
    v2u body_ap;
    v2u legs_ap;
    v2u feet_ap;
    v2u amulet_ap;
    v2u ring_ap;
    v2u first_hand_ap;
    v2u second_hand_ap;
} player_t;

internal b32 heal_player(u32 amount);