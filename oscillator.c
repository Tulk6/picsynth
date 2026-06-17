enum LoopType{
    NO_LOOP,
    FORWARD
};

enum PlayState{
    STOPPED,
    PLAYING
};

struct Oscillator{
    uint32_t step;
    uint32_t pos;
    uint32_t pos_start;
    uint32_t pos_stop;
    enum LoopType loop_type;
    enum PlayState state;
    struct WaveTable* wavetable;
};

void oscillator_init(struct Oscillator* oscillator){
    oscillator->pos = 0;
    oscillator->pos_start = 0;
    oscillator->pos_stop = 0;

    oscillator->step = 0;

    oscillator->loop_type = NO_LOOP;
    oscillator->state = PLAYING;
    
    oscillator->wavetable = NULL;
}

void oscillator_load(struct Oscillator* oscillator, struct WaveTable* wavetable){
    oscillator->pos_stop = wavetable->table_len << 16;
    oscillator->wavetable = wavetable;
}

void oscillator_set_pos(struct Oscillator* oscillator, uint32_t new_pos){
    if (new_pos >= oscillator->pos_stop){
        switch (oscillator->loop_type){
            case NO_LOOP:
                new_pos = oscillator->pos_stop-1;
                oscillator->state = STOPPED;
                break;
            case FORWARD:
                new_pos -= (oscillator->pos_stop-oscillator->pos_start);
                break;
        }
    }
    oscillator->pos = new_pos;
}

int16_t oscillator_get_next_sample(struct Oscillator* oscillator){
    if (oscillator->state == STOPPED) return 0;
    int16_t sample = (wavetable_get_sample(oscillator->wavetable, oscillator->pos >> 16));
    oscillator_set_pos(oscillator, oscillator->pos+oscillator->step);
    return sample;
}

void oscillator_get_samples(struct Oscillator* oscillator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = oscillator_get_next_sample(oscillator);
    }
}

void oscillator_set_frequency(struct Oscillator* oscillator, float frequency){
    oscillator->step = ((uint64_t) (frequency*65536)*oscillator->wavetable->table_len)/((uint32_t) SAMPLE_RATE);
}