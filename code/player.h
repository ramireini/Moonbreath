#ifndef PLAYER_H
#define PLAYER_H

// NOTE(rami): Needed by item.c
internal u32 heal_player(u32 amount);

typedef struct
{
    v2u new_pos;
    v2u pos;
    v2u size;
    char name[16];
    u32 max_hp;
    u32 hp;
    u32 damage;
    u32 armor;
    u32 speed;
    u32 level;
    u32 money;
    u32 fov;
    u32 brightness;
    
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

#endif // PLAYER_H