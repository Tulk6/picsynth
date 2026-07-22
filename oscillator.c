enum LoopType{
    NO_LOOP,
    FORWARD,
    BAND
};

enum PlayState{
    STOPPED,
    PLAYING,
    STOPPING
};

struct Oscillator{
    uint32_t step;
    uint32_t pos;
    uint32_t pos_start;
    uint32_t pos_stop;
    uint32_t loop_start;
    uint32_t loop_stop;
    enum LoopType loop_type;
    enum PlayState state;
    struct WaveTable* wavetable;
    float frequency;
};

void oscillator_init(struct Oscillator* oscillator){
    oscillator->pos = 0;
    oscillator->pos_start = 0;
    oscillator->pos_stop = 0;

    oscillator->step = 0;

    oscillator->loop_start = 0;
    oscillator->loop_stop = 0;

    oscillator->loop_type = NO_LOOP;
    oscillator->state = PLAYING;
    
    oscillator->wavetable = NULL;

    oscillator->frequency = 0;
}

void oscillator_load(struct Oscillator* oscillator, struct WaveTable* wavetable){
    oscillator->pos_stop = wavetable->table_len << 16;
    oscillator->wavetable = wavetable;
}

struct Oscillator* oscillator_new(){
    struct Oscillator* oscillator = (struct Oscillator*) malloc(sizeof(struct Oscillator));
    oscillator_init(oscillator);
    return oscillator;
}

void oscillator_unload(struct Oscillator* oscillator){
    //does not unload wavetables
    free(oscillator);
}

void oscillator_set_pos(struct Oscillator* oscillator, uint32_t new_pos){
    if (new_pos >= oscillator->pos_stop){
        switch (oscillator->loop_type){
            case BAND:
            case NO_LOOP:
                new_pos = oscillator->pos_stop-1;
                oscillator->state = STOPPED;
                break;
            case FORWARD:
                while (new_pos >= oscillator->pos_stop){
                    new_pos -= (oscillator->pos_stop-oscillator->pos_start);
                }
                break;
        }
    }else if (new_pos >= oscillator->loop_stop){
        switch (oscillator->loop_type){
            case BAND:
                if (oscillator->state == PLAYING){
                    new_pos = oscillator->loop_start;
                }
        }
    }
    oscillator->pos = new_pos;
}

int16_t oscillator_advance_sample(struct Oscillator* oscillator){
    oscillator_set_pos(oscillator, oscillator->pos+oscillator->step);
}

int16_t oscillator_get_current_sample(struct Oscillator* oscillator){
    if (oscillator->state == STOPPED) return 0;
    //int16_t sample = (wavetable_get_sample(oscillator->wavetable, oscillator->pos>>16));
    int16_t sample = (wavetable_get_sample_interpolated(oscillator->wavetable, oscillator->pos));
    return sample;
}

int16_t oscillator_get_next_sample(struct Oscillator* oscillator){
    int16_t sample = oscillator_get_current_sample(oscillator);
    oscillator_advance_sample(oscillator);
    return sample;
}

void oscillator_get_samples(struct Oscillator* oscillator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = oscillator_get_next_sample(oscillator);
    }
}

void oscillator_set_frequency(struct Oscillator* oscillator, float frequency){
    oscillator->frequency = frequency;
    oscillator->step = ((uint64_t) (frequency*65536)*oscillator->wavetable->table_len)/((uint32_t) (SAMPLE_RATE*oscillator->wavetable->frequency));
    //printf("freq: %f\n", frequency);
    //printf("step: %f\n", (float)oscillator->step/65536);
}

float oscillator_get_frequency(struct Oscillator* oscillator){
    return oscillator->frequency;
    //return ( (uint32_t) oscillator->step*SAMPLE_RATE*oscillator->wavetable->frequency)/((uint64_t) (65536*oscillator->wavetable->table_len));
}