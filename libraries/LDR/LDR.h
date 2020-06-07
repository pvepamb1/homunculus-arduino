#ifndef LDR_H
#define LDR_H
 
#include <Arduino.h>
#include "Sensor.h"
#include "BaseDevice.h"
 
class LDR : public Sensor {
private:
        int sensorPin;
        int previousMillis;
        int sensorDelay; //in mills
        char currentState;
        BaseDevice* bd;
        int maxThreshold;
        int minThreshold;
public:
        LDR(BaseDevice* bd, int id, int pin, int sensorDelay);
        void execute();
        char* toJson(int value);
};
 
#endif
