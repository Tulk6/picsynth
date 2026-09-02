#define N_ALGORITHM_SETTINGS 60

//const uint8_t N_ALGORITHM_SETTINGS = 10;

enum VoiceParameter {
    NO_PARAMETER,
    //operator parameters
    CARRIER_OPERATOR,
    CARRIER_OSCILLATOR,

    MODULATOR_OPERATOR,
    MODULATOR_OSCILLATOR,

    ENVELOPE_OSCILLATOR,

    OPERATOR_FUNCTION,
    FUNCTION_INTENSITY,

    VOLUME,
    MIX,
    FREQUENCY_RATIO,
    OPERATOR_FREQUENCY,

    FILTER,

    //oscillator parameters

    WAVEFORM,
    LOOP_TYPE,
    LOOP_START,
    LOOP_STOP,
    OSCILLATOR_FREQUENCY,

    //envelope_parameters
    ADSR,

    //filter_parameters
    FILTER_TYPE,
    FILTER_INTENSITY,

    //voice parameters

    OUTPUT_OPERATOR,
};

enum VoiceValue {
    NO_NODE,
    OPERATOR_A,
    OPERATOR_B,
    OPERATOR_C,
    OPERATOR_D,
    OPERATOR_E,
    
    OSCILLATOR_A,
    OSCILLATOR_B,
    OSCILLATOR_C,
    OSCILLATOR_D,
    OSCILLATOR_E,
    OSCILLATOR_F,
    
    ENVELOPE_A,
    ENVELOPE_B,

    LFO_A,

    FILTER_A,

    SINE_WAVE,
    SQUARE_WAVE,
    SAW_WAVE,
    TRIANGLE_WAVE,
    SAMPLE_WAVE,
    ENVELOPE_WAVE,
    HIGH_WAVE,
    LOW_WAVE,
    NOISE_WAVE,

    VOICE
};

struct AlgorithmSetting {
    enum VoiceValue node;
    enum VoiceParameter parameter;
    union {
        float value_float;
        int16_t value_int16;
        uint32_t value_uint32;
        enum VoiceValue value_node;
        enum OperatorMode value_mode;
        enum LoopType value_loop;
        enum FilterType value_filter;
    };
};

struct Algorithm{
    struct AlgorithmSetting settings[N_ALGORITHM_SETTINGS];
};

struct Algorithm algo1 = {
    .settings = {
        {.node=ENVELOPE_A, .parameter=ADSR, .value_uint32=0b01111111000011111111111100001111},
        {.node=ENVELOPE_A, .parameter=OSCILLATOR_FREQUENCY, .value_float=1},
        {.node=ENVELOPE_A, .parameter=LOOP_TYPE, .value_int16=NO_LOOP},

        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_C, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float = 3},
        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int16=1500},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_C},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_A, .parameter=MIX, .value_int16=2048},

        {.node=OPERATOR_A, .parameter=ENVELOPE_OSCILLATOR, .value_node=ENVELOPE_A}
    }
};


