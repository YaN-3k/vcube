#define ALGS_MAX_ALT 4
#define ALGS_MAX_LEN 50

typedef struct AlgSet AlgSet;

AlgSet *algset_create(int argc, char *argv[]);
void algset_load(AlgSet *algs, char *fname);
void algset_next(AlgSet *algs);
void algset_destroy(AlgSet *algs);
