#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <update.h>
#include <item.h>
#include <render.h>
#include <text_render.h>
#include <level_generation.h>

// large
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

// standard
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// small
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

#define CONSOLE_WIDTH WINDOW_WIDTH
#define CONSOLE_HEIGHT 160

#define ENTITY_COUNT 2
#define TEXTURE_COUNT 9
#define FONT_COUNT 2
#define ITEM_COUNT 10
#define INVENTORY_COUNT 10
#define ITEM_INFO_COUNT 10
#define MESSAGE_COUNT 9

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int game_is_running;
extern SDL_Keycode key_pressed;

extern player_t *player;
extern entity_t entities[ENTITY_COUNT];

extern font_t *fonts[FONT_COUNT];
extern SDL_Texture *textures[TEXTURE_COUNT];

extern item_t items[ITEM_COUNT];
extern item_info_t items_info[ITEM_INFO_COUNT];
extern item_t inventory[INVENTORY_COUNT];
extern console_message_t messages[MESSAGE_COUNT];

int game_init();
void game_run(char *level, char *fov, SDL_Rect *camera);
void game_exit(char *level, char *fov);

#endif // GAME_H