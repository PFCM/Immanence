
void runEncMotor() 
{
  if (emState == EM_SEARCH)
    emSearch();
  else if (emState == EM_STOP)
    emStop();
  else if (emState == EM_RAMP)
    emRamp();
}

// the search state call
// ramps up to a random value then stops
void emSearch() 
{
  static byte power = 0;
  static byte cap = random(125, 255);
  static int searchState = 0;
  static long slowStart;
  static long time;
  if (searchState == 0) {
    if (millis() % 10 == 0 && millis() != time) {
      time = millis();
      power += 10;
      if (power >= cap) {
        searchState = 1; 
        slowStart = millis();
      } 
      else {
        analogWrite(encMotor, power);
      }
    } 
  }
  if (searchState == 1) {
    power = 0;
    analogWrite(encMotor, power);
    if (millis() - slowStart >= 1000) {
      searchState = 0;
      cap = random(125, 255);
      Serial.print("new cap: "); 
      Serial.println(cap);
    }
  }
}

// moves the motor slowly to a given stop point
// note these exact numbers will have to change 
// when the motor is loaded
void emStop()
{
  static int madeit = 0;
  float dist = (abs((EM_STOP_POINT-error*revs)-encoderPos));
  if (dist > 8000) madeit=0;
  if (madeit==0) {
    if (dist > 433088/2)
      analogWrite(encMotor, 40);
    else if (dist > 20000)
      analogWrite(encMotor, 25); 
    else if (dist <= 20000 && dist > 8000)
      analogWrite(encMotor, 15);
    else if (dist <= 8000) {
      madeit = 1;
      Serial.println("madeit");
    }
    else 
      analogWrite(encMotor, 0);
  } 
  else {
    analogWrite(encMotor, 0); 
   // Serial.println("stopped");
    // if (encoderPos != EM_STOP_POINT) madeit = 0;
  }
}

void emRamp()
{ 
  static int target = 255;
  static int power = 0;
  static int rate = 50;
  static long time = millis();
  if (millis() % rate == 1 && millis() != time) {
    time = millis();
    if (power < target) {
      analogWrite(encMotor, power++);
    } 
    else {
      analogWrite(encMotor, target); 
    }
  }
}

int wrap(int in, int cap) 
{
  if (in < 0) return cap+in;
  if (in >= cap) return cap-in;
  return in; 
}

// for the the other motors
void runArmMotors() 
{
  if (lState == M_WANDER) {
    mWander(lMotor); 
  } 
  else if (lState == M_TWITCH) {
    mTwitch(lMotor);
  } 
  else if (lState == M_STOP) {
    mStop(lMotor);
  } 
  else if (lState == M_RAMP_UP) {
    mRampUp(lMotor);
  } 
  else if (lState == M_RAMP_DOWN) {
    mRampDown(lMotor);
  }

  if (rState == M_WANDER) {
    mWander(rMotor); 
  } 
  else if (rState == M_TWITCH) {
    mTwitch(rMotor);
  } 
  else if (rState == M_STOP) {
    mStop(rMotor);
  } 
  else if (rState == M_RAMP_UP) {
    mRampUp(rMotor);
  } 
  else if (rState == M_RAMP_DOWN) {
    mRampDown(rMotor);
  }
}

// wanders at fairly random speeds for random amounts of time
// maybe a good idea to come up with some significant numbers for this
void mWander(int motor) 
{
  static int cap = millis() + random(250,1000);
  static int power = random(20, 255);

  if (millis() < cap) {
    analogWrite(motor, power); 
  } 
  else {
    cap = millis() + random(250, 1000);
    power = random(50,200); 
  }
}

// twitches at random time intervals
void mTwitch(int motor)
{
  static int twitch = 1;
  static int power = 255;
  static int length = 2;
  static int first = 1;
  static long time;
  
  if (twitch) {
    if (first) {
        time = millis() + length;
        first = 0;
    }
    if (millis() <= time)
      analogWrite(motor, power);
    else {
       twitch = 0;
       time = millis() + random(100,500);
    } 
  } else {
    analogWrite(motor, 0);
     if (millis() >= time) {
        twitch = 1;
        first = 1;
     } 
  }

}

// ramps upwards to max speed
// (will need to be careful with the super fast one)
void mRampUp(int motor)
{
  static int power = 0;
  static int rate = 100;
  static long time = millis();

  if (millis() % rate == 0 && time != millis()) {
    time = millis();
    if (power < 255)
      analogWrite(motor, power++);
    else 
      analogWrite(motor, 255);
  }
}

// ramps down from max to nothing
// if called off a speed other than max, will kind of twitch
void mRampDown(int motor)
{
  static int power = 255;
  static int rate = 60;
  static long time = millis();
  if (millis() % rate == 0 && time != millis()) {
    if (power > 0)
      analogWrite(motor, power--);
    else
      analogWrite(motor, 0); 
  } 
}  

void mStop(int motor)
{
  analogWrite(motor, 0); 
}
