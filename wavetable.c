struct WaveTable {
    int16_t* table; //table values from -32767 to 32767 -> 0x7fff -> 1<<15 (1 bit for sign)
    uint32_t table_len;
    float frequency;
    //uint32_t pos_max;
};

int16_t wavetable_get_sample(struct WaveTable* wavetable, uint32_t sample_n){
    hard_assert(sample_n < wavetable->table_len);
    if (sample_n < wavetable->table_len){
        return wavetable->table[sample_n];
    }else{
        return 0;
    }
}

int16_t wavetable_get_sample_interpolated(struct WaveTable* wavetable, uint32_t pos){
    uint32_t sample_n = pos >> 16;
    uint32_t sample_n2 = sample_n+1;
    int32_t crossfade = pos & 0xffff;
    if (sample_n2>=wavetable->table_len){
        return (wavetable_get_sample(wavetable, sample_n));
    }
    int32_t sample1 = (0xffff-crossfade)*wavetable_get_sample(wavetable, sample_n);
    int32_t sample2 = crossfade*wavetable_get_sample(wavetable, sample_n2);
    return (sample1+sample2)>>16;
}

void wavetable_init(struct WaveTable* wavetable){
    wavetable->table_len = 0;
    wavetable->table = NULL;
    wavetable->frequency = 0;
}

void wavetable_load(struct WaveTable* wavetable, uint32_t table_len){
    if (wavetable->table != NULL) free(wavetable->table);
    wavetable->table_len = table_len;
    wavetable->table = malloc(sizeof(int16_t)*table_len);
}

void wavetable_unload(struct WaveTable* wavetable){
    if (wavetable->table != NULL) free(wavetable->table);
    wavetable->table = NULL;
    wavetable->table_len = 0;
    wavetable->frequency = 0;
}

void wavetable_load_sample(struct WaveTable* wavetable, uint16_t* sample, uint32_t sample_len, float frequency){
    wavetable_load(wavetable, sample_len);
    wavetable->table = sample;
    wavetable->frequency = frequency;
}

void wavetable_load_sine(struct WaveTable* wavetable, uint32_t table_len){
    wavetable_load(wavetable, table_len);
    wavetable->frequency = 1;
    for (int i = 0; i < table_len; i++) {
        wavetable->table[i] =
            32767 * cosf(i * 2.0f * (float)M_PI / table_len);
    }
}

void wavetable_load_square(struct WaveTable* wavetable, uint32_t table_len){
    wavetable_load(wavetable, table_len);
    wavetable->frequency = 1;
    for (int i = 0; i < table_len; i++) {
        int16_t val;
        if (i < table_len>>1){
            val = 32767;
        }else{
            val = -32767;
        }
        wavetable->table[i] = val;
    }
}

void wavetable_load_ad(struct WaveTable* wavetable, uint32_t attack, uint32_t decay){
    uint32_t table_len = attack+decay;
    wavetable->frequency = 1;
    wavetable_load(wavetable, table_len);
    for (int i = 0; i < table_len; i++){
        int16_t val;
        if (i < attack){
            val = (i*32767)/attack;
        }else if (i < (attack+decay)){
            val = 32767-(32767*(i-attack))/(decay);
        }else{
            val = 0;
        }
        int16_t level = val;
        wavetable->table[i] = level;
    }
}

void wavetable_load_adsr(struct WaveTable* wavetable, uint32_t attack, uint32_t decay, uint16_t sustain, uint32_t release){
    int16_t max = INT16_MAX;
    uint32_t table_len = attack+decay+release+1;
    wavetable->frequency = 1;
    wavetable_load(wavetable, table_len);
    for (int i = 0; i < table_len; i++){
        int16_t val;
        if (i < attack){
            val = (i*max)/attack;
        }
        else if (i < (attack+decay)){
            val = max-((max-sustain)*(i-attack))/(decay);
        }else if (i == attack+decay){
            val = sustain;
        }else if (i < (attack+decay+release)){
            val = sustain-(sustain*(i-attack-decay))/release;
        }
        else{
            val = 0;
        }
        int16_t level = val;
        //printf("val %i\n", val);
        wavetable->table[i] = level;
    }
}