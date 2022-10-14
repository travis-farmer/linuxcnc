#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define R_ONE 10000
#define R_TWO 1000


LiquidCrystal_I2C lcd(0x27,20,4);

char inChar[24]; // incoming char buffer
int inCharCntr = 0;

void inProc(char *inBuffer[24]) {
  // blah

}

void displayLCD() {
  char outStrA[15];
  char outStrB[15];
  char outStrC[15];
  char outStrD[15];
  float five = (((analogRead(0)*(5.00/1024.00))/R_TWO)*(R_ONE+R_TWO));
  delay(10);
  float twelve = (((analogRead(1)*(5.00/1024.00))/R_TWO)*(R_ONE+R_TWO));
  delay(10);
  float twentyfour = (((analogRead(2)*(5.00/1024.00))/R_TWO)*(R_ONE+R_TWO));
  delay(10);
  float stepper = (((analogRead(3)*(5.00/1024.00))/R_TWO)*(R_ONE+R_TWO));
  delay(10);
  dtostrf(five,7, 3, outStrA);
  dtostrf(twelve,7, 3, outStrB);
  dtostrf(twentyfour,7, 3, outStrC);
  dtostrf(stepper,7, 3, outStrD);
  char outputa[20];
  sprintf(outputa, "5v Bus: %s", outStrA);
  lcd.setCursor(0,0);
  lcd.print(outputa);
  char outputb[20];
  sprintf(outputb, "12v Bus: %s", outStrB);
  lcd.setCursor(0,1);
  lcd.print(outputb);
  char outputc[20];
  sprintf(outputc, "24v Bus: %s", outStrC);
  lcd.setCursor(0,2);
  lcd.print(outputc);
  char outputd[20];
  sprintf(outputd, "Stepper Bus: %s", outStrD);
  lcd.setCursor(0,3);
  lcd.print(outputd);

  if (pwrState) {
    lcd.setCursor(19,0);
    lcd.print("*");
  } else {
    lcd.setCursor(19,0);
    lcd.print(" ");
  }
}

void setup()
{
        // start serial port at 9600 bps:
    Serial.begin(9600);
        // initialize the digital pin as an output.
        // Pin 13 has an LED connected on most Arduino boards:
//    pinMode(13, OUTPUT);
        // set up the LCD's number of rows and columns:
    lcd.init();                      // initialize the lcd
    lcd.backlight();


}

void loop()
{
int x;
char ch, ky;
bool rxDataFlag = false;
    if(Serial.available() > 0)
    {
        rcvData = true;
        ch = Serial.read();
    }
    if (ch == '(') {
        rxDataFlag = true;
        inCharCntr = 0;
    } else if (ch == ')') {
        rxDataFlag = false;
        inChar[inCharCntr] = 0x00;
        inProc(*inchar);
        inCharCntr = 0;
    } else {
        if (inCharCntr <= 20) inchar[inCharCntr] = ch;
        inCharCntr++;
    }
}
