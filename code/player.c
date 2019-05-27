internal void
player_keypress(SDL_Scancode key)
{
  if(key == SDL_SCANCODE_Q)
  {
    game.running = false;
  }
  // NOTE(Rami): 
  else if(key == SDL_SCANCODE_P)
  {
    printf("player x: %d\n", player.entity.x);
    printf("player y: %d\n\n", player.entity.y);
  }
  // NOTE(Rami): 
  else if(key == SDL_SCANCODE_C)
  {
    slimes[0].in_combat = !slimes[0].in_combat;
  }
  else if(key == SDL_SCANCODE_I)
  {
    player.inventory.is_open = !player.inventory.is_open;
    player.inventory.item_selected = 1;
  }
  else if(player.inventory.is_open)
  {
    if(key == SDL_SCANCODE_K)
    {
      if((player.inventory.item_selected - 1) == 0)
      {
        player.inventory.item_selected = player.inventory.item_count;
      }
      else
      {
        player.inventory.item_selected--;
      }
    }
    else if(key == SDL_SCANCODE_J)
    {
      if((player.inventory.item_selected + 1) > player.inventory.item_count)
      {
        player.inventory.item_selected = 1;
      }
      else
      {
        player.inventory.item_selected++;
      }
    }
    else if(key == SDL_SCANCODE_D)
    {
      drop_item();

      if(player.inventory.item_selected == player.inventory.item_count - 1)
      {
        player.inventory.item_selected--;
      }
    }
    else if(key == SDL_SCANCODE_E)
    {
      toggle_equipped_item();
    }
    else if(key == SDL_SCANCODE_C)
    {
      consume_item();

      if(player.inventory.item_selected == player.inventory.item_count - 1)
      {
        player.inventory.item_selected--;
      }
    }
  }
  else if(!player.inventory.is_open)
  {
    if(key == SDL_SCANCODE_K)
    {
      player.entity.new_x = player.entity.x;
      player.entity.new_y = player.entity.y - 1;
    }
    else if(key == SDL_SCANCODE_J)
    {
      player.entity.new_x = player.entity.x;
      player.entity.new_y = player.entity.y + 1;
    }
    else if(key == SDL_SCANCODE_H)
    {
      player.entity.new_x = player.entity.x - 1;
      player.entity.new_y = player.entity.y;
    }
    else if(key == SDL_SCANCODE_L)
    {
      player.entity.new_x = player.entity.x + 1;
      player.entity.new_y = player.entity.y;
    }
    else if(key == SDL_SCANCODE_Y)
    {
      player.entity.new_x = player.entity.x - 1;
      player.entity.new_y = player.entity.y - 1;
    }
    else if(key == SDL_SCANCODE_U)
    {
      player.entity.new_x = player.entity.x + 1;
      player.entity.new_y = player.entity.y - 1;
    }
    else if(key == SDL_SCANCODE_B)
    {
      player.entity.new_x = player.entity.x - 1;
      player.entity.new_y = player.entity.y + 1;
    }
    else if(key == SDL_SCANCODE_N)
    {
      player.entity.new_x = player.entity.x + 1;
      player.entity.new_y = player.entity.y + 1;
    }
    else if(key == SDL_SCANCODE_COMMA)
    {
      pickup_item();
    }
    else if(key == SDL_SCANCODE_D)
    {
      if(is_tile(v2(player.entity.x, player.entity.y), tile_path_down))
      {
        add_console_message("You travel deeper into the mountain..", RGBA_COLOR_WHITE_S);
        generate_level();
      }
    }
    else if(key == SDL_SCANCODE_A)
    {
      if(is_tile(v2(player.entity.x, player.entity.y), tile_path_up))
      {
        debug("You flee from the mountain..\n");
        game.running = false;
      }
    }
  }

  game.turn_changed = true;
}

// NOTE(Rami): Think about why this is done in the way it is, could we do it like in Rebirth?
internal void
create_player()
{
  if(!player.active)
  {
    player.active = true;
    player.name = "Frozii";
    player.level = 0;
    player.money = 0;
    player.max_hp = 10;
    player.xp = 0;
    player.turn = 0;

    player.entity.hp = 5;
    player.entity.damage = 10;
    player.entity.armor = 0;
    player.entity.brightness = lighting_max;
    player.entity.fov = 4;
    player.entity.speed = 1;
    player.entity.x = 0;
    player.entity.y = 0;
    player.entity.new_x = 0;
    player.entity.new_y = 0;
    player.entity.w = TILE_SIZE;
    player.entity.h = TILE_SIZE;
    player.entity.anim.frame_current = 0;
    player.entity.anim.frame_count = 4;
    player.entity.anim.frame_delay = 200;
    player.entity.anim.frame_last_changed = 0;

    for(i32 i = 0; i < INVENTORY_SLOT_COUNT; i++)
    {
      player.inventory.slots[i].item_id = id_none;
      player.inventory.slots[i].unique_id = 0;
      player.inventory.slots[i].is_on_ground = 0;
      player.inventory.slots[i].is_equipped = 0;
      player.inventory.slots[i].x = 0;
      player.inventory.slots[i].y = 0;
    }

    player.inventory.is_open = false;
    player.inventory.item_count = 0;
    player.inventory.item_selected = 1;
  }
}

