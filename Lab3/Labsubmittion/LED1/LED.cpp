#include "LED.h"
#include "Arduino.h"

LED::LED(int pin){
  _pin = pin;
  pinMode(pin,OUTPUT);  
}
void LED::on(){ digitalWrite(_pin,LOW); }
void LED::off(){ digitalWrite(_pin,HIGH); }
void LED::interval(int p){ delay(p); }
void LED::toggle(int p){
  on();
  interval(p);
  off();
  interval(p);  
}
