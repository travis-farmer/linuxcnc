#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define REFERENCE_PIN 15
#define R_ONE 10000
#define R_TWO 1000
#define FIVE_PIN 0
#define TWELVE_PIN 1
#define TWENTYFOUR_PIN 2
#define STEPPER_PIN 3

LiquidCrystal_I2C lcd(0x27,20,4);

void SendData(char addr, float data) {
  Serial.write(addr);
  char outStr[15];
  dtostrf(data,5, 3, outStr);
  Serial.println(outStr);
}

char XReading[9];  // 0000.000? pos
char YReading[9];
char ZReading[9];

bool pwrState = false;

unsigned long lastDelay = 0UL;
unsigned long lastTest = 0UL;


void zeroX() { for(int x = 0; x < 9; x++) XReading[x] = 0;}
void zeroY() { for(int x = 0; x < 9; x++) YReading[x] = 0;}
void zeroZ() { for(int x = 0; x < 9; x++) ZReading[x] = 0;}

float readVolts(int pin) {
float operatingVoltage = analogRead(REFERENCE_PIN);

  float rawVoltage = analogRead(pin);

  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  return((rawVoltage * R_TWO) / (R_ONE + R_TWO));
}


void displayLCD() {
  lcd.setCursor(0, 0);
  lcd.print("X");
  lcd.setCursor(1, 0);
  lcd.print(XReading);
  zeroX();
  lcd.setCursor(0, 1);
  lcd.print("Y");
  lcd.setCursor(1, 1);
  lcd.print(YReading);
  zeroY();
  lcd.setCursor(0, 2);
  lcd.print("Z");
  lcd.setCursor(1, 2);
  lcd.print(ZReading);
  zeroZ();
  lcd.setCursor(0,3);
  char outStrA[15];
  char outStrB[15];
  char outStrC[15];
  char outStrD[15];
  float five = readVolts(FIVE_PIN);
  float twelve = readVolts(TWELVE_PIN);
  float twentyfour = readVolts(TWENTYFOUR_PIN);
  float stepper = readVolts(STEPPER_PIN);
  dtostrf(five,4, 1, outStrA);
  dtostrf(twelve,4, 1, outStrB);
  dtostrf(twentyfour,4, 1, outStrC);
  dtostrf(stepper,4, 1, outStrD);
  char output[20];
  sprintf(output, "%s %s %s %s", outStrA, outStrB, outStrC, outStrD);
  lcd.print(output);
  SendData(0x02,five);
  SendData(0x03,twelve);
  SendData(0x04,twentyfour);
  SendData(0x05,stepper);
  if (pwrState) {
    SendData(0x01,1.00);
    lcd.setCursor(19,0);
    lcd.print("*");
  } else {
    SendData(0x01,0.00);
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

        // set the initial field identifiers
    lcd.setCursor(0, 0);
    lcd.print("X");
    lcd.setCursor(0, 1);
    lcd.print("Y");
    lcd.setCursor(0, 2);
    lcd.print("Z");
    pinMode(13,OUTPUT); digitalWrite(13,LOW);
    zeroX();
    zeroY();
    zeroZ();
}

void loop()
{
int x;
char ch, ky;
bool rcvData = false;
    unsigned long curTim = millis();
    if(Serial.available() >= 9)
        {
            rcvData = true;
        ch = Serial.read();
        switch(ch)
            {
            case 'X':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        XReading[x] = Serial.read();

                    break;

            case 'Y':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        YReading[x] = Serial.read();

                    break;

            case 'Z':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        ZReading[x] = Serial.read();

                    break;
            default:
                    break;

            }
        }

    if (curTim - lastDelay >= 500) {
      lastDelay = curTim;
      displayLCD();
    }
    if (rcvData == false && curTim - lastTest >= 5000) {
      pwrState = false;
      digitalWrite(13,LOW);
    } else if (rcvData) {
      lastTest = curTim;
      pwrState = true;
      digitalWrite(13,HIGH);
    }
}
