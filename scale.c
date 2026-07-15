#define C1_HZ 32.703195662574764 //for A4 440
#define NOTE_RATIO 1.0594630943592953 //pow(2, 1/12)

enum MusicalNote {
    Note_C = 0,

    Note_Cs = 1,
    Note_Db = 1,

    Note_D = 2,

    Note_Ds = 3,
    Note_Eb = 3,

    Note_E = 4,

    Note_F = 5,

    Note_Fs = 6,
    Note_Gb = 6,

    Note_G = 7,

    Note_Gs = 8,
    Note_Ab = 8,

    Note_A = 9,

    Note_As = 10,
    Note_Bb = 10,

    Note_B = 11
};

float scale_get_ratio(int note_dif){
    return pow(NOTE_RATIO, note_dif);
}

float scale_get_frequency_i(int note_n){
    return C1_HZ*pow(NOTE_RATIO, note_n);
}

float scale_get_frequency(enum MusicalNote note, int octave){
    int note_n = note + (octave-1)*12;
    return scale_get_frequency_i(note_n);
}