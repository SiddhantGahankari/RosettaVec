#ifndef IVF_H
#define IVF_H

#include "common.h"

// Define the number of clusters (K) for K-means
#define IVF_K 50

// Define the number of nearest clusters to probe during search
#define IVF_NPROBE 3

// Structure representing a single cluster in the IVF index
typedef struct {
    float centroid[EMBEDDING_DIM]; // The center of the cluster
    int *word_indices;             // Array of indices into the original embeddings array
    int size;                      // Number of words currently in this cluster
    int capacity;                  // Allocated capacity of the word_indices array
} IVFCluster;

// Structure representing the entire IVF index
typedef struct {
    IVFCluster clusters[IVF_K];    // Array of K clusters
} IVFIndex;

// Function prototypes
void ivf_build(IVFIndex *index, Embedding *embeddings, int count);
void ivf_search(IVFIndex *index, Embedding *embeddings, float *query_vec, int k, int *top_indices, float *top_scores);
void ivf_free(IVFIndex *index);

// Internal K-means algorithm prototype (exposed for testing if needed)
void kmeans(Embedding *embeddings, int count, float centroids[IVF_K][EMBEDDING_DIM], int *assignments);

#endif
