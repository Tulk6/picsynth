#define FILTER_LENGTH 7

enum FilterType {
    LOW_PASS,
    HIGH_PASS,
    NONE
};

struct Filter {
    enum FilterType type;
    int16_t* samples;
    int8_t n_samples;
    struct Oscillator* intensity_oscillator;
    int16_t intensity_value;
    int16_t param_a;
    int16_t param_b;
};

void filter_init(struct Filter* filter, enum FilterType type){
    filter->samples = NULL;
    filter->n_samples = 0;
    filter->type = type;
    filter->intensity_oscillator = NULL;
    filter->intensity_value = 0;
    filter->param_a = 0;
    filter->param_b = 0;

    switch (type){
        case LOW_PASS:
        case HIGH_PASS:
            filter->n_samples = 2;
            filter->samples = calloc(filter->n_samples, sizeof(int16_t));
            break;
    }
}

struct Filter* filter_new(enum FilterType type){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter, type);
    return filter;
}

int16_t filter_get_intensity(struct Filter* filter){
    if (filter->intensity_oscillator != NULL){
        return oscillator_get_current_sample(filter->intensity_oscillator);
    }
    return filter->intensity_value;
}

void filter_append_sample(struct Filter* filter, int16_t sample){
    for (int8_t i=0; i < (filter->n_samples-1); i++){
        filter->samples[i+1] = filter->samples[i];
    }
    filter->samples[0] = sample;
}

int16_t filter_apply(struct Filter* filter, int16_t sample){
    int64_t new_sample = 0;
    int16_t intensity = filter_get_intensity(filter);
    switch (filter->type){
        case LOW_PASS:
            new_sample = ((intensity*sample)>>15) + (((32767-intensity)*filter->samples[0])>>15);
            break;
        case HIGH_PASS:
            new_sample = ((intensity*sample)>>15) - (((32767-intensity)*filter->samples[0])>>15);
            break;
    }
    filter_append_sample(filter, new_sample);
    return new_sample;
}