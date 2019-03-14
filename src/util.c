// NOTE(Rami): This is supposed to house all of our traversable tiles so we can check against them.
bool is_traversable(iv2_t p)
{
  if(level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_stone ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_grass ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_door_open)
  {
    return true;
  }

  return false;
}

char* read_file(char *path, char *mode)
{
  FILE *file = fopen(path, mode);
  if(!file)
  {
    debug("Could not read file %s", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buff = malloc(size + 1);
  fread(buff, size, 1, file);

  buff[size] = '\0';
  fclose(file);

  return buff;
}

SDL_Texture* load_texture(char *path, SDL_Color *color_key)
{
  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    debug("SDL could not load image %s: %s", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // store the rgb color into color_key in the color format of the surface
    // all pixels with the color of color_key will be transparent
    i32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, 0, 0);
    SDL_SetColorKey(loaded_surf, SDL_TRUE, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game_state.renderer, loaded_surf);
  if(!new_tex)
  {
    debug("SDL could not create a texture from surface: %s", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

bool attack_entity(entity_t *attacker, entity_t *target)
{
  target->hp -= attacker->damage;
  if(target->hp <= 0)
  {
    return true;
  }

  return false;
}

bool is_inside_level(iv2_t p)
{
  if(p.x < 0 || p.y < 0 || p.x >= LEVEL_WIDTH_IN_TILES || p.y >= LEVEL_HEIGHT_IN_TILES)
  {
    return false;
  }

  return true;
}

internal inline bool is_iv2_equal(iv2_t a, iv2_t b)
{
  if(a.x == b.x && a.y == b.y)
  {
    return true;
  }

  return false;
}

SDL_Color hex_to_rgba(i32 hex)
{
  SDL_Color rgb_color = {(hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF};
  return rgb_color;
}

i32 str_to_i(char *str)
{
  i32 result = 0;

  while(*str >= '0' && *str <= '9')
  {
    result *= 10;
    result += *str - '0';
    str++;
  }

  return result;
}

i32 rnum(i32 min, i32 max)
{
  return min + rand() % (max - min + 1);
}

bool str_cmp(char *a, char *b)
{
  while(*a && *b && *a++ == *b++)
  {
    if(*a == '\0' && *b == '\0')
    {
      return true;
    }
  }

  return false;
}

bool line_of_sight(iv2_t a, iv2_t b)
{
  i32 dx = abs(a.x - b.x);
  i32 sx = a.x < b.x ? 1 : -1;

  i32 dy = -abs(a.y - b.y);
  i32 sy = a.y < b.y ? 1 : -1;

  i32 err = dx + dy;
  i32 err_two = 0;

  for(;;)
  {
    if(!is_traversable(a))
    {
      return false;
    }

    err_two = err * 2;
    if(err_two <= dx)
    {
      if(a.y == b.y)
      {
        break;
      }

      err += dx;
      a.y += sy;
    }

    if(err_two >= dy)
    {
      if(a.x == b.x)
      {
        break;
      }

      err += dy;
      a.x += sx;
    }
  }

  return true;
}

bool is_tile_in_range(iv2_t p, i32 tile, i32 w, i32 h)
{
  for(i32 y = p.y; y < p.y + h; y++)
  {
    for(i32 x = p.x; x < p.x + w; x++)
    {
      if(level.tiles[(y * LEVEL_WIDTH_IN_TILES) + x] == tile)
      {
        return true;
      }
    }
  }

  return false;
}

// NOTE(Rami): Distance in tiles, does not consider diagonal movement
i32 dist_in_tiles(iv2_t a, iv2_t b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

i32 tile_div(i32 n)
{
  return n / TILE_SIZE;
}

i32 tile_mul(i32 n)
{
  return n * TILE_SIZE;
}

// NOTE(Rami): pathfinding stuff start

// https://www.gamedev.net/articles/programming/artificial-intelligence/a-pathfinding-for-beginners-r2003/

#define NODE_COUNT 10
#define CARDINAL_COST 10
#define DIAGONAL_COST 14

typedef struct
{
  bool active;
  iv2_t parent;
  iv2_t pos;
  i32 g;
  i32 h;
  i32 f;
} node_t;

void add_node(node_t *list, iv2_t pos, iv2_t parent, i32 g, iv2_t end)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(!list[i].active)
    {
      list[i].active = true;
      list[i].parent = parent;
      list[i].pos = pos;
      list[i].g = g;
      list[i].h = dist_in_tiles(list[i].pos, end) * CARDINAL_COST;
      list[i].f = list[i].g + list[i].h;
      return;
    }
  }

  debug("Pathfinding Error: List is full");
}

void delete_node(node_t *list, iv2_t pos)
{
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(list[i].active && is_iv2_equal(list[i].pos, pos))
    {
      list[i].active = false;
      list[i].parent = (iv2_t){0, 0};
      list[i].pos = (iv2_t){0, 0};
      list[i].g = 0;
      list[i].h = 0;
      list[i].f = 0;
      return;
    }
  }
}

void add_valid_adjacent_nodes(node_t *list, iv2_t pos, iv2_t end)
{
  iv2_t up = {pos.x, pos.y - 1};
  if(is_traversable(up))
  {
    debug("Pathfinding: Up node is traversable");
    add_node(list, up, pos, CARDINAL_COST, end);
  }

  iv2_t down = {pos.x, pos.y + 1};
  if(is_traversable(down))
  {
    debug("Pathfinding: Down node is traversable");
    add_node(list, down, pos, CARDINAL_COST, end);
  }

  iv2_t left = {pos.x - 1, pos.y};
  if(is_traversable(left))
  {
    debug("Pathfinding: Left node is traversable");
    add_node(list, left, pos, CARDINAL_COST, end);
  }

  iv2_t right = {pos.x + 1, pos.y};
  if(is_traversable(right))
  {
    debug("Pathfinding: Right node is traversable");
    add_node(list, right, pos, CARDINAL_COST, end);
  }

  // NOTE(Rami): Add diagonal nodes.
  // NOTE(Rami): node needs to be traversable AND not on the closed list,
  // NOTE(Rami): so we need a query function for that.
}

void find_path(iv2_t a, iv2_t b)
{
  node_t *open_list = calloc(1, sizeof(node_t) * NODE_COUNT);
  node_t *closed_list = calloc(1, sizeof(node_t) * NODE_COUNT);

  // add starting node to the open list
  add_node(open_list, a, a, 0, b);

  // add valid adjacent nodes next to the starting node to the open list
  add_valid_adjacent_nodes(open_list, a, b);

  // delete the starting node from the open list
  delete_node(open_list, a);

  // add starting node to the closed list
  add_node(closed_list, a, a, 0, b);

  printf("\n-OPEN LIST-\n\n");
  for(i32 i = 0; i < NODE_COUNT; i++)
  {
    if(open_list[i].active)
    {
      printf("Node %d\n", i);
      printf("parent.x: %d\n", open_list[i].parent.x);
      printf("parent.y: %d\n", open_list[i].parent.y);
      printf("pos.x: %d\n", open_list[i].pos.x);
      printf("pos.y: %d\n", open_list[i].pos.y);
      printf("g: %d\n", open_list[i].g);
      printf("h: %d\n", open_list[i].h);
      printf("f: %d\n\n", open_list[i].f);
    }
  }

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

  free(open_list);
  free(closed_list);
}

// NOTE(Rami): pathfinding stuff end