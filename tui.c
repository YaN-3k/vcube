#include <stdlib.h>
#include <ncurses.h>
#include "cube.h"
#include "cview.h"
#include "tui.h"

struct TUI {
	CubeView *cview;
	WINDOW *help;
};

static const char *help_txt[] = {
	"F1: quit",
	"Space: stop/start timer",
	"Enter: new scramble",
};

static void
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

TUI *
tui_create(Cube *cube)
{
	TUI *tui;

	setup_ncurses();
	tui = malloc(sizeof(TUI));
	tui->cview = cview_create(cube);
	return tui;
}

void
tui_reset(TUI *tui)
{
	werase(stdscr);
	cview_reset(tui->cview);

}

void
tui_destroy(TUI *tui)
{
	cview_destroy(tui->cview);
	free(tui);
	endwin();
}

void
tui_draw(TUI *tui) {
	cview_draw(tui->cview);
}
