struct VoiceBank {
    struct Voice** voices;
    uint8_t n_voices;
};


//hmmm
//each voice 3 operators
//3 oscillators
//2 envelopes
//1 lfo

struct Voice {
    uint8_t trigger;
    uint8_t group;
    
    struct Operator* output_operator;
    
    struct Operator* operator_a;
    struct Operator* operator_b;
    struct Operator* operator_c;
    struct Operator* operator_d;
    struct Operator* operator_e;

    struct Oscillator* oscillator_a;
    struct Oscillator* oscillator_b;
    struct Oscillator* oscillator_c;
    struct Oscillator* oscillator_d;
    struct Oscillator* oscillator_e;
    struct Oscillator* oscillator_f;

    struct Oscillator* envelope_a;
    struct Oscillator* envelope_b;

    struct Oscillator* lfo_a;

    struct Filter* filter_a;
};

void voices_init(struct VoiceBank* voice_bank){
    voice_bank->voices = NULL;
    voice_bank->n_voices = 0;
}

void voice_init(struct Voice* voice){
    voice->trigger = 0;
    voice->group = 0;

    voice->output_operator = NULL;

    voice->operator_a = NULL;
    voice->operator_b = NULL;
    voice->operator_c = NULL;
    voice->operator_d = NULL;
    voice->operator_e = NULL;

    voice->oscillator_a = NULL;
    voice->oscillator_b = NULL;
    voice->oscillator_c = NULL;
    voice->oscillator_d = NULL;
    voice->oscillator_e = NULL;
    voice->oscillator_f = NULL;

    voice->envelope_a = NULL;
    voice->envelope_b = NULL;

    voice->lfo_a = NULL;

    voice->filter_a = NULL;
}

void voice_load(struct Voice* voice){
    voice->output_operator = NULL;

    voice->operator_a = operator_new();
    voice->operator_b = operator_new();
    voice->operator_c = operator_new();
    voice->operator_d = operator_new();
    voice->operator_e = operator_new();

    voice->oscillator_a = oscillator_new();
    voice->oscillator_b = oscillator_new();
    voice->oscillator_c = oscillator_new();
    voice->oscillator_d = oscillator_new();
    voice->oscillator_e = oscillator_new();
    voice->oscillator_f = oscillator_new();

    voice->envelope_a = oscillator_new();
    oscillator_load(voice->envelope_a, wavetable_new());
    //voice->envelope_a->loop_type = BAND;
    voice->envelope_b = oscillator_new();
    oscillator_load(voice->envelope_b, wavetable_new());
    //voice->envelope_b->loop_type = BAND;

    voice->lfo_a = oscillator_new();

    voice->filter_a = filter_new();
}

struct Voice* voice_new(){
    struct Voice* voice = malloc(sizeof(struct Voice));
    voice_init(voice);
    return voice;
}

void voices_unload(struct VoiceBank* voice_bank){
    for (int i=0;i<voice_bank->n_voices;i++){
        operator_unload(voice_bank->voices[i]->output_operator);
    }
    free(voice_bank->voices);
    voice_bank->n_voices = 0;
    voice_bank->voices = NULL;
}

void voices_load(struct VoiceBank* voice_bank, uint n_voices){
    if (voice_bank->n_voices != 0) voices_unload(voice_bank);
    voice_bank->voices = malloc(sizeof(struct Voice*)*n_voices);
    for (int i=0;i<n_voices;i++){
        voice_bank->voices[i] = voice_new();
        voice_load(voice_bank->voices[i]);

        /*struct Voice* voice = voice_bank->voices[i];
        oscillator_load(voice->oscillator_a, &sample_wave);
        voice->oscillator_a->loop_type = FORWARD;
        voice->oscillator_a->state = STOPPED;
        voice->oscillator_b->loop_type = FORWARD;
        voice->oscillator_b->state = STOPPED;
        voice->operator_a->carrier_oscillator = voice->oscillator_a;
        voice->operator_a->volume = 4096;
        voice->operator_a->mode = CARRIER;
        oscillator_load(voice->envelope_a, &envelope);
        voice->operator_a->envelope = voice->envelope_a;
        voice->operator_a->envelope->loop_start =100<<16;
        voice->operator_a->envelope->loop_stop = 101<<16;
        oscillator_set_frequency(voice->envelope_a, 8);
        voice->operator_a->envelope->state = STOPPED;
        voice->envelope_a->loop_type = BAND;
        voice->output_operator = voice->operator_a;*/
    }
    voice_bank->n_voices = n_voices;
}

void voices_set_intensity(struct VoiceBank* voice_bank, int16_t intensity){
    for (uint i=0; i<voice_bank->n_voices; i++){
        if (voice_bank->voices[i]->output_operator == NULL) continue;
        voice_bank->voices[i]->output_operator->intensity = intensity;
    }
}

