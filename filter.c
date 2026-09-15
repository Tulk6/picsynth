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
    int16_t intensity_value;
};

void filter_init(struct Filter* filter){
    filter->prev_sample = 0;
    filter->type = TEST;
    filter->intensity_oscillator = NULL;
    filter->intensity_value = 0;
}

struct Filter* filter_new(){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter);
    return filter;
}

int16_t filter_get_intensity(struct Filter* filter){
    if (filter->intensity_oscillator != NULL){
        return oscillator_get_current_sample(filter->intensity_oscillator);
    }
    return filter->intensity_value;
}

int16_t filter_apply(struct Filter* filter, int16_t sample){
    int64_t new_sample = 0;
    int16_t intensity = filter_get_intensity(filter);
    switch (filter->type){
        case TEST:
            new_sample = ((intensity*sample)>>15) + (((32767-intensity)*filter->prev_sample)>>15);
            break;
    }
    filter->prev_sample = new_sample;
    return new_sample;
}