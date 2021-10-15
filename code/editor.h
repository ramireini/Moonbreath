#define MAX_EDITOR_SOURCE_COUNT 64
#define MAX_EDITOR_GROUP_COUNT 4

typedef enum
{
 EditorSourceType_None,
 
 EditorSourceType_Wall,
 EditorSourceType_Floor,
 EditorSourceType_Water,
 EditorSourceType_DungeonTile,
 
 EditorSourceType_Trap,
 EditorSourceType_Entity,
} EditorSourceType;

typedef enum
{
 EditorTargetType_None,
 
 EditorTargetType_Entity,
 EditorTargetType_Trap,
 EditorTargetType_Tile,
} EditorTargetType;

typedef struct
{
 u32 id;
 EditorSourceType type;
 u32 subtype; // For knowing which one of type exactly.
 
 char *name;
 v2u pos;
 v4u tile_src;
 v4u tile_dest;
 DungeonTileID tile_id;
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
 EditorDungeonTile tile;
} EditorTarget;

typedef struct
{
 u32 group_id;
 u32 source_id;
} ActiveEditorSourceInfo;

 typedef struct
{
 b32 is_open;
 Font *font;
 v2u pos;
 
 EditorTarget active_target;
 ActiveEditorSourceInfo active_source_info;
 
 EditorGroup groups[MAX_EDITOR_GROUP_COUNT];
} EditorMode;

internal void deselect_editor_mark(EditorGroup *groups);