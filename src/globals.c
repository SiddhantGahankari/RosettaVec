#include "../include/common.h"
#include  "../include/globals.h"
#include "../include/ivf.h"

Embedding en_embeddings[MAX_WORDS]; //english embeddings
Embedding es_embeddings[MAX_WORDS]; //spanish embeddings
Embedding fr_embeddings[MAX_WORDS]; //french embeddings
IVFIndex fr_ivf_index;
IVFIndex es_ivf_index;
TranslationPair test_pairs[MAX_TEST_PAIRS]; //test pairs 
int en_count = 0; //english word count
int es_count = 0; // spanish word count
int fr_count = 0; // french word count
int test_count = 0; // test words count
char lang[100]; // which language have they chosen
