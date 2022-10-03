#include <ncurses.h>
#include "cube.h"
#include "algs.h"
#include "tui.h"

#define BIND_MOVE(m, k1, k2)                        \
	case k1: move = get_move(m, NORM_A); break; \
	case k2: move = get_move(m, PRIM_A); break

struct {
	TUI *tui;
	Cube *cube;
	AlgSet *algs;
} prog;

static int run(void);
static void setup(int argc, char *argv[]);
static void cleanup(void);

static void
setup(int argc, char *argv[])
{
	prog.cube = cube_create();
	prog.tui  = tui_create(prog.cube);
	prog.algs = algset_create(argc, argv);
}

static int
run(void)
{
	Move *move;
	int ch;

	tui_draw(prog.tui);
	if ((ch = getch()) == KEY_F(1)) return 1;
	switch (ch) {
		/* keymap */
		BIND_MOVE('U', 'j', 'f');
		BIND_MOVE('R', 'i', 'k');
		BIND_MOVE('L', 'd', 'e');
		BIND_MOVE('D', 's', 'l');
		BIND_MOVE('F', 'h', 'g');
		BIND_MOVE('x', 't', 'n');
		BIND_MOVE('y', ';', 'a');
		BIND_MOVE('z', 'p', 'q');

		case KEY_RESIZE: tui_reset(prog.tui); return 0;
		default: return 0;
	}
	cube_apply_move(prog.cube, move);

	return 0;
}

static void
cleanup(void)
{
	tui_destroy(prog.tui);
	cube_destroy(prog.cube);
	algset_destroy(prog.algs);
}

int
main(int argc, char *argv[])
{
	setup(argc, argv);
	while (!run());
	cleanup();
	return 0;
}
