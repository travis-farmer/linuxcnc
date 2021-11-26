void SendPacket(byte Addr, int Data) {
  Serial.write((Addr & 0xf0) >> 4);
  Serial.write(0x80 | (Addr & 0x0f));
  Serial.write(0x40 | (Data & 0xf000) >> 12);
  Serial.write(0xc0 | (Data & 0xf00) >> 8);
  Serial.write(0x20 | (Data & 0xf0) >> 4);
  Serial.write(0xa0 | (Date & 0xf));
}

void setup() {
    Serial.begin(9600);
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

    uint16_t v = analogRead(adc) | (adc << 11);
    //if(digitalRead(pinmap[adc])) v |= (1<<10);
    Serial.write((v >> 7) | 0x80);
    Serial.write(v & 0x7f);
    adc = (adc + 1) % 6;
}

