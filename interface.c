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
        {.node=OPERATOR_A, .parameter=OPERATOR_FUNCTION, .value_mode=CARRIER},
        {.node=OPERATOR_A, .parameter=VOLUME, .value_int16=32767},
        {.node=OPERATOR_A, .parameter=MIX, .value_int16=1024},
        {.node=OPERATOR_A, .parameter=FILTER, .value_node=FILTER_A},
        //{.node=OPERATOR_A, .parameter=ENVELOPE_OSCILLATOR, .value_node=ENVELOPE_A},
    }
};


int interface_index = 0;
int interface_nmenus = 2;
char* interface_menus[] = {"hello", "goodbye"};



void interface_init(){
    
};

void interface_update(){
    if (input_dial_rotated()){
        interface_index += input_dial_rotation();
        printf("yippe!! %i", interface_index);
        if (interface_index < 0) interface_index = interface_nmenus - 1;
        else if (interface_index >= interface_nmenus) interface_index = 0;
        display_top_line(interface_menus[interface_index]);
    }
    if (input_button_pressed(5)){
        display_clear();
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