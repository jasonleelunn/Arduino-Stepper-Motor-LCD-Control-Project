
#include <Wire.h>
#include <LiquidCrystal_I2C.h>            // libraries used
#include <Stepper.h>

#define STEPS 200                         // number of steps needed for one whole revolution

Stepper stepper1(STEPS,  2, 3, 4, 5);     // pins used for stepper motors
Stepper stepper2(STEPS,  6, 7, 8, 9);
Stepper stepper3(STEPS, 10,11,12,13);


LiquidCrystal_I2C lcd(0x20, 16, 2);       // set the LCD address to 0x20 for a 16 chars and 2 line display

bool buttons[2]   = {0, 0};               // button[0] is OK/FORWARD, button[1] is RETURN/BACKWARDS
int buttonPins[2] = {17, 16};
int potPins  = 14;
int dirs   = 1;
int menuval = 1;                          // value used to navigate menus
int motorval = 0;

byte lambda[8] = {                        // designs custom characters for lcd
  B00000,
  B10000,
  B01000,
  B00100,
  B00100,
  B01010,
  B10001,
  B00000
};

byte arrow[8] = {
  B00000,
  B00100,
  B01100,
  B11111,
  B11111,
  B01100,
  B00100,
  B00000
};


void setup() {

  Serial.begin(9600);              // open data channel

  lcd.init();                      // initialize the lcd
  lcd.backlight();                 // switches on the backlight of lcd

  stepper1.setSpeed(60);          // set the motor speeds
  stepper2.setSpeed(60);          // ideal value is high as possible for particular model of stepper
  stepper3.setSpeed(60);

  for(int i=0; i<2; i++){         // sets the analogue pins to INPUT mode
    pinMode(buttonPins[i], INPUT);
  }
  pinMode(potPins, INPUT);

  lcd.createChar(0, lambda);      // creates the custom characters used on the display
  lcd.createChar(1, arrow);
}


void loop() {

  int potty = readDirFromAnalog(potPins);   // assign a value to potentiometer state
  readButtonsFromAnalog(buttonPins);

  int selector = potty * dirs;      // dirs is just +ve or -ve 1 so this variable contains complete info on the state of the potentiometer
  int noSteps = selector / 10;      // used to set the possible step range for manual selection


// Menu Selection

  switch(menuval) {                 // each case of 'menuval' is a different menu set/area

case 1:     // Main Menu

//  functionx(selector);

if (selector >= 100) {
    printing(0, 0, "    MOVE "); lcd.write(0); printing(10, 0, "/2    ");
    printing(0, 1, "  (ALL MOTORS)  ");
  }
  else if (selector <= -100) {
    printing(0, 0, "    MANUALLY    ");
    printing(0, 1, "  CHOOSE STEPS  ");
  }
  else {
    printing(0, 0, "INTERFEROMETERY ");
    printing(0, 1, "    IS COOL     ");
  }

  if (buttons[0] && selector <= -100) {       // loading screen for changing menu state
    lcd.clear();
    printing(0,0,"   LOADING...   ");
    menuval = 4;
    delay(1000);
  }

    if (buttons[0] && selector >= 100) {
    lcd.clear();
    printing(0,0,"   LOADING...   ");
    menuval = 3;
    delay(1000);
  }

  break;

case 2:     // Step choice for induvidual motor menu

    printing(0, 0, "HOW MANY STEPS? ");
    printing(0, 1, "Steps:          ");
    printing(8, 1, ""); lcd.print(noSteps);

    if(buttons[0] && motorval == 1) {
      stepper1.step(noSteps);
      delay(1000);
    }

     if(buttons[0] && motorval == 2) {
      stepper3.step(noSteps);
      delay(1000);
    }

    if(abs(potty)<500){
     delay((500-potty)/2);
    }

    if (buttons[1]) {
      lcd.clear();
      printing(0,0,"   LOADING...   ");
      menuval = 4;
      delay(1000);
    }

  break;

case 3:     // Move all motors 16 steps at once menu

  printing(0, 0, "FORWARDS        ");
  printing(0, 1, "BACKWARDS       ");

  if (selector <= 0){
    lcd.setCursor(15, 0);
    lcd.write(1);
  }
  if (selector > 0){
    lcd.setCursor(15, 1);
    lcd.write(1);
  }

  if (buttons[0]) {
    stepper1.step(dirs * 16);
    stepper2.step(dirs * 16);
    stepper3.step(dirs * 16);
    delay(1000);
  }

  if (buttons[1]) {
      lcd.clear();
      printing(0,0,"   LOADING...   ");
      menuval = 1;
      delay(1000);
    }

  break;

case 4:     // Select specific motor to move panel in x or y direction menu

    printing(0, 0, "CHANGE X VAL    ");
    printing(0, 1, "CHANGE Y VAL    ");

  if (selector <= 0){                                                        // ***change selector to dirs??***
    lcd.setCursor(15, 0);
    lcd.write(1);
  }
  if (selector > 0){
    lcd.setCursor(15, 1);
    lcd.write(1);
  }


    if (buttons[0] && selector <= 0) {
      lcd.clear();
      printing(0,0,"   LOADING...   ");
      menuval = 2;
      motorval = 1;
      delay(1000);
  }

    if (buttons[0] && selector > 0) {
      lcd.clear();
      printing(0,0,"   LOADING...   ");
      menuval = 2;
      motorval = 2;
      delay(1000);
  }


    if (buttons[1]) {
      lcd.clear();
      printing(0,0,"   LOADING...   ");
      menuval = 1;
      delay(1000);
    }

  break;

  }

}


int sign(int x) {                 // function to determine potentiometer direction
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

int readDirFromAnalog(int pins[]) {
  int potval = analogRead(pins);
  int potdir = potval - 512;
  dirs = sign(potdir);
  return abs(potdir);
}

void readButtonsFromAnalog(int pins[]) {
  for (int i = 0; i < 2; i++) {
    buttons[i] = analogRead(pins[i]) > 512;
  }
}

int printing(int y, int z, String a) {        // function used to streamline printing to LCD
  lcd.setCursor(y, z);
  lcd.print(a);
}


// ***finish loading screen function for tidiness***

//void loading(int forbac, int menuval,
//
//if (buttons[forbac] && selector <= -100) {
//    lcd.clear();
//    printing(0,0,"   LOADING...   ");
//    menuval = menuval;
//    delay(1000);
//  }
