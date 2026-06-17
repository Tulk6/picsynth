#define C1_HZ 32.703195662574764 //for A4 440
#define NOTE_RATIO 1.0594630943592953 //pow(2, 1/12)

float scale_get_frequency_i(int note_n){
    return C1_HZ*pow(NOTE_RATIO, note_n);
}

float scale_get_frequency(char note, int octave){
    if (note > 96){
        note -= 32; //convert to uppercase 
    }
    if (note<65 && note>71) return 0; //char not valid -> between A and G
    int note_dif = note - 'C';
    int note_n = note_dif + (octave-1)*12;
    return scale_get_frequency_i(note_n);
}