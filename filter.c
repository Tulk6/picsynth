#define FILTER_LENGTH 7

enum FilterType {
    TEST,
    OTHER,
    NONE
};

struct Filter {
    int16_t* samples;
    enum FilterType type;
    uint16_t filter_size; 
    uint16_t n_samples;
};

void filter_init(struct Filter* filter){
    filter->filter_size = FILTER_LENGTH;
    filter->samples = calloc(filter->filter_size, sizeof(int16_t));
    filter->type = TEST;
    filter->n_samples = FILTER_LENGTH;
}

struct Filter* filter_new(){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter);
    return filter;
}

void filter_add_sample(struct Filter* filter, int16_t sample){
    for (int i=filter->filter_size-1; i>1; i--){
        filter->samples[i] = filter->samples[i-1];
    }
    filter->samples[0] = sample;
}

int16_t filter_apply(struct Filter* filter, int16_t sample, int16_t cutoff){
    int64_t new_sample = 0;
    switch (filter->type){
        case TEST:
            int16_t prev_sample;
            new_sample += sample;
            for (int i=0;i<filter->n_samples;i++){
                prev_sample = filter->samples[i];
                new_sample += prev_sample;
            }
            new_sample = new_sample >> 3;
            break;
    }
    filter_add_sample(filter, new_sample);
    return new_sample;
}