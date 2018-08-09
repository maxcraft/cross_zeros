#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

#include "include/color.h"
#include "include/field.h"
#include "include/game.h"
#include "include/ui.h"
#include "include/ui_old.h"

#include <sys/random.h>

#define NOOB_STRING "noob"
#define EASY_STRING "easy"
#define NORMAL_STRING "normal"
#define HARD_STRING "hard"

static enum game_difficulty parse_difficulty( const char *optarg )
{
	enum game_difficulty difficulty = GAME_DIFFICULTY_INVALID;

	if( 0 == strcasecmp( NOOB_STRING, optarg ) )
	{
		difficulty = GAME_DIFFICULTY_NOOB;
	}
	else if( 0 == strcasecmp( EASY_STRING, optarg ) )
	{
		difficulty = GAME_DIFFICULTY_EASY;
	}
	else if( 0 == strcasecmp( NORMAL_STRING, optarg ) )
	{
		difficulty = GAME_DIFFICULTY_NORMAL;
	}
	else if( 0 == strcasecmp( HARD_STRING, optarg ) )
	{
		difficulty = GAME_DIFFICULTY_HARD;
	}

	return difficulty;
}

static void print_usage()
{
	puts( "Usage:" );
	puts( "cross_zeros [options] " );
	puts( "Where " BRIGHT_WHITE "options" RESET_COLOR " are:" );
	puts( "\t-d " BRIGHT_WHITE "DIFFICULTY" RESET_COLOR "\tsets one of the difficulty levels:" );
	puts( "\t\t\t\t" BRIGHT_WHITE NOOB_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE EASY_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE NORMAL_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE HARD_STRING RESET_COLOR );
	puts( "\t-h\t\tprints this usage.");
	exit( 0 );
}

int main( int argc, char** argv )
{
	enum game_difficulty difficulty = GAME_DIFFICULTY_HARD;

	int c;
	while( -1 != ( c = getopt( argc, argv, "hd:" ) ) )
	{
		switch( c )
		{
			case 'd':
				if( GAME_DIFFICULTY_INVALID == ( difficulty = parse_difficulty( optarg ) ) )
				{
					printf( "Unknown difficulty '%s'\n", optarg );
					print_usage();
				}
				break;

			case 'h':
			default:
				print_usage();
		}
	}

	int rnd_seed = 0;

	ssize_t rnd_count;

	do
	{
		rnd_count = getrandom( &rnd_seed, sizeof( rnd_seed ), 0 );
	} while( rnd_count < 1 );

	srandom( rnd_seed );
	ui_t *ui = &ui_old;
	game_t *game = game_create();
	game_init( game, difficulty );

	ui->vtable->print_field( game );
	char answer;
	enum cell_status winner = CELL_INVALID;

	do {
		ui->vtable->print_legend();
		while( !game_is_over( game, &winner ) )
		{
			// puts( "Game is not over\n" );

			if( game_is_user_move( game ) && !game_is_over( game, &winner ) )
			{
				ui->vtable->print_field( game );

				int index = ui->vtable->read_cell_index();


				if( !game_user_move( game, index ) )
				{
					ui->vtable->print_invalid_index( index );
				}
				// print_field( game );
			}
		}

		ui->vtable->print_winner( winner );

		ui->vtable->print_field( game );

		answer = ui->vtable->read_play_again();

		game_restart( game );
	} while( answer != 'n');

	game_destroy( game );
	return 0;
}
