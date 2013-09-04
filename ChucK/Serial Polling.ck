Hid kb;
0 => int device;


//Open keyboard and mouse.

if (!kb.openKeyboard(device)) {   
    <<<"Problem opening keyboard">>>;
    me.exit();   
}

//HID message from the device

HidMsg msg;

SerialIO serial;
string line;
string stringInts[3];
int data[3];

int currentstate;

0 => int CHILLING;
1 => int WAITING;
2 => int RECORDING;
3 => int SPEEDUP; 

SerialIO.list() @=> string list[];
for( int i; i < list.cap(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}
if(!serial.open(0, SerialIO.B9600, SerialIO.ASCII)) {
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
        if (RegEx.match("\\[([0-9]+),([0-9]+),([0-9]+)\\]", line , stringInts))
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
while (true)
{
    if (data[0] != currentstate){
        
        data[0] => currentstate;
        if(currentstate == WAITING){
            <<<"WAITING  :  " + data[1] + "  :  " + data[2]>>>;
            // This thing that we spork is actually our FFT analyzer
            //waiting for a vocal onset. Once this occurs it will send
            //any random 1 character message to Arduino -- serial<= "n" <= IO.newline();
            spork~ thing();
        }
        if(currentstate == RECORDING){
            <<<"RECORDING  :  " + data[1] + "  :  " + data[2]>>>;
            //This thing that gets sporked will start recording an input
            //and will also have hysteresis to determine a stopping point
            //for the audio. it will then send any
            spork~thing();
            
        }
    }
    
    if(currentstate == CHILLING){
        <<<"CHILLING  :  " + data[1] + "  :  " + data[2]>>>;
    }
    
    if(currentstate == SPEEDUP){
        <<<"SPEEDUP  :  " + data[1] + "  :  " + data[2]>>>;
        
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

