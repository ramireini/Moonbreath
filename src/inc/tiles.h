#ifndef TILES_H
#define TILES_H

#define TILE_FLOOR_GRASS '0'
//#define TILE_FLOOR_STONE '1'
#define TILE_WALL_STONE '1'
#define TILE_DOOR_CLOSED '2'
#define TILE_DOOR_OPEN '3'
#define TILE_STAIRS_UP '4'
#define TILE_STAIRS_DOWN '5'

// tile information
#define TILE_SIZE 32
#define NUMBER_OF_TILES_ON_SCREEN 32
#define NUMBER_OF_TILES_ON_TILESHEET 2

// game dimensions
#define GAME_WIDTH 1024
#define GAME_HEIGHT 768

// window dimensions
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// level dimensions
#define LEVEL_WIDTH 2048
#define LEVEL_HEIGHT 2048

#define MAP_SIZE LEVEL_WIDTH / TILE_SIZE

#endif // TILES_H