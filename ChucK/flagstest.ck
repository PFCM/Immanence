Flags flags;

fun void stopRecroding()
{
    while(flags.stopRecording == 0)
        1::ms => now;
    <<<"Stopped">>>;
}

spork ~ stopRecroding();

while (flags.startRecording == 0)
    1::ms => now;
<<<"started">>>;

while (true)
    1::second => now;