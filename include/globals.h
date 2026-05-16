#ifndef GLOBALS_H
#define GLOBALS_H
#include "common.h"
#include "ivf.h"

extern Embedding en_embeddings[MAX_WORDS];
extern Embedding fr_embeddings[MAX_WORDS];
extern Embedding es_embeddings[MAX_WORDS];

extern IVFIndex fr_ivf_index;
extern IVFIndex es_ivf_index;
extern TranslationPair test_pairs[MAX_TEST_PAIRS];
extern int en_count;
extern int es_count;
extern int fr_count;
extern int test_count;
extern char lang[100];
#endif
