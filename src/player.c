void player_keypress(SDL_Scancode key)
{
  if(key == SDL_SCANCODE_ESCAPE)
  {
    game.running = false;
  }
  // NOTE(Rami): 
  else if(key == SDL_SCANCODE_P)
  {
    printf("player x: %d\n", player.entity.pos.x);
    printf("player y: %d\n\n", player.entity.pos.y);
  }
  // NOTE(Rami): 
  else if(key == SDL_SCANCODE_Q)
  {
    slimes[0].in_combat = !slimes[0].in_combat;
    // slimes[1].in_combat = !slimes[1].in_combat;
    // slimes[2].in_combat = !slimes[2].in_combat;
  }
  else if(key == SDL_SCANCODE_I)
  {
    player.inventory.is_open = !player.inventory.is_open;
    player.inventory.item_selected = 0;
  }
  else if(player.inventory.is_open)
  {
    if(key == SDL_SCANCODE_K)
    {
      { // Move inventory highlighter up
        if(player.inventory.item_count > 1)
        {
          player.inventory.item_selected--;
          if(player.inventory.item_selected < 0)
          {
            player.inventory.item_selected = player.inventory.item_count - 1;
          }
        }
      }
    }
    else if(key == SDL_SCANCODE_J)
    {
      { // Move inventory highlighter down
        if(player.inventory.item_count > 1)
        {
          player.inventory.item_selected++;
          if(player.inventory.item_selected > player.inventory.item_count - 1)
          {
            player.inventory.item_selected = 0;
          }
        }
      }
    }
    else if(key == SDL_SCANCODE_D)
    {
      item_drop();

      if(player.inventory.item_selected == player.inventory.item_count - 1)
      {
        player.inventory.item_selected--;
      }
    }
    else if(key == SDL_SCANCODE_E)
    {
      item_toggle_equip();
    }
    else if(key == SDL_SCANCODE_C)
    {
      item_consume();

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
      player.entity.new_pos = (iv2_t){player.entity.pos.x, player.entity.pos.y - 1};
    }
    else if(key == SDL_SCANCODE_J)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x, player.entity.pos.y + 1};
    }
    else if(key == SDL_SCANCODE_H)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x - 1, player.entity.pos.y};
    }
    else if(key == SDL_SCANCODE_L)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x + 1, player.entity.pos.y};
    }
    else if(key == SDL_SCANCODE_Y)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x - 1, player.entity.pos.y - 1};
    }
    else if(key == SDL_SCANCODE_U)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x + 1, player.entity.pos.y - 1};
    }
    else if(key == SDL_SCANCODE_B)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x - 1, player.entity.pos.y + 1};
    }
    else if(key == SDL_SCANCODE_N)
    {
      player.entity.new_pos = (iv2_t){player.entity.pos.x + 1, player.entity.pos.y + 1};
    }
    else if(key == SDL_SCANCODE_COMMA)
    {
      item_pickup();
    }
    else if(key == SDL_SCANCODE_D)
    {
      if(is_tile(player.entity.pos, tile_path_down))
      {
        console_message_add("You travel deeper into the mountain..", RGBA_COLOR_WHITE_S);
        gen_level();
      }
    }
    else if(key == SDL_SCANCODE_A)
    {
      if(is_tile(player.entity.pos, tile_path_up))
      {
        debug("You flee from the mountain..\n");
        game.running = false;
      }
    }
  }

  game.turn_changed = true;
}

