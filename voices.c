struct VoiceBank {
    struct Voice** voices;
    uint8_t n_voices;
};

struct Voice {
    struct Operator* operator;
    int trigger;
};

void voices_init(struct VoiceBank* voice_bank){
    voice_bank->voices = NULL;
    voice_bank->n_voices = 0;
}

struct Voice* voice_new(){
    struct Voice* voice = malloc(sizeof(struct Voice));
    voice->operator = NULL;
    voice->trigger = 0;
    return voice;
}

void voices_unload(struct VoiceBank* voice_bank){
    for (int i=0;i<voice_bank->n_voices;i++){
        operator_unload(voice_bank->voices[i]->operator);
    }
    free(voice_bank->voices);
    voice_bank->n_voices = 0;
    voice_bank->voices = NULL;
}

void voices_load(struct VoiceBank* voice_bank, uint n_voices){
    if (voice_bank->n_voices != 0) voices_unload(voice_bank);
    voice_bank->voices = malloc(sizeof(struct Voice*)*n_voices);
    for (int i=0;i<n_voices;i++){
        struct Operator* operator = operator_new();

        struct Oscillator* envelope_osc = oscillator_new();
        oscillator_load(envelope_osc, &envelope);
        oscillator_set_frequency(envelope_osc, 0.5);
        envelope_osc->loop_start = 199<<16;
        envelope_osc->loop_stop = 200<<16;
        envelope_osc->loop_type = BAND;
        envelope_osc->state = STOPPED;

        struct Oscillator* sine_osc = oscillator_new();
        oscillator_load(sine_osc, &sample_table);
        sine_osc->loop_type = FORWARD;
        sine_osc->state = STOPPED;

        struct Oscillator* square_osc = oscillator_new();
        oscillator_load(square_osc, &sine_wave);
        square_osc->loop_type = FORWARD;
        square_osc->state = STOPPED;

        struct Operator* sine_op = operator_new();
        sine_op->mode = OSCILLATOR;
        sine_op->volume = 32767;
        operator_load_oscillator(sine_op, sine_osc);
        operator_set_frequency(sine_op, scale_get_frequency(Note_C, 4));

        struct Operator* square_op = operator_new();
        square_op->mode = OSCILLATOR;
        square_op->volume = 32767;
        operator_load_oscillator(square_op, square_osc);
        operator_set_frequency(square_op, 10);

        operator->envelope = envelope_osc;
        operator->carrier = sine_op;
        operator->modulator = square_op;
        operator->mode = CARRIER;
        operator->volume = 10000;
        operator->frequency_ratio = 2;
        operator->intensity = 1;
        operator_set_frequency(operator, 400);

        struct Voice* voice = voice_new();
        voice->operator = operator;
        voice_bank->voices[i] = voice;
    }
    voice_bank->n_voices = n_voices;
}

void voices_set_intensity(struct VoiceBank* voice_bank, int16_t intensity){
    for (uint i=0; i<voice_bank->n_voices; i++){
        voice_bank->voices[i]->operator->intensity = intensity;
    }
}

void voices_set_frequency_ratio(struct VoiceBank* voice_bank, float frequency_ratio){
    for (uint i=0; i<voice_bank->n_voices; i++){
        operator_set_frequency_ratio(voice_bank->voices[i]->operator, frequency_ratio);
    } 
}

struct Voice* voices_acquire(struct VoiceBank* voice_bank){
    for (uint i=0; i<voice_bank->n_voices; i++){
        if (voice_bank->voices[i]->operator->envelope->state == STOPPED) return voice_bank->voices[i];
    }
    return voice_bank->voices[0];
}

uint16_t voices_get_next_sample(struct VoiceBank* voice_bank){
    uint16_t sample = 0;
    for (int i=0;i<voice_bank->n_voices;i++){
        sample += operator_get_next_sample(voice_bank->voices[i]->operator);
        sample >> 1;
    } 
    //printf("sample %"PRIu16 "\n", sample);
    return sample;
}

void voices_get_samples(struct VoiceBank* voice_bank, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = voices_get_next_sample(voice_bank);
    }
}