#define MA_ENABLE_MP3
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "wav.h"
#include <gtk-3.0/gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

void formatChanger(const char* file) {
    ma_result result;
    ma_decoder decoder;
    ma_encoder encoder;

    // Init decoder (leave channels and sample rate 0 to auto-detect)
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 0, 0);
    result = ma_decoder_init_file(file, &decoderConfig, &decoder);

    if (result != MA_SUCCESS) {
        printf("❌ Failed to initialize decoder. Error code: %d\n", result);
        return;
    }

    // Init encoder
    ma_encoder_config encoderConfig = ma_encoder_config_init(
        ma_encoding_format_wav,
        decoder.outputFormat,
        decoder.outputChannels,
        decoder.outputSampleRate
    );
    result = ma_encoder_init_file("output.wav", &encoderConfig, &encoder);
    if (result != MA_SUCCESS) {
        printf("❌ Failed to initialize encoder. Error code: %d\n", result);
        ma_decoder_uninit(&decoder);
        return;
    }

    // Buffer size (number of frames to process at a time)
    const ma_uint64 bufferFrameCount = 4096;
    void* buffer = malloc((size_t)(bufferFrameCount * decoder.outputChannels * ma_get_bytes_per_sample(decoder.outputFormat)));
    if (!buffer) {
        printf("❌ Memory allocation failed.\n");
        ma_encoder_uninit(&encoder);
        ma_decoder_uninit(&decoder);
        return;
    }

    // Streaming loop
    ma_uint64 totalFrames = 0;
    ma_uint64 framesRead;
    do {
        ma_uint64 framesToRead = bufferFrameCount;
        ma_result readResult = ma_decoder_read_pcm_frames(&decoder, buffer, framesToRead, &framesToRead);
        framesRead = framesToRead;
        if (readResult != MA_SUCCESS && readResult != MA_AT_END) {
            printf("⚠️ Error reading PCM frames. Error code: %d\n", readResult);
            break;
        }
        if (framesRead > 0) {
            ma_result writeResult = ma_encoder_write_pcm_frames(&encoder, buffer, framesRead, NULL);
            if (writeResult != MA_SUCCESS) {
                printf("⚠️ Error writing PCM frames. Error code: %d\n", writeResult);
                break;
            }
            totalFrames += framesRead;
        }
    } while (framesRead > 0);

    printf("✅ Done. Total frames written: %llu\n", totalFrames);

    // Clean up
    free(buffer);
    ma_encoder_uninit(&encoder);
    ma_decoder_uninit(&decoder);
}
