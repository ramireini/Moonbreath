#include <render.h>

void render_level(uint8 *level, uint8 *fov)
{
  SDL_SetRenderTarget(renderer, textures[TEX_TILEMAP]);
  SDL_RenderClear(renderer);

  int32 to_y = tile_div(camera.y + camera.h);
  int32 to_x = tile_div(camera.x + camera.w);
  for(int32 y = tile_div(camera.y); y < to_y; y++)
  {
    for(int32 x = tile_div(camera.x); x < to_x; x++)
    {
      SDL_Rect src = {tile_mul(level[(y * LEVEL_WIDTH_IN_TILES) + x]), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(x), tile_mul(y), TILE_SIZE, TILE_SIZE};

      // NOTE(Rami): continue lighting debugging from here

      //if(fov[(y * LEVEL_SIZE) + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov[(y * LEVEL_SIZ)E + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(renderer, textures[TEX_GAME_TILESET], &src, &dst);
    }
  }

  SDL_SetRenderTarget(renderer, NULL);

  SDL_Rect dst = {0, 0, camera.w, camera.h};
  SDL_RenderCopy(renderer, textures[TEX_TILEMAP], &camera, &dst);
}