#ifndef TILES_H
#define TILES_H

// NOTE(Rami): The amount of tiles in one row could be something like 10, and after that we move on to the next row.

typedef enum
{
  TILE_NONE = 0,
  TILE_FLOOR_GRASS,
  TILE_WALL_STONE,
  TILE_FLOOR_STONE,
  TILE_DOOR_CLOSED,
  TILE_DOOR_OPEN,
  TILE_STAIRS_UP,
  TILE_STAIRS_DOWN
} dungeon_tiles_e;

// tile information
#define TILE_SIZE 32
#define NUMBER_OF_TILES_ON_SCREEN 32
#define NUMBER_OF_TILES_ON_TILESHEET 2

#endif // TILES_H