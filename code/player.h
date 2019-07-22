#ifndef PLAYER_H
#define PLAYER_H

// NOTE(rami): Needed by item.c
internal i32 heal_player(i32 amount);

typedef struct
{
    v2i new_pos;
    v2i pos;
    v2i size;
    char name[16];
    i32 max_hp;
    i32 hp;
    i32 damage;
    i32 armor;
    i32 speed;
    i32 level;
    i32 money;
    i32 fov;
    i32 brightness;
    
    sprite_t sprite;
    b32 sprite_flip;
    
    v2i head_ap;
    v2i body_ap;
    v2i legs_ap;
    v2i feet_ap;
    v2i amulet_ap;
    v2i first_hand_ap;
    v2i second_hand_ap;
} player_t;

#endif // PLAYER_H