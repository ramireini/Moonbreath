#include "game.h"

double distance(double x1, double y1, double x2, double y2)
{
  double difference_x = (x2 - x1) * (x2 - x1);
  double difference_y = (y2 - y1) * (y2 - y1);
  double sum = difference_x + difference_y;
  double value = sqrt(sum);

  return value;
}

void process_events(unsigned char *map, entity_t *player, int *game_is_running)
{
  // an event struct to hold the current event information
  SDL_Event current_event;

  // handle events as long as there are any
  while (SDL_PollEvent(&current_event))
  {
    if (current_event.type == SDL_QUIT)
    {
      *game_is_running = 0;
    }
    else if (current_event.type == SDL_KEYDOWN)
    {
      // handle key presses
      switch(current_event.key.keysym.sym)
      {
        case SDLK_ESCAPE:
        {
          *game_is_running = 0;
        } break;

        case SDLK_w:
        {
          entity_move(map, player, 0, -player->speed * TILE_SIZE, &(*game_is_running));
        } break;

        case SDLK_a:
        {
          if (entity_move(map, player, -player->speed * TILE_SIZE, 0, &(*game_is_running)))
          {
            player->x_dir = 0;
          }
        } break;

        case SDLK_s:
        {
          entity_move(map, player, 0, player->speed * TILE_SIZE, &(*game_is_running));
        } break;

        case SDLK_d:
        {
          if (entity_move(map, player, player->speed * TILE_SIZE, 0, &(*game_is_running)))
          {
            player->x_dir = 1;
          }
        } break;

        default:
        {
        } break;
      }
    }
  }
}

void update_lighting(unsigned char *map, unsigned char *fov_map, entity_t *player)
{
  // set all elements as not visible
  for (int y = 0; y < MAP_SIZE; y++)
  {
    for (int x = 0; x < MAP_SIZE; x++)
    {
      fov_map[y * MAP_SIZE + x] = 0;
    }
  }

  // hardcoded lighting
  #if 0
  // set the elements inside the players field of view visible
  for (int y = (player->y / TILE_SIZE) - player->view_distance; y < (player->y / TILE_SIZE) + player->view_distance; y++)
  {
    for (int x = (player->x / TILE_SIZE) - player->view_distance; x < (player->x / TILE_SIZE) + player->view_distance; x++)
    {
      fov_map[y * MAP_SIZE + x] = 255;
    }
  }
  #endif

  // raycasted lighting
  #if 1
  for (int angle = 0; angle < 360; angle++)
  {
    // calculate the amount for the ray to progress
    float dx = 0.1 * cos(angle);
    float dy = 0.1 * sin(angle);

    // set the ray to begin from the players location
    float fx = player->x;
    float fy = player->y;

    for (;;)
    {
      // add to the rays location the amount we calculated
      fx += dx;
      fy += dy;

      float dist = distance(player->x + 16, player->y + 16, fx, fy);
      //int idist = dist / 32;

      //printf("dist_between: %d\n", idist);

      // if the ray is over the players view distance then stop the ray
      if (dist > (player->view_distance * TILE_SIZE))
      {
        break;
      }

      // convert to array valid values
      int ifx = fx / 32;
      int ify = fy / 32;

      // make sure the ray isn't going off the level
      if (ifx >= 0 && ifx <= MAP_SIZE && ify >= 0 && ify <= MAP_SIZE)
      {
        //fov_map[ify * MAP_SIZE + ifx] = 255 * ((6 - idist) / 6);
        fov_map[ify * MAP_SIZE + ifx] = 255;


        // if we hit something we can't see through then stop the ray
        if (map[ify * MAP_SIZE + ifx] == TILE_WALL_STONE || map[ify * MAP_SIZE + ifx] == TILE_DOOR_CLOSED)
        {
          break;
        }
      }
    }

    fov_map[(player->y / TILE_SIZE) * MAP_SIZE + ((player->x / TILE_SIZE) - 1)] = 40;
  }
  #endif
}

void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Rect *camera, entity_t *player)
{
  SDL_Rect player_src = {0, 0, TILE_SIZE, TILE_SIZE};
  //SDL_Rect player_dest = {player->x, player->y, player->width, player->height};
  SDL_Rect player_dest = {player->x - camera->x, player->y - camera->y, player->width, player->height};


  if (player->x_dir == 0)
  {
    SDL_RenderCopyEx(renderer, player_tileset_tex, &player_src, &player_dest, 0, NULL, SDL_FLIP_NONE);
  }
  else if (player->x_dir == 1)
  {
    SDL_RenderCopyEx(renderer, player_tileset_tex, &player_src, &player_dest, 0, NULL, SDL_FLIP_HORIZONTAL);
  }
}

void update_camera(SDL_Rect *camera, entity_t *player)
{
  // center camera on player
  camera->x = player->x - (camera->w / 2);
  camera->y = (player->y + (player->height / 2)) - (camera->h / 2);

  if (camera->x < 0)
  {
    camera->x = 0;
  }

  if (camera->y < 0)
  {
    camera->y = 0;
  }

  if (camera->x >= LEVEL_WIDTH - camera->w)
  {
    camera->x = LEVEL_WIDTH - camera->w;
  }

  if (camera->y >= LEVEL_HEIGHT - camera->h)
  {
    camera->y = LEVEL_HEIGHT - camera->h;
  }
}

