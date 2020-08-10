#include "BaseDevice.h"
#include "LDR.h"

BaseDevice bd("Virus_stay_away", "Umich@2018", "http://10.0.0.208:8080");
Sensor* l1 = new LDR(&bd, 1, A0);

void setup() {
  bd.connectToWiFi();
  bd.addSensor(l1);
  bd.getConfig();
}

void loop() {
  bd.heartbeat();
  bd.server->handleClient();
  bd.handleSensors();
}
