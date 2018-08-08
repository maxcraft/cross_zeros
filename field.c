#include "include/field.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/log.h"

struct field
{
  uint8_t cells[ MAX_CELL + 1 ];
};

static const uint8_t lines[][3] = {
  {0,4,8}, {2,4,6}, // diagonals
  {0,3,6}, {1,4,7}, {2,5,8}, // vertical
  {0,1,2}, {3,4,5}, {6,7,8}  // horizontal
};

uint8_t field_get_lines_count()
{
  return sizeof( lines ) / 3;
}

uint8_t field_get_central_index()
{
  return 4; // TODO: add math.
}

field_t *field_create()
{
  return malloc( sizeof( field_t ) );
}

void field_init( field_t *field )
{
  memset( field->cells, CELL_EMPTY, sizeof( field->cells ) );
}

void field_destroy( field_t *field )
{
  free( field );
}

bool field_make_step( field_t *field, bool user, uint8_t cell_index )
{
  dprintf("Making %s step at %d\n", ( user ? "USER" : "CPU" ), cell_index );
  if( MIN_CELL <= cell_index
    && MAX_CELL >= cell_index
    && CELL_EMPTY == field->cells[ cell_index ] )
  {
    field->cells[ cell_index ] = ( user ) ? CELL_USER : CELL_CPU;
    return true;
  }

  return false;
}

static void count_cell_spread_in_line( field_t *field, int line_index, uint8_t *count_by_type )
{
  for( int i = 0; i < 3; ++ i )
  {
    ++count_by_type[ field->cells[ lines[ line_index ][ i ] ] ];
  }
}

enum cell_status field_is_line_in_danger( field_t *field, uint8_t line_index )
{
  uint8_t count[ 3 ] = { 0 };
  count_cell_spread_in_line( field, line_index, count );

  if( count[ CELL_EMPTY ] > 1 )
  {
    return CELL_EMPTY;
  }
  else if( count[ CELL_USER ] > 1 && count[ CELL_EMPTY ] > 0 )
  {
    return CELL_USER;
  }
  else if( count[ CELL_CPU ] > 1 && count[ CELL_EMPTY ] > 0 )
  {
    return CELL_CPU;
  }
  else
  {
    return CELL_INVALID; // That means the line is full.
  }
}

enum cell_status field_is_line_complete( field_t *field, uint8_t line_index )
{
  uint8_t count[ 3 ] = { 0 };
  count_cell_spread_in_line( field, line_index, count );

  if( 3 == count[ CELL_CPU ] )
  {
    return CELL_CPU;
  }
  else if( 3 == count[ CELL_USER ] )
  {
    return CELL_USER;
  }

  return CELL_EMPTY;
}

bool field_is_line_full( field_t *field, uint8_t line_index )
{
  uint8_t count[ 3 ] = { 0 };
  count_cell_spread_in_line( field, line_index, count );

  return ( 0 == count[ CELL_EMPTY ] );
}

enum cell_status field_get_cell_status( field_t *field, uint8_t cell_index )
{
  if( MAX_CELL >= cell_index && MIN_CELL <= cell_index )
  {
    return field->cells[ cell_index ];
  }

  return CELL_INVALID;
}

bool field_are_all_cells_marked( field_t *field )
{
  for( int i = 0; i < sizeof( field->cells ); ++i )
  {
    if( CELL_EMPTY == field->cells[ i ] )
    {
      return false;
    }
  }

  return true;
}

void field_close_line_with_cpu( field_t *field, uint8_t line_index )
{
  dprintf( "Closing line %d: %d %d %d\n", line_index,
   lines[ line_index ][ 0 ],
   lines[ line_index ][ 1 ],
   lines[ line_index ][ 2 ]
  );

  for( uint8_t i = 0; i < 3; ++i )
  {
    if( CELL_EMPTY == field->cells[ lines[ line_index ][ i ] ] )
    {
      dprintf( "cell %d in line %d is empty\n", i, line_index );
      field->cells[ lines[ line_index ][ i ] ] = CELL_CPU;
      return;
    }
  }

  dprintf( "Cannot close line %d\n", line_index );
}

bool field_fill_random_odd_cell( field_t *field )
{
  static const uint8_t odd_cells[] = {1,3,5,7};
  long int rnd = random();
  uint8_t cells[ sizeof( odd_cells ) ];
  uint8_t count = 0;

  for( uint8_t i = 0, c = 4; i < c; ++i )
  {
    if( CELL_EMPTY == field_get_cell_status( field, odd_cells[ i ] ) )
    {
      dprintf( "%d cell is empty\n", odd_cells[ i ]);
      cells[ count++ ] = odd_cells[ i ];
    }
  }

  if( count > 0 )
  {
    long int value = ( rnd % count );
    field_make_step( field, NOT_USER, cells[ value ] );
    return true;
  }
  else
  {
    return false;
  }
}

bool field_fill_random_cell_from_range( field_t *field,
  const uint8_t *in_cells,
  const uint8_t cell_count )
{
  long int rnd = random();
  uint8_t cells[ cell_count ];
  uint8_t count = 0;

  for( uint8_t i = 0, c = cell_count; i < c; ++i )
  {
    if( CELL_EMPTY == field_get_cell_status( field, in_cells[ i ] ) )
    {
      cells[ count++ ] = in_cells[ i ];
    }
  }

  if( count > 0 )
  {
    long int value = ( rnd % count );
    field_make_step( field, NOT_USER, cells[ value ] );
    return true;
  }
  else
  {
    return false;
  }

}

bool field_fill_random_even_cell( field_t *field )
{
  static const uint8_t even_cells[] = {0,2,4,6,8};
  return field_fill_random_cell_from_range( field, even_cells, sizeof( even_cells ) );
}

bool field_fill_random_even_cell_avoid_user( field_t *field )
{
  static const uint8_t cell_candidates[][2] = { { 0, 8 }, { 2, 6 } };
  uint8_t cells[ 4 ];
  uint8_t cell_count = 0;

  for( size_t i = 0, count = 2; i < count; ++i )
  {
    if( CELL_EMPTY == field_get_cell_status( field, cell_candidates[ i ][ 0 ] ) &&
        CELL_EMPTY == field_get_cell_status( field, cell_candidates[ i ][ 1 ] ) )
    {
      cells[ cell_count++ ] = cell_candidates[ i ][ 0 ];
      cells[ cell_count++ ] = cell_candidates[ i ][ 1 ];
    }
  }

  if( cell_count > 0 )
  {
    return field_fill_random_cell_from_range( field, cells, cell_count );
  }
  else
  {
    return field_fill_random_even_cell( field );
  }

}
