#define FILTER_LENGTH 7

enum FilterType {
    TEST,
    OTHER,
    NONE
};

struct Filter {
    enum FilterType type;
    int16_t prev_sample;
    int16_t value;
};

void filter_init(struct Filter* filter){
    filter->prev_sample = 0;
    filter->type = TEST;
    filter->value = 0;
}

struct Filter* filter_new(){
    struct Filter* filter = malloc(sizeof(struct Filter));
    filter_init(filter);
    return filter;
}

int16_t filter_apply(struct Filter* filter, int16_t sample, int16_t cutoff){
    int64_t new_sample = 0;
    switch (filter->type){
        case TEST:
            new_sample = ((filter->value*sample)>>15) + (((32767-filter->value)*filter->prev_sample)>>15);
            break;
    }
    filter->prev_sample = new_sample;
    return new_sample;
}