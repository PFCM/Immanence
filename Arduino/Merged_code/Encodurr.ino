void doEncoderA()
{
  doEncoder();
  oldA = digitalRead(3);
}

void doEncoderB()
{
  newB = digitalRead(2);
  doEncoder();
}

void doEncoder()
{
  encoderPos += (newB ^ oldA);
  encoderPos %= EM_STOP_POINT+1; 
}
