#include <stdlib.h>
#include <time.h>
#include <dungeon_generation.h>

void generate_dungeon(char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t *player)
{
    // NOTE(Rami): move this random seed setup to initialization in the future
    srand(time(NULL));

    room_t rooms[room_count];

    initialize_map(map, map_pitch, map_width, map_height);

    initialize_and_place_rooms(map, map_pitch, map_width, map_height, room_count, rooms);

    connect_rooms(map, map_pitch, room_count, rooms);

    place_spawns(player, map, map_pitch, room_count, rooms);
}

void initialize_map(char *map, int map_pitch, int map_width, int map_height)
{
    // set all cells on the map to a wall
    for(int y = 0; y < map_height; y++)
    {
        for(int x = 0; x < map_width; x++)
        {
            map[(y * map_pitch) + x] = TILE_WALL_STONE;
        }
    }
}

void initialize_and_place_rooms(char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms)
{
    room_t temp;

    // iterate over the rooms
    for(int i = 0; i < room_count; i++)
    {
        // keep attempting to generate room data until it's valid
        for(;;)
        {
            // generate new room data
            temp.w = random_int(3, 10);
            temp.h = random_int(3, 10);
            temp.x = random_int(1, map_width - temp.w - 1);
            temp.y = random_int(1, map_height - temp.h - 1);

            // test if the new data is valid
            if(is_room_valid(map, map_pitch, temp))
            {
                // copy the valid data to the room
                rooms[i] = temp;

                // handle next room
                break;
            }
        }
    }
}

void connect_rooms(char *map, int map_pitch, int room_count, room_t *rooms)
{
    // connect rooms
    for(int i = 1; i < room_count; i++)
    {
        pos_t room_a_cell = random_pos_in_rect(rooms[i - 1]);
        pos_t room_b_cell = random_pos_in_rect(rooms[i]);

        // get the direction towards the next room
        int sx = room_a_cell.x < room_b_cell.x ? 1 : -1;
        int sy = room_a_cell.y < room_b_cell.y ? 1 : -1;

        // create x-axis part of the corridor
        for (int x = room_a_cell.x; x != room_b_cell.x; x += sx)
        {
            map[room_a_cell.y * map_pitch + x] = TILE_FLOOR_STONE;
        }

        // create y-axis part of the corridor
        for (int y = room_a_cell.y; y != room_b_cell.y; y += sy)
        {
            map[y * map_pitch + room_b_cell.x] = TILE_FLOOR_STONE;
        }
    }
}

int is_room_valid(char *map, int map_pitch, room_t room)
{
    // check if any of the cells for the room are already occupied
    // the first offset is so that we can't generate rooms that are directly next to eachother
    for(int temp_y = room.y - 1; temp_y < room.y + room.h + 1; temp_y++)
    {
        for(int temp_x = room.x - 1; temp_x < room.x + room.w + 1; temp_x++)
        {
            // if the cell is not a wall then the cell is occupied
            if(map[(temp_y * map_pitch) + temp_x] != TILE_WALL_STONE)
            {
                // room was not valid so return 0
                return 0;
            }
        }
    }

    // if we get this far then the cells for the room weren't occupied
    for(int temp_y = (room.y - 1); temp_y < (room.y + room.h + 1); temp_y++)
    {
        for(int temp_x = (room.x - 1); temp_x < (room.x + room.w + 1); temp_x++)
        {
            if(temp_y == (room.y - 1) || temp_y == (room.y + room.h) || temp_x == (room.x - 1) || temp_x == (room.x + room.w))
            {
                map[(temp_y * map_pitch) + temp_x] = TILE_WALL_STONE;
            }
            else
            {
                map[(temp_y * map_pitch) + temp_x] = TILE_FLOOR_STONE;
            }
        }
    }

    // room was valid so return 1
    return 1;
}

int random_int(int from, int to)
{
    return from + rand() % (to - from + 1);
}

pos_t random_pos_in_rect(room_t room)
{
    pos_t temp;

    // decrement by one so the chosen cell is not outside the room
    temp.x = random_int(room.x, room.x + (room.w - 1));
    temp.y = random_int(room.y, room.y + (room.h - 1));

    return temp;
}

void place_spawns(entity_t *player, char *map, int map_pitch, int room_count, room_t *rooms)
{
    int spawn_room_number;
    room_t room;

    // place player and upwards ladder
    for(;;)
    {
        // generate random room number
        spawn_room_number = random_int(0, room_count - 1);

        room = rooms[spawn_room_number];

        // all invalid room members have the value of -1
        if(room.x != -1)
        {
            // generate a random position inside the room
            int rand_room_x = random_int(room.x + 1, room.x + (room.w - 2));
            int rand_room_y = random_int(room.y + 1, room.y + (room.h - 2));

            // check if the position is something we can move to
            if(map[rand_room_y * map_pitch + rand_room_x] == TILE_FLOOR_STONE)
            {
                // NOTE(Rami): set to top left for debugging
                // set the player to the new position
                // player->x = rand_room_x * TILE_SIZE;
                // player->y = rand_room_y * TILE_SIZE;

                player->x = 4 * TILE_SIZE;
                player->y = 4 * TILE_SIZE;

                // set the upwards ladder at the player position
                map[(player->y / TILE_SIZE) * map_pitch + (player->x / TILE_SIZE)] = TILE_STAIRS_UP;

                break;
            }
        }
    }

    // same as above but place the downwards ladder instead
    for(;;)
    {
        int next_level_room_number = random_int(0, room_count - 1);

        room = rooms[next_level_room_number];

        if(next_level_room_number != spawn_room_number && room.x != 0 && room.y != 0 && room.w != 0 && room.h != 0)
        {
            int rand_room_x = random_int(room.x + 1, room.x + (room.w - 2));
            int rand_room_y = random_int(room.y + 1, room.y + (room.h - 2));

            if(map[rand_room_y * map_pitch + rand_room_x] == TILE_FLOOR_STONE)
            {
                map[rand_room_y * map_pitch + rand_room_x] = TILE_STAIRS_DOWN;

                break;
            }
        }
    }
}