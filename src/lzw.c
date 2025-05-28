#include "lzw.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_DICT_SIZE 4096

typedef struct {
    char *str;
    int code;
} DictEntry;

static int find_in_dict(DictEntry *dict, int dict_size, const char *str) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(dict[i].str, str) == 0)
            return dict[i].code;
    }
    return -1;
}

void lzw_compress(const char *input, size_t len, int **output, size_t *output_len) {
    DictEntry dict[MAX_DICT_SIZE];
    int dict_size = 256;

    // Initialize dictionary with all single chars
    for (int i = 0; i < 256; i++) {
        dict[i].str = (char *)malloc(2);
        dict[i].str[0] = (char)i;
        dict[i].str[1] = '\0';
        dict[i].code = i;
    }

    char *w = (char *)calloc(len + 1, 1);
    int out_capacity = len * 2; // Allocate some space
    *output = (int *)malloc(out_capacity * sizeof(int));
    *output_len = 0;

    for (size_t i = 0; i < len; ++i) {
        char c[2] = {input[i], '\0'};
        char *wc = (char *)malloc(strlen(w) + 2);
        strcpy(wc, w);
        strcat(wc, c);

        if (find_in_dict(dict, dict_size, wc) != -1) {
            strcpy(w, wc);
        } else {
            int code = find_in_dict(dict, dict_size, w);
            (*output)[(*output_len)++] = code;

            if (dict_size < MAX_DICT_SIZE) {
                dict[dict_size].str = strdup(wc);
                dict[dict_size].code = dict_size++;
            }

            strcpy(w, c);
        }

        free(wc);
    }

    if (*w) {
        int code = find_in_dict(dict, dict_size, w);
        (*output)[(*output_len)++] = code;
    }

    free(w);
    for (int i = 0; i < dict_size; ++i)
        free(dict[i].str);
}
