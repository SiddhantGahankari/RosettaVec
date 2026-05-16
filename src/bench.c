#include "../include/bench.h"
#include <sys/time.h>

// Helper function to get current time in milliseconds
static double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

static void linear_search(Embedding *embeddings, int count, float *query_vec, int k, int *top_indices, float *top_scores) {
    for (int i = 0; i < k; i++) {
        top_indices[i] = -1;
        top_scores[i] = -2.0f;
    }

    for (int i = 0; i < count; i++) {
        float sim = cosine_similarity(query_vec, embeddings[i].vector);
        for (int j = 0; j < k; j++) {
            if (sim > top_scores[j]) {
                for (int l = k - 1; l > j; l--) {
                    top_indices[l] = top_indices[l - 1];
                    top_scores[l] = top_scores[l - 1];
                }
                top_indices[j] = i;
                top_scores[j] = sim;
                break;
            }
        }
    }
}

void run_benchmark(Embedding *embeddings, int count, IVFIndex *index, TranslationPair *test_pairs, int test_count) {
    if (count == 0 || test_count == 0) return;

    printf("\nRunning Benchmark (1000 Random Queries)\n");

    int num_queries = 1000;
    if (test_count < num_queries) {
        num_queries = test_count;
    }

    double linear_total_time = 0.0;
    double ivf_total_time = 0.0;
    
    int exact_top1_matches = 0;
    int top5_recalls = 0;

    int linear_top_indices[TOP_K];
    float linear_top_scores[TOP_K];
    
    int ivf_top_indices[TOP_K];
    float ivf_top_scores[TOP_K];

    for (int i = 0; i < num_queries; i++) {
        char source_word[MAX_WORD_LEN];
        strcpy(source_word, test_pairs[i].source);
        
        int rand_idx = rand() % count;
        float *query_vec = embeddings[rand_idx].vector;

        double start = get_time_ms();
        linear_search(embeddings, count, query_vec, TOP_K, linear_top_indices, linear_top_scores);
        linear_total_time += (get_time_ms() - start);

        start = get_time_ms();
        ivf_search(index, embeddings, query_vec, TOP_K, ivf_top_indices, ivf_top_scores);
        ivf_total_time += (get_time_ms() - start);

        if (ivf_top_indices[0] == linear_top_indices[0]) {
            exact_top1_matches++;
        }

        int linear_top1 = linear_top_indices[0];
        for (int j = 0; j < TOP_K; j++) {
            if (ivf_top_indices[j] == linear_top1) {
                top5_recalls++;
                break;
            }
        }
    }

    double linear_avg = linear_total_time / num_queries;
    double ivf_avg = ivf_total_time / num_queries;
    double speedup = linear_total_time / ivf_total_time;

    printf("Linear Search Latency : %.3f ms/query\n", linear_avg);
    printf("IVF Search Latency    : %.3f ms/query\n", ivf_avg);
    printf("Speedup Ratio         : %.2fx\n", speedup);
    printf("\n");
    printf("Exact Top-1 Match     : %.2f%%\n", (100.0 * exact_top1_matches) / num_queries);
    printf("Top-5 Recall          : %.2f%%\n\n", (100.0 * top5_recalls) / num_queries);
}
