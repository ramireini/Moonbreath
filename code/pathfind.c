// TODO(rami): (Can we) Make the node count even lower?
#define NODE_COUNT 128
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

typedef enum
{
    pathfind_cardinal,
    pathfind_cardinal_and_diagonal
} pathfind_type;

typedef struct
{
    b32 active;
    v2u parent_pos;
    v2u pos;
    u32 g;
    u32 h;
    u32 f;
} node_t;

typedef struct
{
    b32 found;
    u32 length;
    v2u list[NODE_COUNT];
} path_t;

internal void
move_open_node_to_closed(node_t *open_list, node_t *closed_list, v2u pos)
{
    node_t node_to_move = {0};
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(open_list[i].active && V2u_equal(open_list[i].pos, pos))
        {
            node_to_move = open_list[i];
            memset(&open_list[i], 0, sizeof(node_t));
            break;
        }
    }
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(!closed_list[i].active)
        {
            closed_list[i] = node_to_move;
            break;
        }
    }
}

internal void
add_open_node(node_t *open_list, v2u pos, v2u parent_pos, u32 g, v2u end)
{
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(!open_list[i].active)
        {
            open_list[i].active = true;
            open_list[i].parent_pos = parent_pos;
            open_list[i].pos = pos;
            open_list[i].g = g;
            open_list[i].h = tile_dist(pos, end) * CARDINAL_COST;
            open_list[i].f = open_list[i].g + open_list[i].h;
            return;
        }
    }
}

internal b32
in_list(node_t *list, v2u pos)
{
    b32 result = false;
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(V2u_equal(list[i].pos, pos))
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal node_t
find_node(node_t *list, v2u pos)
{
    node_t result = {0};
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(list[i].active && V2u_equal(list[i].pos, pos))
        {
            result = list[i];
            break;
        }
    }
    
    return(result);
}

internal node_t
find_best_node(node_t *list)
{
    node_t result = {0};
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(list[i].active)
        {
            if(result.f == 0)
            {
                result = list[i];
            }
            else if(list[i].f < result.f)
            {
                result = list[i];
            }
        }
    }
    
    return(result);
}

internal void
check_adjacent_nodes(node_t *open_list, node_t *closed_list, v2u pos, v2u end, pathfind_type type)
{
    node_t current_node = find_node(closed_list, pos);
    
    u32 direction_limit = top_left;
    if(type == pathfind_cardinal_and_diagonal)
    {
        direction_limit = bottom_right;
    }
    
    for(u32 direction = 0; direction < direction_limit; ++direction)
    {
        v2u new_pos = {0};
        u32 cost = 0;
        
        if(direction == up)
        {
            new_pos = V2u(pos.x, pos.y - 1);
            cost = CARDINAL_COST;
        }
        else if(direction == down)
        {
            new_pos = V2u(pos.x, pos.y + 1);
            cost = CARDINAL_COST;
        }
        else if(direction == left)
        {
            new_pos = V2u(pos.x - 1, pos.y);
            cost = CARDINAL_COST;
        }
        else if(direction == right)
        {
            new_pos = V2u(pos.x + 1, pos.y);
            cost = CARDINAL_COST;
        }
        
        if(type == pathfind_cardinal_and_diagonal)
        {
            if(direction == top_left)
            {
                new_pos = V2u(pos.x - 1, pos.y - 1);
                cost = DIAGONAL_COST;
            }
            else if(direction == top_right)
            {
                new_pos = V2u(pos.x + 1, pos.y - 1);
                cost = DIAGONAL_COST;
            }
            else if(direction == bottom_left)
            {
                new_pos = V2u(pos.x - 1, pos.y + 1);
                cost = DIAGONAL_COST;
            }
            else if(direction == bottom_right)
            {
                new_pos = V2u(pos.x + 1, pos.y + 1);
                cost = DIAGONAL_COST;
            }
        }
        
        if(is_traversable(new_pos) && !in_list(closed_list, new_pos))
        {
            if(in_list(open_list, new_pos))
            {
                node_t dir_node = find_node(open_list, new_pos);
                
                if(current_node.g + cost < dir_node.g)
                {
                    dir_node.parent_pos = current_node.pos;
                    dir_node.g = current_node.g + cost;
                    dir_node.f = dir_node.g + dir_node.h;
                }
            }
            else
            {
                add_open_node(open_list, new_pos, pos, cost, end);
            }
        }
    }
}

internal void
set_path_list(path_t *path, node_t *closed_list, v2u start, v2u end)
{
    u32 length = 0;
    
    node_t current = find_node(closed_list, end);
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        if(V2u_equal(current.pos, start))
        {
            break;
        }
        
        ++length;
        current = find_node(closed_list, current.parent_pos);
    }
    
    path->length = length;
    
    current = find_node(closed_list, end);
    for(u32 i = length - 1; i >= 0; --i)
    {
        if(V2u_equal(current.pos, start))
        {
            break;
        }
        
        path->list[i] = current.pos;
        current = find_node(closed_list, current.parent_pos);
    }
}

internal path_t *
pathfind(v2u start, v2u end, pathfind_type type)
{
    path_t *path = calloc(1, sizeof(path_t));
    node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
    node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);
    
    add_open_node(open_list, start, start, 0, end);
    
    for(u32 i = 0; i < NODE_COUNT; ++i)
    {
        node_t current_node = find_best_node(open_list);
        move_open_node_to_closed(open_list, closed_list, current_node.pos);
        check_adjacent_nodes(open_list, closed_list, current_node.pos, end, type);
        
        if(in_list(closed_list, end))
        {
            path->found = true;
            set_path_list(path, closed_list, start, end);
            break;
        }
    }
    
    // TODO(rami): Debug
    // printf("\n-OPEN LIST-\n\n");
    // for(u32 i = 0; i < NODE_COUNT; ++i)
    // {
    //   if(open_list[i].active)
    //   {
    //     printf("Node %u\n", i);
    //     printf("parent.x: %u\n", open_list[i].parent.x);
    //     printf("parent.y: %u\n", open_list[i].parent.y);
    //     printf("pos.x: %u\n", open_list[i].pos.x);
    //     printf("pos.y: %u\n", open_list[i].pos.y);
    //     printf("g: %u\n", open_list[i].g);
    //     printf("h: %u\n", open_list[i].h);
    //     printf("f: %u\n\n", open_list[i].f);
    //   }
    // }
    
    // printf("\n-CLOSED LIST-\n\n");
    // for(u32 i = 0; i < NODE_COUNT; ++i)
    // {
    //   if(closed_list[i].active)
    //   {
    //     printf("Node %u\n", i);
    //     printf("parent.x: %u\n", closed_list[i].parent.x);
    //     printf("parent.y: %u\n", closed_list[i].parent.y);
    //     printf("pos.x: %u\n", closed_list[i].pos.x);
    //     printf("pos.y: %u\n", closed_list[i].pos.y);
    //     printf("g: %u\n", closed_list[i].g);
    //     printf("h: %u\n", closed_list[i].h);
    //     printf("f: %u\n\n", closed_list[i].f);
    //   }
    // }
    
    free(open_list);
    free(closed_list);
    
    return(path);
}