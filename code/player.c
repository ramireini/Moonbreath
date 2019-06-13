internal void
add_player()
{
  player.render.frame_start = v2(0, 0);
  player.render.frame_current = player.render.frame_start;
  player.render.frame_count = 4;
  player.render.frame_duration = 200;
  player.render.frame_last_changed = 0;
  player.new_x = 0;
  player.new_y = 0;
  player.x = 0;
  player.y = 0;
  player.w = 32;
  player.h = 32;
  strcpy(player.name, "Zerker");
  player.max_hp = 10;
  player.hp = 10;
  player.damage = 2;
  player.armor = 0;
  player.speed = 1;
  player.level = 1;
  player.money = 0;
  player.fov = 4;
  player.brightness = lighting_max;
}

internal void
render_player()
{
  update_animation(&player.render);

  SDL_Rect src = {tile_mul(player.render.frame_current.x),
                  tile_mul(player.render.frame_current.y),
                  player.w, player.h};
  SDL_Rect dest = {tile_mul(player.x) - game.camera.x,
                   tile_mul(player.y) - game.camera.y,
                   player.w, player.h};

  iv2 player_pos = v2(player.x, player.y);
  if(is_lit(player_pos))
  {
    iv4 color = get_color_for_lighting_value(player_pos);
    SDL_SetTextureColorMod(texture[tex_sprite_sheet], color.r, color.g, color.b);
    SDL_RenderCopy(game.renderer, texture[tex_sprite_sheet], &src, &dest);
  }

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(item[i].id && item[i].is_equipped)
    {
      SDL_Rect src = {tile_mul(item_info[item[i].id - 1].tile), 0,
                      TILE_SIZE, TILE_SIZE};
      SDL_Rect dest = {tile_mul(player.x) - game.camera.x,
                       tile_mul(player.y) - game.camera.y, TILE_SIZE, TILE_SIZE};

      SDL_SetTextureColorMod(texture[tex_item_tileset], 255, 255, 255);
      SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &src, &dest);
    }
  }
}

internal void
player_keypress(SDL_Scancode key)
{
  if(key == SDL_SCANCODE_Q)
  {
    game.state = state_quit;
  }
  // NOTE(rami): 
  else if(key == SDL_SCANCODE_P)
  {
    printf("player x: %d\n", player.x);
    printf("player y: %d\n\n", player.y);
  }
  else if(key == SDL_SCANCODE_I)
  {
    inventory.is_open = !inventory.is_open;
    inventory.item_selected = 1;
  }
  // NOTE(rami):
  else if(key == SDL_SCANCODE_F)
  {
  }
  else if(inventory.is_open)
  {
    if(key == SDL_SCANCODE_K)
    {
      if((inventory.item_selected - 1) == 0)
      {
        inventory.item_selected = inventory.item_count;
      }
      else
      {
        inventory.item_selected--;
      }
    }
    else if(key == SDL_SCANCODE_J)
    {
      if((inventory.item_selected + 1) > inventory.item_count)
      {
        inventory.item_selected = 1;
      }
      else
      {
        inventory.item_selected++;
      }
    }
    else if(key == SDL_SCANCODE_D)
    {
      drop_item(1);
    }
    else if(key == SDL_SCANCODE_E)
    {
      toggle_equipped_item();
    }
    else if(key == SDL_SCANCODE_C)
    {
      consume_item();
    }
  }
  else if(!inventory.is_open)
  {
    if(key == SDL_SCANCODE_K)
    {
      player.new_x = player.x;
      player.new_y = player.y - 1;
    }
    else if(key == SDL_SCANCODE_J)
    {
      player.new_x = player.x;
      player.new_y = player.y + 1;
    }
    else if(key == SDL_SCANCODE_H)
    {
      player.new_x = player.x - 1;
      player.new_y = player.y;
    }
    else if(key == SDL_SCANCODE_L)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y;
    }
    else if(key == SDL_SCANCODE_Y)
    {
      player.new_x = player.x - 1;
      player.new_y = player.y - 1;
    }
    else if(key == SDL_SCANCODE_U)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y - 1;
    }
    else if(key == SDL_SCANCODE_B)
    {
      player.new_x = player.x - 1;
      player.new_y = player.y + 1;
    }
    else if(key == SDL_SCANCODE_N)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y + 1;
    }
    else if(key == SDL_SCANCODE_COMMA)
    {
      pickup_item();
    }
    else if(key == SDL_SCANCODE_D)
    {
      // NOTE(rami):
      if(is_tile(v2(player.x, player.y), tile_path_down))
      {
        add_console_message("You travel deeper into the mountain..", RGBA_COLOR_WHITE_S);
        generate_level();
      }
    }
    else if(key == SDL_SCANCODE_A)
    {
      // NOTE(rami):
      if(is_tile(v2(player.x, player.y), tile_path_up))
      {
        debug("You flee from the mountain..\n");
        game.state = state_quit;
      }
    }
  }

  game.turn_changed = 1;
}

