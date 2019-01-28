#include <level_gen.h>

void fill_rect_with_tile(int32 level_pitch, SDL_Rect rect, int32 fill_tile)
{
    for(int32 x = rect.x; x < rect.x + rect.w; x++)
    {
        for(int32 y = rect.y; y < rect.y + rect.h; y++)
        {
            level[(y * level_pitch) + x] = fill_tile;
        }
    }
}

int32 check_rect_for_tile(int32 level_pitch, SDL_Rect rect, int32 check_tile)
{
    for(int32 x = rect.x; x < rect.x + rect.w; x++)
    {
        for(int32 y = rect.y; y < rect.y + rect.h; y++)
        {
            if(level[(y * level_pitch) + x] == check_tile)
            {
                return 1;
            }
        }
    }

    return 0;
}
 
int32 check_rect_is_only_tile(int32 level_pitch, SDL_Rect rect, int32 check_tile)
{
    for(int32 x = rect.x; x < rect.x + rect.w; x++)
    {
        for(int32 y = rect.y; y < rect.y + rect.h; y++)
        {
            if(level[(y * level_pitch) + x] != check_tile)
            {
                return 0;
            }
        }
    }

    return 1;
}

#define CHANCE_TO_START_ALIVE 50
#define SIMULATION_STEPS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3

#define SOLID TILE_FLOOR_STONE
#define EMPTY TILE_NONE

int32 count_alive_neighbours(int32 cell_x, int32 cell_y)
{
    int32 count = 0;

    for(int32 x = cell_x - 1; x < cell_x + 2; x++)
    {
        for(int32 y = cell_y - 1; y < cell_y + 2; y++)
        {
            if(x == cell_x && y == cell_y)
            {
            }
            else if(x < 0 || y < 0 || x >= LEVEL_WIDTH_IN_TILES || y >= LEVEL_HEIGHT_IN_TILES ||
               level[(y * LEVEL_WIDTH_IN_TILES) + x] == SOLID)
            {
                count++;
            }
        }
    }

    return count;
}

void simulation_step(int32 area_x, int32 area_y, int32 area_w, int32 area_h)
{
    uint8 new_level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

    for(int32 x = area_x; x < area_x + area_w; x++)
    {
        for(int32 y = area_y; y < area_y + area_h; y++)
        {
            int32 count = count_alive_neighbours(x, y);
            if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == SOLID)
            {
                if(count < DEATH_LIMIT)
                {
                    new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = EMPTY;
                }
                else
                {
                    new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = SOLID;
                }
            }
            else
            {
                if(count > BIRTH_LIMIT)
                {
                    new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = SOLID;
                }
                else
                {
                    new_level[(y * LEVEL_WIDTH_IN_TILES) + x] = EMPTY;
                }
            }
        }
    }

    for(int32 x = area_x; x < area_x + area_w; x++)
    {
        for(int32 y = area_y; y < area_y + area_h; y++)
        {
            level[(y * LEVEL_WIDTH_IN_TILES) + x] = new_level[(y * LEVEL_WIDTH_IN_TILES) + x];
        }
    }
}

void generate_room(int32 room_x, int32 room_y, int32 room_w, int32 room_h)
{
    for(int32 x = room_x; x < room_x + room_w; x++)
    {
        for(int32 y = room_y; y < room_y + room_h; y++)
        {
            if(int_between(1, 100) <= CHANCE_TO_START_ALIVE)
            {
                level[(y * LEVEL_WIDTH_IN_TILES) + x] = SOLID;
            }
        }
    }

    for(int32 i = 0; i < SIMULATION_STEPS; i++)
    {
        simulation_step(room_x, room_y, room_w, room_h);
    }
}

void generate_level(int32 level_pitch, int32 level_width, int32 level_height, int32 feature_count)
{
    place_player(0, 0);

    for(int32 x = 0; x < level_width; x++)
    {
        for(int32 y = 0; y < level_height; y++)
        {
            level[(y * level_pitch) + x] = TILE_NONE;
        }
    }

    int32 rand_w = int_between(5, 12);
    int32 rand_h = int_between(5, 12);
    generate_room(1, 1, rand_w, rand_h);
}

