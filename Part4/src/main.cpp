#define BLYNK_TEMPLATE_ID "TMPL4Lpyd9lHe"
#define BLYNK_TEMPLATE_NAME "WaterPlant"
#define BLYNK_PRINT Serial

#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
#include "wifi_secrets.h"

#include "config.h"
#include "display.h"
#include "plant.h"

static_assert(config::PLANT_COUNT >= 1, "the app controls Plant 1, config::PLANTS needs an entry");
static_assert(config::PLANT_COUNT <= Display::MAX_PLANTS, "the display has no row for more plants");

Plant plants[config::PLANT_COUNT];
Display display;

// Returns true once every intervalMs; lastMs keeps the time of the last hit.
bool intervalElapsed(unsigned long& lastMs, unsigned long intervalMs) {
  unsigned long now = millis();
  if (now - lastMs < intervalMs) return false;
  lastMs = now;
  return true;
}

// Runs one watering burst and shows it on the display.
void waterPlant(int index) {
  Plant& plant = plants[index];
  Serial.print(plant.name());
  Serial.print(": watering for ");
  Serial.print(plant.pumpMs());
  Serial.println(" ms");

  display.showPlant(index, true);
  plant.pumpOn();
  display.rain(plant.pumpMs());
  plant.pumpOff();
  display.showPlant(index, false);
}

// --- Blynk app (controls Plant 1) ---

// Show the active values on the sliders. App changes last until the next reboot,
// then config.h applies again.
BLYNK_CONNECTED() {
  Blynk.virtualWrite(VPIN_DRY_THRESHOLD, plants[0].dryThreshold());
  Blynk.virtualWrite(VPIN_PUMP_MS, plants[0].pumpMs());
  Blynk.virtualWrite(VPIN_AIR_CUTOFF, plants[0].airCutoff());
}

BLYNK_WRITE(VPIN_PUMP_BUTTON) {
  if (param.asInt()) waterPlant(0);
}

BLYNK_WRITE(VPIN_PUMP2_BUTTON) {
  if (!param.asInt()) return;
  if (config::PLANT_COUNT < 2) {
    Serial.println("App: Plant 2 is not configured in config.h, ignoring button");
    return;
  }
  waterPlant(1);
}

BLYNK_WRITE(VPIN_DRY_THRESHOLD) {
  int applied = plants[0].setDryThreshold(param.asInt());
  Blynk.virtualWrite(VPIN_DRY_THRESHOLD, applied);  // show the clamped value in the app
  Serial.print("App: dry threshold set to ");
  Serial.println(applied);
}

BLYNK_WRITE(VPIN_AIR_CUTOFF) {
  int applied = plants[0].setAirCutoff(param.asInt());
  Blynk.virtualWrite(VPIN_AIR_CUTOFF, applied);  // show the clamped value in the app
  Serial.print("App: air cutoff set to ");
  Serial.println(applied);
}

BLYNK_WRITE(VPIN_PUMP_MS) {
  unsigned long applied = plants[0].setPumpMs(param.asLong());
  Blynk.virtualWrite(VPIN_PUMP_MS, applied);  // show the clamped value in the app
  Serial.print("App: pump time set to ");
  Serial.print(applied);
  Serial.println(" ms");
}

// --- Loop steps ---

void updateWifiIcon() {
  static bool shownConnected = false;
  bool connected = (WiFi.status() == WL_CONNECTED);
  if (connected == shownConnected) return;
  shownConnected = connected;
  display.drawWifi(connected);
}

void waterPlantsThatNeedIt() {
  for (int i = 0; i < config::PLANT_COUNT; i++) {
    if (!plants[i].shouldWaterNow(millis())) continue;
    waterPlant(i);
    if (plants[i].hourlyCapReached()) {
      Serial.print(plants[i].name());
      Serial.println(": hourly watering cap reached, pausing");
    }
  }
}

void printRawReadings() {
  static unsigned long lastMs = 0;
  if (!intervalElapsed(lastMs, config::RAW_PRINT_INTERVAL_MS)) return;
  for (int i = 0; i < config::PLANT_COUNT; i++) {
    Serial.print(plants[i].name());
    Serial.print(" raw: ");
    Serial.println(plants[i].readRaw());
  }
}

void sendMoistureToApp() {
  static unsigned long lastMs = 0;
  if (!intervalElapsed(lastMs, config::BLYNK_SEND_INTERVAL_MS)) return;
  int percent = plants[0].moisturePercent();
  Blynk.virtualWrite(VPIN_MOISTURE, percent);
  Serial.print(plants[0].name());
  Serial.print(" moisture: ");
  Serial.print(percent);
  Serial.println(" %");
}

void setup() {
  Serial.begin(9600);

  display.begin();
  for (int i = 0; i < config::PLANT_COUNT; i++) {
    plants[i].begin(config::PLANTS[i]);
    display.showPlant(i, false);
  }

  WiFi.begin(SSID, PASSWORD);
  Blynk.config(BLYNK_AUTH_TOKEN);
  Serial.println("Watering system started, connecting to Blynk in the background");
}

void loop() {
  Blynk.run();
  updateWifiIcon();
  waterPlantsThatNeedIt();
  printRawReadings();
  sendMoistureToApp();
  delay(config::LOOP_DELAY_MS);
}
