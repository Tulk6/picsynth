#define RE_CLK 8
#define RE_DT 9

struct InputState {
    uint8_t prev_key_state;
    uint8_t key_state;

    uint8_t prev_button_state;
    uint8_t button_state;


    int8_t re_delta;
    int8_t re_state;
};

struct InputState input_state;

uint n_keys = 5;
uint key_pins[] = {11, 12, 13, 14, 15};

uint n_buttons = 1;
uint button_pins[] = {10};

const int8_t quad_table[16] = {
  0,   // 0000: No change (stable)
  1,   // 0001: A 0>1, count up
  -1,  // 0010: B 0>1, count down
  0,   // 0011: Both changed—invalid transition
  -1,  // 0100: A 1>0, count down
  0,   // 0101: No change (stable)
  0,   // 0110: Both changed—invalid
  1,   // 0111: A 0>1, count up
  1,   // 1000: B 1>0, count up
  0,   // 1001: Both changed—invalid
  0,   // 1010: No change (stable)
  -1,  // 1011: A 1>0, count down
  0,   // 1100: Both changed—invalid
  -1,  // 1101: A 1>0, count down
  1,   // 1110: B 0>1, count up
  0    // 1111: No change (stable)
};

void input_re(uint gpio, uint32_t event_mask){
    uint8_t state = (gpio_get(RE_CLK)<<1)|(gpio_get(RE_DT));
    uint8_t quad = ((input_state.re_state & 0x03) << 2) | (state & 0x03);
    input_state.re_state = state;
    input_state.re_delta = quad_table[quad & 0x0F];
}

void input_init(){
    /*adc_init();
    adc_gpio_init(26);
    adc_select_input(0);*/    

    gpio_init(RE_CLK);
    gpio_set_dir(RE_CLK, GPIO_IN);
    gpio_pull_up(RE_CLK);
    gpio_set_irq_enabled_with_callback(RE_CLK, GPIO_IRQ_EDGE_RISE, true, &input_re);

    gpio_init(RE_DT);
    gpio_set_dir(RE_DT, GPIO_IN);
    gpio_pull_up(RE_DT);
    gpio_set_irq_enabled_with_callback(RE_DT, GPIO_IRQ_EDGE_RISE, true, &input_re);

    for (int i=0;i<n_keys;i++){
        uint pin = key_pins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }

    for (int i=0;i<n_buttons;i++){
        uint pin = button_pins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }
}

uint16_t input_read_adc(){
    //max 4096
    //min greater than 0
    //cheap?? way of quantising more
    //this does 31 steps cos 4096>>7 = 31
    uint16_t adc = (adc_read()>>8)<<8;
    return adc;
}

uint input_read(){
    uint key_states = 0;
    for (int i=0;i<n_keys;i++){
        uint pin = key_pins[i];
        uint key_state = (uint) !(gpio_get(pin));
        key_states = key_states | (key_state<<i);
    }
    input_state.prev_key_state = input_state.key_state;
    input_state.key_state = key_states;

    uint button_states = 0;
    for (int i=0;i<n_buttons;i++){
        uint pin = button_pins[i];
        uint button_state = (uint) !(gpio_get(pin));
        button_states = button_state | (button_state<<i);
    }
    input_state.prev_button_state = input_state.button_state;
    input_state.button_state = button_states;

    /*for (int i=0;i<n_selectors;i++){
        uint pin = selector_pins[i];
        uint button_state = (uint) !(gpio_get(pin));
        state = state | (button_state<<(i+5));
    }*/

    //state = gpio_get(11);
    //printf("state: %u\n", state);
    
}

bool input_just_pressed(){
    return input_state.key_state != input_state.prev_key_state;
}

bool input_key_state(uint n){
    return (bool) ((input_state.key_state>>n) & 1);
}

bool input_key_prev(uint n){
    return (bool) ((input_state.prev_key_state>>n) & 1);
}

bool input_key_pressed(uint n){
    return input_key_state(n) & !input_key_prev(n);
}

bool input_key_released(uint n){
    return !input_key_state(n) & input_key_prev(n);
}


//buttons

bool input_button_state(uint n){
    return (bool) ((input_state.button_state>>n) & 1);
}

bool input_button_prev(uint n){
    return (bool) ((input_state.prev_button_state>>n) & 1);
}

bool input_button_pressed(uint n){
    return input_button_state(n) & !input_button_prev(n);
}

bool input_button_released(uint n){
    return !input_button_state(n) & input_button_prev(n);
}