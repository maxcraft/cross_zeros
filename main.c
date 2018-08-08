#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

#include "include/color.h"
#include "include/field.h"
#include "include/game.h"

#define NOOB_STRING "noob"
#define EASY_STRING "easy"
#define NORMAL_STRING "normal"
#define HARD_STRING "hard"

static inline void print_cell( enum cell_status status, int cell_index )
{
	switch( status )
	{
		case CELL_CPU:
			// printf( BRIGHT_YELLOW_BOLD "X" RESET_COLOR );
			printf( BRIGHT_WHITE BG_BRIGHT_BLACK "X" RESET_COLOR );
			break;

		case CELL_USER:
			// printf( BRIGHT_GREEN_BOLD "O" RESET_COLOR);
			printf( BRIGHT_BLACK BG_BRIGHT_GREEN "O" RESET_COLOR);
			break;

		default:
			printf( "%d", cell_index );
			break;
	}

}

void print_field( game_t *game )
{
	field_t *field = game_get_field( game );
	// puts("\n" );

	for( uint8_t i = MIN_CELL; i <= MAX_CELL; ++i )
	{
		if( ( i % 3 == 0 ) )
		{
			printf("\n" );
		}

		enum cell_status status = field_get_cell_status( field, i );
		print_cell( status, i );
	}

	printf("\n" );
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

static void print_winner( enum cell_status winner )
{
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

int main( int argc, char** argv )
{

	// puts( WHITE BG_BLACK "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_BLACK "X" RESET_COLOR );
	// puts( WHITE BG_RED "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_RED "X" RESET_COLOR );
	// puts( WHITE BG_GREEN "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_GREEN "X" RESET_COLOR );
	// puts( WHITE BG_BLUE "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_BLUE "X" RESET_COLOR );
	// puts( WHITE BG_YELLOW "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_YELLOW "X" RESET_COLOR );
	// puts( WHITE BG_MAGENTA "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_MAGENTA "X" RESET_COLOR );
	// puts( WHITE BG_CYAN "X" RESET_COLOR );
	// puts( WHITE_BOLD BG_CYAN "X" RESET_COLOR );
	//
	// puts( BRIGHT_WHITE BG_BRIGHT_BLACK "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_BLACK "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_RED "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_RED "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_GREEN "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_GREEN "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_BLUE "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_BLUE "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_YELLOW "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_YELLOW "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_MAGENTA "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_MAGENTA "X" RESET_COLOR );
	// puts( BRIGHT_WHITE BG_BRIGHT_CYAN "X" RESET_COLOR );
	// puts( BRIGHT_WHITE_BOLD BG_BRIGHT_CYAN "X" RESET_COLOR );

	enum game_difficulty difficulty = GAME_DIFFICULTY_HARD;

	int c;
	while( -1 != ( c = getopt( argc, argv, "hd:" ) ) )
	{
		switch( c )
		{
			case 'd':
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
				else
				{
					printf( "Unknown difficulty '%s'\n", optarg );
				}
				break;

			case 'h':
			default:
				print_usage();
		}
	}

	game_t *game = game_create();
	game_init( game, difficulty );

	print_field( game );
	char answer;
	enum cell_status winner = CELL_INVALID;

	do {
		print_cell( CELL_USER, 0 );
		puts( " - player" );
		print_cell( CELL_CPU, 0 );
		puts( " - cpu" );
		print_cell( CELL_EMPTY, 0 );
		puts( " - empty cell" );

		while( !game_is_over( game, &winner ) )
		{
			// puts( "Game is not over\n" );

			if( game_is_user_move( game ) && !game_is_over( game, &winner ) )
			{
				print_field( game );

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


				if( !game_user_move( game, index ) )
				{
					printf("Invalid cell index " BRIGHT_WHITE "%d" RESET_COLOR "\n", index );
				}
				// print_field( game );
			}
		}

		puts( BG_BRIGHT_RED "Game is over!" RESET_COLOR );

		print_winner( winner );

		print_field( game );

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


		game_restart( game );
	} while( answer != 'n');

	game_destroy( game );
	return 0;
}
