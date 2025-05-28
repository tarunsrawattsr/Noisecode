#ifndef LZW_H
#define LZW_H

#include <stddef.h>

void lzw_compress(const char *input, size_t len, int **output, size_t *output_len);

#endif // LZW_H
