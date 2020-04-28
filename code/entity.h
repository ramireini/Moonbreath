#define MAX_ENTITIES 128

typedef enum
{
    entity_id_none,
    
    entity_id_player,
    entity_id_baby_slime,
    entity_id_slime,
    entity_id_skeleton,
    entity_id_skeleton_warrior,
    entity_id_orc_warrior,
    entity_id_cave_bat,
    entity_id_python,
    entity_id_kobold,
    entity_id_ogre,
    entity_id_tormentor,
    entity_id_imp,
    entity_id_giant_demon,
    entity_id_hellhound,
    entity_id_undead_elf_warrior,
    entity_id_shadow_thief,
    entity_id_goblin,
    entity_id_viper,
    entity_id_scarlet_kingsnake,
    entity_id_stray_dog,
    entity_id_green_mamba,
    entity_id_wolf,
    entity_id_goblin_warrior,
    entity_id_floating_eye,
    entity_id_devourer,
    entity_id_ghoul,
    entity_id_cyclops,
    entity_id_dwarwen_warrior,
    entity_id_black_knight,
    entity_id_cursed_black_knight,
    entity_id_treant,
    entity_id_minotaur,
    entity_id_centaur_warrior,
    entity_id_centaur,
    entity_id_frost_shards,
    entity_id_frost_walker,
    entity_id_griffin,
    entity_id_spectre,
    entity_id_flying_skull,
    entity_id_brimstone_imp,
    
    entity_id_count
} entity_id;

typedef enum
{
    entity_type_none,
    
    entity_type_player,
    entity_type_enemy
} entity_type;

typedef struct
{
    u32 accuracy;
    f32 attack_speed;
    u32 defence;
    u32 fov;
    // TODO(rami): Maybe enemies could drop gold sometimes.
    u32 gold;
} entity_player_t;

typedef struct
{
    f32 wait_timer;
    u32 level;
    
    b32 is_flipped;
    b32 in_combat;
    b32 has_been_seen;
    
    b32 ghost_is_flipped;
    b32 is_ghost_saved;
    v2u ghost_pos;
    v2u enemy_pos_for_ghost;
} entity_enemy_t;

typedef struct
{
    char name[32];
    u32 max_hp;
    u32 hp;
    v2u pos;
    v2u new_pos;
    u32 w;
    u32 h;
    v2u tile;
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    
    u32 damage;
    u32 evasion;
    
    f32 action_speed;
    
    entity_id id; // NOTE(rami): Which specific creature the entity is.
    entity_type type; // NOTE(rami): The type of creature the entity is.
    union
    {
        entity_player_t p;
        entity_enemy_t e;
    };
} entity_t;

internal void add_enemy_entity(entity_t *entities, dungeon_t *dungeon, entity_id id, u32 x, u32 y, u32 *enemy_levels);
internal void remove_entity(entity_t *entity);
internal void kill_entity(game_state_t *game, dungeon_t *dungeon, entity_t *entity);
internal void move_entity(dungeon_t *dungeon, entity_t *entity);