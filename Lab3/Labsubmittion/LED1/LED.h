#ifndef LED_h
#define LED_h
/***************************************************/
#include "Arduino.h"
class LED{
  public:
    LED(int pin);
    void on();
    void off();
    void toggle(int p);
    void interval(int p);
  private:
    int _pin;
};
/***************************************************/
#endif
