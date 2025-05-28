#include "miniaudio.h"
#include "dct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "cipher.h"

#define M_PI 3.14159265358979323846
#define INIT_FRAME_SAMPLES 100

float compute_energy(const float* frame, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        sum += frame[i] * frame[i];
    }
    return sum / N;
}

float compute_zcr(const float* frame, int N) {
    int zeroCrossings = 0;
    for (int i = 1; i < N; ++i) {
        if ((frame[i - 1] >= 0 && frame[i] < 0) || (frame[i - 1] < 0 && frame[i] >= 0)) {
            zeroCrossings++;
        }
    }
    return (float)zeroCrossings / N;
}

float compute_spectral_entropy(const float* frame, int N) {
    int numBands = 10;
    int bandSize = N / numBands;
    float bandEnergy[10] = {0};
    float totalEnergy = 0.0f;
    for (int b = 0; b < numBands; ++b) {
        bandEnergy[b] = 0.0f;
        for (int i = b * bandSize; i < (b + 1) * bandSize && i < N; ++i) {
            bandEnergy[b] += frame[i] * frame[i];
        }
        totalEnergy += bandEnergy[b];
    }
    float entropy = 0.0f;
    for (int b = 0; b < numBands; ++b) {
        if (bandEnergy[b] > 0 && totalEnergy > 0) {
            float p = bandEnergy[b] / totalEnergy;
            entropy -= p * (log2f(p));
        }
    }
    return entropy;
}

// Overlap-add window (Hann)
static void apply_window(float* buf, int N) {
    for (int i = 0; i < N; ++i) {
        float w = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (N - 1)));
        buf[i] *= w;
    }
}

