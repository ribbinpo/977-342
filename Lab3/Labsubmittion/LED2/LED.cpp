#include "LED.h"
#include "Arduino.h"

LED::LED(int pin){
  _pin = pin;
  pinMode(pin,OUTPUT);  
}
LED::LED(int pin,int bin){
  _pin = pin;
  _bin = bin;
  pinMode(_bin,INPUT_PULLUP);
  pinMode(_pin,OUTPUT);  
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

int LED::button(){
  int state = digitalRead(_bin);
  return state;
}
void LED::toggleSwitch(int p){
    if(button()==0){
      toggle(p);
    }else{
      off();  
    }
}
