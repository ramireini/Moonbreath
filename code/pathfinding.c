// NOTE(Rami): (Can we) Make the node count even lower?
#define NODE_COUNT 128
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

typedef struct
{
  b32 active;
  iv2_t parent;
  iv2_t pos;
  i32 g;
  i32 h;
  i32 f;
} node_t;

typedef struct
{
  b32 success;
  i32 list_length;
  iv2_t list[NODE_COUNT];
} path_t;

internal void
move_open_node_to_closed(node_t *open_list, node_t *closed_list, iv2_t pos)
{
  node_t node_to_move = {0};

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(open_list[i].active && iv2_equal(open_list[i].pos, pos))
    {
      node_to_move = open_list[i];

      open_list[i].active = false;
      open_list[i].parent = (iv2_t){0, 0};
      open_list[i].pos = (iv2_t){0, 0};
      open_list[i].g = 0;
      open_list[i].h = 0;
      open_list[i].f = 0;
      break;
    }
  }

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(!closed_list[i].active)
    {
      closed_list[i] = node_to_move;
      break;
    }
  }
}

internal void
add_open_node(node_t *open_list, iv2_t pos, iv2_t parent, i32 g, iv2_t end)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(!open_list[i].active)
    {
      open_list[i].active = true;
      open_list[i].parent = parent;
      open_list[i].pos = pos;
      open_list[i].g = g;
      open_list[i].h = tile_dist(open_list[i].pos, end) * CARDINAL_COST;
      open_list[i].f = open_list[i].g + open_list[i].h;
      return;
    }
  }
}

internal b32
in_list(node_t *list, iv2_t pos)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(iv2_equal(list[i].pos, pos))
    {
      return true;
    }
  }

  return false;
}

internal node_t
find_node(node_t *list, iv2_t pos)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(list[i].active && iv2_equal(list[i].pos, pos))
    {
      return list[i];
    }
  }

  node_t node = {0};
  return node;
}

internal node_t
find_best_node(node_t *list)
{
  node_t best_node = {0};
  i32 best_f = 0;

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(list[i].active)
    {
      if(best_f == 0)
      {
        best_node = list[i];
        best_f = list[i].f;
      }
      else if(list[i].f < best_f)
      {
        best_node = list[i];
        best_f = list[i].f;
      }
    }
  }

  return best_node;
}

internal void
check_adjacent_nodes(node_t *open_list, node_t *closed_list, iv2_t pos, iv2_t end)
{
  node_t current_node = find_node(closed_list, pos);

  for(i32 i = 0; i < 8; i++)
  {
    iv2_t dir;
    i32 dir_cost;

    if(i == dir_up) {dir = (iv2_t){pos.x, pos.y - 1}; dir_cost = CARDINAL_COST;}
    else if(i == dir_down) {dir = (iv2_t){pos.x, pos.y + 1}; dir_cost = CARDINAL_COST;}
    else if(i == dir_left) {dir = (iv2_t){pos.x - 1, pos.y}; dir_cost = CARDINAL_COST;}
    else if(i == dir_right) {dir = (iv2_t){pos.x + 1, pos.y}; dir_cost = CARDINAL_COST;}
    else if(i == dir_left_up) {dir = (iv2_t){pos.x - 1, pos.y - 1}; dir_cost = DIAGONAL_COST;}
    else if(i == dir_right_up) {dir = (iv2_t){pos.x + 1, pos.y - 1}; dir_cost = DIAGONAL_COST;}
    else if(i == dir_left_down) {dir = (iv2_t){pos.x - 1, pos.y + 1}; dir_cost = DIAGONAL_COST;}
    else {dir = (iv2_t){pos.x + 1, pos.y + 1}; dir_cost = DIAGONAL_COST;}

    if(is_traversable(dir) && (!is_occupied(dir) || iv2_equal(dir, end)) && !in_list(closed_list, dir))
    {
      if(!in_list(open_list, dir))
      {
        add_open_node(open_list, dir, pos, dir_cost, end);
      }
      else
      {
        node_t dir_node = find_node(open_list, dir);

        if(current_node.g + dir_cost < dir_node.g)
        {
          dir_node.parent = current_node.pos;
          dir_node.g = current_node.g + dir_cost;
          dir_node.f = dir_node.g + dir_node.h;
        }
      }

      if(in_list(open_list, dir))
      {
        node_t dir_node = find_node(open_list, dir);

        if(current_node.g + dir_cost < dir_node.g)
        {
          dir_node.parent = current_node.pos;
          dir_node.g = current_node.g + dir_cost;
          dir_node.f = dir_node.g + dir_node.h;
        }
      }
      else
      {
        add_open_node(open_list, dir, pos, dir_cost, end);
      }
    }
  }
}

internal void
set_path_list(path_t *path, node_t *closed_list, iv2_t start, iv2_t end)
{
  i32 list_length = 0;

  node_t current = find_node(closed_list, end);
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(iv2_equal(current.pos, start))
    {
      break;
    }

    list_length++;
    current = find_node(closed_list, current.parent);
  }

  path->list_length = list_length;

  current = find_node(closed_list, end);
  for(i32 i = list_length - 1; i >= 0; i--)
  {
    if(iv2_equal(current.pos, start))
    {
      break;
    }

    path->list[i] = current.pos;
    current = find_node(closed_list, current.parent);
  }
}

internal path_t*
pathfind(iv2_t start, iv2_t end)
{
  path_t *path = calloc(1, sizeof(path_t));
  node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
  node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);

  add_open_node(open_list, start, start, 0, end);

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    node_t current_node = find_best_node(open_list);
    move_open_node_to_closed(open_list, closed_list, current_node.pos);
    check_adjacent_nodes(open_list, closed_list, current_node.pos, end);

    if(in_list(closed_list, end))
    {
      path->success = true;
      set_path_list(path, closed_list, start, end);
      break;
    }
  }

  // NOTE(Rami):
  // printf("\n-OPEN LIST-\n\n");
  // for(i32 i = 0; i < NODE_COUNT; i++)
  // {
  //   if(open_list[i].active)
  //   {
  //     printf("Node %d\n", i);
  //     printf("parent.x: %d\n", open_list[i].parent.x);
  //     printf("parent.y: %d\n", open_list[i].parent.y);
  //     printf("pos.x: %d\n", open_list[i].pos.x);
  //     printf("pos.y: %d\n", open_list[i].pos.y);
  //     printf("g: %d\n", open_list[i].g);
  //     printf("h: %d\n", open_list[i].h);
  //     printf("f: %d\n\n", open_list[i].f);
  //   }
  // }

  // printf("\n-CLOSED LIST-\n\n");
  // for(i32 i = 0; i < NODE_COUNT; i++)
  // {
  //   if(closed_list[i].active)
  //   {
  //     printf("Node %d\n", i);
  //     printf("parent.x: %d\n", closed_list[i].parent.x);
  //     printf("parent.y: %d\n", closed_list[i].parent.y);
  //     printf("pos.x: %d\n", closed_list[i].pos.x);
  //     printf("pos.y: %d\n", closed_list[i].pos.y);
  //     printf("g: %d\n", closed_list[i].g);
  //     printf("h: %d\n", closed_list[i].h);
  //     printf("f: %d\n\n", closed_list[i].f);
  //   }
  // }

  free(open_list);
  free(closed_list);

  return path;
}