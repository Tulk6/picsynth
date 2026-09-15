//argh....

#define MAX_LCD_LINE 16
#define MAX_MENU_ITEMS 16

struct Algorithm algo_mode_gemini = {
    .settings = {
        {.node=ENVELOPE_A, .parameter=ADSR, .value_uint32=0b01111111000011111111111100001111},
        {.node=ENVELOPE_A, .parameter=OSCILLATOR_FREQUENCY, .value_float=1},
        {.node=ENVELOPE_A, .parameter=LOOP_TYPE, .value_int16=NO_LOOP},

        {.node=FILTER_A, .parameter=FILTER_TYPE, .value_filter=TEST},
        {.node=FILTER_A, .parameter=FILTER_INTENSITY, .value_node=ENVELOPE_A},

        {.node=OSCILLATOR_C, .parameter=WAVEFORM, .value_node=SQUARE_WAVE},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_C},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_A, .parameter=MIX, .value_int16=1024},
        {.node=OPERATOR_A, .parameter=FILTER, .value_node=FILTER_A},
        //{.node=OPERATOR_A, .parameter=ENVELOPE_OSCILLATOR, .value_node=ENVELOPE_A},
    }
};

struct Algorithm algo_mode_mage = {
    .settings = {
        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SQUARE_WAVE},

        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=ADDITIVE},
        //{.node=}
    }
};


uint8_t current_mode = 1;

struct GizmoList main_menu;
uint8_t main_menu_index;



struct GizmoList waveform_menu;
uint8_t waveform_menu_index;

uint8_t ratio;
uint8_t intensity;

int8_t current_menu = 0;

//mage menus
struct GizmoList mage_main_menu;
uint8_t mage_main_menu_index;



void interface_init(){
    main_menu = gizmo_split_string("Waveform;Ratio;Intensity", ";");
    mage_main_menu = gizmo_split_string("Waveform A;Waveform B;Mix;Detune;Filter;Vib. Depth;Vib. Rate;Attack;Decay;Sustain;Release;", ";");
    waveform_menu = gizmo_split_string("Sine;Square;Saw;Triangle", ";");


};

void interface_menu_duop(){
    switch (current_menu){
        case 0:
            gizmo_options_menu(&main_menu, &main_menu_index, 0);
            if (input_button_pressed(0)) current_menu = main_menu_index+1;
            break;

        case 1:
            if (gizmo_options_menu(&waveform_menu, &waveform_menu_index, 1) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting) {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=(enum VoiceValue)(16+waveform_menu_index)}, 
                    &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            display_clear_bottom();
            break;

        case 2:
            if (gizmo_value_menu(&ratio) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=(float)ratio/64},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            display_clear_bottom();
            break;

        case 3:
            if (gizmo_value_menu(&intensity) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting) {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int16=intensity*10},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            display_clear_bottom();
            break;
    }
}


uint8_t mage_mix;
int8_t mage_detune;
uint8_t mage_filter = 30;
uint8_t mage_vibrato_depth;
uint8_t mage_vibrato_rate;
uint8_t mage_attack;
uint8_t mage_decay;
uint8_t mage_sustain;
uint8_t mage_release;

struct Algorithm mage_algorithm = {
    .settings={
        {.node=FILTER_A, .parameter=FILTER_TYPE, .value_node=TEST},
        {.node=FILTER_A, .parameter=FILTER_INTENSITY, .value_int16=30<<7},

        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=SAW_WAVE},

        {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=SINE_WAVE},

        {.node=OSCILLATOR_C, .parameter=WAVEFORM, .value_node=SINE_WAVE},
        {.node=OSCILLATOR_C, .parameter=OSCILLATOR_FREQUENCY, .value_float=15},

        {.node=OPERATOR_A, .parameter=CARRIER_OSCILLATOR, .value_node=OSCILLATOR_A},
        {.node=OPERATOR_A, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_B},
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=ADDITIVE},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_A, .parameter=MIX, .value_int16=0},
        {.node=OPERATOR_A, .parameter=FILTER, .value_node=FILTER_A},
        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=1},

        {.node=OPERATOR_B, .parameter=CARRIER_OPERATOR, .value_node=OPERATOR_A},
        {.node=OPERATOR_B, .parameter=MODULATOR_OSCILLATOR, .value_node=OSCILLATOR_C},
        {.node=OPERATOR_B, .parameter=OPERATOR_FUNCTION, .value_mode=PHASE_MODULATION},
        {.node=OPERATOR_B, .parameter=FREQUENCY_RATIO, .value_float=0},
        {.node=OPERATOR_B, .parameter=FUNCTION_INTENSITY, .value_int16=1},
        {.node=OPERATOR_B, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_B, .parameter=MIX, .value_int16=2048},
    }
};

void interface_menu_mage(){
    switch (current_menu){
        case 0:
            gizmo_options_menu(&mage_main_menu, &main_menu_index, 0);
            if (input_button_pressed(0)) current_menu = main_menu_index+1;
            break;

        case 1:
            if (gizmo_options_menu(&waveform_menu, &waveform_menu_index, 1) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting) {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=(enum VoiceValue)(16+waveform_menu_index)}, 
                    &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 2:
            if (gizmo_options_menu(&waveform_menu, &waveform_menu_index, 1) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting) {.node=OSCILLATOR_B, .parameter=WAVEFORM, .value_node=(enum VoiceValue)(16+waveform_menu_index)}, 
                    &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 3:
            if (gizmo_value_menu(&mage_mix) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=OPERATOR_A, .parameter=MIX, .value_int16=mage_mix<<7},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 4:
            if (gizmo_tone_menu(&mage_detune) == RESULT_CHANGED){
                float pitch_ratio = scale_get_ratio(mage_detune);
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=pitch_ratio},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 5:
            if (gizmo_value_menu(&mage_filter) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=FILTER_A, .parameter=FILTER_INTENSITY, .value_int16=mage_filter<<7},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 6:
            if (gizmo_value_menu(&mage_vibrato_depth) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=OPERATOR_B, .parameter=FUNCTION_INTENSITY, .value_int16=mage_vibrato_depth},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 7:
            if (gizmo_value_menu(&mage_vibrato_rate) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=OSCILLATOR_C, .parameter=OSCILLATOR_FREQUENCY, .value_float=mage_vibrato_rate/2},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 8:
            if (gizmo_value_menu(&mage_attack) == RESULT_CHANGED){
                algorithm_apply_setting(
                    (struct AlgorithmSetting){.node=FILTER_A, .parameter=FILTER_INTENSITY, .value_int16=mage_filter<<7},
                     &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;
    }
}

void interface_update(){
    switch (current_mode){
        case 0:
            interface_menu_duop();
            break;
        case 1:
            interface_menu_mage();
            break;
    }
};


/*
enum InterfaceMenuType {
    LIST,
    VALUE,
};

struct InterfaceMenuItem {
    char name[MAX_LCD_LINE];
    enum InterfaceMenuType type;
    void* menu_handler;
};

struct Interface {
    struct InterfaceMenuItem items[MAX_MENU_ITEMS];
};

struct Interface interface;*/