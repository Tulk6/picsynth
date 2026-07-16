LCD my_lcd;

void display_init(){   
    lcd_init(&my_lcd, 0x27, 4, 5, i2c0);
}