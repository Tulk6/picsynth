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




struct GizmoList main_menu;
uint8_t main_menu_index;

struct GizmoList waveform_menu;
uint8_t waveform_menu_index;

uint8_t ratio;
uint8_t intensity;

int8_t current_menu;

void interface_init(){
    main_menu = gizmo_split_string("Waveform;Ratio;Intensity", ";");
    waveform_menu = gizmo_split_string("Sine;Square;Saw;Triangle", ";");
};

void interface_update(){
    switch (current_menu){
        case 0:
            gizmo_options_menu(&main_menu, &main_menu_index);
            if (input_button_pressed(0)) current_menu = main_menu_index+1;
            break;

        case 1:
            if (gizmo_options_menu(&waveform_menu, &waveform_menu_index) == RESULT_CHANGED){
                struct Algorithm algo = {
                    .settings = {
                        {.node=OSCILLATOR_A, .parameter=WAVEFORM, .value_node=(enum VoiceValue)(16+waveform_menu_index)}
                    }
                };
                algorithm_apply(&algo, &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 2:
            if (gizmo_value_menu(&ratio) == RESULT_CHANGED){
                struct Algorithm algo = {
                    .settings = {
                        {.node=OPERATOR_A, .parameter=FREQUENCY_RATIO, .value_float=(float)ratio/64}
                    }
                };
                algorithm_apply(&algo, &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
            break;

        case 3:
            if (gizmo_value_menu(&intensity) == RESULT_CHANGED){
                struct Algorithm algo = {
                    .settings = {
                        {.node=OPERATOR_A, .parameter=FUNCTION_INTENSITY, .value_int16=intensity*10}
                    }
                };
                algorithm_apply(&algo, &voice_bank);
            }
            if (input_button_pressed(0)) current_menu = 0;
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