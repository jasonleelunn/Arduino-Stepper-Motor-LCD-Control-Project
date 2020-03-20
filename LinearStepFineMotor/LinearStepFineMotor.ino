
#include <Wire.h>
#include <Stepper.h>

#define STEPS 200                     // number of steps needed for one whole revolution

Stepper stepper1(STEPS, 7, 8, 9, 10);     // pins used for stepper motors

bool buttons   = 0;
int buttonPins = 19;
int potPins  = 17;
int dirs   = 1;

void setup() {
  
   stepper1.setSpeed(60);          // set the motor speeds

   pinMode(buttonPins, INPUT);
   pinMode(potPins, INPUT);
   pinMode(13, OUTPUT);
   
}

void loop() {

  delay(500);
  readDirFromAnalog(potPins);
  readButtonsFromAnalog(buttonPins);

  if(buttons){
    digitalWrite(LED_BUILTIN, HIGH);
    stepper1.step(100*dirs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
}

int sign(int x) {                 // function to determine potentiometer direction
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

int readDirFromAnalog(int pins) {
  int potval = analogRead(pins);
  int potdir = potval - 512;
  dirs = sign(potdir);
  return abs(potdir);
}

void readButtonsFromAnalog(int pins) {
    buttons = analogRead(pins) > 512;
}
