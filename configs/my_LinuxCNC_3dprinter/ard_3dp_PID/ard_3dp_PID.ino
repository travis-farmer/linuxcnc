#include "ArduPID.h"
double inputA;
double outputA;
double inputB;
double outputB;
ArduPID myControllerA;
ArduPID myControllerB;

double p = 10; // 15?
double i = 1; // .3?
double d = 0.5; // 0?

char buffer[128];
int sofar;
double set_a = 0.0;
double set_b = 0.0;
byte pwmC = 0;
byte pwmD = 0;
bool aAtTemp = false;
bool bAtTemp = false;
bool aAtTempOld = false;
bool bAtTempOld = false;

// Analog output pin
#define outputPinA 9
#define outputPinB 7
#define outputPWMc 8
#define outputPWMd 6

// thermistor analog pin
#define THERMISTORPINa A0
#define THERMISTORPINb A1


void processCommand()
{
    double aa=set_a;
    double bb=set_b;
    byte cc=pwmC;
    byte dd=pwmD;
  char *ptr=buffer;
  while(ptr && ptr<buffer+sofar)
  {
    ptr=strchr(ptr,' ')+1;
    switch(*ptr) {
      case 'a': case 'A': aa=atof(ptr+1); break;
      case 'b': case 'B': bb=atof(ptr+1); break;
      case 'c': case 'C': cc=atoi(ptr+1); break;
      case 'd': case 'D': dd=atoi(ptr+1); break;

      default: ptr=0; break;
    }
  }
  set_a = aa;
  set_b = bb;
  pwmC = cc;
  pwmD = dd;
}

void setup() {
  Serial.begin(115200);
  analogReference(EXTERNAL);
  pinMode(outputPinA, OUTPUT);
  pinMode(outputPinB, OUTPUT);
  pinMode(outputPWMc, OUTPUT);
  pinMode(outputPWMd, OUTPUT);

  //initialize PID setpoint *C
  //turn the PID on
  myControllerA.begin(&inputA, &outputA, &set_a, p, i, d);
  myControllerA.setOutputLimits(0, 255);
  myControllerA.setBias(255.0 / 2.0);
  myControllerA.setWindUpLimits(-10, 10); // Groth bounds for the integral term to prevent integral wind-up

  myControllerA.start();

  myControllerB.begin(&inputB, &outputB, &set_b, p, i, d);
  myControllerB.setOutputLimits(0, 255);
  myControllerB.setBias(255.0 / 2.0);
  myControllerB.setWindUpLimits(-10, 10); // Groth bounds for the integral term to prevent integral wind-up

  myControllerB.start();
  // Initialize serial command buffer.
  sofar=0;
}
void loop() {
  inputA=resistanceToC(inputToResistance(analogRead(THERMISTORPINa)));
  Serial.print("aa");Serial.print((int)inputA);
  myControllerA.compute();
  analogWrite(outputPinA, outputA);
  if (inputA >= (set_a-2) || inputA <= (set_a+2)) {
    aAtTemp = true;
  } else {
    aAtTemp = false;
  }

  inputB=resistanceToC(inputToResistance(analogRead(THERMISTORPINb)));
  Serial.print("bb");Serial.print((int)inputB);
  myControllerB.compute();
  analogWrite(outputPinB, outputB);
  if (inputB >= (set_b-2) || inputB <= (set_b+2)) {
    bAtTemp = true;
  } else {
    bAtTemp = false;
  }

  analogWrite(outputPWMc, pwmC);
  analogWrite(outputPWMd, pwmD);

  // listen for serial commands
  while(Serial.available() > 0) {
    buffer[sofar++]=Serial.read();
    if(buffer[sofar-1]==';') break;  // in case there are multiple instructions

  }
  // if we hit a semi-colon, assume end of instruction.
  if(sofar>0 && buffer[sofar-1]==';') {

    buffer[sofar]=0;

    // do something with the command
    processCommand();

    // reset the buffer
    sofar=0;
  }

  if(aAtTemp != aAtTempOld){aAtTempOld=aAtTemp;Serial.print("a");Serial.print(aAtTemp);}
  if(bAtTemp != bAtTempOld){bAtTempOld=bAtTemp;Serial.print("b");Serial.print(bAtTemp);}
}
double inputToResistance(double input) {
  // funtion to convert the input value to resistance
  // the value of the 'other' resistor
  double SERIESRESISTOR = 10000;
  return SERIESRESISTOR / (1024 / input - 1);
}
double resistanceToC(double resistance) {
  // funtion to convert resistance to c
  // temp/resistance for nominal
  double THERMISTORNOMINAL = 118000;
  double TEMPERATURENOMINAL = 25;
  // beta coefficent
  double BCOEFFICIENT = 3950;
  double steinhart;
  steinhart = resistance / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;   // convert to C
  return steinhart;
}