void init_and_place_rooms(int32 level_pitch, int32 level_width, int32 level_height, int32 room_count, SDL_Rect *rooms)
{
    // clear level data
    for(int32 i = 0; i < level_width * level_height; i++)
    {
        level[i] = TILE_WALL_STONE;
    }

    // iterate over the rooms
    SDL_Rect temp;
    for(int32 i = 0; i < room_count; i++)
    {
        // keep attempting to generate room data until it's valid
        for(;;)
        {
            // generate new room data
            temp.w = int_between(3, 10);
            temp.h = int_between(3, 10);
            temp.x = int_between(1, level_width - temp.w - 1);
            temp.y = int_between(1, level_height - temp.h - 1);

            // test if the new data is valid
            if(is_room_valid(level_pitch, temp))
            {
                // copy the valid data to the room
                // handle next room
                rooms[i] = temp;
                break;
            }
        }
    }
}

void connect_rooms(int32 level_pitch, int32 room_count, SDL_Rect *rooms)
{
    // connect rooms
    for(int32 i = 1; i < room_count; i++)
    {
        pos_t room_a_cell = rand_pos_in_rect(rooms[i - 1]);
        pos_t room_b_cell = rand_pos_in_rect(rooms[i]);

        // get the direction towards the next room
        int32 sx = room_a_cell.x < room_b_cell.x ? 1 : -1;
        int32 sy = room_a_cell.y < room_b_cell.y ? 1 : -1;

        // create x-axis part of the corridor
        for (int32 x = room_a_cell.x; x != room_b_cell.x; x += sx)
        {
            level[(room_a_cell.y * level_pitch) + x] = TILE_FLOOR_STONE;
        }

        // create y-axis part of the corridor
        for (int32 y = room_a_cell.y; y != room_b_cell.y; y += sy)
        {
            level[(y * level_pitch) + room_b_cell.x] = TILE_FLOOR_STONE;
        }
    }
}

int32 is_room_valid(int32 level_pitch, SDL_Rect room)
{
    // check if any of the cells for the room are already occupied
    // the first offset is so that we can't generate rooms that are directly next to eachother
    for(int32 temp_y = room.y - 1; temp_y < room.y + room.h + 1; temp_y++)
    {
        for(int32 temp_x = room.x - 1; temp_x < room.x + room.w + 1; temp_x++)
        {
            // if the cell is not a wall then the cell is occupied
            if(level[(temp_y * level_pitch) + temp_x] != TILE_WALL_STONE)
            {
                // room was not valid so return 0
                return 0;
            }
        }
    }

    // if we get this far then the cells for the room weren't occupied
    for(int32 temp_y = (room.y - 1); temp_y < (room.y + room.h + 1); temp_y++)
    {
        for(int32 temp_x = (room.x - 1); temp_x < (room.x + room.w + 1); temp_x++)
        {
            if(temp_y == (room.y - 1) || temp_y == (room.y + room.h) || temp_x == (room.x - 1) || temp_x == (room.x + room.w))
            {
                level[(temp_y * level_pitch) + temp_x] = TILE_WALL_STONE;
            }
            else
            {
                level[(temp_y * level_pitch) + temp_x] = TILE_FLOOR_STONE;
            }
        }
    }

    // room was valid so return 1
    return 1;
}

pos_t rand_pos_in_rect(SDL_Rect room)
{
    pos_t temp;

    // decrement by one so the chosen cell is not outside the room
    temp.x = int_between(room.x, room.x + (room.w - 1));
    temp.y = int_between(room.y, room.y + (room.h - 1));
    return temp;
}

void place_tile(int32 level_pitch, int32 x, int32 y, int32 tile)
{
    level[(y * level_pitch) + x] = tile;
}