#include <level_generation.h>

void fill_rect_with_tile(uint8 *level, int32 level_pitch, SDL_Rect rect, int32 fill_tile)
{
    for(int32 x = rect.x; x < rect.x + rect.w; x++)
    {
        for(int32 y = rect.y; y < rect.y + rect.h; y++)
        {
            level[(y * level_pitch) + x] = fill_tile;
        }
    }
}

int32 check_rect_for_tile(uint8 *level, int32 level_pitch, SDL_Rect rect, int32 check_tile)
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
 
int32 check_rect_is_only_tile(uint8 *level, int32 level_pitch, SDL_Rect rect, int32 check_tile)
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

// NOTE(Rami): We could generate rooms and bring them close enough to an existing room, then add a door and keep doing that until the level is finished.
// NOTE(Rami): Add checks to the current one se we can't generate a room that goes past the level boundaries.
void generate_level(uint8 *level, int32 level_pitch, int32 level_width, int32 level_height, int32 room_count)
{
    for(int32 x = 0; x < level_width; x++)
    {
        for(int32 y = 0; y < level_width; y++)
        {
            level[(y * level_pitch) + x] = TILE_WALL_STONE;
        }
    }

    printf("\nROOM 0 set in the middle.\n");
    SDL_Rect room = {level_width / 2, level_height / 2, int_between(4, 10), int_between(4, 10)};
    fill_rect_with_tile(level, level_pitch, room, TILE_FLOOR_STONE);
    place_player(room.x, room.y);

    for(int32 i = 1; i < room_count; i++)
    {
        printf("\nROOM %d\n", i);

        for(;;)
        {
            int32 feature_x = int_between(1, level_width - 2);
            int32 feature_y = int_between(1, level_height - 2);
            int32 feature_dir = 0;
            if(level[(feature_y * level_pitch) + feature_x] == TILE_WALL_STONE)
            {
                if(level[(feature_y * level_pitch) + (feature_x - 1)] == TILE_FLOOR_STONE)
                {
                    feature_dir = 1;
                }
                else if(level[(feature_y * level_pitch) + (feature_x + 1)] == TILE_FLOOR_STONE)
                {
                    feature_dir = 2;
                }
                else if(level[((feature_y - 1) * level_pitch) + feature_x] == TILE_FLOOR_STONE)
                {
                    feature_dir = 3;
                }
                else if(level[((feature_y + 1) * level_pitch) + feature_x] == TILE_FLOOR_STONE)
                {
                    feature_dir = 4;
                }
            }

            if(!feature_dir)
            {
                continue;
            }

            printf("Feature chosen, checking if it will fit.\n");
            printf("Feature direction is %d.\n", feature_dir);

            room.w = int_between(4, 10);
            room.h = int_between(4, 10);

            if(feature_dir == 1)
            {
                room.x = feature_x + 1;
                room.y = feature_y - (room.h / 2);
            }
            else if(feature_dir == 2)
            {
                room.x = feature_x - room.w;
                room.y = feature_y - (room.h / 2);
            }
            else if(feature_dir == 3)
            {
                room.x = feature_x - (room.w / 2);
                room.y = feature_y + 1;
            }
            else if(feature_dir == 4)
            {
                room.x = feature_x - (room.w / 2);
                room.y = feature_y - room.h;
            }

            if(room.x > 0 && room.y > 0 && room.x + room.w < level_width && room.y + room.h < level_height &&
               check_rect_is_only_tile(level, level_pitch, room, TILE_WALL_STONE) &&
               !check_rect_for_tile(level, level_pitch, (SDL_Rect){room.x - 1, room.y - 1, room.w + 2, room.h + 2}, TILE_FLOOR_STONE))
            {
                printf("Feature did fit.\n");
                level[(feature_y * level_pitch) + feature_x] = TILE_DOOR_CLOSED;
                fill_rect_with_tile(level, level_pitch, room, TILE_FLOOR_STONE);

                printf("room_x: %d\n", room.x);
                printf("room_y: %d\n", room.y);
                printf("room_w: %d\n", room.w);
                printf("room_h: %d\n", room.h);
                printf("\n");

                break;
            }

            printf("Feature did not fit.\n");
            continue;
        }
    }

    // init_and_place_rooms(level, level_pitch, level_width, level_height, room_count, rooms);
    // connect_rooms(level, level_pitch, room_count, rooms);

    // int32 spawn_room;
    // SDL_Rect room;

    // for(;;)
    // {
    //     spawn_room = int_between(0, room_count - 1);
    //     room = rooms[spawn_room];

    //     int32 rand_room_x = int_between(room.x + 1, room.x + (room.w - 2));
    //     int32 rand_room_y = int_between(room.y + 1, room.y + (room.h - 2));

    //     printf("rand_room_x: %d\n", rand_room_x);
    //     printf("rand_room_y: %d\n\n", rand_room_y);

    //     place_tile(level, level_pitch, rand_room_x, rand_room_y, TILE_PATH_UP);

    //     place_player(rand_room_x, rand_room_y);

    //     int32 offset = int_between(0, 3);

    //     if(offset == 0)
    //     {
    //         place_player(rand_room_x, rand_room_y - 1);
    //     }
    //     else if(offset == 1)
    //     {
    //         place_player(rand_room_x, rand_room_y + 1);
    //     }
    //     else if(offset == 2)
    //     {
    //         place_player(rand_room_x - 1, rand_room_y);
    //     }
    //     else
    //     {
    //         place_player(rand_room_x + 1, rand_room_y);
    //     }
        
    //     break;
    // }

    // for(;;)
    // {
    //     int32 next_level_room = int_between(0, room_count - 1);
    //     room = rooms[next_level_room];

    //     if(next_level_room != spawn_room)
    //     {
    //         int32 rand_room_x = int_between(room.x + 1, room.x + (room.w - 2));
    //         int32 rand_room_y = int_between(room.y + 1, room.y + (room.h - 2));

    //         place_tile(level, level_pitch, rand_room_x, rand_room_y, TILE_PATH_DOWN);
    //         break;
    //     }
    // }
}

void init_and_place_rooms(uint8 *level, int32 level_pitch, int32 level_width, int32 level_height, int32 room_count, SDL_Rect *rooms)
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

void connect_rooms(uint8 *level, int32 level_pitch, int32 room_count, SDL_Rect *rooms)
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

int32 is_room_valid(uint8 *level, int32 level_pitch, SDL_Rect room)
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

void place_tile(uint8 *level, int32 level_pitch, int32 x, int32 y, int32 tile)
{
    level[(y * level_pitch) + x] = tile;
}