enum OperatorMode {
    MODE_NONE,
    ADDITIVE,
    PHASE_MODULATION,
    PULSE_MODULATION,
    RING_MODULATION,
    CARRIER
};

struct Operator {
    struct Operator* carrier_operator;
    struct Operator* modulator_operator;
    struct Oscillator* carrier_oscillator;
    struct Oscillator* modulator_oscillator;
    struct Oscillator* envelope_oscillator;
    struct Filter* filter;
    enum OperatorMode mode;
    bool gate_on;
    int16_t volume;
    int16_t mix;
    int16_t intensity;
    float frequency_ratio;
    float frequency;
};

int16_t operator_get_current_sample(struct Operator* operator);

void operator_init(struct Operator* operator){
    operator->carrier_operator = NULL;
    operator->modulator_operator = NULL;
    operator->carrier_oscillator = NULL;
    operator->modulator_oscillator = NULL;
    operator->envelope_oscillator = NULL;
    operator->gate_on = false;
    operator->filter = NULL;
    operator->mode = MODE_NONE;
    operator->volume = 0;
    operator->mix = 0;
    operator->intensity = 0;
}

struct Operator* operator_new(){
    struct Operator* operator = (struct Operator*) malloc(sizeof(struct Operator));
    operator_init(operator);
    return operator;
}

void operator_load_oscillator(struct Operator* operator, struct Oscillator* oscillator){
    operator->carrier_operator = NULL;
    operator->modulator_operator = NULL;
    operator->carrier_oscillator = oscillator;
    operator->mode = CARRIER;
}

void operator_unload(struct Operator* operator){
    if (operator->carrier_operator != NULL){
        operator_unload(operator->carrier_operator);
        operator->carrier_operator = NULL;
    }
    if (operator->modulator_operator != NULL){
        operator_unload(operator->modulator_operator);
        operator->modulator_operator = NULL;
    }
    if (operator->envelope_oscillator != NULL){
        oscillator_unload(operator->envelope_oscillator);
        operator->envelope_oscillator = NULL;
    }
    if (operator->carrier_oscillator != NULL){
        oscillator_unload(operator->carrier_oscillator);
        operator->carrier_oscillator = NULL;
    }
    if (operator->modulator_oscillator != NULL){
        oscillator_unload(operator->modulator_oscillator);
        operator->modulator_oscillator = NULL;
    }
    free(operator);
}



int16_t operator_get_carrier_sample(struct Operator* operator){
    if (operator->carrier_operator != NULL){
        return operator_get_current_sample(operator->carrier_operator);
    }else if (operator->carrier_oscillator != NULL){
        return oscillator_get_current_sample(operator->carrier_oscillator);
    }
    return 0;
}

int16_t operator_get_modulator_sample(struct Operator* operator){
    if (operator->modulator_operator != NULL){
        return operator_get_current_sample(operator->modulator_operator);
    }else if (operator->modulator_oscillator != NULL){
        return oscillator_get_current_sample(operator->modulator_oscillator);
    }
    return 0;
}

int16_t operator_get_envelope_sample(struct Operator* operator){
    if (operator->gate_on == true){
        if (operator->envelope_oscillator != NULL){
            return abs(oscillator_get_current_sample(operator->envelope_oscillator));
        }
        return INT16_MAX;
    }
    return 0;
}

void operator_set_frequency(struct Operator* operator, float frequency){
    operator->frequency = frequency;
    float modulator_frequency = frequency*operator->frequency_ratio;
    printf("ratio: %f\n", operator->frequency_ratio);
    printf("car freq: %f\n", frequency);
    printf("mod freq: %f\n", modulator_frequency);
    switch (operator->mode){
        case CARRIER:
            if (operator->carrier_operator != NULL){
                operator_set_frequency(operator->carrier_operator, frequency);
            }else if (operator->carrier_oscillator != NULL){
                oscillator_set_frequency(operator->carrier_oscillator, frequency);
            }
            break;
        
        case PULSE_MODULATION:
        case ADDITIVE:
        case RING_MODULATION:
        case PHASE_MODULATION:
            if (operator->carrier_operator != NULL){
                operator_set_frequency(operator->carrier_operator, frequency);
            }
            if (operator->carrier_oscillator != NULL){
                oscillator_set_frequency(operator->carrier_oscillator, frequency);
            }
            if (operator->frequency_ratio != 0){
                if (operator->modulator_operator != NULL){
                    operator_set_frequency(operator->modulator_operator, modulator_frequency);
                }
                if (operator->modulator_oscillator != NULL){
                    oscillator_set_frequency(operator->modulator_oscillator, modulator_frequency);
                }
            }
            break;
    }
}

void operator_set_frequency_ratio(struct Operator* operator, float frequency_ratio){
    operator->frequency_ratio = frequency_ratio;

    if (operator->frequency_ratio != 0){
        float modulator_frequency = operator->frequency*operator->frequency_ratio;
        if (operator->modulator_operator != NULL){
            operator_set_frequency(operator->modulator_operator, modulator_frequency);
        }
        if (operator->modulator_oscillator != NULL){
            oscillator_set_frequency(operator->modulator_oscillator, modulator_frequency);
        }
    }
}

uint32_t operator_get_pos(struct Operator* operator){
    if (operator->carrier_oscillator != NULL){
        return operator->carrier_oscillator->pos;
    }else if (operator->carrier_operator != NULL){
        return operator_get_pos(operator->carrier_operator);
    }
}

