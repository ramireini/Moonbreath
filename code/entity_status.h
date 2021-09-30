typedef enum
{
    EntityStatusStatType_None,
    
    EntityStatusStatType_Str,
    EntityStatusStatType_Int,
    EntityStatusStatType_Dex,
    EntityStatusStatType_Def,
    EntityStatusStatType_EV,
    EntityStatusStatType_StrIntDex
} EntityStatusStatType;

typedef enum
{
    EntityStatusType_None,
    
    EntityStatusType_Damage,
    EntityStatusType_Heal,
    EntityStatusType_Stat,
    EntityStatusType_Poison,
    EntityStatusType_Burn,
    EntityStatusType_Bind,
    EntityStatusType_Bleed,
    EntityStatusType_BrokenArmor,
    EntityStatusType_Sightless,
    EntityStatusType_Confusion,
    EntityStatusType_Summon,
    EntityStatusType_Invisible,
} EntityStatusType;

typedef enum
{
    EntityDamageType_None,
    
    EntityDamageType_Physical,
    EntityDamageType_Fire,
    EntityDamageType_Ice,
    EntityDamageType_Lightning,
    EntityDamageType_Poison,
    EntityDamageType_Holy,
    EntityDamageType_Dark,
    
    EntityDamageType_Count
} EntityDamageType;

typedef struct
{
    b32 skips_defence;
    
    s32 min;
    s32 max;
    
    EntityDamageType type;
    EntityDamageType second_type;
} EntityDamage;

struct EntityStatus
{
    b32 stat_value_applied;
    b32 print_end_on_last_status;
    
    String32 name;
    
    String64 player_max_hp;
    String64 player_start;
    String64 player_end;
    String64 player_active;
    b32 is_player_active_custom;
    Color player_active_color;
    String32 player_active_target;
    
    String64 enemy_start;
    String64 enemy_end;
    String64 enemy_active;
    
    EntityStatusType type;
    EntityStatusStatType stat_type;
    
    u32 stored_value; // Store value for use when status ends.
    b32 is_value_percent;
    union
    {
        EntityDamage value;
        EntityDamage damage;
    };
    
    u32 duration;
    u32 chance;
    
    Spell *spell;
    ItemInfo *item_info;
};