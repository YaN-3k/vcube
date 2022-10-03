#include <ncurses.h>
#include <stdlib.h>
#include "cube.h"
#include "cview.h"

struct CubeView {
	const Cube *cube;
	int x, y;
	int size_x, size_y;
	WINDOW *board;
	WINDOW *stickers[STICKERS * FACES];
};

static void cview_newwin(CubeView *cview);
static void cview_delwin(CubeView *cview);

static const StickerPos CUBE_VIEW[] = {
	{L_FACE, 0}, {U_FACE, 0}, {U_FACE, 1}, {U_FACE, 2}, {R_FACE, 2},
	{L_FACE, 1}, {U_FACE, 3}, {U_FACE, 4}, {U_FACE, 5}, {R_FACE, 1},
	{L_FACE, 2}, {U_FACE, 6}, {U_FACE, 7}, {U_FACE, 8}, {R_FACE, 0},
	{L_FACE, 2}, {F_FACE, 0}, {F_FACE, 1}, {F_FACE, 2}, {R_FACE, 0},
	{L_FACE, 5}, {F_FACE, 3}, {F_FACE, 4}, {F_FACE, 5}, {R_FACE, 3},
	{L_FACE, 8}, {F_FACE, 6}, {F_FACE, 7}, {F_FACE, 8}, {R_FACE, 6},
};

void
cview_newwin(CubeView *cview)
{
	int i;

	cview->board = newwin(CVIEW_ROWS, CVIEW_COLS, CVIEW_Y, CVIEW_X);
	box(cview->board, 0, 0);

	for (i = 0; i < 5; i++) {
		mvwvline(cview->board,
		         1, i * (STICKER_X + 1),
		         ACS_VLINE, CVIEW_ROWS - 2);
	}

	for (i = 0; i < 4; i++) {
		mvwhline(cview->board,
		         (i + 1) * (STICKER_Y + 1) + (i > 1 ? STICKER_Y : 0),
		         1, ACS_HLINE, CVIEW_COLS - 2);
	}

	for (i = 0; i < 16; i++) {
		mvwaddch(cview->board,
		        (i / 4 + 1) * (STICKER_Y + 1) + (i > 7 ? STICKER_Y : 0),
		        (i % 4 + 1) * (STICKER_X + 1), ACS_PLUS);
	}

	for (i = 0; i < CVIEW_SIZ; i++) {
		cview->stickers[i] = derwin(cview->board,
		        STICKER_Y, STICKER_X,
		        (i / 5) * (STICKER_Y + 1) + (i < CVIEW_SIZ / 2 ? 1 : 0),
		        (i % 5) * (STICKER_X + 1) + 1);
	}

	refresh();
	wrefresh(cview->board);
}

void
cview_delwin(CubeView *cview)
{
	int i;

	werase(cview->board);
	for (i = 0; i < CVIEW_SIZ; i++) delwin(cview->stickers[i]);
	delwin(cview->board);
}

void
cview_reset(CubeView *cview)
{
	cview_delwin(cview);
	cview_newwin(cview);
}

CubeView *
cview_create(Cube *cube)
{
	CubeView *cview;
	int i;

	cview = malloc(sizeof(CubeView));
	cview_newwin(cview);
	cview->cube = cube;

	for (i = 0; i < FACES; i++)
		init_pair(i + 1, colorscheme[i],  colorscheme[i]);

	return cview;
}

void
cview_destroy(CubeView *cview)
{
	cview_delwin(cview);
	free(cview);
}

void
cview_draw(CubeView *cview)
{
	int i;

	for (i = 0; i < CVIEW_SIZ; i++) {
		wbkgd(cview->stickers[i],
		      COLOR_PAIR(cube_get2(cview->cube, CUBE_VIEW[i]) + 1));
		wrefresh(cview->stickers[i]);
	}
}
