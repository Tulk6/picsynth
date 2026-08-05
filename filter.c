#define FILTER_LENGTH 7

enum FilterType {
    TEST,
    OTHER,
    NONE
};

struct Filter {
    enum FilterType type;
    int16_t prev_sample;
    struct Oscillator* intensity_oscillator;
};

void filter_init(struct Filter* filter){
    filter->prev_sample = 0;
    filter->type = TEST;
    filter->intensity_oscillator = NULL;
}

struct Filter* filter_new(){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter);
    return filter;
}

int16_t filter_apply(struct Filter* filter, int16_t sample){
    int64_t new_sample = 0;
    int16_t intensity = 0;
    if (filter->intensity_oscillator != NULL){
        
    } intensity = oscillator_get_current_sample(filter->intensity_oscillator);
    //if (intensity != 0) printf("Filter: %i\n", intensity);
    switch (filter->type){
        case TEST:
            new_sample = ((intensity*sample)>>15) + (((32767-intensity)*filter->prev_sample)>>15);
            break;
    }
    filter->prev_sample = new_sample;
    return new_sample;
}