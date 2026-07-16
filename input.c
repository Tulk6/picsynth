uint n_buttons = 5;
uint button_pins[] = {11, 12, 13, 14, 15};

uint selector_pins[] = {8, 9, 10};
uint n_selectors = 3;

uint8_t current_state = 0;
uint8_t prev_state = 0;

void input_init(){
    /*adc_init();
    adc_gpio_init(26);
    adc_select_input(0);*/

    for (int i=0;i<n_buttons;i++){
        uint pin = button_pins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }

    for (int i=0;i<n_selectors;i++){
        uint pin = selector_pins[i];
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
    uint state = 0;
    for (int i=0;i<n_buttons;i++){
        uint pin = button_pins[i];
        uint button_state = (uint) !(gpio_get(pin));
        state = state | (button_state<<i);
    }

    for (int i=0;i<n_selectors;i++){
        uint pin = selector_pins[i];
        uint button_state = (uint) !(gpio_get(pin));
        state = state | (button_state<<(i+5));
    }

    //state = gpio_get(11);
    //printf("state: %u\n", state);
    prev_state = current_state;
    current_state = state;
}

bool input_just_pressed(){
    return current_state != prev_state;
}

bool input_button_state(uint n){
    return (bool) ((current_state>>n) & 1);
}

bool input_button_prev(uint n){
    return (bool) ((prev_state>>n) & 1);
}

bool input_button_pressed(uint n){
    return input_button_state(n) & !input_button_prev(n);
}

bool input_button_released(uint n){
    return !input_button_state(n) & input_button_prev(n);
}