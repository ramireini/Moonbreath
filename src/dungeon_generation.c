#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "dungeon_generation.h"

void generate_dungeon(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t *player)
{
    srand(time(NULL));

    room_t rooms[room_count];

    initialize_map(map, map_pitch, map_width, map_height);

    initialize_and_place_rooms(map, map_pitch, map_width, map_height, room_count, rooms);

    connect_rooms(map, map_pitch, room_count, rooms);

    place_spawns(player, map, map_pitch, room_count, rooms);
}

void initialize_map(unsigned char *map, int map_pitch ,int map_width, int map_height)
{
    // set all cells on the map to a wall
    for (int y = 0; y < map_height; y++)
    {
        for (int x = 0; x < map_width; x++)
        {
            map[y * map_pitch + x] = TILE_WALL_STONE;
        }
    }
}

void initialize_and_place_rooms(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms)
{
    int max_attempts = 20;

    // set all room data to null
    memset(rooms, 0, sizeof(room_t) * room_count);

    room_t temp;

    // iterate over the rooms
    for (int i = 0; i < room_count; i++)
    {
        // only try to generate a room for max_attempt times
        // so we can't get stuck on generating a room
        for (int current_attempts = 0; current_attempts < max_attempts; current_attempts++)
        {
            // generate new room data
            temp.w = random_int(3, 10);
            temp.h = random_int(3, 10);
            temp.x = random_int(1, map_width - temp.w - 1);
            temp.y = random_int(1, map_height - temp.h - 1);

            // test if the new data is valid
            if (is_room_valid(map, map_pitch, temp))
            {
                // copy the valid data to the room
                rooms[i] = temp;

                // handle next room
                break;
            }
        }
    }
}

void connect_rooms(unsigned char *map, int map_pitch, int room_count, room_t *rooms)
{
    // connect rooms
    for (int i = 1; i < room_count; i++)
    {
        if (rooms[i - 1].w != 0 && rooms[i].w != 0)
        {
            // from
            room_t room_a = rooms[i - 1];

            // to
            room_t room_b = rooms[i];

            // if true then room_a is on the left side of room_b
            if (room_a.x + room_a.w < room_b.x)
            {
                place_corridors(map, map_pitch, room_a, room_b, 1);
            }
            // if false then room_a is on the right side of room_b
            else if (room_b.x + room_b.w < room_a.x)
            {
                place_corridors(map, map_pitch, room_b, room_a, 1);
            }

            // if true then room_a is above room_b
            else if (room_a.y + room_a.h < room_b.y)
            {
                place_corridors(map, map_pitch, room_a, room_b, 2);
            }
            // if true then room_a is below room_b
            else if (room_b.y + room_b.h < room_a.y)
            {
                place_corridors(map, map_pitch, room_b, room_a, 2);
            }
        }
    }
}

void place_corridors(unsigned char *map, int map_pitch, room_t room_a, room_t room_b, int direction)
{
    // select a cell in the rooms
    cell_t room_a_cell = random_cell_in_rect(room_a);
    cell_t room_b_cell = random_cell_in_rect(room_b);

    // mark the selected cell so we can print it later
    //map[room_a_cell.y * map_pitch + room_a_cell.x] = '2';
    //map[room_b_cell.y * map_pitch + room_b_cell.x] = '2';

    if (direction == 1)
    {
        // get the absolute distance between the rooms on the x plane
        int dist_between_rooms = abs(room_a.x + room_a.w - room_b.x);

        cell_t room_a_corridor_end;
        room_a_corridor_end.y = room_a_cell.y;

        cell_t room_b_corridor_end;
        room_b_corridor_end.y = room_b_cell.y;

        // hold the location of the cell which is at the end of a corridor
        int start_x;

        // make the room_a x-axis corridor
        for (start_x = room_a.x + room_a.w; start_x < room_a.x + room_a.w + (dist_between_rooms / 2); start_x++)
        {
            if (start_x == room_a.x + room_a.w)
            {
                map[room_a_cell.y * map_pitch + start_x] = TILE_DOOR_CLOSED;
            }
            else
            {
                map[room_a_cell.y * map_pitch + start_x] = TILE_FLOOR_GRASS;
            }
        }

        // set the corridor end to be the newest value of start_x
        room_a_corridor_end.x = start_x;

        // make the room_b x-axis corridor
        for (start_x = room_b.x - 1; start_x > room_a.x + room_a.w + (dist_between_rooms / 2); start_x--)
        {
            if (start_x == room_b.x - 1)
            {
                map[room_b_cell.y * map_pitch + start_x] = TILE_DOOR_CLOSED;
            }
            else
            {
                map[room_b_cell.y * map_pitch + start_x] = TILE_FLOOR_GRASS;
            }
        }

        // set the corridor end to be the newest value of start_x
        room_b_corridor_end.x = start_x;

        // mark the ends of corridors so we can print it later
        //map[room_a_corridor_end.y * map_pitch + room_a_corridor_end.x] = '4';
        //map[room_b_corridor_end.y * map_pitch + room_b_corridor_end.x] = '4';

        // connect the two corridors with a y-axis corridor
        if (room_a_corridor_end.y <= room_b_corridor_end.y)
        {
            for (int start_y = room_a_corridor_end.y; start_y <= room_b_corridor_end.y; start_y++)
            {
                map[start_y * map_pitch + room_a_corridor_end.x] = TILE_FLOOR_GRASS;
            }
        }
        else if (room_a_corridor_end.y >= room_b_corridor_end.y)
        {
            for (int start_y = room_a_corridor_end.y; start_y >= room_b_corridor_end.y; start_y--)
            {
                map[start_y * map_pitch + room_b_corridor_end.x] = TILE_FLOOR_GRASS;
            }
        }
    }
    else if (direction == 2)
    {
        // get the absolute distance between the rooms on the y plane
        int dist_between_rooms = abs(room_a.y + room_a.h - room_b.y);

        cell_t room_a_corridor_end;
        room_a_corridor_end.x = room_a_cell.x;

        cell_t room_b_corridor_end;
        room_b_corridor_end.x = room_b_cell.x;

        // hold the location of the cell which is at the end of a corridor
        int start_y;

        // make the room_a y-axis corridor
        for (start_y = room_a.y + room_a.h; start_y < room_a.y + room_a.h + dist_between_rooms / 2; start_y++)
        {
            if (start_y == room_a.y + room_a.h)
            {
                map[start_y * map_pitch + room_a_cell.x] = TILE_DOOR_CLOSED;
            }
            else
            {
                map[start_y * map_pitch + room_a_cell.x] = TILE_FLOOR_GRASS;
            }
        }

        // set the corridor end to be the newest value of start_y
        room_a_corridor_end.y = start_y;

        // make the room_b y-axis corridor
        for (start_y = room_b.y - 1; start_y > room_a.y + room_a.h + dist_between_rooms / 2; start_y--)
        {
            if (start_y == room_b.y - 1)
            {
                map[start_y * map_pitch + room_b_cell.x] = TILE_DOOR_CLOSED;
            }
            else
            {
                map[start_y * map_pitch + room_b_cell.x] = TILE_FLOOR_GRASS;
            }
        }

        // set the corridor end to be the newest value of start_y
        room_b_corridor_end.y = start_y;

        // mark the ends of corridors so we can print it later
        //map[room_a_corridor_end.y * map_pitch + room_a_corridor_end.x] = '4';
        //map[room_b_corridor_end.y * map_pitch + room_b_corridor_end.x] = '4';
        
        // connect the two corridors with a x-axis corridor
        if (room_a_corridor_end.x <= room_b_corridor_end.x)
        {
            for (int start_x = room_a_corridor_end.x; start_x <= room_b_corridor_end.x; start_x++)
            {
                map[room_a_corridor_end.y * map_pitch + start_x] = TILE_FLOOR_GRASS;
            }
        }
        else if (room_a_corridor_end.x >= room_b_corridor_end.x)
        {
            for (int start_x = room_a_corridor_end.x; start_x >= room_b_corridor_end.x; start_x--)
            {
                map[room_a_corridor_end.y * map_pitch + start_x] = TILE_FLOOR_GRASS;
            }
        }
    }
}

