#include "ArduPID.h"
ArduPID myControllerA;
ArduPID myControllerB;
double inputA;
double outputA;
double inputB;
double outputB;
double p = 15;
double i = .3;
double d = 0;
double set_a = 0.0;
double set_b = 0.0;

void setup() {
    Serial.begin(9600);
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
}   

uint8_t adc=0;
uint8_t firstinByte=0;
uint8_t pinmap[6] = {2,4,8,9,12,13};
uint8_t dacpinmap[6] = {3,5,6,7,10,11};
void loop() {
  while(Serial.available()) {
        uint8_t inByte = Serial.read();
        if(((firstinByte & 0x80) == 0x80) && ((inByte & 0x80) == 0)) {
            // got a packet
            uint16_t payload = (firstinByte << 7) | inByte;
            uint8_t address = (firstinByte >> 4) & 7;
            uint8_t dac = payload & 0xff;
            uint8_t dir = (payload & 0x100) == 0x100;
            uint8_t out = (payload & 0x200) == 0x200;
            if(address < 6) {
                if (dacpinmap[address] == 7) {
                  set_a = dac;
                } else if (dacpinmap[address] == 6) {
                  set_b = dac;
                } else {
                  analogWrite(dacpinmap[address], dac);
                }
                digitalWrite(pinmap[address], out);
                pinMode(pinmap[address], dir);
            }
        }
        firstinByte = inByte;
    }
    int tempC = (double)resistanceToC(inputToResistance(analogRead(adc)));
    if (adc == 0) {
      inputA = tempC;
       myControllerA.compute();
      analogWrite(7,outputA);
    } else if (adc == 1) {
      inputB = tempC;
       myControllerB.compute();
      analogWrite(6,outputB);
    }
    
    uint16_t v = tempC | (adc << 11);
    //if(digitalRead(pinmap[adc])) v |= (1<<10);
    Serial.write((v >> 7) | 0x80);
    Serial.write(v & 0x7f);
    adc = (adc + 1) % 6;
}
float inputToResistance(float input) {
  // funtion to convert the input value to resistance
  // the value of the 'other' resistor
  float SERIESRESISTOR = 10000;
  return SERIESRESISTOR / (1024 / input - 1);
}
int resistanceToC(float resistance) {
  // funtion to convert resistance to c
  // temp/resistance for nominal
  float THERMISTORNOMINAL = 118000;
  float TEMPERATURENOMINAL = 25;
  // beta coefficent
  float BCOEFFICIENT = 3950;
  float steinhart;
  steinhart = resistance / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;   // convert to C
  return (int)steinhart;
}
