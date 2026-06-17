struct Operator {
    struct Oscillator* carrier;
    struct Oscillator* modulator;
    struct Oscillator* envelope;
    int16_t volume;
    int mod_strength;
};

int16_t operator_get_next_sample(struct Operator* operator){
    //raw sample is full volume i.e. -32767 to 32767
    int16_t sample = 0;
    int16_t carrier_level = oscillator_get_next_sample(operator->carrier);
    sample = carrier_level;
    //envelope level is 0-32767
    if (operator->modulator != NULL){
        //make modulation work in ratios of the base step size
        //i.e. perhaps full is double step size etc
        //or even reverse -> as approach zero, greater ratio
        int16_t modulator_level = oscillator_get_next_sample(operator->modulator);
        int32_t step_size = operator->mod_strength*((modulator_level*operator->carrier->step)>>15);
        oscillator_set_pos(operator->carrier, operator->carrier->pos+step_size);
        /*operator->carrier->pos += modulator_level;
        if (operator->carrier->pos > operator->carrier->pos_stop){
            operator->carrier->pos -= operator->carrier->pos_start+;
        }else{
            operator->carrier->pos += modulator_level;
            if (operator->carrier->pos > operator->carrier->pos_stop) operator->carrier->pos = operator->carrier->pos_start;
        }*/
        //printf("mod: %i\n", modulator_level);
        //sample = (sample+modulator_level)>>1;
    }

    if (operator->envelope != NULL){
        int16_t envelope_level = abs(oscillator_get_next_sample(operator->envelope));
        sample = (sample*envelope_level) >> 15;
    }

    sample = (sample*operator->volume) >> 15;
    if (sample > operator->volume) sample = 0;
    return sample;
}


void operator_get_samples(struct Operator* operator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = operator_get_next_sample(operator);
    }
}