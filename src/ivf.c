#include "../include/common.h"
#include "../include/ivf.h"
#include "../include/vector.h"

void ivf_build(IVFIndex *index, Embedding *embeddings, int count) {
    if (count == 0) return;

    int *assignments = (int *)malloc(count * sizeof(int));
    if (!assignments) {
        fprintf(stderr, "Memory allocation failed for assignments\n");
        exit(1);
    }

    float (*centroids)[EMBEDDING_DIM] = malloc(IVF_K * sizeof(*centroids));
    if (!centroids) {
        fprintf(stderr, "Memory allocation failed for centroids\n");
        exit(1);
    }

    kmeans(embeddings, count, centroids, assignments);

    for (int k = 0; k < IVF_K; k++) {
        memcpy(index->clusters[k].centroid, centroids[k], EMBEDDING_DIM * sizeof(float));
        
        index->clusters[k].size = 0;
        index->clusters[k].capacity = 1000;
        
        index->clusters[k].word_indices = (int *)malloc(index->clusters[k].capacity * sizeof(int));
        if (!index->clusters[k].word_indices) {
            fprintf(stderr, "Memory allocation failed for cluster word_indices\n");
            exit(1);
        }
    }

    for (int i = 0; i < count; i++) {
        int cluster_idx = assignments[i];
        IVFCluster *cluster = &index->clusters[cluster_idx];

        if (cluster->size == cluster->capacity) {
            cluster->capacity *= 2;
            cluster->word_indices = (int *)realloc(cluster->word_indices, cluster->capacity * sizeof(int));
            if (!cluster->word_indices) {
                fprintf(stderr, "Memory reallocation failed for cluster word_indices\n");
                exit(1);
            }
        }

        cluster->word_indices[cluster->size++] = i;
    }

    free(centroids);
    free(assignments);
}

void ivf_search(IVFIndex *index, Embedding *embeddings, float *query_vec, int k, int *top_indices, float *top_scores) {
    for (int i = 0; i < k; i++) {
        top_indices[i] = -1;
        top_scores[i] = -2.0f;
    }

    int probe_clusters[IVF_NPROBE];
    float probe_scores[IVF_NPROBE];
    
    for (int i = 0; i < IVF_NPROBE; i++) {
        probe_clusters[i] = -1;
        probe_scores[i] = -2.0f;
    }

    for (int c = 0; c < IVF_K; c++) {
        float sim = cosine_similarity(query_vec, index->clusters[c].centroid);
        
        for (int j = 0; j < IVF_NPROBE; j++) {
            if (sim > probe_scores[j]) {
                for (int l = IVF_NPROBE - 1; l > j; l--) {
                    probe_clusters[l] = probe_clusters[l - 1];
                    probe_scores[l] = probe_scores[l - 1];
                }
                probe_clusters[j] = c;
                probe_scores[j] = sim;
                break;
            }
        }
    }

    for (int p = 0; p < IVF_NPROBE; p++) {
        int cluster_idx = probe_clusters[p];
        if (cluster_idx == -1) continue;

        IVFCluster *cluster = &index->clusters[cluster_idx];
        
        for (int i = 0; i < cluster->size; i++) {
            int word_idx = cluster->word_indices[i];
            float sim = cosine_similarity(query_vec, embeddings[word_idx].vector);

            for (int j = 0; j < k; j++) {
                if (sim > top_scores[j]) {
                    for (int l = k - 1; l > j; l--) {
                        top_indices[l] = top_indices[l - 1];
                        top_scores[l] = top_scores[l - 1];
                    }
                    top_indices[j] = word_idx;
                    top_scores[j] = sim;
                    break;
                }
            }
        }
    }
}

void ivf_free(IVFIndex *index) {
    for (int k = 0; k < IVF_K; k++) {
        if (index->clusters[k].word_indices) {
            free(index->clusters[k].word_indices);
            index->clusters[k].word_indices = NULL;
        }
        index->clusters[k].size = 0;
        index->clusters[k].capacity = 0;
    }
}
