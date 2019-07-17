internal void
add_player()
{
  player.sprite.start_frame = v2(0, 0);
  player.sprite.current_frame = player.sprite.start_frame;
  player.sprite.frame_count = 2;
  player.sprite.frame_duration = 500;
  player.sprite.frame_last_changed = 0;
  player.new_x = 0;
  player.new_y = 0;
  player.x = 0;
  player.y = 0;
  player.w = 32;
  player.h = 32;
  player.flip = SDL_FLIP_NONE;
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
update_player_alignment_points(iv2 pos)
{
  if(player.sprite.current_frame.x == 0)
  {
    player.head_ap = v2(pos.x, pos.y - 7);
    player.body_ap = pos;
    player.legs_ap = pos;
    player.feet_ap = pos;
    player.amulet_ap = pos;
    player.first_hand_ap = v2(pos.x, pos.y + 6);
    player.second_hand_ap = pos;
  }
  else if(player.sprite.current_frame.x == 1)
  {
    player.head_ap = v2(pos.x, pos.y - 9);
    player.body_ap = pos;
    player.legs_ap = pos;
    player.feet_ap = pos;
    player.amulet_ap = pos;
    player.first_hand_ap = v2(pos.x, pos.y + 4);
    player.second_hand_ap = pos;
  }
}

internal iv2
get_player_alignment_point_from_slot(iv2 pos, item_slot slot)
{
  iv2 result = {0};

  switch(slot)
  {
    case slot_head: result = player.head_ap; break;
    case slot_body: result = player.body_ap; break;
    case slot_legs: result = player.legs_ap; break;
    case slot_feet: result = player.feet_ap; break;
    case slot_amulet: result = player.amulet_ap; break;
    case slot_first_hand: result = player.first_hand_ap; break;
    case slot_second_hand: result = player.second_hand_ap; break;
  }

  return(result);
}

internal void
render_player_items(iv2 pos)
{
  for(i32 i = 1; i < slot_ring; ++i)
  {
    for(i32 k = 0; k < INVENTORY_SLOT_COUNT; ++k)
    {
      i32 item_info_index = inventory.slot[k].id - 1;
      if(item_info[item_info_index].slot == i && inventory.slot[k].id &&
         inventory.slot[k].equipped)
      {
        SDL_Rect src = {tile_mul(item_info[item_info_index].tile_x),
                        tile_mul(item_info[item_info_index].tile_y),
                        32, 32};

        iv2 item_pos = get_player_alignment_point_from_slot(pos, item_info[item_info_index].slot);
        SDL_Rect dest = {item_pos.x, item_pos.y, 32, 32};

        SDL_RenderCopyEx(game.renderer, texture[tex_wearable_item_tileset], &src, &dest,
                         0, 0, player.flip);

        break;
      }
    }
  }
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

  update_player_alignment_points(pos);

  iv2 player_pos = v2(player.x, player.y);
  if(is_lit(player_pos))
  {
    iv4 color = get_color_from_lighting_value(player_pos);
    SDL_SetTextureColorMod(texture[tex_sprite_sheet], color.r, color.g, color.b);
    SDL_RenderCopyEx(game.renderer, texture[tex_sprite_sheet], &src, &dest, 0, 0, player.flip);

    if(!is_item_slot_occupied(slot_head))
    {
      SDL_SetTextureColorMod(texture[tex_player_parts], color.r, color.g, color.b);

      SDL_Rect hair_src = {0, 0, 32, 32};
      SDL_Rect hair_dest = {player.head_ap.x, player.head_ap.y, 32, 32};

      SDL_RenderCopyEx(game.renderer, texture[tex_player_parts], &hair_src, &hair_dest, 0, 0, player.flip);
    }

    render_player_items(pos);
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
      player.flip = SDL_FLIP_HORIZONTAL;
    }
    else if(key == SDL_SCANCODE_L)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y;
      player.flip = SDL_FLIP_NONE;
    }
    else if(key == SDL_SCANCODE_Y)
    {
      player.new_x = player.x - 1;
      player.new_y = player.y - 1;
      player.flip = SDL_FLIP_HORIZONTAL;
    }
    else if(key == SDL_SCANCODE_U)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y - 1;
      player.flip = SDL_FLIP_NONE;
    }
    else if(key == SDL_SCANCODE_B)
    {
      player.new_x = player.x - 1;
      player.new_y = player.y + 1;
      player.flip = SDL_FLIP_HORIZONTAL;
    }
    else if(key == SDL_SCANCODE_N)
    {
      player.new_x = player.x + 1;
      player.new_y = player.y + 1;
      player.flip = SDL_FLIP_NONE;
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

internal void
update_player()
{
  // NOTE(rami): Force move
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