void voices_set_mode(struct VoiceBank* voice_bank, enum OperatorMode mode){
    for (uint i=0; i<voice_bank->n_voices; i++){
        if (voice_bank->voices[i]->output_operator == NULL) continue;
        voice_bank->voices[i]->output_operator->mode = mode;
    }
}

void voices_set_frequency_ratio(struct VoiceBank* voice_bank, float frequency_ratio){
    for (uint i=0; i<voice_bank->n_voices; i++){
        if (voice_bank->voices[i]->output_operator == NULL) continue;
        operator_set_frequency_ratio(voice_bank->voices[i]->output_operator, frequency_ratio);
    } 
}

struct Voice* voices_acquire(struct VoiceBank* voice_bank){
    for (uint i=0; i<voice_bank->n_voices; i++){
        struct Voice* voice = voice_bank->voices[i];
        if (voice->output_operator != NULL && voice->output_operator->envelope->state == STOPPED) return voice_bank->voices[i];
    }
    return voice_bank->voices[0];
}

uint16_t voices_get_next_sample(struct VoiceBank* voice_bank){
    uint16_t sample = 0;
    for (int i=0;i<voice_bank->n_voices;i++){
        if (voice_bank->voices[i]->output_operator != NULL){
            sample += operator_get_next_sample(voice_bank->voices[i]->output_operator);
            sample >> 1;
        }
    } 
    //printf("sample %"PRIu16 "\n", sample);
    return sample;
}

void voices_get_samples(struct VoiceBank* voice_bank, int16_t* samples, uint n_samples){
    for (uint i = 0; i < n_samples; i++) {
        samples[i] = voices_get_next_sample(voice_bank);
    }
}


void* voice_algorithm_node_name(struct Voice* voice, enum VoiceValue node_name){
    printf("hmm, what is a %i...\n", node_name);
    void* node = NULL;
    switch (node_name){
        case NO_NODE:
            break;
        
        case OSCILLATOR_A:
            node = voice->oscillator_a;
            break;
        
        case OSCILLATOR_B:
            node = voice->oscillator_b;
            break;

        case OSCILLATOR_C:
            node = voice->oscillator_c;
            break;

        case OSCILLATOR_D:
            node = voice->oscillator_d;
            break;

        case OSCILLATOR_E:
            node = voice->oscillator_e;
            break;

        case OSCILLATOR_F:
            node = voice->oscillator_f;
            break;

        case OPERATOR_A:
            node = voice->operator_a;
            break;

        case OPERATOR_B:
            node = voice->operator_b;
            break;

        case OPERATOR_C:
            node = voice->operator_c;
            break;

        case OPERATOR_D:
            node = voice->operator_d;
            break;

        case OPERATOR_E:
            node = voice->operator_e;
            break;

        case ENVELOPE_A:
            node = voice->envelope_a;
            break;

        case ENVELOPE_B:
            node = voice->envelope_b;
            break;

        case LFO_A:
            node = voice->lfo_a;
            break;

        case FILTER_A:
            node = voice->filter_a;
            break;

        case SINE_WAVE:
            node = &sine_wave;
            break;

        case SQUARE_WAVE:
            node = &square_wave;
            break;

        case SAW_WAVE:
            node = &saw_wave;
            break;

        case TRIANGLE_WAVE:
            node = &triangle_wave;
            break;

        case SAMPLE_WAVE:
            node = &sample_wave;
            break;

        case HIGH_WAVE:
            node = &high_wave;
            break;

        case LOW_WAVE:
            node = &low_wave;
            break;

        case NOISE_WAVE:
            node = &noise_wave;
            break;

        case VOICE:
            node = voice;
            break;
    }

    return node;
}


