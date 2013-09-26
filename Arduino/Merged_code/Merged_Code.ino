// pins
const int ultrasound = A0;
const int infrared = A1;
const int pressure = A2;
const int encMotor = 11;
const int lMotor = 10;
const int rMotor = 9;

volatile byte newB;
volatile byte oldA;
volatile long encoderPos;

// states for the encoder motor (head)
int emState = 0;
const int EM_SEARCH = 0;
const int EM_STOP = 1;
const int EM_RAMP = 2;

long EM_STOP_POINT = 436587;
long revs = 0;

// states for arm motors
// not the motors are different, so applying the same
// settings will have different outcomes
int lState = 0;
int rState = 0;
const int M_WANDER = 0;
const int M_TWITCH = 2;
const int M_STOP = 1;
const int M_RAMP_UP = 3;
const int M_RAMP_DOWN = 4;

/*   --- UNO PINS FROM SCHEMATIC --  
 const int ultrasound = A1;
 const int infrared = A0;
 const int pressure = A2;
 */
// states
const int CHILLING = 0;
const int WAITING = 1;
const int RECORDING = 2;
const int SPEEDUP = 3;

//start at the fourth state?
int currentstate = SPEEDUP;

//READINGS ARE US/IR/Pressure
int readingsize = 3; //Change this once the pressure has been attached
int readings[] = {
  0,0,0,0};
int lastReadings[3];


int count = 0;


void setup(){
  pinMode(encMotor, OUTPUT);
  pinMode(lMotor, OUTPUT);
  pinMode(rMotor, OUTPUT);

  pinMode(ultrasound,INPUT);
  pinMode(infrared, INPUT);
  pinMode(pressure, INPUT);

  // attach interrupts
  attachInterrupt(0, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);

  Serial.begin(9600);

}

void loop(){
  static long time = millis();
  if ((millis() % 10) && ( time != millis()))
  {
    time = millis();
    readings[0] = currentstate;
    readings[1] = (analogRead(ultrasound) + lastReadings[1])/2; 
    readings[2] = (analogRead(infrared) + lastReadings[2])/2; 
    readings[3] = map(analogRead(pressure),0,1024,0,127);
    
    for (int i = 0; i < 4; i++)
      lastReadings[i] = readings[i];

    //Tim's super Proto-cool
    serialPrint(readings);
    //Serial.print(EncoderPostion) as (DEGREES/3)


    switch(currentstate){

      //IF WE ARE IN THE FIRST STATE
    case CHILLING: 
      //                 USTHRESHOLD            IRTHRESHOLD
      count++;
      if (readings[1] > 80){

        if (readings[2] < 50){
          currentstate = 1; 
          emState = EM_STOP;
          rState = M_TWITCH;
          lState = M_TWITCH;
        }
      }
      break;

      //IF WE ARE IN THE SECOND STATE
    case WAITING:

      //WAIT FOR A MESSAGE FROM ChucK
      if (Serial.available() > 0){
        int temp =  Serial.read();
        Serial.flush();
        currentstate = 2;
      }
      break;

      // IF WE ARE IN THE 3rd STATE
    case RECORDING:

      //WAIT FOR A MESSAGE FROM ChucK
      if (Serial.available() > 0){
        int temp =  Serial.read();
        Serial.flush();
        currentstate = 3;
          emState = EM_RAMP;
          rState = M_RAMP_UP;
          lState = M_RAMP_UP;
        count = 0;
      }
      break;

    case SPEEDUP: 
      count++;

      if (count > 100){
        currentstate = 0; 
        count = 0;
          emState = EM_SEARCH;
          rState = M_WANDER;
          lState = M_WANDER;
      }
      break;
    }
  }

  runEncMotor();
  runArmMotors();
}

void serialPrint(int x[]){
  //Start with a left brace
  Serial.print("[");

  //Print all the characters within the array (except the last one), followed by a comma
  for (int i = 0; i < readingsize; i++)
  {
    Serial.print(x[i]);
    Serial.print(",");
  }
  //Print the last character
  Serial.print(x[readingsize-1]);
  //Right Brace
  Serial.print("]");
  //End Line
  Serial.println();//("\n");
}




