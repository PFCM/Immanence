// Genetic

// Load a soundbuf
SndBuf soundfile;
//"/Users/pfcmathews/aspenselfconvx1.wav" => soundfile.read; 
me.dir() + "/audio/cowbell_01.wav" => soundfile.read;


// variables for changing the targets while in flux
// might need quite a high instability (mutation rate) 
// for this to be effective
0.5 => float targetRate;
0.1 => float targetRateRange;
2000::ms => dur targetSize;
0 => int targetBi;

// LiSa
adc => LiSa lisa => dac;
targetSize*2 => lisa.duration;
10 => lisa.maxVoices;

/* transfer (note, copies sequential samples, for stereo need to deal with interleaving)
for (0 => int i; i < soundfile.samples(); i++)
{
    (soundfile.valueAt(i), i::samp) => lisa.valueAt;
}*/

dac => Gain g => WvOut w => blackhole;
"evolution example.wav" => w.wavFilename;
null @=> w;

lisa.record(1);
targetSize*2 => now;
lisa.record(0);
lisa.loopRec(1);

// set up our initial population
Voice population[lisa.maxVoices()];
float fitnesses[population.cap()];
for (0 => int i; i < population.cap(); i++)
{
    population[i].set(lisa, 0.1::ms, i);
}

// fitness function, assess differences between parameters and some target
// try and express as a float somehow
fun float fitness(Voice v)
{
    1.0 => float val;
    // calculate Euclidean distance between this point and target
    // in this particular 3D space
    // need to scale it back when it isn't within the range
    
    // all of our values start from the origin
    v.rate-targetRate => float a;
    (v.size-targetSize)/second => float b;
    (v.bi - targetBi) * 0.5 => float c; // weighting this to slow down how quickly it gets evolved out
    
    /*Math.sqrt*/(a*a + b*b + c*c) => val; //might as well just use the square
    
    <<<v.rate, v.size/second, v.bi, 1.0/val>>>;
    
    return 1.0/val;
}

// does everything necessary to advance generations
fun void nextGeneration(Voice pop[], float fit[], LiSa l) 
{
    <<<"Producing next generation">>>;
    0 => float sum;
    for (0 => int i; i < pop.cap(); i++)
    {
        fitness(pop[i]) => fit[i]; // get fitness
        fit[i] +=> sum; // accumulate sum
    }
    
    // normalise so that all fitnesses sum to 1
    for (0 => int i; i < pop.cap(); i++)
    {
        fit[i]/sum => fit[i];
    }
    
    // now using these weights, construct a new population
    <<<"Fitnesses calculated and normalised, breeding new generation">>>;
    Voice a,b;
    for (0 => int n; n < pop.cap(); n++)
    {
        Math.randomf() => float valA;
        Math.randomf() => float valB;
        0 => float prob;
        0 => int aFlag;
        0 => int bFlag;
        for (0 => int i; i < pop.cap(); i++)
        {
            prob + fit[i] => prob;
            if (valA <= prob && !aFlag) 
            {
                pop[i] @=> a;
                1 => aFlag;
            }
            if (valB <= prob && !bFlag)
            {
                pop[i] @=> b;
                1 => bFlag;
            }
        }
        Voice child;
        breed(a,b, child) @=> pop[n];
        pop[n].set(l, 0.1::ms, n);
    }
    
    <<<"applying mutations">>>;
    0.10 => float mutRate;
    if (sum < l.maxVoices()) mutRate * 2 => mutRate;
    mutate(pop, mutRate);
}

// combines random aspects of two voices, should probably combine best ones but who knows
fun Voice breed(Voice a, Voice b, Voice child)
{
    if (maybe) a.rate => child.rate;
    else b.rate => child.rate;
    
    if (maybe) a.bi => child.bi;
    else b.bi => child.bi;
    
    if (maybe) a.size => child.size;
    else b.size => child.size;
    
    return child;
}

// for every voice has a mRate % chance, per parameter, of reassigning
// that parameter
fun void mutate(Voice pop[], float mRate)
{
    Voice rand;
    for (0 => int i; i < pop.cap(); i++)
    {
        new Voice @=> rand;
        Math.randomf() => float val;
        if (val < mRate) rand.rate => pop[i].rate;
        Math.randomf() => val;
        if (val < mRate) rand.bi => pop[i].bi;
        Math.randomf() => val;
        if (val < mRate) rand.size => pop[i].size;
    }
}


while (true) {
    5::second => now;
    spork ~ nextGeneration(population, fitnesses, lisa);
  //  1 => lisa.record;
  //  1::second => now;
}

// this represents a voice for our LiSa
// note the voice index needs to be set after creation
// the rest of the values will be chosen randomly, but can 
// definitely be set explicitly
private class Voice 
{
    -1 => int index;
    Math.random2f(0.125,4) => float rate;
    Math.random2(0,1) => int bi;
    Math.random2(2, 5000)::ms => dur size;
    
    fun void set(LiSa to, dur start) 
    {
        if (index < 0) 
        {
            <<<"Voice index not set">>>;
        }
        else 
        {
            set(to, start, index);
        }
    }
    
    fun void set(LiSa to, dur start, int i)
    {
        i => index;
        start/samp => float s;
        to.loop(i,1);
        to.play(i,1);
        to.loopStart(i, Math.random2f(0,s)::samp);
        to.loopEnd(i, to.loopStart(i)+size);
        to.rate(i,rate);
        to.bi(i,bi);
    }
}