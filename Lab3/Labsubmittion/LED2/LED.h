#ifndef LED_h
#define LED_h
/***************************************************/
#include "Arduino.h"
class LED{
  public:
    LED(int pin);
    LED(int pin,int bin);
    void on();
    void off();
    void toggle(int p);
    void toggleSwitch(int p);
    void interval(int p);
    int button();
  private:
    int _pin;
    int _bin;
};
/***************************************************/
#endif