internal void
player_attack_monster(monster_t *monster)
{
  monster->hp -= player.damage;
}

internal void
get_player_attack_message(char *message)
{
  i32 i = get_num(1, 4);
  if(i == 1)
  {
    strcpy(message, "bash");
  }
  else if(i == 2)
  {
    strcpy(message, "slash");
  }
  else if(i == 3)
  {
    strcpy(message, "pierce");
  }
  else
  {
    strcpy(message, "smash");
  }
}

internal i32
heal_player(i32 amount)
{
  i32 result = 0;

  if(player.hp != player.max_hp)
  {
    result = 1;

    player.hp += amount;
    if(player.hp > player.max_hp)
    {
      player.hp = player.max_hp;
    }
  }

  return result;
}

internal i32
is_player_colliding_with_entity()
{
  i32 result = 0;

  for(i32 i = 0; i < MONSTER_COUNT; i++)
  {
    if(monster[i].type)
    {
      if(v2_equal(v2(player.new_x, player.new_y), v2(monster[i].x, monster[i].y)))
      {
        result = 1;

        char name[32] = {0};
        get_monster_name(monster[i].type, name);

        char attack[64] = {0};
        get_player_attack_message(attack);

        add_console_message("You %s the %s for %d damage", RGBA_COLOR_WHITE_S, attack, name,
                            player.damage);
        add_pop_up_text("2", tile_mul(monster[i].x) - game.camera.x,
                        tile_mul(monster[i].y) - game.camera.y, RGBA_COLOR_RED_S, 20, up, 500);

        player_attack_monster(&monster[i]);
        monster[i].in_combat = 1;

        if(!monster_is_alive(i))
        {
          add_console_message("You killed the %s!", RGBA_COLOR_ORANGE_S, name);
          remove_monster(i);
        }

        break;
      }
    }
  }

  return result;
}

// NOTE(rami): Think about if we really want x-flip,
// we could basically have the player turn when moving left or right but
// not when moving up or down. Another option would be to just render the
// player as they are and not flip the texture at all
internal void
update_player()
{
  // NOTE(rami):
  #if 0
  set_occupied(v2(player.x, player.y), 0);
  player.x = player.new_x;
  player.y = player.new_y;
  set_occupied(v2(player.new_x, player.new_y), 1);
  return;
  #endif

  if(traversable(v2(player.new_x, player.new_y)))
  {
    if(!is_player_colliding_with_entity())
    {
      player.x = player.new_x;
      player.y = player.new_y;
    }
  }
  else
  {
    if(level.tiles[(player.new_y * LEVEL_WIDTH_IN_TILES) + player.new_x] == tile_wall_stone)
    {
      add_console_message("The wall stops you from moving", RGBA_COLOR_WHITE_S);
    }
    else if(level.tiles[(player.new_y * LEVEL_WIDTH_IN_TILES) + player.new_x] == tile_door_closed)
    {
      add_console_message("You lean forward and push the door open", RGBA_COLOR_WHITE_S);
      level.tiles[(player.new_y * LEVEL_WIDTH_IN_TILES) + player.new_x] = tile_door_open;
    }
    else if(level.tiles[(player.new_y * LEVEL_WIDTH_IN_TILES) + player.new_x] == tile_path_up)
    {
      add_console_message("A path to the surface, [A]scend to flee the mountain", RGBA_COLOR_WHITE_S);
    }
    else if(level.tiles[(player.new_y * LEVEL_WIDTH_IN_TILES) + player.new_x] == tile_path_down)
    {
      add_console_message("A path that leads further downwards.. [D]escend?", RGBA_COLOR_WHITE_S);
    }
  }

  player.new_x = player.x;
  player.new_y = player.y;
  game.turn++;
}