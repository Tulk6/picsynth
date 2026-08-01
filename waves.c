struct WaveTable square_wave;
struct WaveTable sine_wave;
struct WaveTable saw_wave;
struct WaveTable triangle_wave;
struct WaveTable noise_wave;
struct WaveTable high_wave;
struct WaveTable low_wave;
struct WaveTable sample_wave;

void waves_load(){
    wavetable_init(&square_wave);
    wavetable_load_square(&square_wave, WAVE_TABLE_LEN);

    wavetable_init(&sine_wave);
    wavetable_load_sine(&sine_wave, WAVE_TABLE_LEN);

    wavetable_init(&saw_wave);
    wavetable_load_saw(&saw_wave, WAVE_TABLE_LEN);

    wavetable_init(&triangle_wave);
    wavetable_load_triangle(&triangle_wave, WAVE_TABLE_LEN);

    wavetable_init(&noise_wave);
    wavetable_load_noise(&noise_wave, WAVE_TABLE_LEN);

    wavetable_init(&high_wave);
    wavetable_load_value(&high_wave, WAVE_TABLE_LEN, 32767);

    wavetable_init(&low_wave);
    wavetable_load_value(&low_wave, WAVE_TABLE_LEN, -32768);

    wavetable_init(&sample_wave);
    wavetable_load_sample(&sample_wave, sample_table, 14320, 220);
}