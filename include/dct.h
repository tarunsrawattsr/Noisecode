#ifndef DCT_H
#define DCT_H

#include <stdint.h> // For standard integer types
#include "miniaudio.h"


void transform(const char *filename);
float compute_energy(const float* block, int size);
void dct(float* input, float* output, int N);
void inverse_transform(const char* dct_filename, const char* wav_filename, ma_uint32 channels, ma_uint32 sampleRate, int blockSize);
void idct(float* input, float* output, int N);
void write_wav(const char* filename, float* data, ma_uint64 frames, ma_uint32 channels, ma_uint32 sampleRate);
float compute_zcr(const float* frame, int N);
float computer_spectral_entropy(const float* frame, int N);
#endif 