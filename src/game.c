#include "game.h"

void render_console_messages(SDL_Renderer *renderer)
{
  SDL_Rect console = {380, 618, 634, 140};
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
  SDL_RenderDrawRect(renderer, &console);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  TTF_Font *console_message_font = TTF_OpenFont("data/fonts/classic.ttf", 16);
  SDL_Surface *temp_surface;
  SDL_Texture *message_texture;
  int message_width, message_height;

  int message_pos_x = 390;
  int message_pos_y = 624;
  int message_pos_increment = 10;

  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    if (console_messages[i].message != NULL)
    {
      SDL_Color message_color = {console_messages[i].r, console_messages[i].g, console_messages[i].b, 255};
      temp_surface = TTF_RenderText_Solid(console_message_font, console_messages[i].message, message_color);
      message_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);

      TTF_SizeText(console_message_font, console_messages[i].message, &message_width, &message_height);
      SDL_Rect message_rect = {message_pos_x, message_pos_y + (i * message_pos_increment), message_width, message_height};

      SDL_RenderCopy(renderer, message_texture, NULL, &message_rect);

      SDL_FreeSurface(temp_surface);
      temp_surface = NULL;

      SDL_DestroyTexture(message_texture);
      message_texture = NULL;
    }
  }

  TTF_CloseFont(console_message_font);
  console_message_font = NULL;
}

void add_console_message(char *message, unsigned int message_color)
{
  // shift and mask the rgb out of the hex color
  unsigned int r = message_color >> 16;
  unsigned int g = message_color >> 8 & 0xFF;
  unsigned int b = message_color & 0xFF;

  // fill the initial space of the console log
  for (int i = 0; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    if (console_messages[i].message == NULL)
    {
      console_messages[i].message = message;
      console_messages[i].r = r;
      console_messages[i].g = g;
      console_messages[i].b = b;

      return;
    }
  }

  // remove the oldest message
  console_messages[0].message = NULL;
  console_messages[0].r = 0;
  console_messages[0].g = 0;
  console_messages[0].b = 0;

  // move all messages starting from the second oldest message to create space for the new message
  for (int i = 1; i < CONSOLE_MESSAGE_AMOUNT; i++)
  {
    console_messages[i - 1].message = console_messages[i].message;
    console_messages[i - 1].r = console_messages[i].r;
    console_messages[i - 1].g = console_messages[i].g;
    console_messages[i - 1].b = console_messages[i].b;
  }

  // add the new message to the console log
  console_messages[CONSOLE_MESSAGE_AMOUNT - 1].message = message;
  console_messages[CONSOLE_MESSAGE_AMOUNT - 1].r = r;
  console_messages[CONSOLE_MESSAGE_AMOUNT - 1].g = g;
  console_messages[CONSOLE_MESSAGE_AMOUNT - 1].b = b;

  return;
}

void update_game(unsigned char *map, entity_t *player_entity, int *game_is_running, int *current_key)
{
  if (*current_key == SDLK_ESCAPE)
  {
    *game_is_running = 0;
  }
  else if (*current_key == SDLK_w)
  {
    entity_move(map, player_entity, 0, -player_entity->speed * TILE_SIZE, &(*game_is_running));
    *current_key = 0;
  }
  else if (*current_key == SDLK_a)
  {
    entity_move(map, player_entity, -player_entity->speed * TILE_SIZE, 0, &(*game_is_running));
    *current_key = 0;
  }
  else if (*current_key == SDLK_s)
  {
    entity_move(map, player_entity, 0, player_entity->speed * TILE_SIZE, &(*game_is_running));
    *current_key = 0;
  }
  else if (*current_key == SDLK_d)
  {
    entity_move(map, player_entity, player_entity->speed * TILE_SIZE, 0, &(*game_is_running));
    *current_key = 0;
  }
}

double distance(double x1, double y1, double x2, double y2)
{
  double difference_x = (x2 - x1) * (x2 - x1);
  double difference_y = (y2 - y1) * (y2 - y1);
  double sum = difference_x + difference_y;
  double value = sqrt(sum);

  return value;
}

void process_events(int *game_is_running, int *current_key)
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
      // set our current_key to the key that was pressed down
      *current_key = current_event.key.keysym.sym;
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

void render_level(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera)
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
      else if (map[y * MAP_SIZE + x] == TILE_FLOOR_STONE)
      {
        src.x = 2 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_CLOSED)
      {
        src.x = 3 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_DOOR_OPEN)
      {
        src.x = 4 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_UP)
      {
        src.x = 5 * TILE_SIZE;
        src.y = 0;
      }
      else if (map[y * MAP_SIZE + x] == TILE_STAIRS_DOWN)
      {
        src.x = 6 * TILE_SIZE;
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

// NOTE(Rami): think about if we really want the x-flip
int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running)
{
  int entity_map_pos_x = (entity->x + x) / TILE_SIZE;
  int entity_map_pos_y = (entity->y + y) / TILE_SIZE;

  if (entity->x + x >= 0 && entity->x + x < LEVEL_WIDTH && entity->y + y >= 0 && entity->y + y < LEVEL_HEIGHT)
  {
    if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_FLOOR_STONE)
    //if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] != 100)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_CLOSED)
    {
      add_console_message("You lean forward and push the heavy door open", COLOR_ACTION);
      map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] = TILE_DOOR_OPEN;
    }

    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_DOOR_OPEN)
    {
      entity->x += (x * entity->speed);
      entity->y += (y * entity->speed);

      return 1;
    }

    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_UP)
    {
      // NOTE(Rami): end game stuff
      *game_is_running = 0;

      return 0;
    }
    else if (map[entity_map_pos_y * MAP_SIZE + entity_map_pos_x] == TILE_STAIRS_DOWN)
    {
      add_console_message("You descend the ladder..", COLOR_ACTION);
      generate_dungeon(map, MAP_SIZE, MAP_SIZE, MAP_SIZE, 4, entity);

      return 1;
    }
  }

  return 1;
}

player_t* new_player(int class)
{
  player_t *p = malloc(sizeof(player_t));

  p->class = class;

  return p;
}

entity_t* new_entity(int x, int y, int x_dir, int width, int height, int speed, int view_distance)
{
  for (int i = 0; i < ENTITY_AMOUNT; i++)
  {
    if (entities[i] == NULL)
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
    if (window == NULL)
    {
      printf("ERROR: SDL could not create window: %s\n", SDL_GetError());
      success = 0;
    }
    else
    {
      // create the renderer for our window
      *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
      if (renderer == NULL)
      {
        printf("ERROR: SDL could not create a renderer: %s\n", SDL_GetError());
        success = 0;
      }
      else
      {
        // initialize PNG loading
        int image_flags = IMG_INIT_PNG;
        if ((IMG_Init(image_flags) & image_flags) == 0)
        {
          printf("ERROR: SLD image library could not initialize: %s\n", IMG_GetError());
          success = 0;
        }
        else
        {
          if (TTF_Init() < 0)
          {
            printf("ERROR: SDL ttf library could not initialize: %s\n", TTF_GetError());
            success = 0;
          }
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
    if (new_texture == NULL)
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
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}