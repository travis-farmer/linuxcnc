#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

void SendData(char addr, float data) {
  Serial.write(addr);
  char outStr[15];
  dtostrf(data,7, 3, outStr);
  Serial.println(outStr);
}

char XReading[9];  // 0000.000? pos
char YReading[9];
char ZReading[9];

bool bEstop = true;
bool bMist = false;
bool bJog = false;
bool bMachine = false;

void zeroX() { for(int x = 0; x < 9; x++) XReading[x] = 0;}
void zeroY() { for(int x = 0; x < 9; x++) YReading[x] = 0;}
void zeroZ() { for(int x = 0; x < 9; x++) ZReading[x] = 0;}

void setup()
{
        // start serial port at 9600 bps:
    Serial.begin(9600);
        // initialize the digital pin as an output.
        // Pin 13 has an LED connected on most Arduino boards:
//    pinMode(13, OUTPUT);
        // set up the LCD's number of rows and columns:
    lcd.begin(20, 4);
        // Print a message to the LCD.bool bMachine = false;
    lcd.setBacklight(HIGH);
    lcd.noAutoscroll();
        // set the initial field identifiers
    lcd.setCursor(0, 0);
    lcd.print("X");
    lcd.setCursor(0, 1);
    lcd.print("Y");
    lcd.setCursor(0, 2);
    lcd.print("Z");

    zeroX();
    zeroY();
    zeroZ();
}

void loop()
{
int x;
char ch, ky;

    if(Serial.available() >= 9)
        {
        ch = Serial.read();
        switch(ch)
            {
            case 'X':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        XReading[x] = Serial.read();
                    lcd.setCursor(0, 0);
                    lcd.print("X");
                    lcd.setCursor(1, 0);
                    lcd.print(XReading);
                    zeroX();
                    break;

            case 'Y':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        YReading[x] = Serial.read();
                    lcd.setCursor(0, 1);
                    lcd.print("Y");
                    lcd.setCursor(1, 1);
                    lcd.print(YReading);
                    zeroY();
                    break;

            case 'Z':
                    while(Serial.available() < 8)
                        delay(10);
                    for(x = 0; x < 8; x++)
                        ZReading[x] = Serial.read();
                    lcd.setCursor(0, 2);
                    lcd.print("Z");
                    lcd.setCursor(1, 2);
                    lcd.print(ZReading);
                    zeroZ();
                    break;
            default:
                    break;

            }
        }

    if((ky = ourKeypad.getKey()) != NO_KEY )
        sendCommand(ky);
}

//#############################################################################################################


