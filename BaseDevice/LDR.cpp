#include "LDR.h"
#include "BaseDevice.h"

LDR::LDR(int pin, int sensorDelay){
  
  sensorPin = pin;     
  this->sensorDelay = sensorDelay;
  previousMillis = 0;
}

void LDR::execute(){
  if ((millis() - previousMillis) > sensorDelay) {
    previousMillis = millis();
    int value = analogRead(sensorPin);
    Serial.println(value);
    if (value > 600) {
      Serial.println("Sending value");
      sendValue(value);
      Serial.println("Updating state to ON");
      eepromStateWrite('N');
    }
    else if (value < 200 && eepromStateRead() == 'N') {
      Serial.println("Sending value");
      sendValue(value);
      sendValue(value);
      Serial.println("Updating state to OFF");
      eepromStateWrite('F');
    }
  }
}
