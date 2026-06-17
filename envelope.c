struct Envelope {
    uint vol;
    uint32_t attack;
    uint32_t decay;
    uint32_t pos;
};

uint envelope_get_sample(struct Envelope* envelope, uint32_t sample_n){
    uint16_t level;
    if (sample_n < envelope->attack){
        level = (sample_n*65535)/envelope->attack;
    }else if (sample_n < (envelope->attack+envelope->decay)){
        level = 65535-(65535*(sample_n-envelope->attack))/(envelope->decay);
    }else{
        level = 0;
    }

    uint vol = (level*envelope->vol)>>16;
    //printf("vol: %u", vol);

    return vol;
}

uint envelope_get_next_sample(struct Envelope* envelope){
    uint envelope_level = envelope_get_sample(envelope, envelope->pos);
    if (envelope_level>envelope->vol) return 0;
    if (envelope->pos < envelope->decay+envelope->attack){
        envelope->pos += 1;
    }
    return envelope_level;
}