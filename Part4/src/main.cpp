#define BLYNK_TEMPLATE_ID "TMPL4Lpyd9lHe"
#define BLYNK_TEMPLATE_NAME "WaterPlant"
#define BLYNK_PRINT Serial

#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
#include "wifi_secrets.h"

#include <Arduino.h>
#include "waterpumpctrl.h"
#include "display.h"

const int NUM_CHANNELS = 3;

int soilPins[NUM_CHANNELS] = {A0, A1, A2};

int pumpPins[NUM_CHANNELS] = {2, 3, 4};



const int PUMP_ON_MS = 1000;

// Remembers whether each plant was watering last loop, so we only redraw the
// display (and log) when something actually changes.
bool wateringNow[NUM_CHANNELS] = {false, false, false};

int lastSend = 0;

WaterPumpCtrl::waterPumpCtrl waterPumpCtrl;
DisplayCtrl::tftDisplay display;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    pinMode(soilPins[i], INPUT_PULLUP);
    pinMode(pumpPins[i], OUTPUT);
    digitalWrite(pumpPins[i], HIGH);
  }

  display.begin();
  display.showChannel(0, false);

  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASSWORD);
  Serial.println("Blynk verbunden.");

  Serial.println("Watering system ready (button test mode).");
}

// Runs one watering cycle for a channel. For Plant 1 (channel 0) it also
// mirrors the WATER/idle state on the display, so the Blynk button and the
// physical button both update the screen the same way.
void waterCycle(int channel){
    if (channel == 0) {
      display.showChannel(0, true);        // show "WATER" while it runs
      waterPumpCtrl.pumpOn(pumpPins[0]);
      display.rain(PUMP_ON_MS);            // ~1s of rain while the pump is on
      waterPumpCtrl.pumpOff(pumpPins[0]);
      display.showChannel(0, false);       // back to "idle"
    }else {
      waterPumpCtrl.runPump(pumpPins[channel], PUMP_ON_MS);
  }
}

BLYNK_WRITE(V0) {
  int on = param.asInt();
  if (on) waterCycle(0);
  else    waterPumpCtrl.pumpOff(pumpPins[0]);
  Serial.println(on ? "Blynk: Pumpe AN" : "Blynk: Pumpe AUS");
}


void loop() {
  Blynk.run();

  // Show/refresh the wifi symbol when the connection state changes.
  static bool wifiShown = false;
  bool wifiOk = (WiFi.status() == WL_CONNECTED);
  if (wifiOk != wifiShown) {
    wifiShown = wifiOk;
    display.drawWifi(wifiOk);
  }

  for (int i = 0; i < NUM_CHANNELS; i++) {
    bool needsWater = waterPumpCtrl.needsWater(soilPins[i]);

    if (needsWater != wateringNow[i]) {
      wateringNow[i] = needsWater;
      // (the display for Plant 1 is now handled inside waterCycle)
      Serial.print("Plant ");
      Serial.print(i + 1);
      Serial.println(needsWater ? ": needs water -> pump ON" : ": idle");
    }


    // While this plant needs water, run its pump.
    if (needsWater) waterCycle(i);
    //Air = 480
    //Trocken = 380
    //Genug = 240
    //Wasser = 160
  delay(1000);
  };

  if (millis() - lastSend > 5000) {
    lastSend = millis();
    int reading = analogRead(A1);
    int percent = constrain(map(reading, 480, 160, 0, 100), 0, 100);
    Blynk.virtualWrite(V1, percent);
    Serial.print("Feuchte %: "); Serial.println(percent);
  }
}