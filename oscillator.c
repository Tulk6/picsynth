struct Oscillator{
    uint vol;
    uint32_t step;
    uint32_t pos;
    uint32_t start_pos;
    uint32_t pos_max;
    struct WaveTable* wavetable;
};

void oscillator_get_samples(struct Oscillator* oscillator, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] =
            (oscillator->vol * wavetable_get_sample(oscillator->wavetable, oscillator->pos >> 16)) >> 8;

        oscillator->pos += oscillator->step;

        if (oscillator->pos >= oscillator->pos_max){
            oscillator->pos = oscillator->start_pos;
            //oscillator->pos -= oscillator->pos_max;
        }
    }
}

void oscillator_set_frequency(struct Oscillator* oscillator, int frequency){
    oscillator->step = ((frequency*WAVE_TABLE_LEN)/(SAMPLE_RATE))<<16;
}