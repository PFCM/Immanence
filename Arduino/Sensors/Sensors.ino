// pins
const int ultrasound = A15;
const int infrared = A14;
const int pressure = A13;

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


void setup(){

  Serial.begin(9600);

}

void loop(){
  readings[0] = currentstate;
  readings[1] = analogRead(ultrasound); //map(analogRead(ultrasound),1023,0,0,127);
  readings[2] = analogRead(infrared); //map(analogRead(infrared),1023,0,0,127); 
  readings[3] = map(analogRead(pressure),0,1024,0,127);

  //Tim's super Proto-cool
serialPrint(readings);
  //Serial.print(EncoderPostion) as (DEGREES/3)


  switch(currentstate){

    //IF WE ARE IN THE FIRST STATE
  case CHILLING: 
    //                 USTHRESHOLD            IRTHRESHOLD
    if ((readings[0] > 100) && (readings[1] > 23)){
      currentstate = 1;
      break; 
    }


    //IF WE ARE IN THE SECOND STATE
  case WAITING:

    //WAIT FOR A MESSAGE FROM ChucK
    if (Serial.available() > 0){
      int temp =  Serial.read();
      currentstate = 2;
    }
    break;

    // IF WE ARE IN THE 3rd STATE
  case RECORDING:

    //WAIT FOR A MESSAGE FROM ChucK
    if (Serial.available() > 0){
      int temp =  Serial.read();
      currentstate = 3;
    }
    break;

  case SPEEDUP: 
    /*if(motoroutput > motorthreshold){
     currentstate = 0;
     }
     */
    break;
  }

  delay (10);
}

void serialPrint(int x[]){
  //Start with a left brace
  Serial.print("[");
  
  //Print all the characters within the array (except the last one), followed by a comma
  for (int i = 0; i < readingsize-1; i++)
  {
    Serial.print(x[i]);
    Serial.print(",");
  }
  //Print the last character
  Serial.print(x[readingsize-1]);
  //Right Brace
  Serial.print("]");
  //End Line
  Serial.print("\n");
}

