#include "LDR.h"

LDR::LDR(BaseDevice* bd, int id, int pin, int sensorDelay){
  
  sensorPin = pin;     
  this->sensorDelay = sensorDelay;
  previousMillis = 0;
  this->bd = bd;
  this->id = id;
  this->currentState = 'N';
  this->maxThreshold = 600;
  this->minThreshold = 200;
}

void LDR::execute(){
  if ((millis() - previousMillis) > sensorDelay) {
    previousMillis = millis();
    int value = analogRead(sensorPin);
    Serial.println(value);
    if (value > 600) {
      Serial.println("Updating state to ON");
      currentState = 'Y';
      Serial.println("Sending value");
      bd->sendValue(toJson(value));
    }
    else if (value < 200 && currentState == 'Y') {
      Serial.println("Updating state to OFF");
      currentState = 'N';
      Serial.println("Sending value");
      bd->sendValue(toJson(value));
    }
  }
}

char* LDR::toJson(int value){
   const int capacity = JSON_OBJECT_SIZE(3);
   StaticJsonDocument<capacity> doc;
   doc["mac"]=WiFi.macAddress();
   doc["id"]="1";
   doc["value"] = value;
   char output[128];
   serializeJson(doc, output);
   return output;
}
