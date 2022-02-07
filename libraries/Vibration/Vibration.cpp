#include "Vibration.H"

Vibration::Vibration(BaseDevice* bd, int id, int pin){
  sensorPin = pin;     
  previousMillis = 0;
  this->bd = bd;
  this->id = id;
}

void Vibration::execute(){
  if ((millis() - previousMillis) > sensorDelay) {
    Serial.println("\n----------BEGIN SENSOR----------\n");
    previousMillis = millis();
    int value = analogRead(sensorPin);
    Serial.printf("Sensor with ID: %d has value: %d\n", id, value);

    if (value > maxThreshold && !wasHigh) {
      Serial.println("\nUpdating state to ON");
      wasHigh = true;
      Serial.println("Sending value");
      bd->send(1, toJson(value));
    }

    else if (value < minThreshold && wasHigh) {
      Serial.println("\nUpdating state to OFF");
      wasHigh = false;
      Serial.println("Sending value");
      bd->send(1, toJson(value));
    }

    Serial.println("\n----------END SENSOR----------");
  }
}

void Vibration::setConfig(JsonObject& doc){
  this->minThreshold = doc["minThreshold"];
  this->maxThreshold = doc["maxThreshold"];
  this->wasHigh = doc["wasHigh"];
  this->sensorDelay = doc["sensorDelay"];
}

String Vibration::toJson(int value){
   const int capacity = JSON_OBJECT_SIZE(3) + 100;
   StaticJsonDocument<capacity> doc;
   doc["mac"]=WiFi.macAddress();
   doc["id"]=id;
   doc["value"] = value;
   char output[128];
   serializeJson(doc, output);
   return String(output);
}

String LDR::handleRoot(){
  return String(analogRead(sensorPin));
}