typedef enum
{
    PathfindResultType_None,
    
    PathfindResultType_Move,
    PathfindResultType_BlockedByDungeonTrap
} PathfindResultType;

typedef struct
{
    b32 is_valid;
    v2u pos;
    u32 target_dist;
    Direction direction;
} PathfindPosInfo;

internal b32 is_pos_pathfindable(Dungeon *dungeon, v2u pos);