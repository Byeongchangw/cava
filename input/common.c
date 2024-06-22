#include "input/common.h"
#include <math.h>

#include <string.h>

int write_to_cava_input_buffers(int16_t size, int16_t buf[size], void *data) {
    if (size == 0)
        return 0;
    struct audio_data *audio = (struct audio_data *)data;

    pthread_mutex_lock(&audio->lock);

    // shifting input buffer
    for (uint16_t n = audio->input_buffer_size * 4 - 1; n >= size; n--) {
        audio->cava_in[n] = audio->cava_in[n - size];
    }

    // fill the buffer
    uint16_t n = size - 1;
    for (uint16_t i = 0; i < size; i++) {
        audio->cava_in[i] = buf[i];
        n--;
    }

    audio->samples_counter += size;
    pthread_mutex_unlock(&audio->lock);
    return 0;
}

void reset_output_buffers(struct audio_data *data) {
    struct audio_data *audio = (struct audio_data *)data;
    pthread_mutex_unlock(&audio->lock);
    for (uint16_t n = 0; n < audio->input_buffer_size * 4; n++) {
        audio->cava_in[n] = 0;
    }
    pthread_mutex_unlock(&audio->lock);
}