#include "LDR.h"

LDR::LDR(BaseDevice* bd, int id, int pin){
  
  sensorPin = pin;     
  this->sensorDelay = 10000;
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

void LDR::setConfig(JsonObject& doc){

  this->minThreshold = doc["minThreshold"]; // 200
  this->maxThreshold = doc["maxThreshold"]; // 600
  this->currentState = doc["currentState"]; // "N"
  this->sensorDelay = doc["sensorDelay"]; // 10000

  Serial.println(minThreshold);
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

String LDR::handleRoot(){
  return "yes";
}
