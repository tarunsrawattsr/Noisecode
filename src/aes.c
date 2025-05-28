#include "aes.h"
#include <openssl/aes.h>
#include <string.h>

void aes_encrypt_ecb(const unsigned char* input, size_t len, const unsigned char* key, unsigned char* output, size_t padded_len) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);  // 128-bit key
    for (size_t i = 0; i < padded_len; i += AES_BLOCK_SIZE) {
        AES_ecb_encrypt(input + i, output + i, &aes_key, AES_ENCRYPT);
    }
}