void operator_set_pos(struct Operator* operator, uint32_t new_pos){
    if (operator->carrier_oscillator != NULL){
        oscillator_set_pos(operator->carrier_oscillator, new_pos);
    }else if (operator->carrier_operator != NULL){
        operator_set_pos(operator->carrier_operator, new_pos);
    }
}

void operator_change_pos(struct Operator* operator, int32_t delta_pos){
    if (operator->carrier_oscillator != NULL){
        oscillator_set_pos(operator->carrier_oscillator, operator->carrier_oscillator->pos+delta_pos);
    }else if (operator->carrier_operator != NULL){
        operator_change_pos(operator->carrier_operator, delta_pos);
    } 
    if (operator->modulator_oscillator != NULL){
        oscillator_set_pos(operator->modulator_oscillator, operator->modulator_oscillator->pos+delta_pos);
    }else if (operator->modulator_operator != NULL){
        operator_change_pos(operator->modulator_operator, delta_pos);
    }
}

/*void operator_start(struct Operator* operator){
    printf("1");
    if (operator->carrier_operator != NULL){
        operator_start(operator->carrier_operator);
    }
    printf("2");
    if (operator->modulator_operator != NULL){
        operator_start(operator->modulator_operator);
    }
    printf("3");
    if (operator->envelope != NULL){
        operator->envelope->pos = 0;
        operator->envelope->state = PLAYING;
    }
    printf("4");
    if (operator->carrier_oscillator != NULL){
        
    }
    if (operator->modulator_oscillator != NULL){
        operator->modulator_oscillator->pos = 0;
        operator->modulator_oscillator->state = PLAYING;
    }
    printf("5");
}*/

/*void operator_stop(struct Operator* operator){
    if (operator->envelope != NULL && operator->envelope->state == PLAYING){
        operator->envelope->state = STOPPING;
    }
    if (operator->carrier_operator != NULL){
        operator_stop(operator->carrier_operator);
    }
    if (operator->carrier_oscillator != NULL){
        operator->carrier_oscillator->state = STOPPED;
    }
    if (operator->modulator_oscillator != NULL){
        operator->modulator_oscillator->state = STOPPED;
    }

    if (operator->modulator_operator != NULL){
        operator_stop(operator->modulator_operator);
    }
}*/

int16_t operator_get_current_sample(struct Operator* operator){
    int16_t sample = 0;
    int16_t carrier_level = operator_get_carrier_sample(operator);
    int16_t modulator_level = operator_get_modulator_sample(operator);
    switch (operator->mode){
        case MODE_NONE:
            break;
        
        case ADDITIVE:
            int16_t adjusted_modulator_level = (operator->intensity * modulator_level)>>15;
            sample = (carrier_level+modulator_level)>>1;
            break;

        case PHASE_MODULATION:
            int32_t step_size = 0;
            step_size = (operator->intensity*modulator_level)>>2;
            if (operator->carrier_operator != NULL) operator_change_pos(operator->carrier_operator, step_size);
            if (operator->carrier_oscillator != NULL) oscillator_set_pos(operator->carrier_oscillator, operator->carrier_oscillator->pos+step_size);
            sample = carrier_level;
            break;

        case RING_MODULATION:
            sample = (carrier_level*modulator_level)>>15;
            break;

        case PULSE_MODULATION:
            int16_t pos = 0;
            if (operator->carrier_operator != NULL){
                pos = operator_get_pos(operator->carrier_operator)>>16;
            }else if (operator->carrier_oscillator != NULL){
                pos = operator->carrier_oscillator->pos>>16;
            }

            if (pos < operator->intensity){
                sample = carrier_level;
            }else{
                sample = modulator_level;
            }
            break;

        case CARRIER:
            sample = carrier_level;
            break;
    }

    if (operator->filter != NULL){
        sample = filter_apply(operator->filter, sample);
    }
    
    int16_t envelope_level = operator_get_envelope_sample(operator);//abs(oscillator_get_current_sample(operator->envelope));
        sample = (sample*envelope_level) >> 15;

    sample = (sample*operator->volume) >> 15;
    if (sample > operator->volume) sample = 0;
    //printf("sample %i\t", sample);
    return sample;
}

int16_t operator_get_mix_sample(struct Operator* operator){
    return (operator_get_current_sample(operator)*operator->mix)>>15;
}

void operator_gate_on(struct Operator* operator){
    operator->gate_on = true;
}

void operator_gate_off(struct Operator* operator){
    operator->gate_on = false;
}

/*void operator_advance_sample(struct Operator* operator){
    if (operator->carrier_operator != NULL){
        operator_advance_sample(operator->carrier_operator);
    }
    if (operator->modulator_operator != NULL && operator->modulator_operator != operator->carrier_operator){
        operator_advance_sample(operator->modulator_operator);
    }
    if (operator->envelope != NULL){
        oscillator_advance_sample(operator->envelope);
    }
    if (operator->carrier_oscillator != NULL){
        oscillator_advance_sample(operator->carrier_oscillator);
    }
    if (operator->modulator_oscillator != NULL){
        oscillator_advance_sample(operator->modulator_oscillator);
    }
}*/

/*int16_t operator_get_next_sample(struct Operator* operator){
    int16_t sample = operator_get_current_sample(operator);
    operator_advance_sample(operator);
    return sample;
}*/

/*void operator_get_samples(struct Operator* operator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = operator_get_next_sample(operator);
    }
}*/