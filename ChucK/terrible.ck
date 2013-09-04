public class Warble extends Chubgraph
{
    inlet => DelayL d  => outlet;
    Gain cv;
    SinOsc s;
    100 => float dpth;
    1 => float rat;
    {
        cv => blackhole;
        s => blackhole;
        1::second => d.max;
        rat => s.freq;
        1 => s.gain;
        spork ~ _tickAtSampleRate();
    }
    
    fun void _tickAtSampleRate()
    {
        while (true)
        {
            1::samp => now;
            tick(s.last() + 1);
        }
    }
    
    fun void tick(float s){
        
        (s*dpth)::samp => d.delay;
    }
    
    fun void depth(float dp){
        if (dp > 0)
            dp => dpth;
    }
    
    fun float getdepth(){
        return dpth;
    }
    
    fun void rate(float rt){
        if (rt > 0)
        {
            rt => rat;
            rat => s.freq;
        }
    }
    
    fun float getrate(){
        return rat;
    }
}