int is_room_valid(unsigned char *map, int map_pitch, room_t room)
{
    // check if any of the cells for the room are already occupied
    // 
    // add an offset of 1 for each side of the room, this way we can't
    // generate rooms which are directly next to eachother
    for (int temp_y = room.y - 1; temp_y < room.y + room.h + 1; temp_y++)
    {
        for (int temp_x = room.x - 1; temp_x < room.x + room.w + 1; temp_x++)
        {
            // if the cell is not a wall then the cell is occupied
            if (map[temp_y * map_pitch + temp_x] != '1')
            {
                // room was not valid so return 0
                return 0;
            }
        }
    }

    // if we get this far then the cells for the room weren't occupied
    for (int temp_y = room.y; temp_y < room.y + room.h; temp_y++)
    {
        for (int temp_x = room.x; temp_x < room.x + room.w; temp_x++)
        {
            map[temp_y * map_pitch + temp_x] = TILE_FLOOR_GRASS;
        }
    }

    // room was valid so return 1
    return 1;
}

int random_int(int from, int to)
{
    return from + rand() % (to - from + 1);
}

cell_t random_cell_in_rect(room_t room)
{
    cell_t temp;

    // decrement by one so the chosen cell is not outside the room
    temp.x = random_int(room.x, room.x + (room.w - 1));
    temp.y = random_int(room.y, room.y + (room.h - 1));

    return temp;
}

void place_spawns(entity_t *player, unsigned char *map, int map_pitch, int room_count, room_t *rooms)
{
    int spawn_room_number;
    room_t room;

    for (;;)
    {
        // generate random room number
        spawn_room_number = random_int(0, room_count);

        room = rooms[spawn_room_number];

        // check if the room is valid
        if (room.x != 0 && room.y != 0 && room.w != 0 && room.h != 0)
        {
            // generate a random position inside the room
            int rand_x_in_room = random_int(room.x, room.x + (room.w - 1));
            int rand_y_in_room = random_int(room.y, room.y + (room.h - 1));

            // check if the position is something we can move to
            if (map[rand_y_in_room * map_pitch + rand_x_in_room] == TILE_FLOOR_GRASS)
            {
                // set the player to the new position
                player->x = rand_x_in_room * TILE_SIZE;
                player->y = rand_y_in_room * TILE_SIZE;

                // set the upwards ladder at the player position
                map[(player->y / TILE_SIZE) * map_pitch + (player->x / TILE_SIZE)] = TILE_STAIRS_UP;

                break;
            }
        }
    }

    // do the same as above but now for the stairs that go up
    for (;;)
    {
        int next_level_room_number = random_int(0, room_count);

        room = rooms[next_level_room_number];

        if (next_level_room_number != spawn_room_number && room.x != 0 && room.y != 0 && room.w != 0 && room.h != 0)
        {
            int rand_x_in_room = random_int((room.x + 2), room.x + (room.w - 1));
            int rand_y_in_room = random_int((room.y + 2), room.y + (room.h - 1));

            if (map[rand_y_in_room * map_pitch + rand_x_in_room] == TILE_FLOOR_GRASS)
            {
                map[rand_y_in_room * map_pitch + rand_x_in_room] = TILE_STAIRS_DOWN;

                break;
            }
        }
    }
}
