#include "temp.h"
#include <Wire.h>

float tem=0;

Temp temp1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  tem = temp1.readTempSleep(3000);
  Serial.println(tem);
}
