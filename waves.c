struct WaveTable square_wave;
struct WaveTable sine_wave;
struct WaveTable envelope;

void waves_load(){
    wavetable_init(&square_wave);
    wavetable_load_square(&square_wave, WAVE_TABLE_LEN);

    wavetable_init(&sine_wave);
    wavetable_load_sine(&sine_wave, WAVE_TABLE_LEN);

    wavetable_init(&envelope);
    wavetable_load_adsr(&envelope, 1, 10, 16383, 20);
}