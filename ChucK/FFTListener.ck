/* Listens for an RMS peak */
// might eventually need to be more specific about channels
Flags flags;


new Event @=> flags.startRecording;
new Event @=> flags.stopRecording;

adc => FFT fft =^ RMS rms => blackhole;

2048 => int FFT_SIZE;
4 => int OVERLAP;
FFT_SIZE / OVERLAP => int HOP_SIZE;

FFT_SIZE => fft.size;
Windowing.hann(FFT_SIZE) => fft.window;
0.5 => float upThresh;
0.4 => float downThresh;
0.5::second => dur hysteresis;
int gate;
while (true) {
    rms.upchuck();
    
    rms.fval(0)*100 => float val;
    <<<val>>>;
    if (val >= upThresh && !gate) {
        1 => gate;
        flags.startRecording.broadcast;
        <<<"UPPP">>>;
    } else if (val <= downThresh && gate) {
        0 => gate;
        hysteresis => now;
        flags.stopRecording.broadcast;
        <<<"DOWN">>>;
        me.exit();
    }
    10::ms => now;
}