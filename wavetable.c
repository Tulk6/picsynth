struct WaveTable {
    int16_t* table;
    uint32_t table_len;
    uint32_t pos_max;
};

int16_t wavetable_get_sample(struct WaveTable* wavetable, uint32_t sample){
    assert(sample < wavetable->table_len);
    return wavetable->table[sample];
}

void load_wavetable_sine(struct WaveTable* wavetable, uint32_t table_len){
    wavetable->table_len = table_len;
    wavetable->pos_max = table_len<<16; 
    wavetable->table = malloc(sizeof(int16_t)*table_len);
    for (int i = 0; i < table_len; i++) {
        wavetable->table[i] =
            32767 * cosf(i * 2.0f * (float)M_PI / table_len);
    }
}

void load_wavetable_square(struct WaveTable* wavetable, uint32_t table_len){
    wavetable->table_len = table_len;
    wavetable->pos_max = table_len<<16; 
    wavetable->table = malloc(sizeof(int16_t)*table_len);
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