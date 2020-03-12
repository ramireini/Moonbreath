typedef enum
{
    ai_none,
    ai_wandering
} monster_ai;

typedef enum
{
    monster_none,
    monster_baby_slime,
    monster_slime,
    monster_cave_bat,
    monster_python,
    monster_skeleton,
    monster_armored_skeleton,
    monster_orc_warrior,
    monster_kobold,
    monster_ogre,
    
    monster_test_1,
    monster_test_2,
    monster_test_3,
    monster_test_4,
    monster_test_5,
    monster_test_6,
    
    monster_test_7,
    monster_test_8,
    monster_test_9,
    monster_test_10,
    monster_test_11,
    monster_test_12,
    monster_test_13,
    monster_test_14,
    monster_test_15,
    monster_test_16,
    monster_test_17,
    monster_test_18,
    monster_test_19,
    monster_test_20,
    monster_test_21,
    monster_test_22,
    monster_test_23,
    monster_test_24,
    
    monster_total
} monster_type;

typedef enum
{
    monster_tier_low,
    monster_tier_medium,
    monster_tier_high
} monster_tier;

typedef struct
{
    char name[32];
    
    monster_type type;
    monster_ai ai;
    
    v2u pos;
    v2u new_pos;
    u32 w, h;
    
    u32 max_hp;
    u32 hp;
    u32 damage;
    u32 armor;
    u32 attack_speed;
    u32 move_speed;
    u32 level;
    
    b32 in_combat;
    b32 has_attacked;
    
    v2u tile;
    b32 tile_flip;
} monster_t;

internal void add_monster(monster_type type, v2u pos);