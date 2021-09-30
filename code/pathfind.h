typedef enum
{
 PathfindMethod_None,
 
 PathfindMethod_Away,
 PathfindMethod_Toward,
} PathfindMethod;

typedef struct
{
 v2u pos;
 u32 pathfind_value;
 Direction direction;
} PathfindPosInfo;

typedef struct
{
 b32 can_move;
 PathfindPosInfo info;
} PathfindResult;

typedef struct PathfindMap PathfindMap;

internal b32 can_entity_move_to_pos(Entity *entity, Dungeon *dungeon, v2u pos, b32 came_from_entity_pathfind);
internal u32 get_pathfind_value(PathfindMap *map, v2u pos);