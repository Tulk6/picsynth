struct VoiceBank {
    struct Operator** voices;
    uint8_t n_voices;
};

void voices_init(struct VoiceBank* voice_bank){
    voice_bank->voices = NULL;
    voice_bank->n_voices = 0;
}

void voices_unload(struct VoiceBank* voice_bank){
    for (int i=0;i<voice_bank->n_voices;i++){
        operator_unload(voice_bank->voices[i]);
    }
    free(voice_bank->voices);
    voice_bank->n_voices = 0;
    voice_bank->voices = NULL;
}

void voices_load(struct VoiceBank* voice_bank, uint n_voices){
    if (voice_bank->n_voices != 0) voices_unload(voice_bank);
    voice_bank->voices = malloc(sizeof(struct Operator*)*n_voices);
    for (int i=0;i<n_voices;i++){
        struct Operator* operator = operator_new();

        struct Oscillator* envelope_osc = oscillator_new();
        oscillator_load(envelope_osc, &envelope);
        oscillator_set_frequency(envelope_osc, 1);
        envelope_osc->loop_start = 11<<16;
        envelope_osc->loop_stop = 12<<16;
        envelope_osc->loop_type = BAND;
        envelope_osc->state = STOPPED;

        struct Oscillator* sine_osc = oscillator_new();
        oscillator_load(sine_osc, &sine_wave);
        sine_osc->loop_type = FORWARD;
        sine_osc->state = STOPPED;

        operator->envelope = envelope_osc;
        operator->src_wave = sine_osc;
        operator->mode = OSCILLATOR;
        operator->volume = 2048;
        operator_set_frequency(operator, 400);
        voice_bank->voices[i] = operator;
    }
    voice_bank->n_voices = n_voices;
}

struct Operator* voices_acquire(struct VoiceBank* voice_bank){
    for (uint i=0; i<voice_bank->n_voices; i++){
        if (voice_bank->voices[i]->envelope->state == STOPPED) return voice_bank->voices[i];
    }
    return voice_bank->voices[0];
}

uint16_t voices_get_next_sample(struct VoiceBank* voice_bank){
    uint16_t sample = 0;
    for (int i=0;i<voice_bank->n_voices;i++){
        sample += operator_get_next_sample(voice_bank->voices[i]);
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