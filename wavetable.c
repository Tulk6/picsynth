struct WaveTable {
    int16_t* table; //table values from -32767 to 32767 -> 0x7fff -> 1<<15 (1 bit for sign)
    uint32_t table_len;
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

void wavetable_init(struct WaveTable* wavetable){
    wavetable->table_len = 0;
    wavetable->table = NULL;
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
}

void wavetable_load_sine(struct WaveTable* wavetable, uint32_t table_len){
    wavetable_load(wavetable, table_len);
    for (int i = 0; i < table_len; i++) {
        wavetable->table[i] =
            32767 * cosf(i * 2.0f * (float)M_PI / table_len);
    }
}

void wavetable_load_square(struct WaveTable* wavetable, uint32_t table_len){
    wavetable_load(wavetable, table_len);
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