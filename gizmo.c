enum GIZMO_RESULT {
    RESULT_NONE,
    RESULT_CHANGED,
};

struct GizmoList {
    char** option_names;
    int8_t n_options;
};

struct GizmoList gizmo_split_string(char* input_string, char* delimiter){
    ///erm hopefully theres nothing i ought to be freeing here...
    char* string = strdup(input_string); //i believe this is bcos string literals r read only
    char* found_string;
    char** found_array = NULL;
    int8_t n_found = 0;

    struct GizmoList gizmo_list;

    while ( (found_string = strsep(&string, delimiter)) != NULL){
        printf("%s\n", found_string);
        n_found += 1;
        found_array = realloc(found_array, sizeof(char*)*n_found);
        found_array[n_found-1] = found_string;
    }

    gizmo_list.option_names = found_array;
    gizmo_list.n_options = n_found;
    return gizmo_list;
}

char* gizmo_list_get(struct GizmoList* gizmo_list, uint8_t index){
    if (0<=index<(gizmo_list->n_options)){
        return gizmo_list->option_names[index];
    }else{
        return NULL;
    }
}

void gizmo_safe_wrap(uint8_t* value, int8_t increment, int8_t bound){
    printf("value: %" PRIu8 "\n", *value);
    printf("incremenet: %i\n", increment);
    if (increment > 0){
            *value += increment;
            if (*value > bound) *value = 0;
    }else if (increment < 0){
        if (*value == 0){
            *value = bound;
        }else{
            *value += increment;
        }
    }
}

enum GIZMO_RESULT gizmo_options_menu(struct GizmoList* gizmo_list, uint8_t* index, uint8_t line){
    enum GIZMO_RESULT result = RESULT_NONE;
    if (input_state.re_delta != 0){
        gizmo_safe_wrap(index, input_state.re_delta, gizmo_list->n_options-1);
        input_state.re_delta = 0;
        if (line==0) display_clear_top();
        else display_clear_bottom();
        result = RESULT_CHANGED;

        char* current_option = gizmo_list_get(gizmo_list, *index);
        display_set_pos(line, 0);
    
        display_write(current_option);
        
        display_char('<');

        char* f_string;
        asiprintf(&f_string, "%"PRIu8"/%"PRIu8, (*index+1), gizmo_list->n_options);
        display_write(f_string);
        free(f_string);

        display_char('>');
    }

    return result;
}

enum GIZMO_RESULT gizmo_value_menu(uint8_t* value){
    enum GIZMO_RESULT result = RESULT_NONE;
    if (input_state.re_delta != 0){
        *value += input_state.re_delta; //we dont care about over/underflow
        input_state.re_delta = 0;
        display_clear_bottom();
        result = RESULT_CHANGED;

        char* f_string;
        asiprintf(&f_string, "%"PRIu8"", *value);
        display_bottom_line(f_string);
        free(f_string);
    }

    return result;
}

enum GIZMO_RESULT gizmo_tone_menu(int8_t* value){
    enum GIZMO_RESULT result = RESULT_NONE;
    if (input_state.re_delta != 0){
        *value += input_state.re_delta; //we dont care about over/underflow
        input_state.re_delta = 0;
        display_clear_bottom();
        result = RESULT_CHANGED;

        char* f_string;
        asiprintf(&f_string, "%"PRIi8" \xec", *value);
        display_bottom_line(f_string);
        free(f_string);
    }

    return result;
}