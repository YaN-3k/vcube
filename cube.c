#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"

#define nelem(x) (sizeof(x) / sizeof(*x))

#define DEFINE_MOVE(name, axis, cmp)                                 \
	int name##_PREDICATE(Vec3 *vec) { return cmp; }           \
	static const MoveSet name##_MOVE = {{                        \
		[NORM]  = {#name,    NORM,  axis, name##_PREDICATE}, \
		[DOUBL] = {#name"2", DOUBL, axis, name##_PREDICATE}, \
		[PRIME] = {#name"'", PRIME, axis, name##_PREDICATE}, \
	}}

#define S(f, i) ((f) * 9 + i)

typedef struct {
	int get[3];
} Vec3;

typedef enum {
	NORM,
	PRIME,
	DOUBL,
} MoveType;

struct Move {
	const char *id;
	const MoveType angle;
	const Vec3 *const axis;
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

static void vec3_apply_move(Vec3 *vec, const Move *move);
static void vec3_get_cords(Vec3 *vec, int *cords);
static int vec3_get_face(Vec3 *vec);
static int vec3_to_idx(Vec3 *vec);
static void cube_update_view(Cube *cube, Sticker *st);
static void cube_init_sticker(Cube *cube, Sticker *st, int x, int y, int z);

/* positive or negative axis informs which rotation is clockwise */
static const Vec3 XP_AXIS = {{ 1,  0,  0}};
static const Vec3 YP_AXIS = {{ 0,  1,  0}};
static const Vec3 ZP_AXIS = {{ 0,  0,  1}};
static const Vec3 XN_AXIS = {{-1,  0,  0}};
static const Vec3 YN_AXIS = {{ 0, -1,  0}};
static const Vec3 ZN_AXIS = {{ 0,  0, -1}};

DEFINE_MOVE(U, &YP_AXIS, vec->get[1] > 0);
DEFINE_MOVE(D, &YN_AXIS, vec->get[1] < 0);
DEFINE_MOVE(R, &XN_AXIS, vec->get[0] > 0);
DEFINE_MOVE(L, &XP_AXIS, vec->get[0] < 0);
DEFINE_MOVE(F, &ZN_AXIS, vec->get[2] > 0);
DEFINE_MOVE(B, &ZP_AXIS, vec->get[2] < 0);

const Move *
str2move(const char *str, const char **nptr)
{
	const MoveSet *moveset;

	while (isblank(*str)) str++;

	if (!strchr("UDRLFBudrlfbMESxyz", (int)*str)) return NULL;

	switch (*(str++)) {
	case 'U': moveset = &U_MOVE; break;
	case 'D': moveset = &D_MOVE; break;
	case 'R': moveset = &R_MOVE; break;
	case 'L': moveset = &L_MOVE; break;
	case 'F': moveset = &F_MOVE; break;
	case 'B': moveset = &B_MOVE; break;
	/*
	case 'u': moveset = &u_MOVE; break;
	case 'd': moveset = &d_MOVE; break;
	case 'r': moveset = &r_MOVE; break;
	case 'l': moveset = &l_MOVE; break;
	case 'f': moveset = &f_MOVE; break;
	case 'b': moveset = &b_MOVE; break;
	case 'M': moveset = &M_MOVE; break;
	case 'E': moveset = &E_MOVE; break;
	case 'S': moveset = &S_MOVE; break;
	case 'x': moveset = &x_MOVE; break;
	case 'y': moveset = &y_MOVE; break;
	case 'z': moveset = &z_MOVE; break;
	*/
	default: assert(0 && "undefined move");
	}

	switch (*(str)) {
	case '3':
	case '\'': if (nptr) *nptr = str + 1;
	           return &moveset->type[PRIME];
	case '2':  if (nptr) *nptr = str + 1;
	           return &moveset->type[DOUBL];
	default:   if (nptr) *nptr = str;
		   return &moveset->type[NORM];
	}
}

/* rotate Vec3 around 2d plane */
void
vec3_apply_move(Vec3 *vec, const Move *move)
{
	int i, tmp, *cords[2], **cords_ptr, axis_sign, angle;

	cords_ptr = cords;
	for (i = 0; i < 3; i++) {
		if (!move->axis->get[i]) {
			*cords_ptr = &vec->get[i];
			cords_ptr++;
		} else axis_sign = move->axis->get[i];
	}
	angle = move->angle;
	/* if axis sign is negative we need to invert rotation */
	if (axis_sign > 0 && (move->angle == NORM || move->angle == PRIME))
		angle = (move->angle + 1) % 2;

	tmp = *cords[0];
	switch (angle) {
	case NORM:  *cords[0] =  *cords[1]; *cords[1] = -tmp;       break;
	case DOUBL: *cords[0] = -*cords[0]; *cords[1] = -*cords[1]; break;
	case PRIME: *cords[0] = -*cords[1]; *cords[1] =  tmp;       break;
	}
}

void
vec3_get_cords(Vec3 *vec, int *cords)
{
	int i, *cords_ptr;

	cords_ptr = cords;
	for (i = 0; i < 3; i++) {
		if (abs(vec->get[i]) == 2) continue;
		*cords_ptr = vec->get[i] + 1;
		cords_ptr++;
	}
}

int
vec3_get_face(Vec3 *vec)
{
	return vec->get[0] ==  2 ? R_FACE :
	       vec->get[0] == -2 ? L_FACE :
	       vec->get[1] ==  2 ? U_FACE :
	       vec->get[1] == -2 ? D_FACE :
	       vec->get[2] ==  2 ? F_FACE :
	       vec->get[2] == -2 ? B_FACE : (assert(0 && "invalid position"), 0);
}

int
vec3_to_idx(Vec3 *vec)
{
	int face, xy[2];
	face = vec3_get_face(vec);
	vec3_get_cords(vec, xy);

	switch (face) {
	case U_FACE: return S(face, xy[1] * 3 + xy[0]);
	case F_FACE: return S(face, (2 - xy[1]) * 3 + xy[0]);
	case R_FACE: return S(face, (2 - xy[0]) * 3 + (2 - xy[1]));
	case B_FACE: return S(face, (2 - xy[1]) * 3 + (2 - xy[0]));
	case L_FACE: return S(face, (2 - xy[0]) * 3 + xy[1]);
	case D_FACE: return S(face, xy[1] * 3 + (2 - xy[0]));
	}
	assert(0 && "unreachable");
}

void
cube_update_view(Cube *cube, Sticker *st)
{
	cube->view2d[vec3_to_idx(&st->pos)] = st->color;
}

/*
void
vec3_print(Vec3 *vec)
{
	printf("(%d, %d, %d)\n", vec->get[0], vec->get[1], vec->get[2]);
}
*/

void
cube_init_sticker(Cube *cube, Sticker *st, int x, int y, int z)
{
	st->pos.get[0] = x;
	st->pos.get[1] = y;
	st->pos.get[2] = z;
	st->color = vec3_get_face(&st->pos);
	cube->view2d[vec3_to_idx(&st->pos)] = st->color;
}

Cube *
cube_create(void)
{
	Cube *cube;
	Sticker *stickers;
	int face, x, y;

	if (!(cube = malloc(sizeof(Cube)))) return NULL;

	stickers = cube->stickers;
	for (face = -2; face <= 2; face += 4) /* {-2, 2} */ {
		for (x = -1; x <= 1; x++) {
			for (y = -1; y <= 1; y++) {
				cube_init_sticker(cube, stickers++, face, x, y);
				cube_init_sticker(cube, stickers++, x, face, y);
				cube_init_sticker(cube, stickers++, x, y, face);
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

	for (i = 0; i < (int)nelem(cube->stickers); i++) {
		if (move->predicate(&cube->stickers[i].pos)) {
			vec3_apply_move(&cube->stickers[i].pos, move);
			cube_update_view(cube, &cube->stickers[i]);
		}
	}
}

Face
cube_get(Cube *cube, int face, int idx)
{
	return cube->view2d[S(face, idx)];
}
