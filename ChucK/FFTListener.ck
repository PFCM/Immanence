/* Listens for an RMS peak */
// might eventually need to be more specific about channels
Flags flags;



adc => FFT fft =^ RMS rms => blackhole;

2048 => int FFT_SIZE;
4 => int OVERLAP;
FFT_SIZE / OVERLAP => int HOP_SIZE;

FFT_SIZE => fft.size;
Windowing.hann(FFT_SIZE) => fft.window;
0.1 => float upThresh;
0.05 => float downThresh;
0.5::second => dur hysteresis;
int gate;
while (true) {
    rms.upchuck();
    
    rms.fval(0)*100 => float val;
    //<<<val>>>;
    if (val >= upThresh && !gate) {
        1 => gate;
        1 => flags.recording;
        <<<"UPPP">>>;
    } else if (val <= downThresh && gate) {
        0 => gate;
        hysteresis => now;
        0 => flags.recording;
        <<<"DOWN">>>;
        me.exit();
    }
    10::ms => now;
}