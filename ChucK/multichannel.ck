Pan2 pFront;
pFront.left => dac.chan(0);
pFront.right => dac.chan(1);
Pan2 pBack;
pBack.left => dac.chan(2);
pBack.right => dac.chan(3);

SinOsc s => pFront;
220 => s.freq;

SinOsc s2 => pBack;
330 => s2.freq;

while (true) 
{
    1::second => now;
}