#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio.h"
#include <stdio.h>
#include "heap.h"

// Reference: Using examples from open source Audio System https://github.com/mackron/miniaudio

ma_engine engine;
int engine_ini = 0;

struct {
	int cur;
	audio_t* audio_l[255];
} audio_list;

int audio_init() {
	audio_list.cur = 0;
    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return -1;
    }

    return 1;
}

audio_t* read_file(char* file) {
    audio_t* audio = malloc(10000);
    ma_result result;
    ma_sound* sound = (ma_sound*)audio + 1;
    result = ma_sound_init_from_file(&engine, file, 0, NULL, NULL, sound);
    if (result != MA_SUCCESS) {
        return NULL;
    }

    audio->data = sound;
    audio->pause = 1;
    audio->volume = 0.5f;

    audio_list.audio_l[audio_list.cur++] = audio;
    return audio;
}

int play(audio_t* audio) {
    ma_result result;
    result = ma_sound_start(audio->data);
    if (result != MA_SUCCESS) {
        return -1;
    }

    return 0;
}

int pause(audio_t* audio) {
    ma_result result;
    result = ma_sound_stop(audio->data);
    if (result != MA_SUCCESS) {
        return -1;
    }

    return 0;
}


int set_vol(audio_t* audio, float vol) {
    if (audio == NULL) {
        return -1;
    }

    audio->volume = vol;
    ma_sound_set_volume(audio->data, vol);
    return 0;
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_encoder* pEncoder = (ma_encoder*)pDevice->pUserData;
    MA_ASSERT(pEncoder != NULL);

    ma_encoder_write_pcm_frames(pEncoder, pInput, frameCount, NULL);

    (void)pOutput;
}

int playback(char* file) {
    ma_result result;
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;

    ma_backend backends[] = {
        ma_backend_wasapi
    };

    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 44100);

    if (ma_encoder_init_file(file, &encoderConfig, &encoder) != MA_SUCCESS) {
        printf("Failed to initialize output file.\n");
        return -1;
    }

    deviceConfig = ma_device_config_init(ma_device_type_loopback);
    deviceConfig.capture.pDeviceID = NULL; 
    deviceConfig.capture.format = encoder.config.format;
    deviceConfig.capture.channels = encoder.config.channels;
    deviceConfig.sampleRate = encoder.config.sampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &encoder;

    result = ma_device_init_ex(backends, sizeof(backends) / sizeof(backends[0]), NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize loopback device.\n");
        return -2;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        printf("Failed to start device.\n");
        return -3;
    }

    printf("Recording now...\nPress Enter to stop recording...\n");
    getchar();

    ma_device_uninit(&device);
    ma_encoder_uninit(&encoder);
    return 0;
}