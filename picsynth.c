//TODO: make sample advance voice based, as well as start stop
// FIX voice addding code
// make all values have two sets -> static and oscillator
// add operator get_modulator and get_carrier functiions
// same for filter, voices, etc
// ERM.... in operator set frequency, rn does not handle if modulation operator and oscillator...
//finish DX7 compatability
//maybe abstract all platform dependant code to seperate file, then spam #ifdefs for win compat
//CLEAN!!!


///okee half way through need to figure out oscillator adding
// voice stopping
// adding all oscillators
// etc


//TODO: interpolation
//FIX voice adding code
//saw wave, triangle wave, etc...
//TODO: make sample advance voice based, as well as start stop

//FIX SCALE GEN -> sharps + flats

#define WAVE_TABLE_LEN 2048
#define SAMPLES_PER_BUFFER 256
#define SAMPLE_RATE 22000

//#define PICO_AUDIO_I2S_DATA_PIN
//#define PICO_AUDIO_I2S_CLOCK_PIN_BASE


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/audio_i2s.h"
#include "pico/rand.h"

#include "lcd_1602_i2c.c"

#include "wavetable.c"
#include "sample.c"
#include "waves.c"
#include "oscillator.c"
#include "envelope.c"
#include "filter.c"
#include "operator.c"
#include "scale.c"
#include "input.c"
#include "voices.c"
#include "display.c"
#include "algorithms.c"
#include "interface.c"


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

    

    while (true){
        if (getchar_timeout_us(0) >= 0) break;
    }
   lcd_main();
    struct audio_buffer_pool *ap = init_audio();

    display_init();
    interface_init();

    waves_load();

    struct VoiceBank voice_bank;
    voices_init(&voice_bank);
    voices_load(&voice_bank, 2);

    /*struct WaveTable sine_wave;
    wavetable_init(&sine_wave);
    wavetable_load_sine(&sine_wave, WAVE_TABLE_LEN);

    struct WaveTable square_wave;
    wavetable_init(&square_wave);
    wavetable_load_square(&square_wave, WAVE_TABLE_LEN);

    struct WaveTable ad_envelope;
    wavetable_init(&ad_envelope);
    wavetable_load_ad(&ad_envelope, 10, 700);
    
    struct WaveTable sample;
    sample.table = sample_wave;
    sample.table_len = 3976;
    sample.frequency = 130.8128;
    */
    

    /*uint32_t step = 0x200000;
    uint32_t pos = 0;
    uint32_t pos_max = 0x10000 * SINE_WAVE_TABLE_LEN;
    uint vol = 32;*/

    /*uint note = 45;
    struct Oscillator oscillator;
    oscillator_init(&oscillator);
    oscillator_load(&oscillator, &sine_wave);
    oscillator_set_frequency(&oscillator, 400);
    oscillator.loop_type = FORWARD;

    struct Oscillator vib_oscillator;
    oscillator_init(&vib_oscillator);
    oscillator_load(&vib_oscillator, &sine_wave);
    oscillator_set_frequency(&vib_oscillator, 5);
    vib_oscillator.loop_type = FORWARD;

    struct Oscillator envelope;
    oscillator_init(&envelope);
    oscillator_load(&envelope, &ad_envelope);
    oscillator_set_frequency(&envelope, 2);
    envelope.loop_type = NO_LOOP;

    struct Oscillator short_envelope;
    oscillator_init(&short_envelope);
    oscillator_load(&short_envelope, &ad_envelope);
    oscillator_set_frequency(&short_envelope, 10);
    short_envelope.loop_type = NO_LOOP;

    struct Oscillator modulator;
    oscillator_init(&modulator);
    oscillator_load(&modulator, &square_wave);
    oscillator_set_frequency(&modulator, 200);
    modulator.loop_type = FORWARD;

    struct Oscillator sample_osc;
    oscillator_init(&sample_osc);
    oscillator_load(&sample_osc, &sample);
    sample_osc.loop_type = NO_LOOP;

    struct Operator carrier_operator;
    operator_init(&carrier_operator);
    operator_load_oscillator(&carrier_operator, &oscillator);
    carrier_operator.volume = 32767;

    struct Operator mod_operator;
    operator_init(&mod_operator);
    operator_load_oscillator(&mod_operator, &modulator);
    mod_operator.volume = 32767;

    struct Operator vibrato_operator;
    operator_init(&vibrato_operator);
    operator_load_oscillator(&vibrato_operator, &vib_oscillator);
    vibrato_operator.volume = 32767;
    vibrato_operator.envelope = &short_envelope;

    struct Operator operator;
    operator_init(&operator);
    operator.mode = CARRIER;
    operator.carrier = &carrier_operator;
    operator.modulator = &mod_operator;
    operator.intensity = 200;
    operator.volume = 32767;
    operator.frequency_ratio = 2;

    struct Operator fin_operator;
    operator_init(&fin_operator);
    fin_operator.mode = PHASE_MODULATION;
    fin_operator.carrier = &operator;
    fin_operator.modulator = &vibrato_operator;
    fin_operator.envelope = &envelope;
    fin_operator.intensity = 2000;
    fin_operator.volume = 2048;
    fin_operator.frequency_ratio = 5.1236;*/

    /*struct Oscillator oscillator2;
    oscillator2.pos = pos;
    oscillator2.step = step+0x12345;
    oscillator2.pos_max = pos_max;
    oscillator2.vol = vol;
    oscillator2.table = sine_wave_table;*/

    algorithm_apply(&algo1, &voice_bank);
    printf("algo applied!\n");

    
    uint i = 0;
    while (true) {
        input_read();
        interface_update();
        
        /*adc = (input_read_adc()>>9);
        if (adc != prev_adc){
            printf("adc: %i\n", adc);
            prev_adc = adc;
            voices_set_intensity(&voice_bank, adc);
        }*/
        if (input_just_pressed()){
            for (int j=0;j<n_buttons;j++){
                if (input_button_pressed(j)){
                    printf("just pressed %i\n", j);
                    struct Voice* voice = voices_acquire(&voice_bank);
                    voice->trigger = j;
                    float freq = 0;
                    switch (j){
                        case 0:
                            freq = scale_get_frequency(Note_A, 5);
                            break;
                        case 1:
                            freq = scale_get_frequency(Note_C, 2);
                            break;
                        case 2:
                            freq = scale_get_frequency(Note_C, 3);
                            break;
                        case 3:
                            freq = scale_get_frequency(Note_E, 2);
                            break;
                        case 4:
                            freq = scale_get_frequency(Note_E, 3);
                            break;
                    }
                    printf("a");
                    voice_set_frequency(voice, freq);
                    printf("b");
                    voice_start(voice);
                    printf("c");
                }else if (input_button_released(j)){
                    for (int k=0;k<voice_bank.n_voices;k++){
                        struct Voice* voice = voice_bank.voices[k];
                        if (voice->trigger==j){
                            voice_stop(voice);
                        }
                    }
                    
                }
            }
            /*if (current_state>>5 & 1){
                voices_set_intensity(&voice_bank, 100);
                voices_set_mode(&voice_bank, CARRIER);
                lcd_string(&my_lcd, "Carrier Only");
                //voices_set_frequency_ratio(&voice_bank, 0.01);
            }else if (current_state>>6 & 1){
                lcd_string(&my_lcd, "Phase Modulation");
                voices_set_intensity(&voice_bank, 2000);
                voices_set_mode(&voice_bank, RING_MODULATION);
                voices_set_intensity(&voice_bank, 500);
                //voices_set_frequency_ratio(&voice_bank, 0.5f);
            }else if (current_state>>7 & 1){
                voices_set_intensity(&voice_bank, 800);
                lcd_string(&my_lcd, "Additive");
                voices_set_intensity(&voice_bank, 32767);
                voices_set_mode(&voice_bank, ADDITIVE);
                //voices_set_frequency_ratio(&voice_bank, 4);
            }*/
        

            /*if (input_button_pressed(0)){
                operator_set_frequency(operator, 587.3295/2);
            }
            if (input_button_pressed(1)){
                operator_set_frequency(operator, 659.4565/2);
            }
            if (input_button_pressed(2)){
                operator_set_frequency(operator, 783.9909/2);
            }
            if (input_button_pressed(3)){
                operator_set_frequency(operator, 880/2);
            }
            if (input_button_pressed(4)){
                operator_set_frequency(operator, 987.7666/2);
            }*/
        }


        struct audio_buffer *buffer =
            take_audio_buffer(ap, true);

        int16_t *samples =
            (int16_t *)buffer->buffer->bytes;

        voices_get_samples(&voice_bank, samples, buffer->max_sample_count);

        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
    }

    return 0;
}