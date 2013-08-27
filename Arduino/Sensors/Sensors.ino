int ultrasound = A15;
int infrared = A14;



int readings[] = {0,0};


void setup(){
 
  
 Serial.begin(9600);
  
}

void loop(){
 
 readings[0] = analogRead(ultrasound);
 readings[1] = analogRead(infrared);
 
 Serial.print(readings[0]);
 Serial.print("  /  ");
 Serial.println(readings[1]);
 
 delay (100);
 
  
}
