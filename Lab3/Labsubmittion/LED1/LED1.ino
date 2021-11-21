#include "LED.h"

  LED LED1(12);
  LED LED2(2);
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  LED1.toggle(500);
  LED2.toggle(500);
}
