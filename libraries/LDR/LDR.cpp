#include "LDR.h"

LDR::LDR(BaseDevice* bd, int id, int pin, int sensorDelay){
  
  sensorPin = pin;     
  this->sensorDelay = sensorDelay;
  previousMillis = 0;
  this->bd = bd;
  this->id = id;
}

void LDR::execute(){
  if ((millis() - previousMillis) > sensorDelay) {
    previousMillis = millis();
    int value = analogRead(sensorPin);
    Serial.println(value);
    if (value > 600) {
      Serial.println("Sending value");
      bd->sendValue(toJson(value));
      Serial.println("Updating state to ON");
      //eepromStateWrite('N');
    }
    else if (value < 200 /*&& eepromStateRead() == 'N'*/) {
      Serial.println("Sending value");
      bd->sendValue(toJson(value));
      Serial.println("Updating state to OFF");
      //eepromStateWrite('F');
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
