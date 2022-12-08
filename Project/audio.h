#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Reference: Using open source Audio System from https://github.com/mackron/miniaudio

typedef struct audio_t {
	ma_sound* data;
	int pause;
	float volume;
}audio_t;

int audio_init();

audio_t* read_file(char* file);

int play(audio_t* audio);

int pause(audio_t* audio);

int set_vol(audio_t* audio, float vol);

int playback(char* file);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

