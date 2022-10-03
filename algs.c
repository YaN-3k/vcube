#include <stdio.h>
#include <stdlib.h>
#include "algs.h"

static const char *err;

struct AlgSet {
	const char *name;
	FILE *file;
	const char *algs[ALGS_MAX_ALT + 1][ALGS_MAX_LEN];
	int last;
	int random;
};

AlgSet *
algset_create(int argc, char *argv[])
{
	AlgSet *algs;

	algs = malloc(sizeof(AlgSet));

	return algs;
}

const char *
algset_error(void)
{
	return err;
}

void
algset_destroy(AlgSet *algs)
{
	free(algs);
}
