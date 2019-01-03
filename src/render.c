#include <render.h>

void render_level(char *level, char *fov)
{
  // set render target to tilemap
  SDL_SetRenderTarget(renderer, textures[TEX_TILEMAP]);

  // clear the old contents of the tilemap
  SDL_RenderClear(renderer);

  int32 to_y = to_tiles(camera.y + camera.h);
  int32 to_x = to_tiles(camera.x + camera.w);

  for(int32 y = to_tiles(camera.y); y < to_y; y++)
  {
    for(int32 x = to_tiles(camera.x); x < to_x; x++)
    {
      // what part of the tileset to render on the tilemap
      SDL_Rect src;
      src.w = TILE_SIZE;
      src.h = TILE_SIZE;

      // where to render on the tilemap
      SDL_Rect dst = {to_pixels(x), to_pixels(y), TILE_SIZE, TILE_SIZE};

      switch(level[(y * LEVEL_SIZE) + x])
      {
        case TILE_WALL_STONE:
        {
          src.x = to_pixels(TILE_WALL_STONE);
          src.y = 0;
        } break;

        case TILE_FLOOR_GRASS:
        {
          src.x = to_pixels(TILE_FLOOR_GRASS);
          src.y = 0;
        } break;

        case TILE_FLOOR_STONE:
        {
          src.x = to_pixels(TILE_FLOOR_STONE);
          src.y = 0;
        } break;

        case TILE_DOOR_CLOSED:
        {
          src.x = to_pixels(TILE_DOOR_CLOSED);
          src.y = 0;
        } break;

        case TILE_DOOR_OPEN:
        {
          src.x = to_pixels(TILE_DOOR_OPEN);
          src.y = 0;
        } break;

        case TILE_PATH_UP:
        {
          src.x = to_pixels(TILE_PATH_UP);
          src.y = 0;
        } break;

        case TILE_PATH_DOWN:
        {
          src.x = to_pixels(TILE_PATH_DOWN);
          src.y = 0;
        } break;
      }

      // NOTE(Rami): continue lighting debugging from here

      //if(fov[(y * LEVEL_SIZE) + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov[(y * LEVEL_SIZ)E + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      // render to tilemap
      SDL_RenderCopy(renderer, textures[TEX_GAME_TILESET], &src, &dst);
    }
  }

  // unset render target from tilemap
  SDL_SetRenderTarget(renderer, NULL);

  // render tilemap to window
  SDL_Rect dst = {0, 0, camera.w, camera.h};
  SDL_RenderCopy(renderer, textures[TEX_TILEMAP], &camera, &dst);
}