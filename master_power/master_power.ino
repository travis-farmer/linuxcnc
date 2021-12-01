#include "Nextion.h"
#define R_ONE 10000
#define R_TWO 1000
#define OUTPUT_PIN 24
#define INPUT_A 22
#define INPUT_B 23

NexText t4 = NexText(0, 5, "t4");
NexText t5 = NexText(0, 6, "t5");
NexText t6 = NexText(0, 7, "t6");
NexText t7 = NexText(0, 8, "t7");
NexText t8 = NexText(0, 9, "t8");
NexPage page0    = NexPage(0, 0, "page0");
NexPage page1    = NexPage(1, 0, "page1");
NexPage page2    = NexPage(2, 0, "page2");
NexPage page3    = NexPage(3, 0, "page3");
page1.show();


bool pwrState = false;
bool lastInpState = false;
unsigned long lastDelay = 0UL;
unsigned long lastTest = 0UL;

void tftDim(int bright) {
  bright = constrain(bright, 0, 100);
  String dim = "dim=" + String(bright);
  sendCommand(dim.c_str());

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
  dtostrf(five,6, 3, outStrA);
  dtostrf(twelve,6, 3, outStrB);
  dtostrf(twentyfour,6, 3, outStrC);
  dtostrf(stepper,6, 3, outStrD);
  char outputa[20];
  sprintf(outputa, "%sV", outStrA);
  t4.setText(outputa);
  char outputb[20];
  sprintf(outputb, "%sV", outStrB);
  t5.setText(outputb);
  char outputc[20];
  sprintf(outputc, "%sV", outStrC);
  t6.setText(outputc);;
  char outputd[20];
  sprintf(outputd, "%sV", outStrD);
  t7.setText(outputd);
}

void setup()
{
        // start serial port at 9600 bps:
    Serial.begin(9600);
    nexInit();


        // set the initial field identifiers
    analogReference(DEFAULT);
    pinMode(OUTPUT_PIN,OUTPUT); digitalWrite(OUTPUT_PIN,LOW);
    pinMode(INPUT_A,INPUT_PULLUP);
    pinMode(INPUT_B,INPUT_PULLUP);
    t4.setText("0.00V");
    t5.setText("0.00V");
    t6.setText("0.00V");
    t7.setText("0.00V");
    t8.setText("");
}

void loop()
{
    bool rcvData = false;
    unsigned long curTim = millis();
    bool curPumpA = digitalRead(INPUT_A);
    bool curPumpB = digitalRead(INPUT_B);
    if (curTim - lastDelay >= 500) {
      lastDelay = curTim;
      displayLCD();
    }
    if ((!curPumpA && !curPumpB) || (curPumpA && curPumpB) && curTim - lastTest >= 1500) { // wrong input, starve the fake watchdog
      pwrState = false;
      digitalWrite(OUTPUT_PIN,LOW);
      tftDim(5);
    } else if ((!curPumpA && curPumpB) || (curPumpA && !curPumpB) && curPumpA != lastInpState) { // differential input
      lastInpState = curPumpA; // make sure the pump is pumping
      lastTest = curTim; // feed the fake watchdog
      pwrState = true;
      digitalWrite(OUTPUT_PIN,HIGH);
      tftDim(100);
    }
}
