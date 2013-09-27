// Genetic

Flags flags;
1 => flags.simulationRunning;
// Load a soundbuf
SndBuf soundfile;
//"/Users/pfcmathews/aspenselfconvx1.wav" => soundfile.read; 
//me.dir() + "/audio/cowbell_01.wav" => soundfile.read;

Std.atoi(me.arg(0)) => int whichpan;

spork~ panner(whichpan);


// variables for changing the targets while in flux
// might need quite a high instability (mutation rate) 
// for this to be effective
1.0 => float targetRate;
0.1 => float targetRateRange;
5000::ms => dur maxSize;
0 => int targetBi;
int current;

// LiSa
JCRev r => Dyno limit => Pan2 pan =>  dac;
LiSa lisa[3];
.01 => r.mix;

limit.limit();


while (flags.recording == 0) 
    1::ms => now;

for (int i; i < lisa.cap(); i++) 
{
    maxSize => lisa[i].duration;
    20 => lisa[i].maxVoices;
    adc => lisa[i] => r;
    lisa[i].record(1);
    lisa[i].loopRec(1);
}

now => time start;
while (flags.recording == 1)
    1::ms => now;
now-start => dur targetSize;

for (int i; i < lisa.cap(); i++)
{
    lisa[i].record(0);
    lisa[i].loopRec(0);
}

0.5 => lisa[0].gain;
0.0 => lisa[1].gain => lisa[2].gain;

// set up our initial population
Voice population[lisa[0].maxVoices()];
float fitnesses[population.cap()];
for (0 => int i; i < population.cap(); i++)
{
    for (int j; j < lisa.cap(); j++)
        population[i].set(lisa[j], 0.1::ms, i);
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
    (v.bi - targetBi) /* 0.5*/ => float c; // weighting this to slow down how quickly it gets evolved out
    
    /*Math.sqrt*/1/(a*a + b*b + c*c) => val; //might as well just use the square
    
   // <<<v.rate, v.size/second, v.bi, val>>>;
    
    return val;
}

// does everything necessary to advance generations
fun void nextGeneration(Voice pop[], float fit[], LiSa l) 
{
    for (int i; i < population.cap(); i++)
        population[i].stopLoop();
   // <<<"Producing next generation">>>;
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
   // <<<"Fitnesses calculated and normalised, breeding new generation">>>;
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
    
  //  <<<"applying mutations">>>;
    0.10 => float mutRate;
    if (sum < l.maxVoices()) mutRate * 2 => mutRate;
    mutate(pop, mutRate);
    
    for (int i; i < population.cap(); i++)
        population[i].startLoop(lisa[current]);
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

now => time beginning;
while (true) {
    5::second => now;
    nextGeneration(population, fitnesses, lisa[current]);
    (1+current)%lisa.cap() => current;
    for (int i; i<lisa.cap(); i++)
    {
        for (int j; j < lisa[i].maxVoices(); j++)
        {
            if (current != i)
                lisa[i].rampDown(j, 100::ms);
            else
                lisa[i].rampUp(j,100::ms);
        }
    }
    0.5 => lisa[current].gain;
    0.0 => lisa[(current+1)%lisa.cap()].gain;
    0.0 => lisa[(current+2)%lisa.cap()].gain;
  //  1 => lisa.record;
  //  1::second => now;
  if (now-beginning >= 50::second) {
      0 => flags.simulationRunning;
      <<<"LEAVING SIM">>>;
      break;
  }
}

// this represents a voice for our LiSa
// note the voice index needs to be set after creation
// the rest of the values will be chosen randomly, but can 
// definitely be set explicitly
private class Voice 
{
    -1 => int index;
    Math.random2f(-4,4) => float rate;
    Math.random2(0,1) => int bi;
    Math.random2(200, 5000)::ms => dur size;
    dur loopStart;
    dur loopEnd;
    Shred shred;
    100::ms => dur fade;
    
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
        Math.random2f(0,s)::samp => loopStart;
        to.playPos(i, loopStart);
        loopStart + size => loopEnd;
        to.rate(i,rate);
        to.bi(i,bi);
    }
    
    fun void doLoop(LiSa l)
    {
        while (true) // ms precision
        {
            if (rate < 0) 
            {
                if (l.playPos(index) <= loopStart-fade)
                {
                    l.rampDown(fade);
                    fade => now;
                    l.playPos(index, loopEnd);
                    l.rampUp(fade);
                    fade => now;
                }
            }
            else
            {
                if (l.playPos(index) >= loopEnd)
                {
                    l.rampDown(fade);
                    fade => now;
                    l.playPos(index, loopStart);
                    l.rampUp(fade);
                    fade => now;
                }
            }
            1::ms => now;
        }
    }
    
    fun void startLoop(LiSa l)
    {
        spork ~ doLoop(l) @=> shred;
    }
    
    fun void stopLoop()
    {
        shred.exit();
    }
}

fun void panner(int which) {
    while (true) {
        if (which) {
            flags.panA => pan.pan;
        } else {
            flags.panB => pan.pan;
        }
        10::ms => now;
    }
}