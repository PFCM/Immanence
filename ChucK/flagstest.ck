Flags flags;

fun void stopRecroding()
{
    flags.stopRecording => now;
    <<<"Stopped">>>;
}

spork ~ stopRecroding();

flags.startRecording => now;
<<<"started">>>;