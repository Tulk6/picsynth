LCD my_lcd;

void display_init(){   
    lcd_init(&my_lcd, 0x27, 4, 5, i2c0);
}

void display_set_pos(uint8_t line, uint8_t index){
    lcd_set_cursor_pos(&my_lcd, line, index);
}

void display_write(char* string){
    lcd_string(&my_lcd, string);
}

void display_char(char c){
    lcd_char(&my_lcd, c);
}

void display_text(char* string, uint8_t line, uint8_t index){
    display_set_pos(line, index);
    display_write(string);
}

void display_top_line(char* string){
    display_set_pos(0, 0);
    display_write(string);
}

void display_bottom_line(char* string){
    display_set_pos(1, 0);
    display_write(string);
}

void display_clear_top(){
    display_set_pos(0, 0);
    display_write("                ");
}

void display_clear_bottom(){
    display_set_pos(1, 0);
    display_write("                ");
}

void display_clear(){
    lcd_clear(&my_lcd);
}