void player_create()
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
    player.entity.move_speed = 1;
    player.entity.pos = (iv2_t){0, 0};
    player.entity.new_pos = (iv2_t){0, 0};
    player.entity.aspect.w = TILE_SIZE;
    player.entity.aspect.h = TILE_SIZE;
    player.entity.anim.frame_num = 0;
    player.entity.anim.frame_count = 4;
    player.entity.anim.frame_delay = 200;
    player.entity.anim.frame_last_changed = 0;

    for(i32 i = 0; i < INVENTORY_SLOT_COUNT; i++)
    {
      player.inventory.slots[i].item_id = id_none;
      player.inventory.slots[i].unique_id = 0;
      player.inventory.slots[i].is_on_ground = 0;
      player.inventory.slots[i].is_equipped = 0;
      player.inventory.slots[i].pos.x = 0;
      player.inventory.slots[i].pos.y = 0;
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
void player_update()
{
  b32 can_move = true;

  if(player.entity.hp <= 0)
  {
    // NOTE(Rami): Need to think about the process of the player dying more closely.
    console_message_add("Player is dead now", RGBA_COLOR_BLUE_S);
    can_move = false;
  }

  if(can_move)
  {
    if(level.tiles[(player.entity.new_pos.y * LEVEL_WIDTH_IN_TILES) + player.entity.new_pos.x] == tile_wall_stone)
    {
      console_message_add("The wall stops you from moving", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_pos.y * LEVEL_WIDTH_IN_TILES) + player.entity.new_pos.x] == tile_door_closed)
    {
      console_message_add("You lean forward and push the door open", RGBA_COLOR_WHITE_S);
      level.tiles[(player.entity.new_pos.y * LEVEL_WIDTH_IN_TILES) + player.entity.new_pos.x] = tile_door_open;
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_pos.y * LEVEL_WIDTH_IN_TILES) + player.entity.new_pos.x] == tile_path_up)
    {
      console_message_add("A path to the surface, [A]scend to flee the mountain", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
    else if(level.tiles[(player.entity.new_pos.y * LEVEL_WIDTH_IN_TILES) + player.entity.new_pos.x] == tile_path_down)
    {
      console_message_add("A path that leads further downwards.. [D]escend?", RGBA_COLOR_WHITE_S);
      can_move = false;
    }
  }

  if(can_move)
  {
    for(i32 i = 0; i < SLIME_COUNT; i++)
    {
      if(slimes[i].active)
      {
        if(iv2_equal(player.entity.new_pos, slimes[i].entity.pos))
        {
          can_move = false;

          if(!attack_entity(&player.entity, &slimes[i].entity))
          {
            console_message_add("You attack the Slime for %d damage", RGBA_COLOR_WHITE_S, player.entity.damage);
            slimes[i].in_combat = true;
          }
          else
          {
            console_message_add("You killed the Slime!", RGBA_COLOR_ORANGE_S);
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
    set_occupied(player.entity.pos, false);
    player.entity.pos = player.entity.new_pos;
    set_occupied(player.entity.new_pos, true);
  }

  player.turn++;
}

void player_render()
{
  animation_update(&player.entity);

  SDL_Rect src = {tile_mul(player.entity.anim.frame_num), 0, TILE_SIZE, TILE_SIZE};
  SDL_Rect dst = {tile_mul(player.entity.pos.x) - game.camera.x, tile_mul(player.entity.pos.y) - game.camera.y, player.entity.aspect.w, player.entity.aspect.h};

  if(is_lit(player.entity.pos))
  {
    SDL_RenderCopy(game.renderer, assets.textures[tex_player_sprite_sheet], &src, &dst);
  }

  // NOTE(Rami): Will probably want to move this to item.c, and have some kind of x, y unique offsets for each item
  // so that we know how much and were to offset it when we draw it on the player.

  // sword one
  i32 sword_one = 0;
  SDL_Rect sword_one_dst = {tile_mul(player.entity.pos.x) - game.camera.x + 0, tile_mul(player.entity.pos.y) - game.camera.y - 3, TILE_SIZE, TILE_SIZE};

  // sword two
  i32 sword_two = 0;
  SDL_Rect sword_two_dst = {tile_mul(player.entity.pos.x) - game.camera.x + 11,tile_mul(player.entity.pos.y) - game.camera.y - 3, player.entity.aspect.w, player.entity.aspect.h};

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
          SDL_RenderCopy(game.renderer, assets.textures[tex_item_tileset], &item_src, &sword_one_dst);
        }
        else if(!sword_two)
        {
          sword_two = 1;

          item_src.x = tile_mul(items_info[items[i].item_id].tile);

          SDL_RenderCopy(game.renderer, assets.textures[tex_item_tileset], &item_src, &sword_two_dst);
        }
      }
    }
  }
}