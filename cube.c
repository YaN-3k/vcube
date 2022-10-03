#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"

#define DEFINE_MOVE(name, axis, cmp)                                   \
	int name##_PREDICATE(Vec3 *vec) { return cmp; }                \
	static const MoveSet name##_MOVE = {{                          \
		[NORM_A] = {#name,    NORM_A, axis, name##_PREDICATE}, \
		[DOUB_A] = {#name"2", DOUB_A, axis, name##_PREDICATE}, \
		[PRIM_A] = {#name"'", PRIM_A, axis, name##_PREDICATE}, \
	}}

typedef struct {
	int x;
	int y;
	int z;
} Vec3;

typedef enum {
	X_A,
	Y_A,
	Z_A,
} AxisType;

/* this is used to sets the direction of rotation properly */
typedef enum {
	P_S,
	N_S,
} AxisSign;

typedef struct {
	AxisType type;
	AxisSign sign;
} Axis;

struct Move {
	const char *id;
	const AngleType angle;
	const Axis *const axis;
	int (*const predicate)(Vec3 *);
};

typedef struct {
	Move type[3];
} MoveSet;

typedef struct {
	Face color;
	Vec3 pos;
} Sticker;

struct Cube {
	Sticker stickers[FACES * STICKERS];
	Face view2d[FACES * STICKERS];
};

static int  vec3_to_idx(Vec3 *vec);
static void vec3_get_cords(Vec3 *vec, Face *face, int *x, int *y);
static void vec3_apply_move(Vec3 *vec, const Move *move);

static void cube_update_view(Cube *cube, Sticker *st);
static void cube_init_sticker(Cube *cube, Sticker *st, int x, int y, int z);

static const Axis XP_AXIS = {X_A, P_S};
static const Axis YP_AXIS = {Y_A, N_S};
static const Axis ZP_AXIS = {Z_A, P_S};
static const Axis XN_AXIS = {X_A, N_S};
static const Axis YN_AXIS = {Y_A, P_S};
static const Axis ZN_AXIS = {Z_A, N_S};

DEFINE_MOVE(U, &YP_AXIS, vec->y > 0);
DEFINE_MOVE(D, &YN_AXIS, vec->y < 0);
DEFINE_MOVE(R, &XP_AXIS, vec->x > 0);
DEFINE_MOVE(L, &XN_AXIS, vec->x < 0);
DEFINE_MOVE(F, &ZP_AXIS, vec->z > 0);
DEFINE_MOVE(B, &ZN_AXIS, vec->z < 0);

DEFINE_MOVE(x, &XP_AXIS, ((void)vec, 1));
DEFINE_MOVE(y, &YP_AXIS, ((void)vec, 1));
DEFINE_MOVE(z, &ZP_AXIS, ((void)vec, 1));

const Move *
get_move(char type, AngleType angle)
{
	const MoveSet *moveset;

	switch (type) {
	case 'U': moveset = &U_MOVE; break;
	case 'D': moveset = &D_MOVE; break;
	case 'R': moveset = &R_MOVE; break;
	case 'L': moveset = &L_MOVE; break;
	case 'F': moveset = &F_MOVE; break;
	case 'B': moveset = &B_MOVE; break;
	case 'x': moveset = &x_MOVE; break;
	case 'y': moveset = &y_MOVE; break;
	case 'z': moveset = &z_MOVE; break;
	default:  return NULL;
	}

	return &moveset->type[angle];
}

const Move *
str2move(const char *str, const char **nptr)
{
	AngleType angle;
	char move_t;

	while (isblank(*str)) str++;

	if (!strchr("UDRLFBudrlfbMESxyz", (int)*str)) return NULL;

	move_t = *str;
	str++;
	angle = NORM_A;
	if (*str == '2' || *str == '3' || *str == '\'') {
		str += 1;
		angle = (*str == '2' ? DOUB_A : PRIM_A);
	}

	if (nptr) *nptr = str;
	return get_move(move_t, angle);
}

/* rotate Vec3 around 2d plane */
void
vec3_apply_move(Vec3 *vec, const Move *move)
{
	int tmp, *x, *y;

	switch (move->axis->type) {
	case X_A: x = &vec->y; y = &vec->z; break;
	case Y_A: x = &vec->x; y = &vec->z; break;
	case Z_A: x = &vec->x; y = &vec->y; break;
	}

	tmp = *x;
	switch (move->axis->sign == P_S ? move->angle : (move->angle + 1)%2) {
	case NORM_A: *x =  *y; *y = -tmp; break;
	case DOUB_A: *x = -*x; *y = -*y;  break;
	case PRIM_A: *x = -*y; *y = tmp;  break;
	}
}

void
vec3_get_cords(Vec3 *vec, Face *face, int *x, int *y)
{
	if (vec->x == 2) {
		*face = R_FACE;
		goto X_CORDS;
	}

	if (vec->x == -2) {
		*face = L_FACE;
		goto X_CORDS;
	}

	if (vec->y == 2) {
		*face = U_FACE;
		goto Y_CORDS;
	}

	if (vec->y == -2) {
		*face = D_FACE;
		goto Y_CORDS;
	}

	if (vec->z == 2) {
		*face = F_FACE;
		goto Z_CORDS;
	}

	if (vec->z == -2) {
		*face = B_FACE;
		goto Z_CORDS;
	}

	assert(0 && "invalid sticker position");

X_CORDS:
	*x = vec->y + 1;
	*y = vec->z + 1;
	return;
Y_CORDS:
	*x = vec->x + 1;
	*y = vec->z + 1;
	return;
Z_CORDS:
	*x = vec->x + 1;
	*y = vec->y + 1;
	return;
}

int
vec3_to_idx(Vec3 *vec)
{
	Face face;
	int x, y;
	vec3_get_cords(vec, &face, &x, &y);

	switch (face) {
	case U_FACE: return face * STICKERS + y * 3 + x;
	case F_FACE: return face * STICKERS + (2 - y) * 3 + x;
	case R_FACE: return face * STICKERS + (2 - x) * 3 + (2 - y);
	case B_FACE: return face * STICKERS + (2 - y) * 3 + (2 - x);
	case L_FACE: return face * STICKERS + (2 - x) * 3 + y;
	case D_FACE: return face * STICKERS + y * 3 + (2 - x);
	default: assert(0 && "unreachable");
	}
}

void
cube_update_view(Cube *cube, Sticker *st)
{
	cube->view2d[vec3_to_idx(&st->pos)] = st->color;
}

void
cube_init_sticker(Cube *cube, Sticker *st, int x, int y, int z)
{
	st->pos.x = x;
	st->pos.y = y;
	st->pos.z = z;
	vec3_get_cords(&st->pos, &st->color, &x, &y);
	cube->view2d[vec3_to_idx(&st->pos)] = st->color;
}

Cube *
cube_create(void)
{
	Cube *cube;
	Sticker *stickers;
	int face, x, y;

	cube = malloc(sizeof(Cube));

	stickers = cube->stickers;
	for (face = -2; face <= 2; face += 4) /* {-2, 2} */ {
		for (x = -1; x <= 1; x++) {
			for (y = -1; y <= 1; y++) {
				cube_init_sticker(cube,stickers++, face, x, y);
				cube_init_sticker(cube,stickers++, x, face, y);
				cube_init_sticker(cube,stickers++, x, y, face);
			}
		}
	}

	return cube;
}

void
cube_destroy(Cube *cube)
{
	free(cube);
}

void
cube_apply_move(Cube *cube, const Move *move)
{
	int i;

	for (i = 0; i < FACES * STICKERS; i++) {
		if (move->predicate(&cube->stickers[i].pos)) {
			vec3_apply_move(&cube->stickers[i].pos, move);
			cube_update_view(cube, &cube->stickers[i]);
		}
	}
}

Face
cube_get(const Cube *cube, int face, int idx)
{
	return cube->view2d[face * STICKERS + idx];
}
