#pragma once
#include "game.h"
#include "field.h"

struct ui;
typedef struct ui ui_t;

typedef struct ui_vtable
{
  void ( *init )( ui_t *ui );
  void ( *stop )( ui_t *ui );
  void ( *print_field )( ui_t *ui, game_t *game );
  void ( *print_winner )( ui_t *ui, enum cell_status winner );
  void ( *print_legend )( ui_t *ui );
  int ( *read_cell_index )( ui_t *ui );
  char ( *read_play_again )( ui_t *ui );
  void ( *print_invalid_index )( ui_t *ui, uint8_t index );

}ui_vtable_t;

struct ui
{
  ui_vtable_t *vtable;
};
