#ifndef BENCH_H
#define BENCH_H

#include "common.h"
#include "ivf.h"
#include "vector.h"

// Function to benchmark IVF search against linear scan
void run_benchmark(Embedding *embeddings, int count, IVFIndex *index, TranslationPair *test_pairs, int test_count);

#endif
