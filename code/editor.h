#define MAX_EDITOR_SOURCE_COUNT 64
#define MAX_EDITOR_GROUP_COUNT 4

typedef enum
{
 EditorSourceType_None,
 
 EditorSourceType_Wall,
 EditorSourceType_Floor,
 EditorSourceType_Water,
 
 EditorSourceType_Trap,
 EditorSourceType_Entity,
} EditorSourceType;

typedef struct
{
 u32 id;
 EditorSourceType type;
 u32 subtype; // For knowing which one of type exactly.
 
 v2u pos;
 v4u tile_src;
 v4u tile_dest;
} EditorSource;

typedef struct
{
 u32 id;
 char *name;
 
 v2u pos;
 v2u start_pos;
 u32 sources_per_row;
 
 EditorSource sources[MAX_EDITOR_SOURCE_COUNT];
} EditorGroup;

typedef struct
{
 b32 is_set;
 
 Entity *entity;
  DungeonTrap *trap;
} EditorSelection;

 typedef struct
{
 b32 is_open;
 Font *font;
 v2u pos;
 
    EditorSelection selection;
 u32 active_group_id;
 u32 active_source_id;
 EditorGroup groups[MAX_EDITOR_GROUP_COUNT];
 } EditorMode;