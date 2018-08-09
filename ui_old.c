#include <stdio.h>

#include "include/ui_old.h"
#include "include/color.h"

static inline void print_cell_term( enum cell_status status, int cell_index )
{
	switch( status )
	{
		case CELL_CPU:
			printf( BRIGHT_WHITE BG_BRIGHT_BLACK "X" RESET_COLOR );
			break;

		case CELL_USER:
			printf( BRIGHT_BLACK BG_BRIGHT_GREEN "O" RESET_COLOR);
			break;

		default:
			printf( "%d", cell_index );
			break;
	}

}

static void print_field_term( game_t *game )
{
	field_t *field = game_get_field( game );

	for( uint8_t i = MIN_CELL; i <= MAX_CELL; ++i )
	{
		if( ( i % 3 == 0 ) )
		{
			printf("\n" );
		}

		enum cell_status status = field_get_cell_status( field, i );
		print_cell_term( status, i );
	}

	printf("\n" );
}

static void print_winner_term( enum cell_status winner )
{
	puts( BG_BRIGHT_RED "Game is over!" RESET_COLOR );

	switch( winner )
	{
		case CELL_CPU:
			puts( BG_BRIGHT_RED "CPU WON!" RESET_COLOR );
			break;

		case CELL_USER:
			puts( BRIGHT_RED "PLAYER WON!" RESET_COLOR );
			break;

		case CELL_INVALID:
		default:
			puts( BRIGHT_WHITE "DRAW!" RESET_COLOR );
			break;
	}
}

static void print_legend_term()
{
	print_cell_term( CELL_USER, 0 );
	puts( " - player" );
	print_cell_term( CELL_CPU, 0 );
	puts( " - cpu" );
	print_cell_term( CELL_EMPTY, 0 );
	puts( " - empty cell" );
}

static int read_cell_index_term()
{
	int index = 0;
	puts( "Input cell index: " );
	int s = 0;

	do
	{
		s = scanf( "%d", &index );

		if( EOF == s )
		{
			return -1;
		}
		else if( 0 == s )
		{
			while (fgetc(stdin) != '\n');
		}
	}while( 1 != s );

	return index;
}

static char read_play_again_term()
{
	char answer;
	printf( "Play again [y/n]? " );

	do
	{
		int s = scanf("%c", &answer);
		if( EOF == s )
		{
			return -1;
		}
		else if( 0 == s )
		{
			while (fgetc(stdin) != '\n');
		}
	}
	while( 'y' != answer && 'n' != answer );

	return answer;
}

static void print_invalid_index_term( uint8_t index )
{
	printf("Invalid cell index " BRIGHT_WHITE "%d" RESET_COLOR "\n", index );
}

static ui_vtable_t ui_vtable_old = {
  .print_field = print_field_term,
  .print_winner = print_winner_term,
  .print_legend = print_legend_term,
  .read_cell_index = read_cell_index_term,
  .read_play_again = read_play_again_term,
  .print_invalid_index = print_invalid_index_term

};

ui_t ui_old = { .vtable = &ui_vtable_old };