void* algorithm_node_name(struct Voice* voice, enum VoiceValue node_name){
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


void algorithm_voice_load(struct Algorithm* algorithm, struct Voice* voice){
    for (int i=0; i<N_ALGORITHM_SETTINGS; i++){
        printf("LOOKING AT SETTING %i\n", i);
        struct AlgorithmSetting setting = algorithm->settings[i];
        if (setting.node == NO_NODE || setting.parameter == NO_PARAMETER) continue;
        printf("finding node...\n");
        void* node = algorithm_node_name(voice, setting.node);
        if (node == NULL) continue;
        printf("finding parameter\n");
        enum VoiceParameter parameter = setting.parameter;
        void* value_node = algorithm_node_name(voice, setting.value_node);
        switch (parameter){
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

            case MIX:
                ((struct Operator*) node)->mix = setting.value_int16;
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

void algorithm_apply(struct Algorithm* algorithm, struct VoiceBank* voice_bank){
    for (int i=0;i<voice_bank->n_voices;i++){
        if (voice_bank->voices[i] != NULL){
            algorithm_voice_load(algorithm, voice_bank->voices[i]);
        } 
    }
}




        /*{.node=ENVELOPE_A, .parameter=ADSR, .value_uint32=0b01111111000011111111111100001111},
        {.node=ENVELOPE_A, .parameter=OSCILLATOR_FREQUENCY, .value_float=1},
        {.node=ENVELOPE_A, .parameter=LOOP_TYPE, .value_int16=NO_LOOP},
        {.node=ENVELOPE_A, .parameter=LOOP_START, .value_int16=143},
        {.node=ENVELOPE_A, .parameter=LOOP_STOP, .value_int16=144},

        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SQUARE_WAVE},
        {.node=OSCILLATOR_C, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_C, .parameter=OSCILLATOR_FREQUENCY, .value_float=2},
        

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_C},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_node=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int16=2},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=0},


        {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SAW_WAVE},
        {.node=OSCILLATOR_D, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_D, .parameter=OSCILLATOR_FREQUENCY, .value_float=2.5},

        {.node=OPERATOR_B, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_B},
        {.node=OPERATOR_B, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_D},
        {.node=OPERATOR_B, .parameter=OPERATOR_FUNCTION, .value_node=PHASE_MODULATION},
        {.node=OPERATOR_B, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_B, .parameter=FUNCTION_INTENSITY, .value_int16=2},
        {.node=OPERATOR_B, .parameter=FREQUENCY_RATIO, .value_float=0},

        {.node=OPERATOR_C, .parameter=CARRIER_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_C, .parameter=MODULATOR_OPERATOR, .value_node=OPERATOR_B},
        {.node=OPERATOR_C, .parameter=VOLUME, .value_int16=1024},
        {.node=OPERATOR_C, .parameter=OPERATOR_FUNCTION, .value_mode=ADDITIVE},
        {.node=OPERATOR_C, .parameter=FUNCTION_INTENSITY, .value_int16=32767},
        {.node=OPERATOR_C, .parameter=FREQUENCY_RATIO, .value_float=1},
        {.node=OPERATOR_C, .parameter=ENVELOPE_OSCILLATOR, .value_node=ENVELOPE_A},


        {.node=VOICE, .parameter=OUTPUT_OPERATOR, .value_node=OPERATOR_C},*/

        /*{.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SAW_WAVE},
        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SAW_WAVE},
        {.node=OSCILLATOR_A, .parameter=LOOP_TYPE, .value_loop=FORWARD},
        {.node=OSCILLATOR_B, .parameter=LOOP_TYPE, .value_loop=FORWARD},

        {.node=FILTER_A, .parameter=FILTER_TYPE, .value_filter=TEST},
        {.node=FILTER_A, .parameter=FILTER_INTENSITY, .value_node=ENVELOPE_B},

        {.node=ENVELOPE_B, .parameter=ADSR, .value_uint32=0b00001111000011111111111100001111},
        {.node=ENVELOPE_B, .parameter=OSCILLATOR_FREQUENCY, .value_float=10},
        

        {.node=ENVELOPE_A, .parameter=ADSR, .value_uint32=0b00001111000011111111111100001111},
        {.node=ENVELOPE_A, .parameter=OSCILLATOR_FREQUENCY, .value_float=10},

        {.node=OPERATOR_A, .parameter=ENVELOPE_OSCILLATOR, .value_node=ENVELOPE_B},
        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_B},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=ADDITIVE},
        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int16=16000},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=1.01},
        {.node=OPERATOR_A, .parameter=FILTER, .value_node=FILTER_A}, 
        {.node=VOICE, .parameter=OUTPUT_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16 = 8000}*/

        /*{.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_B},

        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=0.5},
        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int=800},


        {.node=VOICE, .parameter=OUTPUT_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int = 2048}*/

/*struct Algorithm algo1 = {
    .settings = {
        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_C, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_D, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_E, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_B},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int=32767},

        {.node=OPERATOR_C, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_C},
        {.node=OPERATOR_C, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_D},
        {.node=OPERATOR_C, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_C, .parameter=VOLUME, .value_int=32767},

        {.node=OPERATOR_B, .parameter=CARRIER_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_B, .parameter=MODULATOR_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_B, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_B, .parameter=VOLUME, .value_int=32767},

        {.node=OPERATOR_D, .parameter=CARRIER_OPERATOR, .value_node=OPERATOR_C},
        {.node=OPERATOR_D, .parameter=MODULATOR_OPERATOR, .value_node=OPERATOR_C},
        {.node=OPERATOR_D, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_D, .parameter=VOLUME, .value_int=32767},

        {.node=OPERATOR_E, .parameter=CARRIER_OPERATOR, .value_node=OPERATOR_B},
        {.node=OPERATOR_E, .parameter=MODULATOR_OPERATOR, .value_node=OPERATOR_D},
        {.node=OPERATOR_E, .parameter=OPERATOR_FUNCTION, .value_mode=ADDITIVE},
        {.node=OPERATOR_E, .parameter=VOLUME, .value_int=4096},
        {.node=OUTPUT_OPERATOR, .parameter=OUTPUT_OPERATOR, .value_node=OPERATOR_E},
    }
};*/



/*
Algorithm 4:
    OscA -> OpA.m
    OscB -> OpA.c

    OscC -> OpC.m
    OscD -> OpC.c

    OscE -> OpB.m
    OpA -> OpB.c

    OscF -> OpD.m
    OpC -> OpD.c

    OpB -> OpE.m
    OpD -> OpE.c

    OpE.mode -> Additive
*/