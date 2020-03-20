
#include <Wire.h>
#include <LiquidCrystal_I2C.h>            // libraries used
#include <Stepper.h>
#include <IRremote.h>

const int RECV_PIN = 0;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

#define STEPS 200                         // number of steps needed for one whole revolution

Stepper stepper1(STEPS,  2, 3, 4, 5);     // pins used for stepper motors
Stepper stepper2(STEPS,  6, 7, 8, 9);
Stepper stepper3(STEPS, 10, 11, 12, 13);

LiquidCrystal_I2C lcd(0x20, 16, 2);       // set the LCD address to 0x20 for a 16 chars and 2 line display

int menuval = 1;                          // value used to navigate menu levals
int menustate = 0;                        // value used to navigate options in each menu
int motorval = 0;
int power = 1;                            // value to check state of lcd backlight
int dirs = 1;
int first = 0;
int second = 0;

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

byte smile[8] = {
  B00000,
  B11011,
  B11011,
  B00000,
  B00000,
  B10001,
  B11111,
  B00000
};

void setup() {

  Serial.begin(9600);              // open data channel

  irrecv.enableIRIn();             // initialize IR receiver
  irrecv.blink13(true);

  lcd.init();                      // initialize the lcd
  lcd.backlight();                 // switches on the backlight of lcd

  stepper1.setSpeed(35);          // set the motor speeds
  stepper2.setSpeed(35);          // ideal value is high as possible for particular model of stepper
  stepper3.setSpeed(60);

  lcd.createChar(0, lambda);      // creates the custom characters used on the display
  lcd.createChar(1, arrow);
  lcd.createChar(2, smile);
}


void loop() {

int noSteps = dirs*((first*10) + (second));

  if (irrecv.decode(&results)) {

    if (results.value == 0XFFFFFFFF)
      results.value = key_value;

    switch (results.value) {

      case 0xFD00FF: //POWER BUTTON

        delay(200);
        if (power == 1) {
          lcd.noBacklight();
          power = 0;}
        else {
          lcd.backlight();
          power = 1;}

                    break;

      case 0xFD609F: //RIGHT BUTTON
        delay(200);

        switch(menuval){
          case 1:
            menustate = menustate + 1;
            wrap(menustate, 1, -1);
          break;

          case 2:
            menustate = menustate + 1;
            wrap(menustate, 1, 0);
          break;
        }
                    break;

      case 0xFD20DF: //LEFT BUTTON
        delay(200);

        switch(menuval){
          case 1:
            menustate = menustate - 1;
            wrap(menustate, 1, -1);
          break;

          case 2:
            menustate = menustate - 1;
            wrap(menustate, 1, 0);
          break;
        }
                    break;

      case 0xFD807F: //UP-VOL+ BUTTON
        delay(200);
        menustate = menustate + 1;
        wrap(menustate, 1, 0);
                    break;

      case 0xFD906F: //DOWN-VOL- BUTTON
        delay(200);
        menustate = menustate - 1;
        wrap(menustate, 1, 0);
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

         case 4:

          if(menustate == 0){
            menuval = 2;
            motorval = 1;
          }
          else if(menustate == 1){
            menuval = 2;
            motorval = 2;
          }
            lcd.clear();
            printing(0, 0, "   LOADING...   ");
            delay(500);

            printing(0, 0, "HOW MANY STEPS? ");
            printing(0, 1, "Steps:  XX      ");
            menustate = 0;
            lcd.setCursor(8, 1);
            lcd.blink();

         break;

         case 2:
          if(motorval == 1){
            stepper1.step(noSteps);
            lcd.noBlink();
            menustate = 0;
            delay(500);
          }
          else if(motorval == 2){
            stepper3.step(noSteps);
            lcd.noBlink();
            menustate = 0;
            delay(500);
          }
         break;
        }
                    break;

      case 0xFD40BF: //RETURN BUTTON

        lcd.clear();
        printing(0, 0, "   LOADING...   ");

        if(menuval == 3){menuval = 1; menustate = 1;}
        if(menuval == 4){menuval = 1; menustate = -1;}
        if(menuval == 2){menuval = 4; menustate = 0; lcd.noBlink();}
        delay(500);

                    break;

      case 0xFDB04F: //EQ-CLEAR BUTTON

        delay(200);
        if(menuval == 2){
          first = 0;
          second = 0;
          menustate = 0;
          printing(8, 1,"XX");
          lcd.setCursor(8, 1);
          lcd.blink();
        }
                    break;

      case 0xFD708F: //+VE OR -VE DIRECTION BUTTON

        delay(200);
        if(menuval == 2 && dirs == 1){
          dirs = -1;
          printing(7, 1, "-");
        }
        else if(menuval == 2 && dirs == -1){
          dirs = 1;
          printing(7, 1, " ");
        }

        switch(menustate){
          case 0:
            lcd.setCursor(8,1);
          break;
          case 1:
            lcd.setCursor(9,1);
          break;
        }

                    break;

      case 0xFD30CF: //0

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 0;
              menustate = menustate + 1;
              wrap(menustate, 2, 1);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 0;
              menustate = menustate + 1;
              wrap(menustate, 2, 1);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD08F7: //1

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 1;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 1;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD8877: //2

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 2;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 2;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD48B7: //3

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 3;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 3;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD28D7: //4

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 4;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 4;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFDA857: //5

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 5;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 5;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD6897: //6

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 6;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 6;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD18E7: //7

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 7;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 7;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD9867: //8

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 8;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 8;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
                    break;

      case 0xFD58A7: //9

        if(menuval == 2){
          switch(menustate){
            case 0:
              first = 9;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.setCursor(8,1);
              lcd.print(first);
              lcd.blink();
            break;

            case 1:
              second = 9;
              menustate = menustate + 1;
              wrap(menustate, 1, 0);
              lcd.print(second);
              lcd.setCursor(8,1);
            break;
        }
        }
        delay(500);
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
        printing(0, 0, "    MANUALLY    ");
        printing(0, 1, "  CHOOSE STEPS  ");
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

    case 4:     // Select specific motor to move panel in x or y direction menu

      printing(0, 0, "CHANGE X VAL    ");
      printing(0, 1, "CHANGE Y VAL    ");

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

int printing(int y, int z, String a) {        // function used to streamline printing to LCD
  lcd.setCursor(y, z);
  lcd.print(a);
}

int wrap(int m, int high, int low){
  if(m > high){menustate = low;}
  if(m < low) {menustate = high;}
}
