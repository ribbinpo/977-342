#ifndef Temp_h
#define Temp_h
#include "Arduino.h"
class Temp{
  public:
    Temp();
    float readTemp();
    float readTempSleep(int ms);
    void interval(int p);
  private:
    float temp;
};

#endif
