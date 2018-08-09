#pragma once
#include "game.h"
#include "field.h"

typedef struct ui_vtable
{
  void ( *print_field )( game_t *game );
  void ( *print_winner )( enum cell_status winner );
  void ( *print_legend )();
  int ( *read_cell_index ) ();
  char ( *read_play_again )();
  void ( *print_invalid_index )( uint8_t index );

}ui_vtable_t;

typedef struct ui
{
  ui_vtable_t *vtable;
}ui_t;