// NOTE(Rami): Think about if we really want x-flip,
// we could basically have the player turn when moving left or right but
// not when moving up or down. Another option would be to just render the
// player as they are and not flip the texture at all.
internal void
update_player()
{
  b32 can_move = true;

  if(player.entity.hp <= 0)
  {
    // NOTE(Rami): Need to think about the process of the player dying more closely.
    add_console_message("Player is dead now", RGBA_COLOR_BLUE_S);
    can_move = false;
  }

  if(can_move)
  {
    if(level.tiles[(player.entity.new_y * LEVEL_WIDTH_IN_TILES) + player.entity.new_x] == tile_wall_stone)
    {
      add_console_message("The wall stops you from moving", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_y * LEVEL_WIDTH_IN_TILES) + player.entity.new_x] == tile_door_closed)
    {
      add_console_message("You lean forward and push the door open", RGBA_COLOR_WHITE_S);
      level.tiles[(player.entity.new_y * LEVEL_WIDTH_IN_TILES) + player.entity.new_x] = tile_door_open;
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_y * LEVEL_WIDTH_IN_TILES) + player.entity.new_x] == tile_path_up)
    {
      add_console_message("A path to the surface, [A]scend to flee the mountain", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_y * LEVEL_WIDTH_IN_TILES) + player.entity.new_x] == tile_path_down)
    {
      add_console_message("A path that leads further downwards.. [D]escend?", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
  }

  if(can_move)
  {
    for(i32 i = 0; i < SLIME_COUNT; i++)
    {
      if(slimes[i].active)
      {
        if(v2_equal(v2(player.entity.new_x, player.entity.new_y), v2(slimes[i].entity.x, slimes[i].entity.y)))
        {
          can_move = false;

          if(!attack_entity(&player.entity, &slimes[i].entity))
          {
            add_console_message("You attack the Slime for %d damage", RGBA_COLOR_WHITE_S, player.entity.damage);
            slimes[i].in_combat = true;
          }
          else
          {
            add_console_message("You killed the Slime!", RGBA_COLOR_ORANGE_S);
          }

          break;
        }
      }
    }
  }

  #if MOONBREATH_DEBUG
  can_move = true;
  #endif
  if(can_move)
  {
    set_occupied(v2(player.entity.x, player.entity.y), false);
    player.entity.x = player.entity.new_x;
    player.entity.y = player.entity.new_y;
    set_occupied(v2(player.entity.new_x, player.entity.new_y), true);
  }

  player.turn++;
}

internal void
render_player()
{
  update_animation(&player.entity);

  SDL_Rect src = {tile_mul(player.entity.anim.frame_current), 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect dest = {tile_mul(player.entity.x) - game.camera.x, tile_mul(player.entity.y) - game.camera.y, player.entity.w, player.entity.h};

  if(is_lit(v2(player.entity.x, player.entity.y)))
  {
    SDL_RenderCopy(game.renderer, assets.textures[player_sprite_sheet_tex], &src, &dest);
  }

  // NOTE(Rami):
  // Will probably want to move this to item.c, and have some kind of x, y unique offsets for each item so that we know how much and were to offset it when we draw it on the player

  // sword one
  i32 sword_one = 0;
  SDL_Rect sword_one_dest = {tile_mul(player.entity.x) - game.camera.x + 0, tile_mul(player.entity.y) - game.camera.y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  i32 sword_two = 0;
  SDL_Rect sword_two_dest = {tile_mul(player.entity.x) - game.camera.x + 11, tile_mul(player.entity.y) - game.camera.y - 3, player.entity.w, player.entity.h};

  // source for the item texture
  SDL_Rect item_src;
  item_src.y = 0;
  item_src.w = TILE_SIZE;
  item_src.h = TILE_SIZE;
  
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // if equipped
    if(items[i].is_equipped)
    {
      // if an iron sword
      if(items[i].item_id == id_iron_sword)
      {
        // if hasn't been rendered before
        if(!sword_one)
        {
          sword_one = 1;

          // get the correct x-axis position for the item tile
          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          // render it
          SDL_RenderCopy(game.renderer, assets.textures[item_tileset_tex], &item_src, &sword_one_dest);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          SDL_RenderCopy(game.renderer, assets.textures[item_tileset_tex], &item_src, &sword_two_dest);
        }
      }
    }
  }
}