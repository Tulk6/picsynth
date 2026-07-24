#define N_ALGORITHM_SETTINGS 28

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
        {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SAW_WAVE},
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
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16 = 8000}

        /*{.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_B},

        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=0.5},
        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int=800},


        {.node=VOICE, .parameter=OUTPUT_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int = 2048}*/
    }
};

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