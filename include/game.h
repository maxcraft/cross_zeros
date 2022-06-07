#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "field.h"

enum game_difficulty
{
  GAME_DIFFICULTY_INVALID = -1,
  GAME_DIFFICULTY_NOOB,
  GAME_DIFFICULTY_EASY,
  GAME_DIFFICULTY_NORMAL,
  GAME_DIFFICULTY_HARD
};

typedef struct game game_t;

game_t *game_create();
void game_init( game_t *game, enum game_difficulty difficulty );
void game_restart( game_t *game );
void game_destroy( game_t *game );
bool game_is_user_move( game_t *game );
bool game_is_over( game_t *game, enum cell_status *status );
bool game_user_move( game_t *game, uint8_t cell_index );
field_t *game_get_field( game_t *game );
