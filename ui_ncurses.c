#include <curses.h>
#include "include/ui_ncurses.h"

#define COLOR_INDEX_DEFAULT 1
#define COLOR_INDEX_USER 2
#define COLOR_INDEX_CPU 3
#define COLOR_INDEX_EMPHASIZE 4
#define COLOR_INDEX_ERROR 5
#define COLOR_INDEX_ERROR_BORDER 6

#define FIELD_LENGTH 7

typedef struct ui_ncurses
{
	ui_t base;
	WINDOW *legend;
	WINDOW *field;
	WINDOW *end_game_banner;
	WINDOW *prompt;
	bool error_mode;
	const char *status;
}ui_ncurses_t;

static inline void print_cell( WINDOW *win, enum cell_status status, int cell_index )
{
	switch( status )
	{
		case CELL_CPU:
			wattron( win, COLOR_PAIR( COLOR_INDEX_CPU ) );
			wprintw( win, "X" );
			wattroff( win, COLOR_PAIR( COLOR_INDEX_CPU ) );
			break;

		case CELL_USER:
			wattron( win, COLOR_PAIR( COLOR_INDEX_USER ) );
			wattron( win, A_UNDERLINE );
			wprintw( win, "O" );
			wattroff( win, A_UNDERLINE );
			wattroff( win, COLOR_PAIR( COLOR_INDEX_USER ) );
			break;

		default:
			wprintw( win, "%d", cell_index );
			break;
	}

}

static void print_edge_grid( WINDOW *win, int y, int tee )
{
	mvwaddch( win, y, 2, tee );
	mvwaddch( win, y, 4, tee );
}

static void print_central_grid( WINDOW *win, int y )
{
	wmove( win, y, 0 );
	waddch( win, ACS_LTEE );
	waddch( win, ACS_HLINE );
	waddch( win, ACS_PLUS );
	waddch( win, ACS_HLINE );
	waddch( win, ACS_PLUS );
	waddch( win, ACS_HLINE );
	waddch( win, ACS_RTEE );
}

static void print_field( ui_t *in_ui, game_t *game )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;
	
	field_t *field = game_get_field( game );

	box( ui->field, 0, 0 );

	print_edge_grid( ui->field, 0, ACS_TTEE );
	print_edge_grid( ui->field, 6, ACS_BTEE );

	print_central_grid( ui->field, 2 );
	print_central_grid( ui->field, 4 );

	for( uint8_t i = MIN_CELL; i <= MAX_CELL; ++i )
	{
		if( ( i % 3 == 0 ) )
		{
			wmove( ui->field, 1 + i / 3 + i / 3, 1 );
		}

		enum cell_status status = field_get_cell_status( field, i );
		print_cell( ui->field, status, i );
		waddch( ui->field, ACS_VLINE );
	}

	wrefresh( ui->field );
}

static void print_winner( ui_t *in_ui, enum cell_status winner )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	move( 15, 0 );

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

static const char *select_cell_status = "Input cell index: ";
static const char *play_again_status = "Play again [y/n]? "; 

static void print_status( ui_ncurses_t *ui )
{
	if( !ui->error_mode )
	{
		wclear( ui->prompt );
		box( ui->prompt, 0, 0 );
		mvwprintw( ui->prompt, 1, 1, ui->status );
		wrefresh( ui->prompt );
	}
}

static void resize_windows( ui_ncurses_t *ui )
{
	int maxx, maxy;
	getmaxyx( stdscr, maxy, maxx );

	clear();

	wresize( ui->legend, 5, maxx );
	wresize( ui->field, FIELD_LENGTH, FIELD_LENGTH );

	mvwin( ui->field, 6, ( maxx - FIELD_LENGTH ) / 2 );

	wresize( ui->prompt, 3, maxx );
	mvwin( ui->prompt, maxy - 3, 0 );

	refresh();

	wclear( ui->legend );
	box( ui->legend, 0, 0 );
	ui->base.vtable->print_legend( &ui->base );

	//ui->base.vtable->print_field( ui->base );
	wrefresh( ui->field );

	ui->error_mode = false;
	//wrefresh( ui->prompt );
	print_status( ui );

	refresh();
}

static int read_cell_index( ui_t *in_ui )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	ui->status = select_cell_status;
	print_status( ui );

	int s = 0;

	do
	{
		s = getch();
		
		if( KEY_RESIZE == s )
		{
			resize_windows( ui );
		}

	}while( '0' > s && '8' < s );

	ui->error_mode = false;

	return s - '0';
}

static char read_play_again( ui_t *in_ui )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	ui->status = play_again_status;
	print_status( ui );

	int s = 0;

	do
	{
		s = getch();
			
		if( KEY_RESIZE == s )
		{
			resize_windows( ui );
		}

	} while( 'y' != s && 'n' != s );

	return s;
}

static void print_invalid_index( ui_t *in_ui, uint8_t index )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;
	ui->error_mode = true;

	wclear( ui->prompt );

	wattron( ui->prompt, COLOR_PAIR( COLOR_INDEX_ERROR_BORDER ) );
	box( ui->prompt, 0, 0 );
	wattroff( ui->prompt, COLOR_PAIR( COLOR_INDEX_ERROR_BORDER ) );

	wattron( ui->prompt, COLOR_PAIR( COLOR_INDEX_ERROR ) );
	mvwprintw(ui->prompt, 1, 1, "Invalid cell index " );
	wattron( ui->prompt, A_BOLD );
	wprintw( ui->prompt, "%d", index );
	wattroff( ui->prompt, A_BOLD );
	wattroff( ui->prompt, COLOR_PAIR( COLOR_INDEX_ERROR ) );
	wrefresh( ui->prompt );
}

static void init( ui_t *in_ui )
{
	ui_ncurses_t *ui = ( ui_ncurses_t * )in_ui;

	initscr();
	cbreak();
	noecho();

	if( has_colors )
	{
		start_color();

		init_pair( COLOR_INDEX_DEFAULT, COLOR_WHITE, COLOR_BLACK );
		init_pair( COLOR_INDEX_USER, COLOR_WHITE, COLOR_BLUE );
		init_pair( COLOR_INDEX_CPU, COLOR_BLACK, COLOR_GREEN );
		init_pair( COLOR_INDEX_EMPHASIZE, COLOR_WHITE, COLOR_BLACK );
		init_pair( COLOR_INDEX_ERROR, COLOR_WHITE, COLOR_RED );
		init_pair( COLOR_INDEX_ERROR_BORDER, COLOR_RED, COLOR_BLACK );
	}

	int maxx, maxy;

	getmaxyx( stdscr, maxy, maxx );

	ui->legend = newwin( 5, maxx, 0, 0 );
	ui->field = newwin( FIELD_LENGTH, FIELD_LENGTH, 6, ( maxx - FIELD_LENGTH ) / 2 );
	ui->prompt = newwin( 3, maxx, maxy - 3, 0 );
	refresh();

	box( ui->legend, 0, 0 );
	wrefresh( ui->legend );

	box( ui->prompt, 0, 0 );
	wrefresh( ui->prompt );
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
                            .prompt = NULL,
                            .error_mode = false,
                            .status = NULL };

