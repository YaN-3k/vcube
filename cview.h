#define CVIEW_SIZ (int)(sizeof(CUBE_VIEW) / sizeof(*CUBE_VIEW))

#define STICKER_X 4
#define STICKER_Y 2

#define CVIEW_COLS (5 * STICKER_X + 6)
#define CVIEW_ROWS (2 * (3 * STICKER_Y + 3))

#define CVIEW_X ((COLS - CVIEW_COLS) / 2)
#define CVIEW_Y 0

typedef struct CubeView CubeView;

static const int colorscheme[] = {
	[U_FACE] = COLOR_WHITE,
	[F_FACE] = COLOR_GREEN,
	[R_FACE] = COLOR_RED,
	[B_FACE] = COLOR_BLUE,
	[L_FACE] = COLOR_YELLOW,
	[D_FACE] = COLOR_BLACK,
};

CubeView *cview_create(Cube *cube);
void cview_destroy(CubeView *cview);
void cview_draw(CubeView *cview);
void cview_reset(CubeView *cview);
