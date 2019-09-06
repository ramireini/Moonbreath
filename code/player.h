#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
    char name[16];
    v2u pos;
    v2u new_pos;
    v2u size;
    u32 max_hp;
    u32 hp;
    u32 damage;
    u32 armor;
    u32 speed;
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
    v2u first_hand_ap;
    v2u second_hand_ap;
} player_t;

internal void set_player_start_position(v2u start_pos);
internal u32 heal_player(u32 amount);

#endif // PLAYER_H