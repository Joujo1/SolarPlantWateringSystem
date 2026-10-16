#pragma once

#include "config.h"
#include "watergate.h"

// One plant: its moisture sensor, its pump and the rules for watering it.
class Plant {
public:
  // Sets up the pins and switches the pump off. Call once from setup().
  void begin(const PlantConfig& config);

  const char* name() const { return config_->name; }
  int readRaw() const;
  int moisturePercent() const;
  bool needsWater() const;

  // True if the plant needs water and the soak time and hourly cap allow it.
  // Books the cycle, so call it once per check.
  bool shouldWaterNow(unsigned long now);
  bool hourlyCapReached() const { return gate_.cyclesThisHour >= config::MAX_CYCLES_PER_HOUR; }

  void pumpOn() const;
  void pumpOff() const;

  // Runtime tuning (from the app). Values are clamped to a safe range and the
  // applied value is returned. Keeps rawInWater <= dryThreshold < airCutoff <= rawInAir.
  int setDryThreshold(int raw);
  int dryThreshold() const { return dryThreshold_; }
  int setAirCutoff(int raw);
  int airCutoff() const { return airCutoff_; }
  unsigned long setPumpMs(long ms);
  unsigned long pumpMs() const { return pumpMs_; }

private:
  const PlantConfig* config_ = nullptr;
  int dryThreshold_ = 0;
  int airCutoff_ = 0;
  unsigned long pumpMs_ = 0;
  WaterGate gate_;
};
