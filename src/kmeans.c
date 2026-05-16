#include "../include/common.h"
#include "../include/ivf.h"
#include "../include/vector.h"

void kmeans(Embedding *embeddings, int count, float centroids[IVF_K][EMBEDDING_DIM], int *assignments) {
    if (count == 0) return;

    int first_idx = rand() % count;
    memcpy(centroids[0], embeddings[first_idx].vector, EMBEDDING_DIM * sizeof(float));

    float *min_dists = (float *)malloc(count * sizeof(float));
    if (!min_dists) {
        fprintf(stderr, "Memory allocation failed for min_dists\n");
        exit(1);
    }
    for (int i = 0; i < count; i++) {
        min_dists[i] = 1e9;
    }

    for (int k = 1; k < IVF_K; k++) {
        float sum_dists = 0.0f;
        
        for (int i = 0; i < count; i++) {
            float sim = cosine_similarity(embeddings[i].vector, centroids[k - 1]);
            float dist = 1.0f - sim; 
            if (dist < 0.0f) dist = 0.0f;

            if (dist < min_dists[i]) {
                min_dists[i] = dist;
            }
            sum_dists += min_dists[i];
        }

        float r = ((float)rand() / RAND_MAX) * sum_dists;
        float cumulative = 0.0f;
        int chosen_idx = count - 1;

        for (int i = 0; i < count; i++) {
            cumulative += min_dists[i];
            if (cumulative >= r) {
                chosen_idx = i;
                break;
            }
        }
        memcpy(centroids[k], embeddings[chosen_idx].vector, EMBEDDING_DIM * sizeof(float));
    }
    free(min_dists);

    int max_iterations = 100;
    float movement_threshold = 1e-5f;

    int *cluster_sizes = (int *)malloc(IVF_K * sizeof(int));
    if (!cluster_sizes) {
        fprintf(stderr, "Memory allocation failed for cluster_sizes\n");
        exit(1);
    }

    float (*new_centroids)[EMBEDDING_DIM] = malloc(IVF_K * sizeof(*new_centroids));
    if (!new_centroids) {
        fprintf(stderr, "Memory allocation failed for new_centroids\n");
        exit(1);
    }

    for (int iter = 0; iter < max_iterations; iter++) {
        memset(cluster_sizes, 0, IVF_K * sizeof(int));
        memset(new_centroids, 0, IVF_K * EMBEDDING_DIM * sizeof(float));

        for (int i = 0; i < count; i++) {
            float max_sim = -2.0f;
            int best_cluster = 0;
            
            for (int k = 0; k < IVF_K; k++) {
                float sim = cosine_similarity(embeddings[i].vector, centroids[k]);
                if (sim > max_sim) {
                    max_sim = sim;
                    best_cluster = k;
                }
            }
            
            assignments[i] = best_cluster;
            cluster_sizes[best_cluster]++;
            
            for (int d = 0; d < EMBEDDING_DIM; d++) {
                new_centroids[best_cluster][d] += embeddings[i].vector[d];
            }
        }

        float total_movement = 0.0f;
        
        for (int k = 0; k < IVF_K; k++) {
            if (cluster_sizes[k] > 0) {
                for (int d = 0; d < EMBEDDING_DIM; d++) {
                    new_centroids[k][d] /= cluster_sizes[k];
                }
                normalize_vector(new_centroids[k]);
            } else {
                int random_idx = rand() % count;
                memcpy(new_centroids[k], embeddings[random_idx].vector, EMBEDDING_DIM * sizeof(float));
            }

            float cluster_movement = 0.0f;
            for (int d = 0; d < EMBEDDING_DIM; d++) {
                float diff = new_centroids[k][d] - centroids[k][d];
                cluster_movement += diff * diff;
            }
            total_movement += cluster_movement;
            memcpy(centroids[k], new_centroids[k], EMBEDDING_DIM * sizeof(float));
        }

        if (total_movement < movement_threshold) {
            break;
        }
    }

    free(cluster_sizes);
    free(new_centroids);
}
