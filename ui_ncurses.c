#include <curses.h>
#include "include/ui_ncurses.h"

typedef struct ui_ncurses
{
  ui_t base;
  WINDOW *legend;
  WINDOW *field;
  WINDOW *end_game_banner;
  WINDOW *prompt;
}ui_ncurses_t;

static inline void print_cell( WINDOW *win, enum cell_status status, int cell_index )
{
	switch( status )
	{
		case CELL_CPU:
			wprintw( win, /*BRIGHT_WHITE BG_BRIGHT_BLACK*/ "X" /*RESET_COLOR*/ );
			break;

		case CELL_USER:
			wprintw( win, /*BRIGHT_BLACK BG_BRIGHT_GREEN*/ "O" /*RESET_COLOR*/);
			break;

		default:
			wprintw( win, "%d", cell_index );
			break;
	}

}

static void print_field( ui_t *in_ui, game_t *game )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	field_t *field = game_get_field( game );

	for( uint8_t i = MIN_CELL; i <= MAX_CELL; ++i )
	{
		if( ( i % 3 == 0 ) )
		{
			wmove( ui->field, i / 3, 0 );
		}

		enum cell_status status = field_get_cell_status( field, i );
		print_cell( ui->field, status, i );
	}

	// printf("\n" );
  wrefresh( ui->field );
}

static void print_winner( ui_t *in_ui, enum cell_status winner )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	printw( /*BG_BRIGHT_RED*/ "Game is over!"/* RESET_COLOR*/ );

	switch( winner )
	{
		case CELL_CPU:
			printw( /*BG_BRIGHT_RED*/ "CPU WON!" /*RESET_COLOR*/ );
			break;

		case CELL_USER:
			printw( /*BRIGHT_RED */"PLAYER WON!"/* RESET_COLOR*/ );
			break;

		case CELL_INVALID:
		default:
			printw( /*BRIGHT_WHITE*/ "DRAW!" /*RESET_COLOR*/ );
			break;
	}

  refresh();
}

static void print_legend( ui_t *in_ui )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

  wmove( ui->legend, 0, 4 );
  wprintw( ui->legend, "legend" );

  wmove( ui->legend, 1, 2 );
	print_cell( ui->legend, CELL_USER, 0 );
	wprintw( ui->legend, " - player" );

  wmove( ui->legend, 2, 2 );
	print_cell( ui->legend, CELL_CPU, 0 );
	wprintw( ui->legend, " - cpu" );

  wmove( ui->legend, 3, 2 );
	print_cell( ui->legend, CELL_EMPTY, 0 );
	wprintw( ui->legend, " - empty cell" );

  wrefresh( ui->legend );
}

static int read_cell_index( ui_t *in_ui )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

  // getmaxyx( , )
  move( 10, 0 );
	printw( "Input cell index: " );
  refresh();
	int s = 0;

	do
	{
		s = getch(); // "%d", &index );

		if( EOF == s )
		{
			return -1;
		}
		else if( 0 == s )
		{
			while( getch() != '\n');
		}
	}while( '0' > s && '8' < s );

	return s - '0';
}

static char read_play_again( ui_t *in_ui )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	char answer;
	printw( "Play again [y/n]? " );
  refresh();

	do
	{
		int s = getch();//"%c", &answer);
		if( EOF == s )
		{
			return -1;
		}
		else if( 0 == s )
		{
			while( getch() != '\n' );
		}
	}
	while( 'y' != answer && 'n' != answer );

	return answer;
}

static void print_invalid_index( ui_t *in_ui, uint8_t index )
{
  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	printw("Invalid cell index " /*BRIGHT_WHITE*/ "%d" /*RESET_COLOR*/ "\n", index );
  refresh();
}

static void init( ui_t *in_ui )
{
  //TODO: set color pairs

  ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

  initscr();

  ui->legend = newwin( 5, 100, 0, 0 );
  ui->field = newwin( 3, 3, 6, 0 );
  refresh();

  box( ui->legend, 0, 0 );
  wrefresh( ui->legend );
}

static void stop( ui_t *ui )
{
  endwin();
}

static ui_vtable_t ui_vtable_ncurses = {
	.init = init,
	.stop = stop,
  .print_field = print_field,
  .print_winner = print_winner,
  .print_legend = print_legend,
  .read_cell_index = read_cell_index,
  .read_play_again = read_play_again,
  .print_invalid_index = print_invalid_index

};

ui_ncurses_t ui_ncurses = { .base = { .vtable = &ui_vtable_ncurses },
                            .legend = NULL,
                            .field = NULL,
                            .end_game_banner = NULL,
                            .prompt = NULL };
