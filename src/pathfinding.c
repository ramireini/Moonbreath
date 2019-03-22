// NOTE(Rami): (Can we) Make the node count even lower?
#define NODE_COUNT 128
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

enum
{
  up,
  down,
  left,
  right,
  left_up,
  right_up,
  left_down,
  right_down,
  adjacent_count
};

typedef struct
{
  bool active;
  iv2_t parent;
  iv2_t pos;
  i32 g;
  i32 h;
  i32 f;
} node_t;

typedef struct
{
  bool success;
  i32 list_length;
  iv2_t list[NODE_COUNT];
} path_t;

void move_open_node_to_closed(node_t *open_list, node_t *closed_list, iv2_t pos)
{
  node_t node_to_move = {0};

  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(open_list[i].active && is_iv2_equal(open_list[i].pos, pos))
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

void add_open_node(node_t *open_list, iv2_t pos, iv2_t parent, i32 g, iv2_t end)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(!open_list[i].active)
    {
      open_list[i].active = true;
      open_list[i].parent = parent;
      open_list[i].pos = pos;
      open_list[i].g = g;
      open_list[i].h = dist_in_tiles(open_list[i].pos, end) * CARDINAL_COST;
      open_list[i].f = open_list[i].g + open_list[i].h;
      return;
    }
  }
}

bool node_is_in_list(node_t *list, iv2_t pos)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(is_iv2_equal(list[i].pos, pos))
    {
      return true;
    }
  }

  return false;
}

node_t find_node(node_t *list, iv2_t pos)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(list[i].active && is_iv2_equal(list[i].pos, pos))
    {
      return list[i];
    }
  }

  node_t node = {0};
  return node;
}

node_t find_best_node(node_t *list)
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

void check_adjacent_nodes(node_t *open_list, node_t *closed_list, iv2_t pos, iv2_t end)
{
  node_t current_node = find_node(closed_list, pos);

  for(i32 i = 0; i < adjacent_count; i++)
  {
    iv2_t direction = {0};
    i32 direction_cost = 0;

    if(i == up) {direction = (iv2_t){pos.x, pos.y - 1}; direction_cost = CARDINAL_COST;}
    else if(i == down) {direction = (iv2_t){pos.x, pos.y + 1}; direction_cost = CARDINAL_COST;}
    else if(i == left) {direction = (iv2_t){pos.x - 1, pos.y}; direction_cost = CARDINAL_COST;}
    else if(i == right) {direction = (iv2_t){pos.x + 1, pos.y}; direction_cost = CARDINAL_COST;}
    else if(i == left_up) {direction = (iv2_t){pos.x - 1, pos.y - 1}; direction_cost = DIAGONAL_COST;}
    else if(i == right_up) {direction = (iv2_t){pos.x + 1, pos.y - 1}; direction_cost = DIAGONAL_COST;}
    else if(i == left_down) {direction = (iv2_t){pos.x - 1, pos.y + 1}; direction_cost = DIAGONAL_COST;}
    else if(i == right_down) {direction = (iv2_t){pos.x + 1, pos.y + 1}; direction_cost = DIAGONAL_COST;}

    if(is_traversable(direction) && !node_is_in_list(closed_list, direction))
    {
      if(!node_is_in_list(open_list, direction))
      {
        add_open_node(open_list, direction, pos, direction_cost, end);
      }
      else
      {
        node_t direction_node = find_node(open_list, direction);

        if(current_node.g + direction_cost < direction_node.g)
        {
          direction_node.parent = current_node.pos;
          direction_node.g = current_node.g + direction_cost;
          direction_node.f = direction_node.g + direction_node.h;
        }
      }
    }
  }
}

void set_path_list(path_t *path, node_t *closed_list, iv2_t start, iv2_t end)
{
  i32 list_length = 0;

  node_t current = find_node(closed_list, end);
  for(;;)
  {
    if(is_iv2_equal(current.pos, start))
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
    if(is_iv2_equal(current.pos, start))
    {
      break;
    }

    path->list[i] = current.pos;
    current = find_node(closed_list, current.parent);
  }
}

path_t* pathfind(iv2_t start, iv2_t end)
{
  path_t *path = calloc(1, sizeof(path_t));

  node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
  node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);

  node_t current_node = {0};
  add_open_node(open_list, start, start, 0, end);

  for(;;)
  {
    current_node = find_best_node(open_list);
    if(!current_node.active)
    {
      path->success = false;
      break;
    }

    move_open_node_to_closed(open_list, closed_list, current_node.pos);
    check_adjacent_nodes(open_list, closed_list, current_node.pos, end);

    if(node_is_in_list(closed_list, end))
    {
      path->success = true;
      set_path_list(path, closed_list, start, end);
      break;
    }
  }

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