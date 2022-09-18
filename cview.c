#include <ncurses.h>
#include <stdlib.h>
#include "cube.h"
#include "cview.h"

#define STICKER_X 4
#define STICKER_Y 2

#define FACE_STICKER_COUNT (STICKERS + (3 * 2))

#define nelem(x) (sizeof(x) / sizeof(*x))

static void cube_view_init(CubeView *view);
static void cube_view_del(CubeView *view);

void
cube_view_init(CubeView *view)
{
	int x, y, i;

	(void)y;
	getmaxyx(stdscr, y, x);

	view->size_x =      5 * STICKER_X + 6;
	view->size_y = 2 * (3 * STICKER_Y + 3);
	view->y      = (x - view->size_x) / 2;
	view->x      = 0;

	view->board = newwin(view->size_y, view->size_x, view->x, view->y);

	box(view->board, 0, 0);

	for (i = 0; i < FACE_STICKER_COUNT * 2; i++) {
		view->stickers[i] = derwin(view->board, STICKER_Y, STICKER_X,
		                    (i / 5) * (STICKER_Y + 1) + (i < FACE_STICKER_COUNT ? 1 : 0),
		                    (i % 5) * (STICKER_X + 1) + 1);
	}

	for (i = 0; i < 5; i++)
		mvwvline(view->board, 1, i * (STICKER_X + 1), ACS_VLINE, view->size_y - 2);

	for (i = 0; i < 4; i++)
		mvwhline(view->board, (i + 1) * (STICKER_Y + 1) + (i>1?STICKER_Y:0),
		         1, ACS_HLINE, view->size_x - 2);

	for (i = 0; i < 16; i++) {
		mvwaddch(view->board, (i / 4 + 1) * (STICKER_Y + 1) + (i>7?STICKER_Y:0),
		                      (i % 4 + 1) * (STICKER_X + 1), ACS_PLUS);
	}
}

void
cube_view_del(CubeView *view)
{
	int i;

	for (i = 0; i < (int)nelem(view->stickers); i++) {
		delwin(view->stickers[i]);
	}
	delwin(view->board);
}

void
cube_view_reset(CubeView *view)
{
	cube_view_del(view);
	cube_view_init(view);
}

CubeView *
cube_view_create(void)
{
	CubeView *view;
	int i;

	view = malloc(sizeof(CubeView));
	cube_view_init(view);
	view->cube = cube_create();

	for (i = 0; i < FACES; i++)
		init_pair(i + 1, colorscheme[i],  colorscheme[i]);

	return view;
}

void
cube_view_destroy(CubeView *view)
{
	cube_view_del(view);
	cube_destroy(view->cube);
	free(view);
}

void
cube_view_draw(CubeView *view)
{
	int i;

	wrefresh(view->board);
	for (i = 0; i < FACE_STICKER_COUNT * 2; i++) {
		wbkgd(view->stickers[i], COLOR_PAIR(cube_get2(view->cube, CUBE_VIEW[i]) + 1));
		wrefresh(view->stickers[i]);
	}
}
