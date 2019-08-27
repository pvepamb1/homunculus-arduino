#include "LDR.h"

LDR::LDR(int pin, int sensorDelay){
  
  sensorPin = pin;     
  this->sensorDelay = sensorDelay;
  previousMillis = 0;
}
