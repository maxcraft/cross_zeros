#include "include/game.h"
#include <stdlib.h>
#include <stdio.h>
#include "include/field.h"
#include "include/log.h"

#define field_for_each_line   for( uint8_t i = 0, count = field_get_lines_count(); i < count; ++ i )


typedef struct vtable
{
  void (*init)( game_t *game );
  bool (*cpu_move)( game_t *game );
}vtable_t;

struct game
{
  field_t *field;
  vtable_t *vtable;
  bool is_user_move;
};

static bool fill_center( game_t *game )
{
  if( CELL_EMPTY == field_get_cell_status( game->field, field_get_central_index() ) )
  {
    dprintf( "center is empty\n" );
    field_make_step( game->field, NOT_USER, field_get_central_index() );
    return true;
  }

  dprintf( "center is filled\n" );

  return false;
}

static void close_line_in_danger( game_t *game, uint8_t line_index )
{
  dprintf( "closing line %d\n", line_index );
  field_close_line_with_cpu( game->field, line_index );
}

static void init_noob( game_t *game )
{
  game->is_user_move = true;
}

static void init_hard( game_t *game )
{
  game->is_user_move = false;
}

static bool cpu_move_noob( game_t *game )
{
  dprintf("\n" );
  // first move odd.
  field_for_each_line
  {
    enum cell_status status = field_is_line_in_danger( game->field, i );
    if( CELL_USER == status || CELL_CPU == status )
    {
      close_line_in_danger( game, i );
      return true;
    }
  }

  if( !field_fill_random_odd_cell( game->field ) )
  {
    return ( field_fill_random_even_cell( game->field ) );
  }

  return true;

}

static bool cpu_move_easy( game_t *game )
{
  dprintf("\n" );
  // first move even.
  field_for_each_line
  {
    enum cell_status status = field_is_line_in_danger( game->field, i );

    if( CELL_USER == status || CELL_CPU == status )
    {
      close_line_in_danger( game, i );
      return true;
    }
  }

  if( !field_fill_random_even_cell( game->field ) )
  {
    return ( field_fill_random_odd_cell( game->field ) );
  }

  return true;
}

static bool cpu_move_hard( game_t *game )
{
  dprintf("\n" );

  uint8_t lines [ field_get_lines_count() ];
  uint8_t line_count = 0;

  // first move even.
  field_for_each_line
  {
    enum cell_status status = field_is_line_in_danger( game->field, i );

    if( CELL_CPU == status )
    {
      close_line_in_danger( game, i );
      return true;
    }
    else if( CELL_USER == status )
    {
      lines[ line_count++ ] = i;
    }
  }

  if( line_count > 0 )
  {
    close_line_in_danger( game, lines[ 0 ] );
    return true;
  }

  if( fill_center( game ) )
  {
    return true;
  }
  else
  {
    dprintf( "do random step\n" );

    if( !field_fill_random_even_cell_avoid_user( game->field ) )
    {
      if( field_fill_random_odd_cell( game->field ) )
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    return true;
  }

  return false;
}

vtable_t vtable_noob = { .init = init_noob, .cpu_move = cpu_move_noob };
vtable_t vtable_easy = { .init = init_noob, .cpu_move = cpu_move_easy };
vtable_t vtable_normal = { .init = init_noob, .cpu_move = cpu_move_hard };
vtable_t vtable_hard = { .init = init_hard, .cpu_move = cpu_move_hard };

game_t *game_create()
{
  return malloc( sizeof( game_t ) );
}

void game_init( game_t *game, enum game_difficulty difficulty )
{
  game->field = field_create();
  field_init( game->field );

  switch( difficulty )
  {
    case GAME_DIFFICULTY_NOOB:
      game->vtable = &vtable_noob;
      break;

    case GAME_DIFFICULTY_EASY:
      game->vtable = &vtable_easy;
      break;

    case GAME_DIFFICULTY_NORMAL:
      game->vtable = &vtable_normal;
      break;

    case GAME_DIFFICULTY_HARD:
      game->vtable = &vtable_hard;
      break;

    default:
      game->vtable = &vtable_noob;
      break;
  }

  game->vtable->init( game );
}

void game_destroy( game_t *game )
{
  field_destroy( game->field );
  free( game );
}

bool game_is_user_move( game_t *game )
{
  if( !game->is_user_move )
  {
    game->vtable->cpu_move( game );
    game->is_user_move = true;
  }
  else
  {
    dprintf( "Game still in user mode" );
  }

  return true;
}

bool game_is_over( game_t *game, enum cell_status *status )
{
  enum cell_status stat = CELL_INVALID;
  // any line complete
  for( uint8_t i = 0, count = field_get_lines_count(); i < count; ++ i )
  {
    stat = field_is_line_complete( game->field, i );

    if( CELL_EMPTY != stat )
    {
      // dprintf( "someone won %d\n", field_is_line_complete( game->field, i ) );
      *status = stat;
      return true;
    }
  }

  // all cells full
  if( field_are_all_cells_marked( game->field ) )
  {
    dprintf( "Cells are marked all\n" );
    *status = CELL_INVALID;
    return true;
  }


  return false;
}

bool game_user_move( game_t *game, uint8_t cell_index )
{
  if( field_make_step( game->field, USER, cell_index ) )
  {
    game->is_user_move = false;
    return true;
  }

  return false;
}

field_t *game_get_field( game_t *game )
{
  return game->field;
}

void game_restart( game_t *game )
{
  field_init( game->field );
  game->vtable->init( game );
}
