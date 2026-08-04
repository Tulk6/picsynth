uint n_buttons = 5;
uint button_pins[] = {11, 12, 13, 14, 15};

/*uint selector_pins[] = {8, 9, 10};
uint n_selectors = 3;*/
#define RE_CLK 8
#define RE_DT 9
#define RE_SW 10

uint8_t current_state = 0;
uint8_t prev_state = 0;

uint8_t re_buffer = 0;

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
    /*uint8_t state = (gpio_get(RE_CLK)<<1)|(gpio_get(RE_DT));
    uint8_t quad = ((old_re_state & 0x03) << 2) | (state & 0x03);
    old_re_state = state;
    re_delta = quad_table[quad & 0x0F];
    re_pos += re_delta;*/
}

int8_t input_test_re(){
    uint8_t clk = gpio_get(RE_CLK);
    if (clk == 0){
        re_buffer = (re_buffer<<1) | clk;
        if (re_buffer==0b10000000){
            re_buffer = 0;
            if (gpio_get(RE_DT)){
                return 1;
            }else{
                return -1;
            }
        }
        return 0;
    }
    else{
        re_buffer = 1;
        return 0;
    }
}

void input_init(){
    /*adc_init();
    adc_gpio_init(26);
    adc_select_input(0);*/

    gpio_init(RE_SW);
    gpio_set_dir(RE_SW, GPIO_IN);
    gpio_pull_up(RE_SW);
    

    gpio_init(RE_CLK);
    gpio_set_dir(RE_CLK, GPIO_IN);
    gpio_pull_up(RE_CLK);
    //gpio_set_irq_enabled_with_callback(RE_CLK, GPIO_IRQ_EDGE_RISE, true, &input_re);

    gpio_init(RE_DT);
    gpio_set_dir(RE_DT, GPIO_IN);
    gpio_pull_up(RE_DT);
    //gpio_set_irq_enabled_with_callback(RE_DT, GPIO_IRQ_EDGE_RISE, true, &input_re);

    for (int i=0;i<n_buttons;i++){
        uint pin = button_pins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }

    /*for (int i=0;i<n_selectors;i++){
        uint pin = selector_pins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }*/
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

    //two bits a b
    //if a, then re has rotated, b indicates direction
    //if not a, then re clicked
    int8_t re_state = input_test_re();
    if (re_state != 0){
        state = state | (1 << n_buttons+1);
        if (re_state == -1){
            state = state | (1 << n_buttons);
        }
    }else{
        state = state | (gpio_get(RE_SW) << n_buttons);
    }

    /*for (int i=0;i<n_selectors;i++){
        uint pin = selector_pins[i];
        uint button_state = (uint) !(gpio_get(pin));
        state = state | (button_state<<(i+5));
    }*/

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

bool input_dial_rotated(){
    return input_button_state(n_buttons+1);
}

int8_t input_dial_rotation(){
    if (!input_dial_rotated()) return 0;
    if (input_button_state(n_buttons)) return -1;
    return 1;
}