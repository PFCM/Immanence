Flags flags;
1 => flags.panA;
-1 => flags.panB;

Hid kb;
0 => int device;

SinOsc s => dac;

0 => s.gain;

//Open keyboard and mouse.

if (!kb.openKeyboard(device)) {   
    <<<"Problem opening keyboard">>>;
    me.exit();   
}

//HID message from the device

HidMsg msg;

SerialIO serial;
string line;
string stringInts[4];
int data[4];

int currentstate;

0 => int CHILLING;
1 => int WAITING;
2 => int RECORDING;
3 => int SPEEDUP; 

spork ~ thing();  

SerialIO.list() @=> string list[];
for( int i; i < list.cap(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}
if(!serial.open(2, SerialIO.B9600, SerialIO.ASCII)) {
    <<<"did not open">>>   ;
}

fun void serialPoller(){
    while( true )
    {
        // Grab Serial data
        serial.onLine()=>now;
        //serial.getLine()=>line;
        serial.getLine()=>line;
        if( line$Object == null ){
            continue;
        }
        
        0 => stringInts.size;
        
        // Line Parser
        if (RegEx.match("\\[([0-9]+),([0-9]+),([0-9]+),([0-9]+)\\]", line , stringInts))
        {
            for( 1=>int i; i<stringInts.cap(); i++)  
            {
                // Convert string to Integer
                Std.atoi(stringInts[i])=>data[i-1];
            }
        }
    }
}
spork ~ serialPoller();

// COMPOSITION
1 => int once;
1 => int readyfornext;
0 => int simNum;
while (true)
{
    if (data[0] != currentstate){
        
        data[0] => currentstate;
        if(currentstate == WAITING){
            <<<"WAITING  :  " + data[1], data[2], data[3]>>>;
            // This thing that we spork is actually our FFT analyzer
            //waiting for a vocal onset. Once this occurs it will send
            //any random 1 character message to Arduino -- serial<= "n" <= IO.newline();
            //spork~ thing();
            me.dir() => string dir;
            Machine.add(dir + "/FFTListener.ck");
            Machine.add(dir + "/evolutionbufferswap.ck:"+simNum);
            1 => flags.simulationRunning;
            (simNum+1)%2 => simNum;
            while (flags.recording == 0)
                10::ms => now;
            <<<"BEGINNING RECORD">>>;
            serial <= "n";
        }
        if(currentstate == RECORDING){
            <<<"RECORDING  :  " + data[1], data[2], data[3]>>>;
            //This thing that gets sporked will start recording an input
            //and will also have hysteresis to determine a stopping point
            //for the audio. it will then send any
            //  spork~thing();
            while (flags.recording == 1) {
                10::ms => now;
                <<<"waiting for recording to end">>>;
            }
            <<<"SENDING END RECORD">>>;
            serial <= "n";
            0 => once;
            21::second => now;
            1 => readyfornext;
        }
    }
    
    if(currentstate == SPEEDUP){
        <<<"SPEEDUP  :  " + data[1], data[2], data[3]>>>;
        
    }
    if(currentstate == CHILLING){
       // <<<"CHILLING  :  " + data[1], data[2], data[3]>>>;
        while (readyfornext && flags.recording == 0 || once == 0) {
            <<<"SHOULD BE GOOD TO GO">>>;
            serial <= "n";
            1 => once;
            spork~ rotator();
            0 => readyfornext;
        }
    }
    
    
    10::ms => now;
}

fun void thing(){
    
    
    kb => now;  
    if(kb.recv(msg)){
        
        // if (msg.isButtonDown())
        {
            serial <= "n";
        }
    }
}

fun void rotator() {
    2.0 / 1300 => float step;
    
    step * flags.panB => float dA;
    step * flags.panA => float dB;
    
    for (int i; i < 130; i++) {
        dA +=> flags.panA;
        dB +=> flags.panB;
        100::ms => now;
    }
}