void render_background_texture(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera)
{
  // set render target to tilemap
  SDL_SetRenderTarget(renderer, tilemap_tex);

  // clear the old contents of the texture
  SDL_RenderClear(renderer);

  int to_y = (camera->y + camera->h) / TILE_SIZE;
  int to_x = (camera->x + camera->w) / TILE_SIZE;
  for (int y = camera->y / TILE_SIZE; y < to_y; y++)
  {
    for (int x = camera->x / TILE_SIZE; x < to_x; x++)
    {
      SDL_Rect src, dest;

      src.w = TILE_SIZE;
      src.h = TILE_SIZE;

      dest.w = TILE_SIZE;
      dest.h = TILE_SIZE;

      if (map[y * MAP_SIZE + x] == TILE_FLOOR_GRASS)
      {
        src.x = 0 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_WALL_STONE)
      {
        src.x = 1 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
      {
        src.x = 2 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_OPEN)
      {
        src.x = 3 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_UP)
      {
        src.x = 4 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
      {
        src.x = 5 * TILE_SIZE;
        src.y = 0;
      }

      dest.x = (x * TILE_SIZE);
      dest.y = (y * TILE_SIZE);

      // NOTE(Rami): continue lighting debugging from here

      //if (fov_map[y * MAP_SIZE + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if (fov_map[y * MAP_SIZE + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(renderer, tileset_tex, &src, &dest);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  SDL_Rect dest = {0, 0, camera->w, camera->h};

  // render to the window
  SDL_RenderCopy(renderer, tilemap_tex, camera, &dest);
}

int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running)
{
  int entity_map_pos_x = (entity->x + x) / TILE_SIZE;
  int entity_map_pos_y = (entity->y + y) / TILE_SIZE;

  if (entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    //if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_FLOOR_GRASS)
    if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_WALL_STONE || map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_FLOOR_GRASS)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }
/*    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_CLOSED)
    {
      map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] = TILE_DOOR_OPEN;
    }

    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }*/

/*    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_UP)
    {
      printf("You run away from the dungeon..\n");
      *game_is_running = 0;

      return 0;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_DOWN)
    {
      printf("You descend downwards..\n");
      generate_dungeon(map, MAP_SIZE, MAP_SIZE, 4, entity);

      return 1;
    }*/
  }

  return 1;
}

player_t* player_new(int class)
{
  player_t *p = malloc(sizeof(player_t));

  p->class = class;

  return p;
}

entity_t* entity_new(int x, int y, int x_dir, int width, int height, int speed, int view_distance)
{
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    if (!entities[i])
    {
      entities[i] = malloc(sizeof(entity_t));

      entities[i]->id = i;
      entities[i]->x = x;
      entities[i]->y = y;
      entities[i]->x_dir = x_dir;
      entities[i]->width = width;
      entities[i]->height = height;
      entities[i]->speed = speed;
      entities[i]->view_distance = view_distance;

      return entities[i];
    }
  }

  return NULL;
}

int initialize(SDL_Window **window, SDL_Renderer **renderer)
{
  // success flag
  int success = 1;

  // initialize SDL video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
    success = 0;
  }
  else
  {
    // create the window
    *window = SDL_CreateWindow("Hamxe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
      printf("ERROR: SDL could not create window: %s\n", SDL_GetError());
      success = 0;
    }
    else
    {
      // create the renderer for our window
      *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
      if (!renderer)
      {
        printf("ERROR: SDL could not create a renderer: %s\n", SDL_GetError());
        success = 0;
      }
      else
      {
        // initialize PNG loading
        int image_flags = IMG_INIT_PNG;
        if (!(IMG_Init(image_flags) & image_flags))
        {
          printf("ERROR: SLD could not initialize: %s\n", IMG_GetError());
          success = 0;
        }
      }
    }
  }

  return success;
}

SDL_Texture* load_texture(SDL_Renderer *renderer, const char *string)
{
  // optimized surface
  SDL_Texture *new_texture = NULL;

  SDL_Surface *loaded_surface = IMG_Load(string);
  if (loaded_surface == NULL)
  {
    printf("ERROR: SDL could not load image %s: %s\n", string, IMG_GetError());
  }
  else
  {
    // create a texture from the surface
    new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if (!new_texture)
    {
      printf("ERROR: SDL could not create a texture from surface: %s\n", SDL_GetError());
    }

    // get rid of the old surface
    SDL_FreeSurface(loaded_surface);
  }

  return new_texture;
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, player_t *player)
{
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    if (entities[i] != NULL)
    {
      free(entities[i]);
    }
  }

  free(player);
  player = NULL;

  SDL_DestroyTexture(tileset_tex);
  tileset_tex = NULL;

  SDL_DestroyTexture(player_tileset_tex);
  player_tileset_tex = NULL;

  SDL_DestroyTexture(tilemap_tex);
  tilemap_tex = NULL;

  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  // quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}