

int printing(int y, int z, String a) {        // function used to streamline printing to LCD
  lcd.setCursor(y, z);
  lcd.print(a);
}

int wrap(int m, int high, int low){
  if(m > high){menustate = low;}
  if(m < low) {menustate = high;}
  // menustate = (menustate+1)%3 - 1
}

void toggleBacklight(){
  
      delay(200);
        if (power == 1) {
          lcd.noBacklight();
          power = 0;}
        else {
          lcd.backlight();
          power = 1;}
}

void returnButton(){
  
        lcd.clear();
        printing(0, 0, "   LOADING...   ");

        if(menuval == 3){menuval = 1; menustate = 1;}
        if(menuval == 4){menuval = 1; menustate = -1;}
        delay(500);
}

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

byte percentage[8] = {
  B00000,
  B00001,
  B10010,
  B00100,
  B01001,
  B10000,
  B00000,
  B00000
};