void transform(const char* filename) {
    ma_result result;
    ma_decoder decoder;
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 0, 0);  
    result = ma_decoder_init_file(filename, &decoderConfig, &decoder);

    if (result != MA_SUCCESS) {
        printf("❌ Failed to initialize decoder. Error code: %d\n", result);
        return;
    }

    ma_uint32 channels = decoder.outputChannels;
    ma_uint32 sampleRate = decoder.outputSampleRate;
    ma_format format = decoder.outputFormat;

    if (format != ma_format_f32) {
        printf("❌ Unsupported format. Only float32 is supported.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    int blockSize = 1024;
    int hopSize = blockSize / 2; // 50% overlap
    ma_uint64 totalFrames = ma_decoder_get_length_in_pcm_frames(&decoder, NULL);
    if (totalFrames == 0) {
        printf("❌ Failed to get total frames in the audio file.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    float* inputBuffer = (float*)malloc((blockSize + hopSize) * channels * sizeof(float));
    float** channelInput = (float**)malloc(channels * sizeof(float*));
    float** channelOutput = (float**)malloc(channels * sizeof(float*));
    for (ma_uint32 ch = 0; ch < channels; ++ch) {
        channelInput[ch] = (float*)malloc(blockSize * sizeof(float));
        channelOutput[ch] = (float*)malloc(blockSize * sizeof(float));
    }
    if (!inputBuffer || !channelInput || !channelOutput) {
        printf("❌ Memory allocation failed.\n");
        goto cleanup;
    }

    FILE* dctFile = fopen("dct_output.txt", "w");
    if (!dctFile) {
        printf("❌ Failed to open output file.\n");
        goto cleanup;
    }

    ma_uint64 framesRead = 0;
    int blockIndex = 0;
    int barWidth = 40;

    float energySamples[INIT_FRAME_SAMPLES];
    int collected = 0;
    float energySum = 0.0f;
    float energySqSum = 0.0f;

    // Collect first N energies
    while (collected < INIT_FRAME_SAMPLES) {
        ma_uint64 read = 0;
        if (ma_decoder_read_pcm_frames(&decoder, inputBuffer, blockSize, &read) != MA_SUCCESS || read < blockSize)
            break;

        for (ma_uint32 ch = 0; ch < channels; ++ch) {
            for (int i = 0; i < blockSize; ++i) {
                channelInput[ch][i] = inputBuffer[i * channels + ch];
            }
            float e = compute_energy(channelInput[ch], blockSize);
            energySamples[collected++] = e;
            energySum += e;
            energySqSum += e * e;
        }
    }

    float mean = energySum / collected;
    float stddev = sqrtf((energySqSum / collected) - (mean * mean));
    const float energyThreshold = mean + 0.05f * stddev; // Tune this threshold as needed

    // Pre-fill buffer for overlap
    memset(inputBuffer, 0, (blockSize + hopSize) * channels * sizeof(float));
    ma_uint64 prefill = 0;
    if (ma_decoder_read_pcm_frames(&decoder, inputBuffer, blockSize, &prefill) != MA_SUCCESS) {
        printf("⚠️ Error reading PCM frames.\n");
        goto cleanup;
    }
    framesRead = prefill;

    while (framesRead > 0) {
        // Deinterleave and window each channel
        for (ma_uint32 ch = 0; ch < channels; ++ch) {
            for (int i = 0; i < blockSize; ++i) {
                channelInput[ch][i] = inputBuffer[i * channels + ch];
            }
            apply_window(channelInput[ch], blockSize);
            dct(channelInput[ch], channelOutput[ch], blockSize);
        }

        // Compute energy for each channel and decide to skip or not
        int skipBlock = 1; // Assume skip

        for (ma_uint32 ch = 0; ch < channels; ++ch) {
            float energy = compute_energy(channelInput[ch], blockSize);
            float zcr = compute_zcr(channelInput[ch], blockSize);
            float entropy = compute_spectral_entropy(channelInput[ch], blockSize);

            int selectByEnergy = (energy > energyThreshold);
            int selectByZCR = (zcr > 0.05f);            // empirical threshold, tune as needed
            int selectByEntropy = (entropy > 2.0f);      // empirical threshold, tune as needed

            if (selectByEnergy || selectByZCR || selectByEntropy) {
                skipBlock = 0;
                dct(channelInput[ch], channelOutput[ch], blockSize);
            } else {
                memset(channelOutput[ch], 0, blockSize * sizeof(float));
            }
        }

        if (!skipBlock) {
            // Write actual DCT output for each channel
            for (ma_uint32 ch = 0; ch < channels; ++ch) {
                for (int i = 0; i < blockSize; ++i) {
                    fprintf(dctFile, "%f%c", channelOutput[ch][i], (i == blockSize - 1) ? '\n' : ',');
                }
            }
        } else {
            // Write zeros for skipped block to keep file structure intact
            for (ma_uint32 ch = 0; ch < channels; ++ch) {
                for (int i = 0; i < blockSize; ++i) {
                    fprintf(dctFile, "0.0%c", (i == blockSize - 1) ? '\n' : ',');
                }
            }
            // No skipping message printed
        }

        // Progress bar
        int pos = blockIndex % (barWidth + 1);
        printf("] Block #%d", blockIndex);
        printf("\r[");
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }
        fflush(stdout);

        // Shift buffer for overlap
        memmove(inputBuffer, inputBuffer + hopSize * channels, blockSize * channels * sizeof(float));

        // Read next hopSize frames
        ma_uint64 justRead = 0;
        ma_decoder_read_pcm_frames(&decoder, inputBuffer + blockSize * channels, hopSize, &justRead);
        framesRead = justRead;
        if (framesRead < hopSize) {
            // Zero-pad last block if needed
            memset(inputBuffer + (blockSize + framesRead) * channels, 0, (hopSize - framesRead) * channels * sizeof(float));
        }

        blockIndex++;
    }
    printf("\n");
    fclose(dctFile);
    printf("✅ DCT transformation completed.\n");
    read_user_input(1024);
    inverse_transform("dct_final_output.txt", "idct_output.wav", channels, sampleRate, blockSize);

cleanup:
    if (inputBuffer) free(inputBuffer);
    if (channelInput) {
        for (ma_uint32 ch = 0; ch < channels; ++ch) free(channelInput[ch]);
        free(channelInput);
    }
    if (channelOutput) {
        for (ma_uint32 ch = 0; ch < channels; ++ch) free(channelOutput[ch]);
        free(channelOutput);
    }
    ma_decoder_uninit(&decoder);
}

void dct ( float* input, float* output, int N ) {
    for (int k = 0; k < N; k++) {
        output[k] = 0.0f;
        for (int n = 0; n < N; n++) {
            output[k] += input[n] * cos(M_PI / N * (n + 0.5) * k);
        }
        if (k == 0) {
            output[k] *= sqrt(1.0 / N);
        } else {
            output[k] *= sqrt(2.0 / N);
        }
    }
}

// Inverse DCT (Type III, matching the above DCT-II)
void idct(float* input, float* output, int N) {
    for (int n = 0; n < N; n++) {
        output[n] = input[0] * sqrt(1.0 / N);
        for (int k = 1; k < N; k++) {
            output[n] += input[k] * sqrt(2.0 / N) * cos(M_PI / N * k * (n + 0.5));
        }
    }
}

// Helper to write float PCM to .wav using miniaudio
void write_wav(const char* filename, float* data, ma_uint64 frames, ma_uint32 channels, ma_uint32 sampleRate) {
    ma_encoder encoder;
    ma_encoder_config config = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, channels, sampleRate);
    if (ma_encoder_init_file(filename, &config, &encoder) != MA_SUCCESS) {
        printf("❌ Failed to initialize WAV encoder.\n");
        return;
    }
    ma_encoder_write_pcm_frames(&encoder, data, frames, NULL);
    ma_encoder_uninit(&encoder);
}

// Read DCT output file, perform inverse DCT, and write to .wav
void inverse_transform(const char* dct_filename, const char* wav_filename, ma_uint32 channels, ma_uint32 sampleRate, int blockSize) {
    FILE* dctFile = fopen(dct_filename, "r");
    if (!dctFile) {
        printf("❌ Failed to open DCT file for reading.\n");
        return;
    }
    // Estimate total frames (optional, for allocation)
    float* block = (float*)malloc(blockSize * sizeof(float));
    float* idctBlock = (float*)malloc(blockSize * sizeof(float));
    float* output = NULL;
    ma_uint64 totalBlocks = 0;
    int hopSize = blockSize / 2;

    // First pass: count lines (each channel is a line, so divide by channels)
    char line[16 * 1024];
    ma_uint64 channelLines = 0;
    while (fgets(line, sizeof(line), dctFile)) {
        channelLines++;
    }
    totalBlocks = channelLines / channels;
    rewind(dctFile);

    // Output buffer: overlap-add, so total frames = (totalBlocks - 1) * hopSize + blockSize
    ma_uint64 totalFrames = (totalBlocks - 1) * hopSize + blockSize;
    output = (float*)calloc(totalFrames * channels, sizeof(float));
    if (!output) {
        printf("❌ Memory allocation failed for output buffer.\n");
        free(block);
        free(idctBlock);
        fclose(dctFile);
        return;
    }

    ma_uint64 blockIndex = 0;
    int barWidth = 40;

    printf("Starting inverse DCT transformation...\n");
    while (blockIndex < totalBlocks) {
        int channelReadError = 0;
        for (ma_uint32 ch = 0; ch < channels; ++ch) {
            if (!fgets(line, sizeof(line), dctFile)) {
                channelReadError = 1;
                break;
            }
            // Parse comma-separated floats for this channel
            char lineCopy[16 * 1024];
            strncpy(lineCopy, line, sizeof(lineCopy));
            lineCopy[sizeof(lineCopy) - 1] = '\0';
            char* token = strtok(lineCopy, ",\n");
            int i = 0;
            while (token && i < blockSize) {
                block[i++] = (float)atof(token);
                token = strtok(NULL, ",\n");
            }
            // Perform inverse DCT
            idct(block, idctBlock, blockSize);
            // Overlap-add into output buffer
            ma_uint64 frameOffset = blockIndex * hopSize;
            for (int j = 0; j < blockSize; ++j) {
                if (frameOffset + j < totalFrames) {
                    output[(frameOffset + j) * channels + ch] += idctBlock[j];
                }
            }
        }
        if (channelReadError) {
            break;
        }

        // Progress bar similar to dct()
        int pos = blockIndex % (barWidth + 1);  // loop the bar
        printf("] Block #%llu", (unsigned long long)blockIndex);
        printf("\r[");
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }
        fflush(stdout);
        blockIndex++;
    }
    printf("\n");

    write_wav(wav_filename, output, totalFrames, channels, sampleRate);

    free(block);
    free(idctBlock);
    free(output);
    fclose(dctFile);
    printf("✅ Inverse DCT and WAV file written to %s\n", wav_filename);
}