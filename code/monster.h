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
    monster_skeleton,
    monster_armored_skeleton,
    monster_orc_warrior,
    monster_cave_bat,
    monster_python,
    monster_kobold,
    monster_ogre,
    monster_tormentor,
    monster_imp,
    monster_giant_demon,
    monster_hellhound,
    monster_undead_elf_warrior,
    monster_dark_thief,
    monster_goblin,
    monster_viper,
    monster_scarlet_kingsnake,
    monster_dog,
    monster_green_mamba,
    monster_wolf,
    monster_goblin_warrior,
    monster_floating_eye,
    monster_devourer,
    monster_ghoul,
    monster_cyclops,
    monster_dwarwen_warrior,
    monster_black_knight,
    monster_cursed_black_knight,
    monster_treant,
    monster_minotaur,
    monster_centaur_warrior,
    monster_centaur,
    monster_frost_shards,
    monster_frost_walker,
    monster_griffin,
    monster_spectre,
    monster_flying_skull,
    monster_brimstone_imp,
    
    monster_total
} monster_id;

typedef struct
{
    monster_id id;
    u32 unique_id;
    monster_ai ai;
    
    u32 hp;
    u32 max_hp;
    
    v2u pos;
    v2u new_pos;
    
    f32 action_speed;
    
    b32 tile_flipped;
    b32 in_combat;
} monster_t;

typedef struct
{
    monster_id id;
    char name[32];
    u32 w, h;
    u32 level;
    u32 max_hp;
    u32 damage;
    u32 armor;
    u32 evasion;
    u32 attack_speed;
    u32 move_speed;
    v2u tile;
} monster_info_t;

internal u32 monster_info_index_from_monster_id(monster_id id);
internal void add_monster(monster_id id, u32 x, u32 y);