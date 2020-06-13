#define MAX_ENTITIES 128

typedef enum
{
    entity_none,
    
    entity_player,
    entity_rat,
    entity_snail,
    entity_slime,
    entity_giant_slime,
    entity_skeleton,
    entity_skeleton_warrior,
    entity_orc_warrior,
    entity_cave_bat,
    entity_python,
    entity_kobold,
    entity_ogre,
    entity_tormentor,
    entity_imp,
    entity_giant_demon,
    entity_hellhound,
    entity_undead_elf_warrior,
    entity_assassin,
    entity_goblin,
    entity_goblin_warrior,
    entity_viper,
    entity_scarlet_kingsnake,
    entity_dog,
    entity_green_mamba,
    entity_wolf,
    entity_floating_eye,
    entity_devourer,
    entity_ghoul,
    entity_cyclops,
    entity_dwarwen_warrior,
    entity_black_knight,
    entity_cursed_black_knight,
    entity_treant,
    entity_minotaur,
    entity_centaur_warrior,
    entity_centaur,
    entity_frost_shards,
    entity_frost_walker,
    entity_griffin,
    entity_spectre,
    entity_flying_skull,
    entity_brimstone_imp,
    
    entity_count
} entity;

typedef enum
{
    entity_type_none,
    
    entity_type_player,
    entity_type_enemy
} entity_type;

typedef enum
{
    effect_might,
    effect_wisdom,
    effect_agility,
    effect_fortitude,
    effect_resistance,
    effect_focus,
    effect_curing,
    effect_flight,
    effect_decay,
    effect_weakness,
    effect_infection,
    effect_confusion,
    
    effect_count
} effect;

typedef struct
{
    b32 is_enabled;
    u32 value;
    u32 duration;
} effect_t;

typedef struct
{
    u32 accuracy;
    f32 attack_speed;
    u32 defence;
    u32 fov;
    u32 gold; // TODO(rami): Should enemies drop gold sometimes?
    
    // TODO(Rami): Moved into statuses eventually.
    b32 is_poisoned; // TODO(rami): Should enemies be poisoned sometimes?
    
    effect_t effects[effect_count];
} entity_player_t;

typedef struct
{
    f32 wait_timer;
    
    u32 level;
    b32 is_flipped;
    b32 in_combat;
    b32 has_been_seen;
    
    b32 is_ghost_enabled;
    b32 is_ghost_flipped;
    v2u pos_save_for_ghost;
    v2u ghost_pos;
    
    // TODO(Rami): Do we want an is_aggressive?
    // TODO(Rami): Maybe have a boolean for if
    // the enemy can open doors and implement that.
    b32 is_red_blooded;
    b32 is_green_blooded;
    b32 is_made_of_bone;
} entity_enemy_t;

typedef struct
{
    char name[32];
    u32 max_hp;
    u32 hp;
    v2u pos;
    v2u new_pos;
    u32 w, h;
    v2u tile;
    
    u32 strength;
    u32 intelligence;
    u32 dexterity;
    
    u32 damage;
    u32 evasion;
    
    f32 action_speed;
    
    entity id; // Which specific creature the entity is.
    entity_type type; // The type of creature the entity is.
    union
    {
        entity_player_t p;
        entity_enemy_t e;
    };
} entity_t;

internal void move_entity(dungeon_t *dungeon, v2u new_pos, entity_t *entity);
internal void add_enemy_entity(entity_t *entities, dungeon_t *dungeon, u32 *enemy_levels, entity id, u32 x, u32 y);
internal void kill_enemy_entity(game_state_t *game, dungeon_t *dungeon, string_128_t *log, entity_t *entity);
internal void remove_entity(entity_t *entity);