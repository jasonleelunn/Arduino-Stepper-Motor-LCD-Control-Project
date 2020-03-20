#include <Stepper.h>

#define STEPS 513

Stepper stepper1(STEPS, 10,11,12,13);
Stepper stepper2(STEPS,  6, 7, 8, 9);
Stepper stepper3(STEPS,  2, 3, 4, 5);


bool buttons[3]   = {0,0,0};
int buttonPins[3] = {17,18,19};
int dirs[3]       = {1,1,1};
int potPins[3]    = {14,15,16};



void setup() {
  Serial.begin(9600);
  //while (! Serial);
  //Serial.println("CHOOSE A FRACTION OF REVOLUTION:");
 
  stepper1.setSpeed(25);
  stepper2.setSpeed(25);
  stepper3.setSpeed(35);


  for(int i=0; i<3; i++){
    pinMode(buttonPins[i], INPUT);
    pinMode(potPins[i], INPUT);
  }
}

void loop() {
  int extremity = readDirFromAnalog(potPins);
  readButtonsFromAnalog(buttonPins);
  
  if(buttons[0]){stepper1.step(dirs[0]);}
  if(buttons[1]){stepper2.step(dirs[1]);}

  
  if(buttons[2]){stepper3.step(dirs[2]); delay(1000);}

  
  Serial.println(extremity);
  if(abs(extremity)<500){
     delay((500-extremity)/2);
  }
 
}

int sign(int x){
    if(x > 0) return 16;
    if(x < 0) return -16;
    return 0;
}


void printToSerial(int extremity){
  Serial.read(); 
  Serial.println("DELAY: "+String((513-extremity)/2)+" Extremity: "+String(extremity));
  Serial.println("buttons: "+String(buttons[0])+", "+String(buttons[1])+", "+String(buttons[2]));
}


int readDirFromAnalog(int pins[]){
  int extremity = analogRead(pins[0])-512;
  dirs[0] = sign(extremity);
  dirs[1] = sign(analogRead(pins[1])-512);
  dirs[2] = sign(analogRead(pins[2])-512);
  return abs(extremity);
}

void readButtonsFromAnalog(int pins[]){
  for(int i=0; i<3; i++){
    buttons[i] = analogRead(pins[i])>512;
  }
}
