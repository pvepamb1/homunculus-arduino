#ifndef LDR_H
#define LDR_H
 
#include <Arduino.h>
#include "Sensor.h"
 
class LDR : public Sensor {
private:
        int sensorPin;
        int previousMillis;
        int sensorDelay; //in mills
public:
        LDR(int pin, int Sensordelay);
        void execute();
};
 
#endif
