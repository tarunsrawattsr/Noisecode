#ifndef AES_H
#define AES_H // AES block size in bytes

#include <stddef.h>
void aes_encrypt_ecb(const unsigned char* input, size_t len, const unsigned char* key, unsigned char* output, size_t padded_len);

#endif // AES_H
