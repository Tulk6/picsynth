struct WaveTable square_wave;
struct WaveTable sine_wave;
struct WaveTable saw_wave;
struct WaveTable triangle_wave;
struct WaveTable envelope;
struct WaveTable sample_table;

void waves_load(){
    wavetable_init(&square_wave);
    wavetable_load_square(&square_wave, WAVE_TABLE_LEN);

    wavetable_init(&sine_wave);
    wavetable_load_sine(&sine_wave, WAVE_TABLE_LEN);

    wavetable_init(&saw_wave);
    wavetable_load_saw(&saw_wave, WAVE_TABLE_LEN);

    wavetable_init(&triangle_wave);
    wavetable_load_saw(&triangle_wave, WAVE_TABLE_LEN);

    wavetable_init(&envelope);
    wavetable_load_adsr(&envelope, 600, 1, 32767, 0);

    wavetable_init(&sample_table);
    wavetable_load_sample(&sample_table, sample_wave, 23713, 60);
}