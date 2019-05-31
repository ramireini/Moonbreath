// NOTE(Rami): (Can we) Make the node count even lower?
#define NODE_COUNT 128
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

enum
{
  dir_up,
  dir_down,
  dir_left,
  dir_right,
  dir_left_up,
  dir_right_up,
  dir_left_down,
  dir_right_down,
} direction_e;

typedef struct
{
  b32 active;
  i32 parent_x, parent_y;
  i32 x, y;
  i32 g;
  i32 h;
  i32 f;
} node_t;

typedef struct
{
  b32 success;
  i32 list_length;
  v2_t list[NODE_COUNT];
} path_t;

internal void
move_open_node_to_closed(node_t *open_list, node_t *closed_list, v2_t pos)
{
  node_t node_to_move = {0};

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(open_list[i].active && v2_equal(v2(open_list[i].x, open_list[i].y), pos))
    {
      node_to_move = open_list[i];

      open_list[i].active = false;
      open_list[i].parent_x = 0;
      open_list[i].parent_y = 0;
      open_list[i].x = 0;
      open_list[i].y = 0;
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
add_open_node(node_t *open_list, v2_t pos, v2_t parent, i32 g, v2_t end)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(!open_list[i].active)
    {
      open_list[i].active = true;
      open_list[i].parent_x = parent.x;
      open_list[i].parent_y = parent.y;
      open_list[i].x = pos.x;
      open_list[i].y = pos.y;
      open_list[i].g = g;
      open_list[i].h = tile_dist(v2(open_list[i].x, open_list[i].y), end) * CARDINAL_COST;
      open_list[i].f = open_list[i].g + open_list[i].h;
      return;
    }
  }
}

internal i32
in_list(node_t *list, v2_t pos)
{
  i32 result = 0;

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(v2_equal(v2(list[i].x, list[i].y), pos))
    {
      result = 1;
      break;
    }
  }

  return result;
}

internal node_t
find_node(node_t *list, v2_t pos)
{
  node_t result = {0};

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(list[i].active && v2_equal(v2(list[i].x, list[i].y), pos))
    {
      result = list[i];
      break;
    }
  }

  return result;
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
check_adjacent_nodes(node_t *open_list, node_t *closed_list, v2_t pos, v2_t end)
{
  node_t current_node = find_node(closed_list, pos);

  for(i32 i = 0; i < 8; i++)
  {
    v2_t direction = {0};
    i32 direction_cost = 0;

    if(i == dir_up) {direction = v2(pos.x, pos.y - 1); direction_cost = CARDINAL_COST;}
    else if(i == dir_down) {direction = v2(pos.x, pos.y + 1); direction_cost = CARDINAL_COST;}
    else if(i == dir_left) {direction = v2(pos.x - 1, pos.y); direction_cost = CARDINAL_COST;}
    else if(i == dir_right) {direction = v2(pos.x + 1, pos.y); direction_cost = CARDINAL_COST;}
    else if(i == dir_left_up) {direction = v2(pos.x - 1, pos.y - 1); direction_cost = DIAGONAL_COST;}
    else if(i == dir_right_up) {direction = v2(pos.x + 1, pos.y - 1); direction_cost = DIAGONAL_COST;}
    else if(i == dir_left_down) {direction = v2(pos.x - 1, pos.y + 1); direction_cost = DIAGONAL_COST;}
    else {direction = v2(pos.x + 1, pos.y + 1); direction_cost = DIAGONAL_COST;}

    if((!is_occupied(direction) || v2_equal(direction, end))
       && is_traversable(direction) && !in_list(closed_list, direction))
    {
      if(!in_list(open_list, direction))
      {
        add_open_node(open_list, direction, pos, direction_cost, end);
      }
      else
      {
        node_t dir_node = find_node(open_list, direction);

        if(current_node.g + direction_cost < dir_node.g)
        {
          dir_node.parent_x = current_node.x;
          dir_node.parent_y = current_node.y;
          dir_node.g = current_node.g + direction_cost;
          dir_node.f = dir_node.g + dir_node.h;
        }
      }

      if(in_list(open_list, direction))
      {
        node_t dir_node = find_node(open_list, direction);

        if(current_node.g + direction_cost < dir_node.g)
        {
          dir_node.parent_x = current_node.x;
          dir_node.parent_y = current_node.y;
          dir_node.g = current_node.g + direction_cost;
          dir_node.f = dir_node.g + dir_node.h;
        }
      }
      else
      {
        add_open_node(open_list, direction, pos, direction_cost, end);
      }
    }
  }
}

internal void
set_path_list(path_t *path, node_t *closed_list, v2_t start, v2_t end)
{
  i32 list_length = 0;

  node_t current = find_node(closed_list, end);
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(v2_equal(v2(current.x, current.y), start))
    {
      break;
    }

    list_length++;
    current = find_node(closed_list, v2(current.parent_x, current.parent_y));
  }

  path->list_length = list_length;

  current = find_node(closed_list, end);
  for(i32 i = list_length - 1; i >= 0; i--)
  {
    if(v2_equal(v2(current.x, current.y), start))
    {
      break;
    }

    path->list[i].x = current.x;
    path->list[i].y = current.y;
    current = find_node(closed_list, v2(current.parent_x, current.parent_y));
  }
}

internal path_t *
pathfind(v2_t start, v2_t end)
{
  path_t *path = calloc(1, sizeof(path_t));
  node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
  node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);

  add_open_node(open_list, start, start, 0, end);

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    node_t current_node = find_best_node(open_list);
    move_open_node_to_closed(open_list, closed_list, v2(current_node.x, current_node.y));
    check_adjacent_nodes(open_list, closed_list, v2(current_node.x, current_node.y), end);

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