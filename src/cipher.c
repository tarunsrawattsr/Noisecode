#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cipher.h"
#include "aes.h"
#include "lzw.h"
#include "huffman.h"
#include "spectrum.h"

#define AES_BLOCK_SIZE 16 // AES block size in bytes

void read_user_input(size_t size) {
    char* input = malloc(size);
    if (input == NULL) {
        fprintf(stderr, "❌ Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    memset(input, 0, size);
    printf("Enter the text to encrypt (max %zu characters): ", size - 1);
    if (fgets(input, size, stdin) == NULL) {
        fprintf(stderr, "⚠️ Error reading input.\n");
        exit(EXIT_FAILURE);
    }

    // Remove newline character if present
    size_t len = strcspn(input, "\n");
    if (input[len] == '\n') {
        input[len] = '\0';
    }

    // Only flush stdin if input was too long to fit in buffer
    if (len == size - 1 && input[len] != '\0') {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);  // clear remaining characters
    }
    printf("✅ Input processed successfully.\n");
    encrypt_and_compress(input, len);
    free(input);
}

void encrypt_and_compress(const char *input, size_t input_len) {
    size_t padded_len = ((input_len + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    unsigned char *encrypted = calloc(padded_len, 1);

    const unsigned char aes_key[16] = "0123456789abcdef";  // or declare externally

    aes_encrypt_ecb((const unsigned char *)input, input_len, aes_key, encrypted, padded_len);

    printf("AES Encrypted (hex):\n");
    for (size_t i = 0; i < padded_len; ++i)
        printf("%02x", encrypted[i]);
    printf("\n");

    // implement huffman compression here
    encrypted = huffman_encode((const unsigned char *)encrypted, padded_len);
    printf("Huffman Encoded:\n");
    for (size_t i = 0; encrypted[i] != '\0'; ++i)
        printf("%c", encrypted[i]);
    printf("\n");
    // Now compress the encrypted data using LZW
    if (encrypted == NULL) {
        fprintf(stderr, "❌ Huffman encoding failed.\n");
        free(encrypted);
        return;
    }
    int *compressed = NULL;
    size_t compressed_len = 0;

    lzw_compress((char *)encrypted, padded_len, &compressed, &compressed_len);

    printf("LZW Compressed:\n");
    for (size_t i = 0; i < compressed_len; ++i)
        printf("%d ", compressed[i]);
    printf("\n");

    free(encrypted);
    free(compressed);
    printf("✅ Encryption and compression completed successfully.\n");
    printf("🔒 Data is now encrypted and compressed.\n");
    embed_data_into_dct("dct_output.txt", "dct_final_output.txt", (unsigned char *)compressed, compressed_len);
}