void sendCommand(char keyPress)
{
        switch(keyPress)
            {
            case 'D': // ESTOP
                    if(bEstop)
                        {
                        // send estop reset
                        Serial.write('e');
                        lcd.setCursor(15, 0);
                        lcd.print("     ");
                        bMachine = false;
                        bEstop = false;
                        }
                    else
                        {
                        // send estop activate & switch machine off too
                        Serial.write('E');
                        lcd.setCursor(15, 0);
                        lcd.print("ESTOP");
                        bEstop = true;
                        sendCommand('P');  // M OFF
                        sendCommand('Q');  // blank MDI display
                        sendCommand('R');  // JOG OFF
                        sendCommand('S');  // MIST OFF
                        }
                    break;

            case '9': // MACHINE ON / OFF
                   if(bMachine)
                        {
                        // send machine off
                        Serial.write('m');
                        sendCommand('P');  // M OFF
                        sendCommand('Q');  // blank MDI display
                        sendCommand('R');  // JOG OFF
                        sendCommand('S');  // MIST OFF
                        }
                    else
                        {
                        // send machine on unless in estop
                        if(!bEstop)
                            {
                            Serial.write('M');
                            lcd.setCursor(15, 1);
                            lcd.print("M ON ");
                            bMachine = true;
                            }
                        }
                    break;

            case '7':   // HOME ALL
                    if(!bEstop)
                        {
                        lcd.setCursor(16, 3);
                        lcd.print("    ");
                        bMist = false;
                        Serial.write('H');
                        }
                    break;

            case '*':   // MDI 1
                    if(!bEstop && bMachine)
                        {
                        Serial.write('1');
                        lcd.setCursor(15, 2);
                        lcd.print("MDI 1");
                        }
                    break;

            case '0':   // MDI 2
                    if(!bEstop && bMachine)
                        {
                        Serial.write('2');
                        lcd.setCursor(15, 2);
                        lcd.print("MDI 2");
                        }
                    break;

            case '#':   // MDI 3
                    if(!bEstop && bMachine)
                        {
                        Serial.write('3');
                        lcd.setCursor(15, 2);
                        lcd.print("MDI 3");
                        }
                    break;

//###########################################
// re-entrant cases to carry out blanking etc
// to save duplicating code
//###########################################

            case 'P':    // machine off
                        lcd.setCursor(15, 1);
                        lcd.print("M OFF");
                        bMachine = false;
                        break;

            case 'Q':  // MDI off for use by estop etc
                        lcd.setCursor(15, 2);  // blank MDI display
                        lcd.print("     ");
                        break;

            case 'R':   // JOG OFF
                        lcd.setCursor(0, 3);
                        lcd.print("JOG OFF");
                        bJog = false;
                        break;

            case 'S':   // MIST OFF
                        lcd.setCursor(16, 3);
                        lcd.print("    ");
                        bMist = false;
                        break;

//###########################################

            case '8':   // Mist On / OFF
                    if(bMist)
                        {
                        // send mist off
                        Serial.write('c');
                        sendCommand('S');
                        }
                    else
                        {
                        // send mist activate
                        if(!bEstop && bMachine)
                            {
                            Serial.write('C');
                            lcd.setCursor(16, 3);
                            lcd.print("MIST");
                            bMist = true;
                            }
                        }
                    break;

            case 'A':   // X
                    Serial.write('X');
                    lcd.setCursor(1, 0);
                    lcd.print("*");
                    lcd.setCursor(1, 1);
                    lcd.print(" ");
                    lcd.setCursor(1, 2);
                    lcd.print(" ");
                    break;

            case 'B':   // Y
                    Serial.write('Y');
                    lcd.setCursor(1, 0);
                    lcd.print(" ");
                    lcd.setCursor(1, 1);
                    lcd.print("*");
                    lcd.setCursor(1, 2);
                    lcd.print(" ");
                    break;

            case 'C':   // Z
                    Serial.write('Z');
                    lcd.setCursor(1, 0);
                    lcd.print(" ");
                    lcd.setCursor(1, 1);
                    lcd.print(" ");
                    lcd.setCursor(1, 2);
                    lcd.print("*");
                    break;

            case '2': // JOG ON /OFF
                    if(bJog)
                        {
                        // send jog off
                        Serial.write('j');
                        sendCommand('R');  // JOG OFF
                        }
                    else
                        {
                        // send jog activate
                        if(!bEstop && bMachine)
                            {
                            Serial.write('J');
                            lcd.setCursor(0, 3);
                            lcd.print("JOG ON ");
                            bJog = true;
                            }
                        }
                    break;

            case '4': // 100mm/min
                    Serial.write('4');
                    lcd.setCursor(8, 3);
                    lcd.print("VEL 100");
                    break;

            case '5': // 10mm/min
                    Serial.write('5');
                    lcd.setCursor(8, 3);
                    lcd.print("VEL 010");

                    break;
            case '6': // 1mm/min
                    Serial.write('6');
                    lcd.setCursor(8, 3);
                    lcd.print("VEL 001");
                    break;

            case '1': // Jog minus <
                    if(bJog)
                        Serial.write('<');
                    break;

            case '3': // Jog plus >
                    if(bJog)
                        Serial.write('>');
                    break;

            default:
                    break;
            }

}
