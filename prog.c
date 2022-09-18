#include <ncurses.h>
#include "cube.h"
#include "cview.h"

void
setup_ncurses(void)
{
	initscr();
	cbreak();
	noecho();
	start_color();
	curs_set(0);
	keypad(stdscr, 1);
	use_default_colors();
}

int
main(void)
{
	CubeView *cview;
	const char *move;
	int ch;

	setup_ncurses();
	cview = cube_view_create();

	while (1) {
		refresh();
		cube_view_draw(cview);
		if ((ch = getch()) == KEY_F(1)) break;
		switch (ch) {
		case 'j': move = "U "; break;
		case 'f': move = "U'"; break;
		case 'k': move = "R "; break;
		case 'i': move = "R'"; break;
		case 'd': move = "L "; break;
		case 'e': move = "L'"; break;
		case 's': move = "D "; break;
		case 'l': move = "D'"; break;
		case 'h': move = "F "; break;
		case 'g': move = "F'"; break;
		case KEY_RESIZE: cube_view_reset(cview); clear(); continue;
		default: continue;
		}
		cube_apply_move(cview->cube, str2move(move, NULL));
	};

	cube_view_destroy(cview);
	endwin();
}