void voice_load_algorithm(struct Voice* voice, struct Algorithm* algorithm){
    for (int i=0; i<N_ALGORITHM_SETTINGS; i++){
        printf("LOOKING AT SETTING %i\n", i);
        struct AlgorithmSetting setting = algorithm->settings[i];
        if (setting.node == NO_NODE || setting.parameter == NO_PARAMETER) continue;
        printf("finding node...\n");
        void* node = voice_algorithm_node_name(voice, setting.node);
        if (node == NULL) continue;
        printf("finding parameter\n");
        enum VoiceParameter parameter = setting.parameter;
        void* value_node = voice_algorithm_node_name(voice, setting.value_node);
        switch (parameter){
            case OUTPUT_OPERATOR:
                voice->output_operator = value_node;

            case CARRIER_OPERATOR:
                ((struct Operator*) node)->carrier_operator = (struct Operator*) value_node;
                break;

            case CARRIER_OSCILLATOR:
                ((struct Operator*) node)->carrier_oscillator = (struct Oscillator*) value_node;
                break;

            case MODULATOR_OPERATOR:
                ((struct Operator*) node)->modulator_operator = (struct Operator*) value_node;
                break;

            case MODULATOR_OSCILLATOR:
                ((struct Operator*) node)->modulator_oscillator = (struct Oscillator*) value_node;
                break;

            case ENVELOPE_OSCILLATOR:
                ((struct Operator*) node)->envelope = (struct Oscillator*) value_node;
                break;

            case OPERATOR_FUNCTION:
                ((struct Operator*) node)->mode = setting.value_mode;
                break;

            case FUNCTION_INTENSITY:
                ((struct Operator*) node)->intensity = setting.value_int16;
                break;

            case VOLUME:
                ((struct Operator*) node)->volume = setting.value_int16;
                break;

            case FREQUENCY_RATIO:
                ((struct Operator*) node)->frequency_ratio = setting.value_float;
                break;

            case OPERATOR_FREQUENCY:
                operator_set_frequency((struct Operator*) node, setting.value_float);
                break;

            case FILTER:
                ((struct Operator*) node)->filter = value_node;
                break;

            case OSCILLATOR_FREQUENCY:
                oscillator_set_frequency((struct Oscillator*) node, setting.value_float);
                break;

            case WAVEFORM:
                oscillator_load(((struct Oscillator*) node), value_node);
                break;

            case LOOP_TYPE:
                ((struct Oscillator*) node)->loop_type = setting.value_loop;
                break;

            case LOOP_START:
                ((struct Oscillator*) node)->loop_start = setting.value_int16;
                break;

            case LOOP_STOP:
                ((struct Oscillator*) node)->loop_stop = setting.value_int16;
                break;

            case ADSR:
                wavetable_load_adsr_int(((struct Oscillator*) node)->wavetable, setting.value_uint32);
                oscillator_reload((struct Oscillator*) node);
                oscillator_set_frequency((struct Oscillator*) node, 1);
                break;


            case FILTER_TYPE:
                ((struct Filter*) node)->type = setting.value_filter;
                break;
            
            case FILTER_INTENSITY:
                ((struct Filter*) node)->intensity_oscillator = value_node;
                break;
        }
    }
}

void voices_load_algorithm(struct VoiceBank* voice_bank, struct Algorithm* algorithm){
    for (int i=0;i<voice_bank->n_voices;i++){
        if (voice_bank->voices[i] != NULL){
            voice_load_algorithm(voice_bank->voices[i], algorithm);
        } 
    }
}


/*if (setting.node != NO_NODE){
            if (setting.node <= OPERATOR_E){ //then is an operator
                
            }else if (setting.node <= LFO_A){ // then is an oscillator

            }else if (setting.node == FILTER_A){ //u get it
                
            }else if (setting.node <= SAMPLE_WAVE){ //hopefully

            }
        }*/




/*
struct Operator* operator = operator_new();

        struct Oscillator* envelope_osc = oscillator_new();
        oscillator_load(envelope_osc, &envelope);
        oscillator_set_frequency(envelope_osc, 0.25);
        envelope_osc->loop_start =10<<16;
        envelope_osc->loop_stop = 11<<16;
        envelope_osc->loop_type = BAND;
        envelope_osc->state = STOPPED;

        struct Oscillator* sample_osc = oscillator_new();
        oscillator_load(sample_osc, &sample_table);
        sample_osc->loop_type = FORWARD;
        sample_osc->state = STOPPED;

        struct Oscillator* sine_osc = oscillator_new();
        oscillator_load(sine_osc, &sine_wave);
        sine_osc->loop_type = FORWARD;
        sine_osc->state = STOPPED;

        struct Oscillator* square_osc = oscillator_new();
        oscillator_load(square_osc, &square_wave);
        square_osc->loop_type = FORWARD;
        square_osc->state = STOPPED;

        struct Operator* sine_op = operator_new();
        sine_op->mode = OSCILLATOR;
        sine_op->volume = 32767;
        operator_load_oscillator(sine_op, sine_osc);
        operator_set_frequency(sine_op, 10);

        struct Operator* square_op = operator_new();
        square_op->mode = OSCILLATOR;
        square_op->volume = 32767;
        operator_load_oscillator(square_op, square_osc);
        operator_set_frequency(square_op, 10);

        struct Operator* sample_op = operator_new();
        sample_op->mode = PHASE_MODULATION;
        sample_op->carrier = sine_op;
        sample_op->modulator = square_op;
        sample_op->intensity = 32767;
        sample_op->frequency_ratio = 0.5;
        //operator_load_oscillator(sample_op, sample_osc);
        sample_op->volume = 32767;

        struct Filter* filter = filter_new();
        filter->type = TEST;
        filter->value = 32000;

        operator->envelope = envelope_osc;
        operator->carrier = sample_op;
        operator->modulator = sine_op;
        operator->mode = RING_MODULATION;
        operator->filter = NULL;//filter;
        operator->volume = 2048;
        operator->frequency_ratio = 2;
        operator->intensity = 400;
        operator_set_frequency(operator, 400);

        struct Voice* voice = voice_new();
        voice->operator = operator;
        voice_bank->voices[i] = voice;
        */