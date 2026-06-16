#define WAVE_TABLE_LEN 2048
#define SAMPLES_PER_BUFFER 256
#define SAMPLE_RATE 24000

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "pico/binary_info.h"
#include "pico/audio_i2s.h"

#include "wavetable.c"
#include "oscillator.c"
#include "envelope.c"
#include "operator.c"


bi_decl(bi_3pins_with_names(
    PICO_AUDIO_I2S_DATA_PIN, "I2S DIN",
    PICO_AUDIO_I2S_CLOCK_PIN_BASE, "I2S BCK",
    PICO_AUDIO_I2S_CLOCK_PIN_BASE + 1, "I2S LRCK"));



static int16_t sine_wave_table[WAVE_TABLE_LEN];

struct audio_buffer_pool *init_audio(void) {
    static audio_format_t audio_format = {
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .sample_freq = SAMPLE_RATE,
        .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool =
        audio_new_producer_pool(
            &producer_format,
            3,
            SAMPLES_PER_BUFFER
        );

    struct audio_i2s_config config = {
        .data_pin = PICO_AUDIO_I2S_DATA_PIN,
        .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
        .dma_channel = 0,
        .pio_sm = 0,
    };

    const struct audio_format *output_format =
        audio_i2s_setup(&audio_format, &config);

    if (!output_format) {
        panic("Unable to open I2S device\n");
    }

    bool ok = audio_i2s_connect(producer_pool);
    assert(ok);

    audio_i2s_set_enabled(true);

    return producer_pool;
}

/*void add_samples(struct Oscillator* oscillator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] +=
            (oscillator->vol * oscillator->table[oscillator->pos >> 16]) >> 8;

        oscillator->pos += oscillator->step;

        if (oscillator->pos >= oscillator->pos_max)
            oscillator->pos -= oscillator->pos_max;
    }
}*/

int main(void) {
    stdio_init_all();

    struct WaveTable sine_wave;
    load_wavetable_sine(&sine_wave, WAVE_TABLE_LEN);

    struct audio_buffer_pool *ap = init_audio();

    /*uint32_t step = 0x200000;
    uint32_t pos = 0;
    uint32_t pos_max = 0x10000 * SINE_WAVE_TABLE_LEN;
    uint vol = 32;*/

    struct Oscillator oscillator1;
    oscillator1.pos = 0;
    oscillator1.start_pos = 0;
    oscillator1.pos_max = WAVE_TABLE_LEN<<16;
    oscillator1.step = 0x200000;
    oscillator1.vol = 32;
    oscillator1.wavetable = &sine_wave;
    oscillator_set_frequency(&oscillator1, 440);

    /*struct Oscillator oscillator2;
    oscillator2.pos = pos;
    oscillator2.step = step+0x12345;
    oscillator2.pos_max = pos_max;
    oscillator2.vol = vol;
    oscillator2.table = sine_wave_table;*/

    uint i = 0;
    while (true) {
        int c = getchar_timeout_us(0);
        i += 1;

        if (c >= 0) {
            if (c == '0')
                oscillator1.vol = 0;
            if (c == '1')
                oscillator1.vol = 64;
            if (c == 'a')
                oscillator_set_frequency(&oscillator1, 440);

            if (c == 'b')
                oscillator_set_frequency(&oscillator1, 494);

            if (c == 'c')
                oscillator_set_frequency(&oscillator1, 523);
            
            if (c == 'd')
                oscillator_set_frequency(&oscillator1, 587);
            
            if (c == 'e')
                oscillator_set_frequency(&oscillator1, 659);

            if (c == 'f')
                oscillator_set_frequency(&oscillator1, 698);

            if (c == 'g')
                oscillator_set_frequency(&oscillator1, 392);

            if (c == 'q')
                break;
        }

        struct audio_buffer *buffer =
            take_audio_buffer(ap, true);

        int16_t *samples =
            (int16_t *)buffer->buffer->bytes;

        oscillator_set_frequency(&oscillator1, 440*(0.2*sinf(i*i)+1));

        oscillator_get_samples(&oscillator1, samples, buffer->max_sample_count);

        //add_samples(&oscillator2, samples, buffer->max_sample_count);

        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
    }

    return 0;
}


/*for (uint i = 0; i < buffer->max_sample_count; i++) {
            samples[i] =
                (vol * sine_wave_table[pos >> 16]) >> 8;

            pos += step;

            if (pos >= pos_max)
                pos -= pos_max;
        }*/
