internal void
add_player()
{
  player.sprite.start_frame = v2(0, 0);
  player.sprite.current_frame = player.sprite.start_frame;
  player.sprite.frame_count = 1;
  player.sprite.frame_duration = 200;
  player.sprite.frame_last_changed = 0;
  player.new_x = 0;
  player.new_y = 0;
  player.x = 0;
  player.y = 0;
  player.w = 32;
  player.h = 32;
  strcpy(player.name, "Zerker");
  player.max_hp = 10;
  player.hp = 10;
  player.damage = 1;
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
  update_sprite(&player.sprite);

  SDL_Rect src = {tile_mul(player.sprite.current_frame.x),
                  tile_mul(player.sprite.current_frame.y),
                  player.w, player.h};

  iv2 pos = get_real_position(player.x, player.y);
  SDL_Rect dest = {pos.x, pos.y, player.w, player.h};

  iv2 player_pos = v2(player.x, player.y);
  if(is_lit(player_pos))
  {
    iv4 color = get_color_for_lighting_value(player_pos);
    SDL_SetTextureColorMod(texture[tex_sprite_sheet], color.r, color.g, color.b);
    SDL_RenderCopy(game.renderer, texture[tex_sprite_sheet], &src, &dest);
  }

  // Render items the player is wearing
  for(i32 i = 0; i < ITEM_COUNT; ++i)
  {
    if(item[i].id && item[i].equipped)
    {
      SDL_Rect src = {tile_mul(item_info[item[i].id - 1].tile_x),
                      tile_mul(item_info[item[i].id - 1].tile_y),
                      32, 32};

      SDL_Rect dest = {pos.x, pos.y, TILE_SIZE, TILE_SIZE};

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

    printf("player x mul: %d\n", tile_mul(player.x));
    printf("player y mul: %d\n\n", tile_mul(player.y));
  }
  else if(key == SDL_SCANCODE_I)
  {
    inventory.x = 0;
    inventory.y = 0;
    inventory.open = !inventory.open;
  }
  // NOTE(rami):
  else if(key == SDL_SCANCODE_F)
  {
    monster[0].in_combat = !monster[0].in_combat;
  }
  else if(inventory.open)
  {
    if(key == SDL_SCANCODE_K)
    {
      if((inventory.y - 1) > -1)
      {
        --inventory.y;
      }
      else
      {
        inventory.y = INVENTORY_HEIGHT - 1;
      }
    }
    else if(key == SDL_SCANCODE_J)
    {
      if((inventory.y + 1) < INVENTORY_HEIGHT)
      {
        ++inventory.y;
      }
      else
      {
        inventory.y = 0;
      }
    }
    else if(key == SDL_SCANCODE_H)
    {
      if((inventory.x - 1) > -1)
      {
        --inventory.x;
      }
      else
      {
        inventory.x = INVENTORY_WIDTH - 1;
      }
    }
    else if(key == SDL_SCANCODE_L)
    {
      if((inventory.x + 1) < INVENTORY_WIDTH)
      {
        ++inventory.x;
      }
      else
      {
        inventory.x = 0;
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
  else if(!inventory.open)
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
      pick_up_item();
    }
    else if(key == SDL_SCANCODE_D)
    {
      if(is_tile(v2(player.x, player.y), tile_path_down))
      {
        generate_level();
      }
    }
    else if(key == SDL_SCANCODE_A)
    {
      if(is_tile(v2(player.x, player.y), tile_path_up))
      {
        game.state = state_quit;
      }
    }
  }

  if(!inventory.open)
  {
    game.turn_changed = 1;
  }
}

internal void
player_attack_monster(i32 i)
{
  monster[i].hp -= player.damage;
}

internal void
get_player_attack_message(char *message)
{
  i32 i = rand_num(1, 4);
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

  if(player.hp < player.max_hp)
  {
    result = 1;

    player.hp += amount;
    if(player.hp > player.max_hp)
    {
      player.hp = player.max_hp;
    }

    add_pop_up_text("%d", player.x, player.y, (player.w / 2) / 2, 8,
                    type_fading, color_green, 20, 500, amount);
  }


  return(result);
}

internal i32
is_player_colliding_with_monster()
{
  i32 result = 0;

  for(i32 i = 0; i < MONSTER_COUNT; ++i)
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

        add_console_message("You %s the %s for %d damage", color_white, attack, name,
                            player.damage);
        add_pop_up_text("%d", monster[i].x, monster[i].y, (monster[i].w / 2) / 2, 8,
                        type_fading, color_white, 20, 500, player.damage);

        player_attack_monster(i);
        monster[i].in_combat = 1;

        if(!monster_is_alive(i))
        {
          add_console_message("You killed the %s!", color_orange, name);
          remove_monster(i);
        }

        break;
      }
    }
  }

  return(result);
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
  player.x = player.new_x;
  player.y = player.new_y;
  return;
#endif

  if(is_traversable(v2(player.new_x, player.new_y)))
  {
    if(!is_player_colliding_with_monster())
    {
      player.x = player.new_x;
      player.y = player.new_y;
    }
  }
  else
  {
    if(level.tiles[(player.new_y * LEVEL_TILE_WIDTH) + player.new_x] == tile_wall_stone)
    {
      add_console_message("A wall stops you", color_white);
    }
    else if(level.tiles[(player.new_y * LEVEL_TILE_WIDTH) + player.new_x] == tile_door_closed)
    {
      add_console_message("You push the door open", color_white);
      level.tiles[(player.new_y * LEVEL_TILE_WIDTH) + player.new_x] = tile_door_open;
    }
    else if(level.tiles[(player.new_y * LEVEL_TILE_WIDTH) + player.new_x] == tile_path_up)
    {
      add_console_message("A path to the surface, [A]scend to flee the mountain", color_white);
    }
    else if(level.tiles[(player.new_y * LEVEL_TILE_WIDTH) + player.new_x] == tile_path_down)
    {
      add_console_message("A path that leads further downwards.. [D]escend?", color_white);
    }
  }

  player.new_x = player.x;
  player.new_y = player.y;
  ++game.turn;
}