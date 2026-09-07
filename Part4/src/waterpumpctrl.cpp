#include "waterpumpctrl.h"

using namespace WaterPumpCtrl;

#define MAXDRY 380 //Experimental derived value of dry soil
#define AIR 450

bool waterPumpCtrl::needsWater(int soilPin) {
  int moistureValue = analogRead(soilPin);
  if (moistureValue > MAXDRY && moistureValue < AIR) {
    return true;
  }
  return false;
}

//   LOW  = pump ON
//   HIGH = pump OFF
void waterPumpCtrl::pumpOn(int pumpPin) { digitalWrite(pumpPin, LOW); }
void waterPumpCtrl::pumpOff(int pumpPin) { digitalWrite(pumpPin, HIGH); }

void waterPumpCtrl::runPump(int pumpPin, int durationMs) {
  pumpOn(pumpPin);
  delay(durationMs);
  pumpOff(pumpPin);
}
