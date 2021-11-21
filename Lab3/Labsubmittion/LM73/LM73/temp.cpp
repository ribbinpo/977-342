#include "temp.h"
#include "Arduino.h"
#include <Wire.h>
#define LM73_ADDR 0x4D

Temp::Temp(){
  temp = 0.0;
  Wire1.begin(4, 5);
}

void Temp::interval(int p){ delay(p); }

float Temp::readTemp(){
  Wire1.beginTransmission(LM73_ADDR);
  Wire1.write(0x00); // Temperature Data Register
  Wire1.endTransmission();
  uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
  temp = 0.0;
  if (count == 2) {
    byte buff[2];
    buff[0] = Wire1.read();
    buff[1] = Wire1.read();
    temp += (int)(buff[0]<<1);
    if (buff[1]&0b10000000) temp += 1.0;
    if (buff[1]&0b01000000) temp += 0.5;
    if (buff[1]&0b00100000) temp += 0.25;
    if (buff[0]&0b10000000) temp *= -1.0;
  }
  return temp;
}

float Temp::readTempSleep(int ms){
    temp=readTemp();
    //1000*60*10
    delay(ms);
    return temp;
}
