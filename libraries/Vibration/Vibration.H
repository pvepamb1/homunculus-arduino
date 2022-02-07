#ifndef VIBRATION_H
#define VIBRATION_H

#include <Arduino.h>
#include "Sensor.h"
#include "BaseDevice.h"

class Vibration : public Sensor{
private:
        int sensorPin;
        unsigned long previousMillis;
        unsigned long sensorDelay; //in mills
        bool wasHigh;
        BaseDevice* bd;
        int maxThreshold;
        int minThreshold;
public:
        Vibration(BaseDevice* bd, int id, int pin);
        void execute();
        void setConfig(JsonObject& doc);
        String toJson(int value);
        String handleRoot();
};

#endif
