#include <level_generation.h>

void generate_level(char *level, int level_pitch, int level_width, int level_height, int room_count, player_t *player)
{
    room_t rooms[room_count];

    init_and_place_rooms(level, level_pitch, level_width, level_height, room_count, rooms);

    connect_rooms(level, level_pitch, room_count, rooms);

    place_spawns(player, level, level_pitch, room_count, rooms);
}

void init_and_place_rooms(char *level, int level_pitch, int level_width, int level_height, int room_count, room_t *rooms)
{
    room_t temp;

    // iterate over the rooms
    for(int i = 0; i < room_count; i++)
    {
        // keep attempting to generate room data until it's valid
        for(;;)
        {
            // generate new room data
            temp.w = rand_int(3, 10);
            temp.h = rand_int(3, 10);
            temp.x = rand_int(1, level_width - temp.w - 1);
            temp.y = rand_int(1, level_height - temp.h - 1);

            // test if the new data is valid
            if(is_room_valid(level, level_pitch, temp))
            {
                // copy the valid data to the room
                rooms[i] = temp;

                // handle next room
                break;
            }
        }
    }
}

void connect_rooms(char *level, int level_pitch, int room_count, room_t *rooms)
{
    // connect rooms
    for(int i = 1; i < room_count; i++)
    {
        pos_t room_a_cell = rand_pos_in_rect(rooms[i - 1]);
        pos_t room_b_cell = rand_pos_in_rect(rooms[i]);

        // get the direction towards the next room
        int sx = room_a_cell.x < room_b_cell.x ? 1 : -1;
        int sy = room_a_cell.y < room_b_cell.y ? 1 : -1;

        // create x-axis part of the corridor
        for (int x = room_a_cell.x; x != room_b_cell.x; x += sx)
        {
            level[(room_a_cell.y * level_pitch) + x] = TILE_FLOOR_STONE;
        }

        // create y-axis part of the corridor
        for (int y = room_a_cell.y; y != room_b_cell.y; y += sy)
        {
            level[(y * level_pitch) + room_b_cell.x] = TILE_FLOOR_STONE;
        }
    }
}

int is_room_valid(char *level, int level_pitch, room_t room)
{
    // check if any of the cells for the room are already occupied
    // the first offset is so that we can't generate rooms that are directly next to eachother
    for(int temp_y = room.y - 1; temp_y < room.y + room.h + 1; temp_y++)
    {
        for(int temp_x = room.x - 1; temp_x < room.x + room.w + 1; temp_x++)
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
    for(int temp_y = (room.y - 1); temp_y < (room.y + room.h + 1); temp_y++)
    {
        for(int temp_x = (room.x - 1); temp_x < (room.x + room.w + 1); temp_x++)
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

int rand_int(int from, int to)
{
    return from + rand() % (to - from + 1);
}

pos_t rand_pos_in_rect(room_t room)
{
    pos_t temp;

    // decrement by one so the chosen cell is not outside the room
    temp.x = rand_int(room.x, room.x + (room.w - 1));
    temp.y = rand_int(room.y, room.y + (room.h - 1));

    return temp;
}

void place_spawns(player_t *player, char *level, int level_pitch, int room_count, room_t *rooms)
{
    int spawn_room_number;
    room_t room;

    // place player and upwards ladder
    for(;;)
    {
        // generate random room number
        spawn_room_number = rand_int(0, room_count - 1);

        room = rooms[spawn_room_number];

        // all invalid room members have the value of -1
        if(room.x != -1)
        {
            // generate a random position inside the room
            int rand_room_x = rand_int(room.x + 1, room.x + (room.w - 2));
            int rand_room_y = rand_int(room.y + 1, room.y + (room.h - 2));

            // make sure the position is valid
            if(level[(rand_room_y * level_pitch) + rand_room_x] == TILE_FLOOR_STONE)
            {
                // set the upwards ladder at the player position
                level[(rand_room_y * level_pitch) + rand_room_x] = TILE_PATH_UP;

                // set the player bellow the upwards ladder
                player->x = rand_room_x * TILE_SIZE;
                player->y = rand_room_y * TILE_SIZE + TILE_SIZE;

                break;
            }
        }
    }

    // place the downwards ladder
    for(;;)
    {
        int next_level_room_number = rand_int(0, room_count - 1);
        room = rooms[next_level_room_number];

        if(next_level_room_number != spawn_room_number && room.x != 0)
        {
            int rand_room_x = rand_int(room.x + 1, room.x + (room.w - 2));
            int rand_room_y = rand_int(room.y + 1, room.y + (room.h - 2));

            if(level[(rand_room_y * level_pitch) + rand_room_x] == TILE_FLOOR_STONE)
            {
                level[(rand_room_y * level_pitch) + rand_room_x] = TILE_PATH_DOWN;

                break;
            }
        }
    }
}