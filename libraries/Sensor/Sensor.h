#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
public:
        int id = 0;
        Sensor();
        virtual void execute()= 0;
};
 
#endif
