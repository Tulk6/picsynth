struct VoiceBank {
    struct Voice** voices;
    uint8_t n_voices;
};


//hmmm
//each voice 3 operators
//3 oscillators
//2 envelopes
//1 lfo

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
        oscillator_set_frequency(envelope_osc, 0.25);
        envelope_osc->loop_start = 199<<16;
        envelope_osc->loop_stop = 200<<16;
        envelope_osc->loop_type = BAND;
        envelope_osc->state = STOPPED;

        struct Oscillator* sample_osc = oscillator_new();
        oscillator_load(sample_osc, &sample_table);
        sample_osc->loop_type = NO_LOOP;
        sample_osc->state = STOPPED;

        struct Oscillator* sine_osc = oscillator_new();
        oscillator_load(sine_osc, &sine_wave);
        sine_osc->loop_type = FORWARD;
        sine_osc->state = STOPPED;

        struct Oscillator* square_osc = oscillator_new();
        oscillator_load(square_osc, &square_wave);
        square_osc->loop_type = FORWARD;
        square_osc->state = STOPPED;

        struct Operator* sine_op = operator_new();
        sine_op->mode = OSCILLATOR;
        sine_op->volume = 32767;
        operator_load_oscillator(sine_op, sine_osc);
        operator_set_frequency(sine_op, 10);

        struct Operator* square_op = operator_new();
        square_op->mode = OSCILLATOR;
        square_op->volume = 32767;
        operator_load_oscillator(square_op, square_osc);
        operator_set_frequency(square_op, 10);

        struct Operator* sample_op = operator_new();
        /*sample_op->mode = ADDITIVE;
        sample_op->carrier = sine_op;
        sample_op->modulator = square_op;
        sample_op->intensity = 32767;
        sample_op->frequency_ratio = 2;*/
        operator_load_oscillator(sample_op, sample_osc);
        sample_op->volume = 32767;

        struct Filter* filter = filter_new();
        filter->type = TEST;

        operator->envelope = envelope_osc;
        operator->carrier = square_op;
        operator->modulator = NULL;//square_op;
        operator->mode = CARRIER;
        operator->filter = filter;
        operator->volume = 2048;
        operator->frequency_ratio = 4;
        operator->intensity = 400;
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

void voices_set_mode(struct VoiceBank* voice_bank, enum OperatorMode mode){
    for (uint i=0; i<voice_bank->n_voices; i++){
        voice_bank->voices[i]->operator->mode = mode;
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