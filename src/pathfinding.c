#define NODE_COUNT LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

#define ADJACENT_COUNT 8
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define LEFT_UP 4
#define RIGHT_UP 5
#define LEFT_DOWN 6
#define RIGHT_DOWN 7

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
  bool found;
  iv2_t next;
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

  debug("Pathfinding Error: open_list is full");
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

  for(i32 i = 0; i < ADJACENT_COUNT; i++)
  {
    iv2_t direction = {0};

    if(i == UP) {direction = (iv2_t){pos.x, pos.y - 1};}
    else if(i == DOWN) {direction = (iv2_t){pos.x, pos.y + 1};}
    else if(i == LEFT) {direction = (iv2_t){pos.x - 1, pos.y};}
    else if(i == RIGHT) {direction = (iv2_t){pos.x + 1, pos.y};}
    else if(i == LEFT_UP) {direction = (iv2_t){pos.x - 1, pos.y - 1};}
    else if(i == RIGHT_UP) {direction = (iv2_t){pos.x + 1, pos.y - 1};}
    else if(i == LEFT_DOWN) {direction = (iv2_t){pos.x - 1, pos.y + 1};}
    else if(i == RIGHT_DOWN) {direction = (iv2_t){pos.x + 1, pos.y + 1};}

    if(is_traversable(direction) && !node_is_in_list(closed_list, direction))
    {
      if(!node_is_in_list(open_list, direction))
      {
        add_open_node(open_list, direction, pos, CARDINAL_COST, end);
      }
      else
      {
        node_t direction_node = find_node(open_list, direction);

        if(current_node.g + CARDINAL_COST < direction_node.g)
        {
          direction_node.parent = current_node.parent;

          // direction_node.g = current_node.g + CARDINAL_COST;
          // direction_node.f = direction_node.g + direction_node.h;
        }
      }
    }
  }
}

path_t find_path(iv2_t start, iv2_t end)
{
  node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
  node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);

  path_t path = {0};
  node_t current_node = {0};
  add_open_node(open_list, start, start, 0, end);

  for(;;)
  {
    current_node = find_best_node(open_list);

    if(!current_node.active)
    {
      path.found = false;
      break;
    }

    move_open_node_to_closed(open_list, closed_list, current_node.pos);
    check_adjacent_nodes(open_list, closed_list, current_node.pos, end);

    if(node_is_in_list(closed_list, end))
    {
      path.found = true;
      path.next = closed_list[1].pos;
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

  printf("\n-CLOSED LIST-\n\n");
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(closed_list[i].active)
    {
      printf("Node %d\n", i);
      printf("parent.x: %d\n", closed_list[i].parent.x);
      printf("parent.y: %d\n", closed_list[i].parent.y);
      printf("pos.x: %d\n", closed_list[i].pos.x);
      printf("pos.y: %d\n", closed_list[i].pos.y);
      printf("g: %d\n", closed_list[i].g);
      printf("h: %d\n", closed_list[i].h);
      printf("f: %d\n\n", closed_list[i].f);
    }
  }

  // NOTE(Rami): Sketch out the final path
  node_t node = find_node(closed_list, end);
  for(;;)
  {
    level.tiles[(node.pos.y * LEVEL_WIDTH_IN_TILES) + node.pos.x] = tile_floor_grass;
    node = find_node(closed_list, node.parent);

    if(is_iv2_equal(node.pos, start))
    {
      break;
    }
  }

  free(open_list);
  free(closed_list);

  return path;
}