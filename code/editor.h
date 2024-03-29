#define MAX_EDITOR_SOURCE_COUNT 64
#define MAX_EDITOR_GROUP_COUNT 4

// TODO(rami): Add an editor group for items.

typedef enum
{
    EditorSourceType_None,
    
    EditorSourceType_Wall,
    EditorSourceType_Floor,
    EditorSourceType_Water,
    EditorSourceType_DungeonTile,
    
    EditorSourceType_Trap,
    EditorSourceType_Entity,
    EditorSourceType_Item,
} EditorSourceType;

typedef enum
{
    EditorTargetType_None,
    
    EditorTargetType_Entity,
    EditorTargetType_Trap,
    EditorTargetType_Item,
    EditorTargetType_Tile,
} EditorTargetType;

typedef struct
{
    u32 id;
    EditorSourceType type;
    u32 subtype; // This can be anything, ItemID, EntityID, DungeonTileID etc.
    ItemRarity item_rarity;
    
    char *name;
    v2u pos;
    v4u tile_src;
    v4u tile_dest;
} EditorSource;

typedef struct
{
    b32 is_searchable;
    
    u32 id;
    char *name;
    v2u pos;
    v2u start_pos;
    u32 sources_per_row;
    
    Mark mark;
    EditorSource sources[MAX_EDITOR_SOURCE_COUNT];
} EditorGroup;

typedef struct
{
    v2u pos;
    DungeonTileID id;
} EditorDungeonTile;

typedef struct
{
    EditorTargetType type;
    Entity *entity;
    DungeonTrap *trap;
    Item *item;
    EditorDungeonTile tile;
} EditorTarget;

typedef struct
{
    u32 group_id;
    u32 source_id;
} ActiveEditorSourceInfo;

typedef struct
{
    b32 is_set;
    b32 is_open;
    Font *font;
    v2u pos;
    
    EditorTarget active_target;
    ActiveEditorSourceInfo active_source_info;
    
    EditorGroup groups[MAX_EDITOR_GROUP_COUNT];
} EditorMode;

internal void deselect_editor_mark(EditorGroup *groups);