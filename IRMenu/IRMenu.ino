
#include <Wire.h>
#include <LiquidCrystal_I2C.h>            // libraries used
#include <Stepper.h>
#include <IRremote.h>

const int RECV_PIN = 0;                   // prepares the board to receive and decifer IR remote inputs
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

#define STEPS 513                         // number of steps needed for one whole revolution

Stepper stepper1(STEPS,  1, 2, 3, 4);     // pins used for stepper motors
Stepper stepper2(STEPS,  5, 6, 7, 8);     // stepper1 is for the y direction adjustment, stepper3 is for the x direction adjustment
Stepper stepper3(STEPS, 9, 10, 11, 12);

LiquidCrystal_I2C lcd(0x20, 16, 2);       // set the LCD address to 0x20 for a 16 chars and 2 line display

int menuval = 1;                          // value used to navigate menu levals
int menustate = 0;                        // value used to navigate options in each menu
int motorval = 0;
int power = 1;                            // value to check state of lcd backlight
int dirs = 1;
int first = 0;
int second = 0;
int wait = 0;

extern byte lambda[8];                    // declares lcd custom characters defined in 'helperFunctions'
extern byte arrow[8];
extern byte smile[8];
extern byte percentage[8];

void setup() {

  irrecv.enableIRIn();             // initialize IR receiver
  irrecv.blink13(true);

  lcd.init();                      // initialize the lcd
  lcd.backlight();                 // switches on the backlight of lcd

  stepper1.setSpeed(35);           // set the motor speeds
  stepper2.setSpeed(35);           // ideal value is high as possible for particular model of stepper
  stepper3.setSpeed(35);

  lcd.createChar(0, lambda);       // creates the custom characters used on the display
  lcd.createChar(1, arrow);
  lcd.createChar(2, smile);
  lcd.createChar(3, percentage);

}

void loop() {
  
int percent = 100 - (wait/10);

  if (irrecv.decode(&results)) {

    if (results.value == 0XFFFFFFFF)
      results.value = key_value;

    switch (results.value) {

      case 0xFD00FF: //POWER BUTTON
      
        toggleBacklight();
        break;

      case 0xFD609F: //RIGHT BUTTON
        //Serial.println("Right Button");
        switch(menuval){
          case 1:
            menustate = (menustate + 2)%3-1;
            //wrap(menustate, 1, -1);
            delay(200);
          break;

          case 4:
            stepper3.step(5);
            delay(wait);
          break;
        }
                    break;

      case 0xFD20DF: //LEFT BUTTON
        //Serial.println("Left Button");
        switch(menuval){
          case 1:
            menustate = menustate - 1;
            wrap(menustate, 1, -1);
            delay(200);
          break;

          case 4:
            stepper3.step(-5);
            delay(wait);
          break;
        }
                    break;

      case 0xFD807F: //UP-VOL+ BUTTON
        //Serial.println("Up Button");
        switch(menuval){

          case 3:
            menustate = menustate + 1;
            wrap(menustate, 1, 0);
            delay(200);
          break;

          case 4:
            stepper1.step(5);
            delay(wait);
          break;
                  }

                  break;

      case 0xFD906F: //DOWN-VOL- BUTTON
        //Serial.println("Down Button");
        switch(menuval){

          case 3:
            menustate = menustate - 1;
            wrap(menustate, 1, 0);
            delay(200);

          case 4:
            stepper1.step(-5);
            delay(wait);
          break;
                  }
                    break;

      case 0xFDA05F: //OK-PLAY BUTTON
        delay(200);

        switch(menuval){

         case 1:
          if(menustate == 1){
            menuval = 3;
            menustate = 0;
            lcd.clear();
            printing(0, 0, "   LOADING...   ");
            delay(500);
          }
          else if(menustate == -1){
            menuval = 4;
            menustate = 0;
            lcd.clear();
            printing(0, 0, "   LOADING...   ");
            delay(500);

            printing(0, 0, "  CHANGE SPEED  ");
            if(percent < 10){lcd.setCursor(8,1);}
            if(percent >= 10){lcd.setCursor(7,1);}
            if(percent = 100){lcd.setCursor(6,1);}
            lcd.print(percent);
            lcd.setCursor(9,1);
            lcd.write(3);
            }
          

         break;

         case 3:

          if(menustate == 0){
            stepper1.step(16);
            stepper2.step(16);
            stepper3.step(16);
            delay(500);
          }
          if(menustate == 1){
            stepper1.step(-16);
            stepper2.step(-16);
            stepper3.step(-16);
            delay(500);
          }
         break;

        }
                    break;

      case 0xFD40BF: //FUNC/STOP BUTTON

        returnButton();
        
                    break;

      case 0xFD50AF: // UP BUTTON (SPEED)

      switch(menuval){
        case 4:
          if(wait > 0){wait = wait - 100;}
          if(wait <= 0){wait = 0;}
          printing(6, 1, "   ");
          if(percent < 10){lcd.setCursor(8,1);}
          if(percent >= 10){lcd.setCursor(7,1);}
          if(percent >= 100){lcd.setCursor(6,1);}
          lcd.print(percent);
        break;
      }
      delay(200);
                    break;

      case 0xFD10EF: // DOWN BUTTON (SPEED)

      switch(menuval){
        case 4:
          if(wait < 1000){wait = wait + 100;}
          if(wait >= 1000){wait = 1000;}
          printing(6, 1, "   ");
          if(percent < 10){lcd.setCursor(8,1);}
          if(percent >= 10){lcd.setCursor(7,1);}
          if(percent >= 100){lcd.setCursor(6,1);}
          lcd.print(percent);
        break;
      }
      delay(200);
                    break;
    }

    key_value = results.value;
    irrecv.resume();
  }

  // Menu Selection

  switch (menuval) {                // each case of 'menuval' is a different menu set/area

    case 1:     // Main Menu

      switch (menustate) {

        case 0:
        printing(0, 0, "    WELCOME!    ");
        printing(0, 1, "  ");
        lcd.write(2);
        printing(3, 1, "          ");
        lcd.write(2);
        printing(14, 1, "  ");
        break;

        case -1:
        printing(0, 0, "     MANUAL     ");
        printing(0, 1, "   ADJUSTMENT   ");
        break;

        case 1:
        printing(0, 0, "    MOVE "); lcd.write(0); printing(10, 0, "/2    ");
        printing(0, 1, "  (ALL MOTORS)  ");
        break;
        }

      break;

    case 3:     // Move all motors 16 steps at once menu

      printing(0, 0, "FORWARDS        ");
      printing(0, 1, "BACKWARDS       ");

      switch (menustate) {

        case 0:
          lcd.setCursor(15, 0);
          lcd.write(1);
        break;

        case 1:
          lcd.setCursor(15, 1);
          lcd.write(1);
        break;
      }

      break;
  }
}
