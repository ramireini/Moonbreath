#include <render.h>

void render_level()
{
  SDL_SetRenderTarget(global_state.renderer, global_state.assets.textures[tex_tilemap]);
  SDL_RenderClear(global_state.renderer);

  int32 to_y = tile_div(global_state.camera.y + global_state.camera.h);
  int32 to_x = tile_div(global_state.camera.x + global_state.camera.w);
  for(int32 y = tile_div(global_state.camera.y); y < to_y; y++)
  {
    for(int32 x = tile_div(global_state.camera.x); x < to_x; x++)
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

      SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_game_tileset], &src, &dst);
    }
  }

  SDL_SetRenderTarget(global_state.renderer, NULL);

  SDL_Rect dst = {0, 0, global_state.camera.w, global_state.camera.h};
  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_tilemap], &global_state.camera, &dst);
}