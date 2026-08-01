//argh....

#define MAX_LCD_LINE 16
#define MAX_MENU_ITEMS 16

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

struct Interface interface;

void interface_init(){
    strncpy(interface.items[0].name, "he", MAX_LCD_LINE);
};

void interface_update(){

};