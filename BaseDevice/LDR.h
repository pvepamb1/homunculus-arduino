#ifndef LDR_H
#define LDR_H
 
#include <Arduino.h>
 
class LDR {
private:
        int sensorPin;
        int previousMillis;
        int sensorDelay; //in mills
public:
        LDR(int pin, int Sensordelay);
};
 
#endif
