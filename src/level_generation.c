#include <level_generation.h>

void generate_level(char *level, int32 level_pitch, int32 level_width, int32 level_height, int32 room_count)
{
    room_t rooms[room_count];
    init_and_place_rooms(level, level_pitch, level_width, level_height, room_count, rooms);
    connect_rooms(level, level_pitch, room_count, rooms);

    int32 spawn_room;
    room_t room;

    for(;;)
    {
        spawn_room = rand_int(0, room_count - 1);
        room = rooms[spawn_room];

        int32 rand_room_x = rand_int(room.x + 1, room.x + (room.w - 2));
        int32 rand_room_y = rand_int(room.y + 1, room.y + (room.h - 2));

        place_tile(level, level_pitch, rand_room_x, rand_room_y, TILE_PATH_UP);

        int32 offset = rand_int(0, 3);

        if(offset == 0)
        {
            place_player(rand_room_x, rand_room_y - 1);
        }
        else if(offset == 1)
        {
            place_player(rand_room_x, rand_room_y + 1);
        }
        else if(offset == 2)
        {
            place_player(rand_room_x - 1, rand_room_y);
        }
        else
        {
            place_player(rand_room_x + 1, rand_room_y);
        }
        
        break;
    }

    for(;;)
    {
        int32 next_level_room = rand_int(0, room_count - 1);
        room = rooms[next_level_room];

        if(next_level_room != spawn_room)
        {
            int32 rand_room_x = rand_int(room.x + 1, room.x + (room.w - 2));
            int32 rand_room_y = rand_int(room.y + 1, room.y + (room.h - 2));

            place_tile(level, level_pitch, rand_room_x, rand_room_y, TILE_PATH_DOWN);
            break;
        }
    }
}

void init_and_place_rooms(char *level, int32 level_pitch, int32 level_width, int32 level_height, int32 room_count, room_t *rooms)
{
    // clear level data
    for(int32 i = 0; i < level_width * level_height; i++)
    {
        level[i] = 0;
    }

    // iterate over the rooms
    room_t temp;
    for(int32 i = 0; i < room_count; i++)
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
                // handle next room
                rooms[i] = temp;
                break;
            }
        }
    }
}

void connect_rooms(char *level, int32 level_pitch, int32 room_count, room_t *rooms)
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

int32 is_room_valid(char *level, int32 level_pitch, room_t room)
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

pos_t rand_pos_in_rect(room_t room)
{
    pos_t temp;

    // decrement by one so the chosen cell is not outside the room
    temp.x = rand_int(room.x, room.x + (room.w - 1));
    temp.y = rand_int(room.y, room.y + (room.h - 1));
    return temp;
}

void place_tile(char *level, int32 level_pitch, int32 x, int32 y, int32 tile)
{
    level[(y * level_pitch) + x] = tile;
}