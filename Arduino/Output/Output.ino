#include <math.h>

const int motor_1 = 11;

volatile byte newB;
volatile byte oldA;
volatile int encoderPos;

int emState = 0;
const int EM_SEARCH = 0;
const int EM_STOP = 1;
const int EM_RAMP = 2;

const int EM_STOP_POINT = 4330;


void setup() {
  pinMode(motor_1, OUTPUT);

  // attach interrupts
  attachInterrupt(0, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);

  Serial.begin(9600);
}

void loop() {
  static long start = millis();

  runEncMotor();

  //if ((millis()%1000) == 0)
  //Serial.println(encoderPos);

  if (millis() - start == 5000) {
    emState = EM_STOP;
    Serial.println("STOPPING");
  }
  else if (millis() - start == 10000) {
    emState = EM_RAMP;
    Serial.println("RAMPING");
  }
  else if (millis() - start == 13000) {
    emState = EM_SEARCH;
    Serial.println("SEARCHING");
  }
  else if (millis() - start == 18000) {
    emState = EM_STOP;
    Serial.println("STOPPING");
  }
}  

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
  encoderPos += newB ^ oldA;
  encoderPos %= 4331; // this is not precise, after a few revs it gets all jacked up
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
        analogWrite(motor_1, power);
      }
    } 
  }
  if (searchState == 1) {
    power = 0;
    analogWrite(motor_1, power);
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
  if (abs(encoderPos - EM_STOP_POINT) > 3 && !madeit) {
    float dist =  EM_STOP_POINT - encoderPos;
    if (dist > 0 && dist > 4331/2)
      analogWrite(motor_1, 40);
    else if (dist > 0 && dist > 400)
      analogWrite(motor_1, 25); 
    else if (dist > 0 && dist < 400 && dist > 100)
      analogWrite(motor_1, 17);
    else if (dist > 0 && dist < 3) {
      madeit = 1;
    }
    else 
      analogWrite(motor_1, 15);
  } 
  else {
    analogWrite(motor_1, 0); 
   // if (encoderPos != EM_STOP_POINT) madeit = 0;
  }
}

void emRamp()
{ 
  static int target = 255;
  static int power = 0;
  static int rate = 10;
  static long time = millis();
  if (millis() % rate == 1 && millis() != time) {
    time = millis();
    if (power < target) {
      analogWrite(motor_1, power++);
    } 
    else {
      analogWrite(motor_1, target); 
    }
  }
}

int wrap(int in, int cap) 
{
  if (in < 0) return cap+in;
  if (in >= cap) return cap-in;
  return in; 
}




