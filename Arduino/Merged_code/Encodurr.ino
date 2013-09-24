void doEncoderA()
{
  doEncoder();
  oldA = digitalRead(2);
}

void doEncoderB()
{
  newB = digitalRead(3);
  doEncoder();
}

void doEncoder()
{
  static long lastPos = 0;
  encoderPos += (newB ^ oldA)*100;
  encoderPos %= EM_STOP_POINT+1; 
  if (lastPos > encoderPos)
    revs++;
  lastPos = encoderPos; 
}
