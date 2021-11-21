
#include "LED.h"

  LED LED1(12,16);
  LED LED2(2,14);
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  LED1.toggleSwitch(500);
  LED2.toggleSwitch(500);
  delay(1000);
}
