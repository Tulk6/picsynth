//TODO: interpolation
//FIX SCALE GEN -> sharps + flats

#define WAVE_TABLE_LEN 2048
#define SAMPLES_PER_BUFFER 256
#define SAMPLE_RATE 22000

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/audio_i2s.h"

#include "wavetable.c"
#include "oscillator.c"
#include "envelope.c"
#include "operator.c"
#include "sample.c"
#include "scale.c"
#include "input.c"


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
    input_init();

    /*while (true){
        if (getchar_timeout_us(0) >= 0) break;
    }*/

    struct WaveTable sine_wave;
    wavetable_init(&sine_wave);
    wavetable_load_sine(&sine_wave, WAVE_TABLE_LEN);

    struct WaveTable square_wave;
    wavetable_init(&square_wave);
    wavetable_load_square(&square_wave, WAVE_TABLE_LEN);

    struct WaveTable ad_envelope;
    wavetable_init(&ad_envelope);
    wavetable_load_ad(&ad_envelope, 100, 900);
    
    struct WaveTable sample;
    sample.table = sample_wave;
    sample.table_len = 3976;
    sample.frequency = 130.8128;

    struct audio_buffer_pool *ap = init_audio();

    /*uint32_t step = 0x200000;
    uint32_t pos = 0;
    uint32_t pos_max = 0x10000 * SINE_WAVE_TABLE_LEN;
    uint vol = 32;*/

    uint note = 45;
    struct Oscillator oscillator;
    oscillator_init(&oscillator);
    oscillator_load(&oscillator, &sine_wave);
    oscillator_set_frequency(&oscillator, 400);
    oscillator.loop_type = FORWARD;

    struct Oscillator envelope;
    oscillator_init(&envelope);
    oscillator_load(&envelope, &ad_envelope);
    oscillator_set_frequency(&envelope, 0.5);
    envelope.loop_type = NO_LOOP;

    float mod_ratio = 1;
    struct Oscillator modulator;
    oscillator_init(&modulator);
    oscillator_load(&modulator, &sine_wave);
    oscillator_set_frequency(&modulator, 200);
    modulator.loop_type = FORWARD;

    float sample_frequency = 1;
    struct Oscillator sample_osc;
    oscillator_init(&sample_osc);
    oscillator_load(&sample_osc, &sample);
    oscillator_set_frequency(&sample_osc, sample_frequency);
    sample_osc.loop_type = NO_LOOP;

    /*struct Envelope envelope;
    envelope.vol = 32;
    envelope.attack = SAMPLE_RATE>>4;
    envelope.decay = SAMPLE_RATE>>3;
    envelope.pos = 0;*/

    int mod_strength = 1;

    struct Operator operator;
    operator.carrier = &sample_osc;
    operator.modulator = NULL;//&modulator;
    operator.envelope = NULL;//&envelope;
    operator.volume = 4096;
    operator.mod_strength = 0;

    /*struct Oscillator oscillator2;
    oscillator2.pos = pos;
    oscillator2.step = step+0x12345;
    oscillator2.pos_max = pos_max;
    oscillator2.vol = vol;
    oscillator2.table = sine_wave_table;*/

    

    uint i = 0;
    while (true) {
        input_read();
        if (input_just_pressed()){
            envelope.state = PLAYING;
            envelope.pos = 0;
            sample_osc.pos = 0;
            sample_osc.state = PLAYING;
        }

        if (input_button_state(0)){
            oscillator_set_frequency(&sample_osc, 587.3295);
        }
        if (input_button_state(1)){
            oscillator_set_frequency(&sample_osc, 659.4565);
        }
        if (input_button_state(2)){
            oscillator_set_frequency(&sample_osc, 783.9909);
        }
        if (input_button_state(3)){
            oscillator_set_frequency(&sample_osc, 880);
        }
        if (input_button_state(4)){
            oscillator_set_frequency(&sample_osc, 987.7666);
        }

        /*int c = getchar_timeout_us(0);
        if (c >= 0) {
            envelope.state = PLAYING;
            envelope.pos = 0;
            sample_osc.pos = 0;
            sample_osc.state = PLAYING;  

            if (c=='e'){
                mod_ratio += 1;
                oscillator_set_frequency(&modulator, scale_get_frequency_i(note)*mod_ratio);
            }
            if (c=='d'){
                if (mod_ratio > 1) mod_ratio -= 1;
                oscillator_set_frequency(&modulator, scale_get_frequency_i(note)*mod_ratio);
            }

            if (c=='w'){
                mod_strength += 1;
                operator.mod_strength = mod_strength;
            }
            if (c=='s'){
                if (mod_strength > 1) mod_strength -= 1;
                operator.mod_strength = mod_strength;
            }

            if (c=='f'){
                note += 1
            }
            if (c=='g'){
                note -= 1
            }

            printf("mod ratio: %f\tmod_strength: %d\n", mod_ratio, mod_strength);

            if (c=='z'){
                note += 1;
                sample_frequency += 0.1;
                oscillator_set_frequency(&sample_osc, sample_frequency);
                //oscillator_set_frequency(&oscillator, scale_get_frequency_i(note));
            }

            if (c=='x'){
                if (note > 0) note -= 1;
                if (sample_frequency > 0.1) sample_frequency -= 0.1;
                oscillator_set_frequency(&sample_osc, sample_frequency);
                //oscillator_set_frequency(&oscillator, scale_get_frequency_i(note));
            }
                      
            if (c == 'a')
                oscillator_set_frequency(&oscillator, 440);
                oscillator_set_frequency(&modulator, 440);
            if (c == 'b')
                oscillator_set_frequency(&oscillator, 494);
                oscillator_set_frequency(&modulator, 494);
            if (c == 'c')
                oscillator_set_frequency(&oscillator, 523);
                oscillator_set_frequency(&modulator, 523);
            
            if (c == 'd')
                oscillator_set_frequency(&oscillator, 587);
                oscillator_set_frequency(&modulator, 587);
            
            if (c == 'e')
                oscillator_set_frequency(&oscillator, 659);
                oscillator_set_frequency(&modulator, 659);

            if (c == 'f')
                oscillator_set_frequency(&oscillator, 698);
                oscillator_set_frequency(&modulator, 698);

            if (c == 'g')
                oscillator_set_frequency(&oscillator, 392);
                oscillator_set_frequency(&modulator, 392);
            

            if (c == 'q')
                break;
        }*/

        struct audio_buffer *buffer =
            take_audio_buffer(ap, true);

        int16_t *samples =
            (int16_t *)buffer->buffer->bytes;

        //oscillator_set_frequency(&oscillator1, 440*(0.2*sinf(i*i)+1));

        //oscillator_get_samples(&oscillator, samples, buffer->max_sample_count);
        operator_get_samples(&operator, samples, buffer->max_sample_count);

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
