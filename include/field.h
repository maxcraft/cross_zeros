#pragma once

#include <stdbool.h>
#include <stdint.h>

#define NOT_USER false
#define USER true


/*
Field is a set of nine cells:
0|1|2
-----
3|4|5
-----
6|7|8

*/
#define MIN_CELL 0
#define MAX_CELL 8

// Line indices
#define DIAGONAL_1 0
#define DIAGONAL_2 1
#define VERTICAL_1 2
#define VERTICAL_2 3
#define VERTICAL_3 4
#define HORIZONTAL_1 5
#define HORIZONTAL_2 6
#define HORIZONTAL_3 7

enum cell_status
{
  CELL_EMPTY,
  CELL_USER,
  CELL_CPU,
  CELL_INVALID
};

typedef struct field field_t;

uint8_t field_get_lines_count();
uint8_t field_get_central_index();

field_t *field_create();
void field_init( field_t *field );
void field_destroy( field_t *field );
bool field_make_step( field_t *field, bool user, uint8_t cell_index );
enum cell_status field_is_line_in_danger( field_t *field, uint8_t line_index );
enum cell_status field_is_line_complete( field_t *field, uint8_t line_index );
bool field_is_line_full( field_t *field, uint8_t line_index );
enum cell_status field_get_cell_status( field_t *field, uint8_t cell_index );
bool field_are_all_cells_marked( field_t *field );

void field_close_line_with_cpu( field_t *field, uint8_t line_index );
bool field_fill_random_odd_cell( field_t *field );
bool field_fill_random_even_cell( field_t *field );
bool field_fill_random_even_cell_avoid_user( field_t *field );
bool field_fill_random_cell_from_range( field_t *field, const uint8_t *in_cells, const uint8_t cell_count );
