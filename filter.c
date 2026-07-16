enum FilterType {
    TEST,
    OTHER,
    NONE
};

struct Filter {
    int16_t prev_sample;
    enum FilterType type;
};

void filter_init(struct Filter* filter){
    filter->prev_sample = 0;
    filter->type = TEST;
}

struct Filter* filter_new(){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter);
    return filter;
}

int16_t filter_apply(struct Filter* filter, int16_t sample, uint16_t cutoff){
    switch (filter->type){
        case TEST:
            if (abs(sample - filter->prev_sample) > cutoff){
                if (sample < filter->prev_sample){
                    sample = filter->prev_sample-cutoff;
                    filter->prev_sample = sample;
                    return sample;
                }else{
                    sample = filter->prev_sample+cutoff;
                    filter->prev_sample = sample;
                    return sample;
                }
            }
            break;
        case OTHER:
            if (abs(sample - filter->prev_sample) < cutoff){
                if (sample < filter->prev_sample){
                    sample = filter->prev_sample-cutoff;
                    filter->prev_sample = sample;
                    return sample;
                }else{
                    sample = filter->prev_sample+cutoff;
                    filter->prev_sample = sample;
                    return sample;
                }
            }
            break;
    }
    
    filter->prev_sample = sample;
    return sample;
}