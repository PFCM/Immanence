Flags flags;

new Event @=> flags.startRecording;
new Event @=> flags.stopRecording;

fun void stopRecroding()
{
    flags.stopRecording => now;
    <<<"Stopped">>>;
}

spork ~ stopRecroding();

flags.startRecording => now;
<<<"started">>>;