#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "spectrum.h"

#define BLOCK_SIZE 1024
#define EMBED_SIZE 64     // Use first 64 DCT coefficients
#define PN_LENGTH 8       // One PN bit per data bit
#define ALPHA 0.05        // Embedding strength

// Sample PN sequence (8 bits) for spreading 1 bit of data
int pn_sequence[PN_LENGTH];

void generate_pn_sequence() {
    for (int i = 0; i < PN_LENGTH; ++i) {
        pn_sequence[i] = (rand() % 2) ? 1 : -1;
    }
}

// Call this before embedding
__attribute__((constructor))
static void init_pn_sequence() {
    srand((unsigned int)time(NULL));
    generate_pn_sequence();
}

// LZW-compressed message

void embed_data_into_dct(const char* dct_in, const char* dct_out, const unsigned char* lzw_data, int lzw_len) {
    if (lzw_len <= 0 || lzw_data == NULL) {
        printf("⚠️ No data to embed.\n");
        return;
    }
    FILE *fin = fopen(dct_in, "r");
    FILE *fout = fopen(dct_out, "w");

    if (!fin || !fout) {
        printf("⚠️ Error opening file(s).\n");
        return;
    }

    char line[BLOCK_SIZE * 20];  // Buffer to hold each line
    int byte_index = 0;

    while (fgets(line, sizeof(line), fin)) {
        float coeffs[BLOCK_SIZE];
        char* token = strtok(line, ",");
        int i = 0;

        // Parse DCT coefficients
        while (token && i < BLOCK_SIZE) {
            coeffs[i++] = atof(token);
            token = strtok(NULL, ",");
        }

        // Embed data if bytes remain
        if (byte_index < lzw_len) {
            unsigned char byte = lzw_data[byte_index++];

            for (int bit = 0; bit < 8; ++bit) {
                int data_bit = (byte >> (7 - bit)) & 1;

                for (int j = 0; j < PN_LENGTH; ++j) {
                    int pn = pn_sequence[j];
                    float spread = ALPHA * pn * (data_bit ? 1 : -1);

                    int index = bit * PN_LENGTH + j;
                    if (index < EMBED_SIZE) {
                        coeffs[index] += spread;
                    }
                }
            }
        }

        // Write modified line
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            fprintf(fout, "%f", coeffs[k]);
            if (k < BLOCK_SIZE - 1) fprintf(fout, ",");
        }
        fprintf(fout, "\n");
    }

    fclose(fin);
    fclose(fout);
    printf("✅ Spread spectrum embedding complete. Output: %s\n", dct_out);
    printf("📂 Output file: %s\n", dct_out);
    printf("📊 DCT coefficients modified to include embedded data.\n");
}
