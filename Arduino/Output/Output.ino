const int encMotor = 9;
const int lMotor = 10;
const int rMotor = 11;

volatile byte newB;
volatile byte oldA;
volatile long encoderPos;

// states for the encoder motor (head)
int emState = 0;
const int EM_SEARCH = 0;
const int EM_STOP = 1;
const int EM_RAMP = 2;

long EM_STOP_POINT = 4331;

// states for arm motors
// note the motors are different, so applying the same
// settings will have different outcomes
int lState = 0;
int rState = 0;
const int M_WANDER = 0;
const int M_TWITCH = 2;
const int M_STOP = 1;
const int M_RAMP_UP = 3;
const int M_RAMP_DOWN = 4;


void setup() {
  // set up motor pins
  pinMode(encMotor, OUTPUT);
  pinMode(lMotor, OUTPUT);
  pinMode(rMotor, OUTPUT);

  // attach interrupts
  attachInterrupt(1, doEncoderA, CHANGE);
  attachInterrupt(0, doEncoderB, CHANGE);

  Serial.begin(9600);
}

void loop() {
  static long start = millis();

  runEncMotor();
  runArmMotors();

  //if ((millis()%1000) == 0)
  //Serial.println(encoderPos);

// for the sake of testing
  if (millis() - start == 5000) {
    emState = EM_STOP;
    lState = M_RAMP_DOWN;
    rState = M_RAMP_DOWN;
    Serial.println("STOPPING");
  }
  else if (millis() - start == 10000) {
    emState = EM_RAMP;
    lState = M_RAMP_UP;
    rState = M_RAMP_UP;
    Serial.println("RAMPING");
  }
  else if (millis() - start == 15000) {
    emState = EM_SEARCH;
    lState = M_WANDER;
    rState = M_WANDER;
    Serial.println("SEARCHING");
  }
  else if (millis() - start == 25000) {
    emState = EM_STOP;
    lState = M_TWITCH;
    rState = M_TWITCH;
    Serial.println("STOPPING");
    
    start = millis()+5000;
  }
  
  
}  

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
  static long lastPos = 0;
  encoderPos += (newB ^ oldA);
  encoderPos %= EM_STOP_POINT+1; 
  lastPos = encoderPos;
  
}

// runs the encoder motor, depending on what state it should be in at the moment

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
  float dist = (abs((EM_STOP_POINT)-encoderPos));
    if (dist > 4331/2)
      analogWrite(encMotor, 34);
    else if (dist > 100)
      analogWrite(encMotor, 25); 
    else if (dist <= 100 && dist > 50)
      analogWrite(encMotor, 17);
    else if (dist <= 50) {
      analogWrite(encMotor, 0);
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
    mWander(lMotor, 50,200); 
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
    mWander(rMotor, 100,255); 
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
void mWander(int motor, int base, int top) 
{
  static int cap = millis() + random(250,1000);
  static int power = random(20, 255);

  if (millis() < cap) {
    analogWrite(motor, power); 
  } 
  else {
    cap = millis() + random(250, 1000);
    power = random(base,top); 
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





