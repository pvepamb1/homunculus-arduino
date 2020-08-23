#ifndef LDR_H
#define LDR_H
 
#include <Arduino.h>
#include "Sensor.h"
#include "BaseDevice.h"
 
class LDR : public Sensor {
private:
        int sensorPin;
        unsigned long previousMillis;
        unsigned long sensorDelay; //in mills
        bool wasHigh;
        BaseDevice* bd;
        int maxThreshold;
        int minThreshold;
public:
        LDR(BaseDevice* bd, int id, int pin);
        void execute();
        void setConfig(JsonObject& doc);
        String toJson(int value);
        String handleRoot();
};
 
#endif
