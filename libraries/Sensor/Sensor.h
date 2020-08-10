#ifndef SENSOR_H
#define SENSOR_H

#include<ArduinoJson.h>

class Sensor {
public:
        int id = 0;
        Sensor();
        virtual void execute() = 0;
        virtual void setConfig(JsonObject& doc) = 0;
        virtual String handleRoot() = 0;
};
 
#endif
