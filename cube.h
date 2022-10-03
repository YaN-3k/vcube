#define STICKERS 9
#define FACES 6

typedef struct {
	int face;
	int idx;
} StickerPos;

typedef enum {
	U_FACE,
	F_FACE,
	R_FACE,
	B_FACE,
	L_FACE,
	D_FACE,
} Face;

typedef enum {
	NORM_A,
	PRIM_A,
	DOUB_A,
} AngleType;

typedef const struct Move Move;
typedef struct Cube Cube;

const Move *get_move(char type, AngleType angle);
Cube *cube_create(void);
void cube_destroy(Cube *cube);
const Move *str2move(const char *str, const char **nptr);
void cube_apply_move(Cube *cube, const Move *move);
Face cube_get(const Cube *cube, int face, int idx);
#define cube_get2(cube, pos) cube_get(cube, (pos).face, (pos).idx)
