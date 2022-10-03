typedef struct TUI TUI;

TUI *tui_create(Cube *cube);
void tui_reset(TUI *tui);
void tui_destroy(TUI *tui);
void tui_draw(TUI *tui);
