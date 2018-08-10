#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <signal.h>

#include "include/color.h"
#include "include/field.h"
#include "include/game.h"
#include "include/ui.h"
#include "include/ui_old.h"
#include "include/ui_ncurses.h"

#if defined __GLIBC__ && defined __linux__
# if __GLIBC__ > 2 || __GLIBC_MINOR__ > 24
#  include <sys/random.h>
# else
# include <sys/syscall.h>
# include <linux/random.h>
# define getrandom( buf, buf_len, flags ) syscall( SYS_getrandom, buf, buf_len, flags )
# endif
#else
# include <sys/random.h>
#endif

#include <getopt.h>

#define NOOB_STRING "noob"
#define EASY_STRING "easy"
#define NORMAL_STRING "normal"
#define HARD_STRING "hard"

#define UI_NAME_OLD "old"
#define UI_NAME_NCURSES "ncurses"

#define OPTION_UI_OLD 0x100
#define OPTION_UI_NCURSES 0x101

static ui_t *ui;

static void sig_handler( int signal )
{
	switch( signal )
	{
		case SIGINT:
			ui->vtable->stop( ui );
			exit( 0 );
			break;

		default:
			return;
	}
}

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
	puts( "\t-d  " BRIGHT_WHITE "DIFFICULTY" RESET_COLOR ",\tsets one of the difficulty levels:" );
	puts( "\t--difficulty=" BRIGHT_WHITE "DIFFICULTY" RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE NOOB_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE EASY_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE NORMAL_STRING RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE HARD_STRING RESET_COLOR );
	puts( "\t--" NOOB_STRING "\t\tthe same as " BRIGHT_WHITE "-d " NOOB_STRING RESET_COLOR);
	puts( "\t--" EASY_STRING "\t\tthe same as " BRIGHT_WHITE "-d " EASY_STRING RESET_COLOR );
	puts( "\t--" NORMAL_STRING "\tthe same as " BRIGHT_WHITE "-d " NORMAL_STRING RESET_COLOR );
	puts( "\t--" HARD_STRING "\t\tthe same as " BRIGHT_WHITE "-d " HARD_STRING RESET_COLOR );

	puts( "\t-i " BRIGHT_WHITE "INTERFACE" RESET_COLOR ",\tspecify one of the interfaces:" );
	puts( "\t--ui=" BRIGHT_WHITE "INTERFACE" RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE UI_NAME_OLD RESET_COLOR );
	puts( "\t\t\t\t" BRIGHT_WHITE UI_NAME_NCURSES RESET_COLOR );
	puts( "\t--" UI_NAME_OLD );
	puts( "\t--" UI_NAME_NCURSES );

	puts( "\t-h,\t\tprints this usage.");
	puts( "\t--help" );
	exit( 0 );
}

static enum game_difficulty difficulty = GAME_DIFFICULTY_HARD;

static const struct option longopts[] = {
	{
		.name = "difficulty",
		.has_arg = required_argument,
		.flag = NULL,
		.val = 'd'
	},
	{
		.name = NOOB_STRING,
		.has_arg = no_argument,
		.flag = &difficulty,
		.val = GAME_DIFFICULTY_NOOB
	},
	{
		.name = EASY_STRING,
		.has_arg = no_argument,
		.flag = &difficulty,
		.val = GAME_DIFFICULTY_EASY
	},
	{
		.name = NORMAL_STRING,
		.has_arg = no_argument,
		.flag = &difficulty,
		.val = GAME_DIFFICULTY_NORMAL
	},
	{
		.name = HARD_STRING,
		.has_arg = no_argument,
		.flag = &difficulty,
		.val = GAME_DIFFICULTY_HARD
	},
	{
		.name = "ui",
		.has_arg = required_argument,
		.flag = NULL,
		.val = 'i'
	},
	{
		.name = UI_NAME_OLD,
		.has_arg = no_argument,
		.flag = NULL,
		.val = OPTION_UI_OLD
	},
	{
		.name = UI_NAME_NCURSES,
		.has_arg = no_argument,
		.flag = NULL,
		.val = OPTION_UI_NCURSES
	},
	{
		.name = "help",
		.has_arg = no_argument,
		.flag = NULL,
		.val = 'h'
	},
	{0}
};

int main( int argc, char** argv )
{
	ui = &ui_old;

	int c, opt_ind;
	while( -1 != ( c = getopt_long( argc, argv, "hd:", longopts, &opt_ind ) ) )
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

			case OPTION_UI_OLD:
				ui = &ui_old;
				break;

			case OPTION_UI_NCURSES:
				ui = &ui_ncurses;
				break;

			case 0:
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

	ui->vtable->init( ui );

	signal( SIGINT, sig_handler );

	game_t *game = game_create();
	game_init( game, difficulty );

	ui->vtable->print_field( ui, game );
	char answer;
	enum cell_status winner = CELL_INVALID;

	do {
		ui->vtable->print_legend( ui );
		while( !game_is_over( game, &winner ) )
		{
			// puts( "Game is not over\n" );

			if( game_is_user_move( game ) && !game_is_over( game, &winner ) )
			{
				ui->vtable->print_field( ui, game );

				int index = ui->vtable->read_cell_index( ui );


				if( !game_user_move( game, index ) )
				{
					ui->vtable->print_invalid_index( ui, index );
				}
				// print_field( game );
			}
		}

		ui->vtable->print_winner( ui, winner );

		ui->vtable->print_field( ui, game );

		answer = ui->vtable->read_play_again( ui );

		game_restart( game );
	} while( answer != 'n');

	ui->vtable->stop( ui );
	game_destroy( game );
	return 0;
}
