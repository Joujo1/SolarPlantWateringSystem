#pragma once

#include <Arduino.h>

// Everything you may want to tune lives in this file.

struct PlantConfig {
  const char* name;
  uint8_t sensorPin;
  uint8_t pumpPin;        // relay input, LOW = pump on
  int rawInWater;         // sensor reading fully in water (shown as 100 %)
  int rawInAir;           // sensor reading dry, in air (upper limit for airCutoff)
  int dryThreshold;       // readings above this mean the soil needs water
  int airCutoff;          // readings at or above this mean "not in soil", never water (shown as 0 %)
  unsigned long pumpMs;   // how long one watering burst runs
};

namespace config {

// One entry per installed plant. Add a line to add a plant.
// shortcut: dryThreshold and airCutoff are provisional, replace with real dry/moist soil readings
constexpr PlantConfig PLANTS[] = {
  // name       sensor pump  water  air  dry  cutoff  pumpMs
  { "Plant 1",  A0,    2,    205,   500, 440, 470,    1000 },
  { "Plant 2",  A1,    3,    205,   500, 440, 470,    1000 },
  // { "Plant 3",  A2,    4,    205,   500, 440, 470,    1000 },
};
constexpr int PLANT_COUNT = sizeof(PLANTS) / sizeof(PLANTS[0]);

// Watering rules
constexpr unsigned long SOAK_MS = 60000UL;     // wait after watering so water reaches the sensor
constexpr int MAX_CYCLES_PER_HOUR = 10;        // safety cap so a bad reading cannot empty the tank
constexpr unsigned long MIN_PUMP_MS = 100;     // limits for the pump time set from the app
constexpr unsigned long MAX_PUMP_MS = 10000;

// Timing
constexpr unsigned long LOOP_DELAY_MS = 100;
constexpr unsigned long RAW_PRINT_INTERVAL_MS = 1000;
constexpr unsigned long BLYNK_SEND_INTERVAL_MS = 5000;

// Display (ST7789) pins
constexpr uint8_t TFT_CS = 10;
constexpr uint8_t TFT_DC = 8;
constexpr uint8_t TFT_RST = 9;
constexpr uint8_t TFT_MOSI = 11;
constexpr uint8_t TFT_SCLK = 13;

}  // namespace config

// Blynk virtual pins (macros because BLYNK_WRITE needs them at compile time).
// The app controls Plant 1.
#define VPIN_PUMP_BUTTON V0     // switch: run one watering burst
#define VPIN_PUMP2_BUTTON V5    // switch: run one watering burst on Plant 2
#define VPIN_MOISTURE V1        // value display: moisture in %
#define VPIN_DRY_THRESHOLD V2   // slider: dryThreshold
#define VPIN_PUMP_MS V3         // slider: pumpMs
#define VPIN_AIR_CUTOFF V4      // slider: airCutoff
