#include "../include/common.h"
#include "../include/eval.h"
#include "../include/io.h"
#include "../include/translate.h"
#include "../include/vector.h"
#include "../include/globals.h"
#include "../include/ivf.h"
#include "../include/bench.h"
#include <sys/time.h>

// Helper function to measure time
static double get_time_in_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}



int main() {
    //to enable utf8 encoding, so that french words with accents are shown 
    setlocale(LC_ALL, "en_US.UTF-8");

    printf("Which language do you want to Translate to: \n");
    printf("1.For English to French \tEnter french\t\t\t");
    printf("2.For English to Spanish \tEnter spanish\n");

    scanf("%s", lang);
    for (int i = 0; i<strlen(lang); i++) {
        lang[i] = tolower(lang[i]);
    }

    if (strcmp(lang,"french")==0)
    {
        printf("Loading English embeddings...\n");
        en_count = load_embeddings("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_wiki.en.align.vec", en_embeddings);
        printf("Loaded %d English words.\n", en_count);

        printf("Loading French embeddings...\n");
        fr_count = load_embeddings("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_wiki.fr.align.vec", fr_embeddings);
        printf("Loaded %d French words.\n", fr_count);

        if (en_count == 0 || fr_count == 0)
        {
            printf("Failed to load embeddings.\n");
            return 1;
        }

        printf("Loading test pairs...\n");
        test_count = load_test_pairs("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_test_data_fr.txt");
        if (test_count > 0)
        {
            printf("Loaded %d test pairs.\n", test_count);
        }
        else
        {
            printf("No test pairs loaded. Continuing without evaluation.\n");
        }

        printf("Building IVF Index for French vocabulary...\n");
        double start_time = get_time_in_ms();
        ivf_build(&fr_ivf_index, fr_embeddings, fr_count);
        double end_time = get_time_in_ms();
        printf("IVF build time: %.2f ms\n", end_time - start_time);

        if (test_count > 0) {
            run_benchmark(fr_embeddings, fr_count, &fr_ivf_index, test_pairs, test_count);
            evaluate_accuracy_fr();
        }

        printf("\nTranslate(type 'quit' to exit):\n");
        char input[MAX_LINE_LEN];
        int top_indices[TOP_K];
        float top_scores[TOP_K];

        while (1)
        {
            printf("Enter English word or sentence: ");
            fgets(input, MAX_LINE_LEN, stdin);
            input[strcspn(input, "\n")] = '\0';
            
            char *newline = strchr(input, '\n');
            if (newline)
            {
                *newline = '\0';
            }

            if (strcmp(input, "quit") == 0)
            {
                break;
            }

            if (strchr(input, ' '))
            {
                translate_sentence_fr(input);
            }
            else
            {
                char *translation = translate_word_fr(input, TOP_K, top_indices, top_scores);

                if (strcmp(translation, "<same>") == 0)
                {
                    translation = fr_embeddings[top_indices[1]].word;
                    printf("No suitable translation found (best match was same as input)\n");
                }
                else
                {
                    printf("Translation: %s\n", translation);
                }

                printf("Top %d candidates:\n", TOP_K);
                for (int i = 0; i < TOP_K && top_indices[i] >= 0; i++)
                {
                    printf("%d. %s (score: %.4f)", i + 1, fr_embeddings[top_indices[i]].word, top_scores[i]);
                    if (strcmp(fr_embeddings[top_indices[i]].word, input) == 0)
                    {
                        printf(" <same as input>");
                    }
                    printf("\n");
                }
            }
        }
        
        ivf_free(&fr_ivf_index);
    }

    else if(strcmp(lang,"spanish") ==0)
    {
        printf("Loading English embeddings...\n");
        en_count = load_embeddings("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_wiki.en.align.vec", en_embeddings);
        printf("Loaded %d English words.\n", en_count);
        
        es_count = load_embeddings("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_wiki.es.align.vec", es_embeddings);
        printf("Loaded %d Spanish words.\n", es_count);

        if (en_count == 0 || es_count == 0)
        {
            printf("Failed to load embeddings.\n");
            return 1;
        }

        printf("Loading test pairs...\n");
        test_count = load_test_pairs("data/B24CS1029_B24CM1016_B24CS1069_B24CM1058_test_data_es.txt");
        if (test_count > 0)
        {
            printf("Loaded %d test pairs.\n", test_count);
        }
        else
        {
            printf("No test pairs loaded. Continuing without evaluation.\n");
        }

        printf("Building IVF Index for Spanish vocabulary...\n");
        double start_time = get_time_in_ms();
        ivf_build(&es_ivf_index, es_embeddings, es_count);
        double end_time = get_time_in_ms();
        printf("IVF build time: %.2f ms\n", end_time - start_time);

        if (test_count > 0) {
            run_benchmark(es_embeddings, es_count, &es_ivf_index, test_pairs, test_count);
            evaluate_accuracy_es();
        }

        printf("\nInteractive translation (type 'quit' to exit):\n");
        char input[MAX_LINE_LEN];
        int top_indices[TOP_K];
        float top_scores[TOP_K];

        while (1)
        {
            printf("Enter English word or sentence: ");
            fgets(input, MAX_LINE_LEN, stdin);
            input[strcspn(input, "\n")] = '\0';
            
            char *newline = strchr(input, '\n');
            if (newline)
            {
                *newline = '\0';
            }

            if (strcmp(input, "quit") == 0)
            {
                break;
            }

            if (strchr(input, ' '))
            {
                translate_sentence_es(input);
            }
            else
            {
                char *translation = translate_word_es(input, TOP_K, top_indices, top_scores);

                if (strcmp(translation, "<same>") == 0)
                {
                    translation = es_embeddings[top_indices[1]].word;
                    printf("Translation: %s\n", translation);
                }
                else
                {
                    printf("Translation: %s\n", translation);
                }

                printf("Top %d candidates:\n", TOP_K);
                for (int i = 0; i < TOP_K && top_indices[i] >= 0; i++)
                {
                    printf("%d. %s (score: %.4f)", i + 1, es_embeddings[top_indices[i]].word, top_scores[i]);
                    if (strcmp(es_embeddings[top_indices[i]].word, input) == 0)
                    {
                        printf(" <same as input>");
                    }
                    printf("\n");
                }
            }
        }

        ivf_free(&es_ivf_index);
    }
    else
    {
        printf("Invalid language selection.\n");
        return 1;
    }

    return 0;
}
