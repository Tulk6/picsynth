enum OperatorMode {
    MODE_NONE,
    ADDITIVE,
    PHASE_MODULATION,
    RING_MODULATION,
    OSCILLATOR,
    CARRIER
};

struct Operator {
    struct Operator* carrier;
    struct Operator* modulator;
    struct Oscillator* src_wave;
    struct Oscillator* envelope;
    struct Filter* filter;
    enum OperatorMode mode;
    int16_t volume;
    int16_t intensity;
    float frequency_ratio;
    float frequency;
};

void operator_init(struct Operator* operator){
    operator->carrier = NULL;
    operator->modulator = NULL;
    operator->src_wave = NULL;
    operator->envelope = NULL;
    operator->filter = NULL;
    operator->mode = MODE_NONE;
    operator->volume = 0;
    operator->intensity = 0;
}

struct Operator* operator_new(){
    struct Operator* operator = (struct Operator*) malloc(sizeof(struct Operator));
    operator_init(operator);
    return operator;
}

void operator_load_oscillator(struct Operator* operator, struct Oscillator* oscillator){
    operator->carrier = NULL;
    operator->modulator = NULL;
    operator->src_wave = oscillator;
    operator->mode = OSCILLATOR;
}

void operator_unload(struct Operator* operator){
    if (operator->carrier != NULL){
        operator_unload(operator);
        operator->carrier = NULL;
    }
    if (operator->modulator != NULL){
        operator_unload(operator->modulator);
        operator->modulator = NULL;
    }
    if (operator->envelope != NULL){
        oscillator_unload(operator->envelope);
        operator->envelope = NULL;
    }
    if (operator->src_wave != NULL){
        oscillator_unload(operator->src_wave);
        operator->src_wave = NULL;
    }
    free(operator);
}

void operator_set_frequency(struct Operator* operator, float frequency){
    operator->frequency = frequency;
    switch (operator->mode){
        case OSCILLATOR:
            if (operator->src_wave != NULL){
                oscillator_set_frequency(operator->src_wave, frequency);
            }
            break;
        
        case CARRIER:
            if (operator->carrier != NULL){
                operator_set_frequency(operator->carrier, frequency);
            }
            break;
        
        case ADDITIVE:
        case RING_MODULATION:
        case PHASE_MODULATION:
            if ((operator->carrier != NULL) && (operator->modulator != NULL)){
                if (operator->frequency_ratio != 0){
                    float modulator_frequency = frequency*operator->frequency_ratio;
                    operator_set_frequency(operator->modulator, modulator_frequency);
                }
                operator_set_frequency(operator->carrier, frequency);
            }
            break;
    }
}

void operator_set_frequency_ratio(struct Operator* operator, float frequency_ratio){
    operator->frequency_ratio = frequency_ratio;
    operator_set_frequency(operator->modulator, operator->frequency*frequency_ratio);
    /*switch (operator->mode){       
        case ADDITIVE:
        case PHASE_MODULATION:
            if ((operator->carrier != NULL) && (operator->modulator != NULL)){
                operator_set_frequency(operator->modulator, 2*operator_get_frequency(operator->carrier));
            }
            break;
    }*/
}

void operator_set_pos(struct Operator* operator, uint32_t new_pos){
    if (operator->src_wave != NULL){
        oscillator_set_pos(operator->src_wave, new_pos);
    }else{
        operator_set_pos(operator->carrier, new_pos);
    }
}

void operator_change_pos(struct Operator* operator, int32_t delta_pos){
    if (operator->src_wave != NULL){
        oscillator_set_pos(operator->src_wave, operator->src_wave->pos+delta_pos);
    }else{
        operator_change_pos(operator->carrier, delta_pos);
    } 
}

void operator_start(struct Operator* operator){
    if (operator->carrier != NULL){
        operator_start(operator->carrier);
    }
    if (operator->modulator != NULL){
        operator_start(operator->modulator);
    }
    if (operator->envelope != NULL){
        operator->envelope->pos = 0;
        operator->envelope->state = PLAYING;
    }
    if (operator->src_wave != NULL){
        operator->src_wave->pos = 0;
        operator->src_wave->state = PLAYING;
    }
}

void operator_stop(struct Operator* operator){
    if (operator->envelope != NULL & operator->envelope->state == PLAYING){
        operator->envelope->state = STOPPING;
    }
}

int16_t operator_get_current_sample(struct Operator* operator){
    int16_t sample = 0;
    switch (operator->mode){
        case MODE_NONE:
            break;

        case OSCILLATOR:
            if (operator->src_wave != NULL){
                sample = oscillator_get_current_sample(operator->src_wave);
            }
            break;
        
        case ADDITIVE:
            if ((operator->carrier != NULL) && (operator->modulator != NULL)){
                int16_t carrier_level = operator_get_current_sample(operator->carrier);
                int16_t modulator_level = (operator->intensity * operator_get_current_sample(operator->modulator))>>15;
                sample = (carrier_level+modulator_level)>>1;
            }
            break;

        case PHASE_MODULATION:
            if ((operator->carrier != NULL) && (operator->modulator != NULL)){
                int16_t carrier_level = operator_get_current_sample(operator->carrier);
                int16_t modulator_level = operator_get_current_sample(operator->modulator);
                int32_t step_size = (operator->intensity*modulator_level)>>2;
                operator_change_pos(operator->carrier, step_size);
                sample = carrier_level;
            }
            break;

        case RING_MODULATION:
            if ((operator->carrier != NULL) && (operator->modulator != NULL)){
                int16_t carrier_level = operator_get_current_sample(operator->carrier);
                int16_t modulator_level = operator_get_current_sample(operator->modulator);
                sample = (carrier_level*modulator_level)>>15;
            }
            break;

        case CARRIER:
            if (operator->carrier != NULL){
                sample = operator_get_current_sample(operator->carrier);
            }
            break;
    }

    if (operator->filter != NULL){
        sample = filter_apply(operator->filter, sample, operator->intensity);
    }
    
    if (operator->envelope != NULL){
        int16_t envelope_level = abs(oscillator_get_current_sample(operator->envelope));
        operator->intensity = envelope_level;
        //sample = (sample*envelope_level) >> 15;
    }

    sample = (sample*operator->volume) >> 15;
    if (sample > operator->volume) sample = 0;
    return sample;
}

void operator_advance_sample(struct Operator* operator){
    if (operator->carrier != NULL){
        operator_advance_sample(operator->carrier);
    }
    if (operator->modulator != NULL && operator->modulator != operator->carrier){
        operator_advance_sample(operator->modulator);
    }
    if (operator->envelope != NULL){
        oscillator_advance_sample(operator->envelope);
    }
    if (operator->src_wave != NULL){
        oscillator_advance_sample(operator->src_wave);
    }
}

int16_t operator_get_next_sample(struct Operator* operator){
    int16_t sample = operator_get_current_sample(operator);
    operator_advance_sample(operator);
    return sample;
}

void operator_get_samples(struct Operator* operator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = operator_get_next_sample(operator);
    }
}