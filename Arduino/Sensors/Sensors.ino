int ultrasound = A15;
int infrared = A14;
int pressure = A13;

//START AT THE FOURTH STATE
int currentstate = 3;

//READINGS ARE US/IR/Pressure
int readings[] = {0,0,0};


void setup(){
  
 Serial.begin(9600);
  
}

void loop(){
  
 readings[0] = map(analogRead(ultrasound),1023,0,0,127);
 readings[1] = map(analogRead(infrared),1023,0,0,127); 
 readings[2] = map(analogRead(pressure),0,1024,0,127);
 
 //Tim's super Proto-cool
 Serial.print(200);
 Serial.print(readings[0]);
 Serial.print(205);
 Serial.print(readings[1]);
 Serial.print(210);
 Serial.print(readings[2]);
 Serial.print(215);
 //Serial.print(EncoderPostion) as (DEGREES/3)
 
 
switch(currentstate){

//IF WE ARE IN THE FIRST STATE
case 0: 
 //                 USTHRESHOLD            IRTHRESHOLD
 if ((readings[0] > 100) && (readings[1] > 23)){
  currentstate = 1;
 break; 
 }

 
 //IF WE ARE IN THE SECOND STATE
 case 1:
 
 //WAIT FOR A MESSAGE FROM ChucK
 if (Serial.available() > 0){
 int temp =  Serial.read();
 currentstate = 2;
 }
 break;
 
 // IF WE ARE IN THE 3rd STATE
 case 2:
 
  //WAIT FOR A MESSAGE FROM ChucK
 if (Serial.available() > 0){
 int temp =  Serial.read();
 currentstate = 3;
 }
 break;
 
 case 3: 
 /*if(motoroutput > motorthreshold){
   currentstate = 0;
 }
 */
 break;
}

 delay (10);
}
