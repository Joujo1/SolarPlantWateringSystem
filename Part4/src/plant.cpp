#include "plant.h"

void Plant::begin(const PlantConfig& config) {
  config_ = &config;
  dryThreshold_ = config.dryThreshold;
  airCutoff_ = config.airCutoff;
  pumpMs_ = config.pumpMs;

  pinMode(config.sensorPin, INPUT);
  pinMode(config.pumpPin, OUTPUT);
  pumpOff();
}

int Plant::readRaw() const {
  return analogRead(config_->sensorPin);
}

int Plant::moisturePercent() const {
  // 0 % at the air cutoff, so "not in soil" reads as 0 %
  long percent = map(readRaw(), airCutoff_, config_->rawInWater, 0, 100);
  return constrain(percent, 0, 100);
}

bool Plant::needsWater() const {
  int raw = readRaw();
  return raw > dryThreshold_ && raw < airCutoff_;
}

bool Plant::shouldWaterNow(unsigned long now) {
  return needsWater() &&
         tryStartCycle(gate_, now, config::SOAK_MS, config::MAX_CYCLES_PER_HOUR);
}

void Plant::pumpOn() const { digitalWrite(config_->pumpPin, LOW); }
void Plant::pumpOff() const { digitalWrite(config_->pumpPin, HIGH); }

int Plant::setDryThreshold(int raw) {
  dryThreshold_ = constrain(raw, config_->rawInWater, airCutoff_ - 1);
  return dryThreshold_;
}

int Plant::setAirCutoff(int raw) {
  airCutoff_ = constrain(raw, dryThreshold_ + 1, config_->rawInAir);
  return airCutoff_;
}

unsigned long Plant::setPumpMs(long ms) {
  pumpMs_ = constrain(ms, (long)config::MIN_PUMP_MS, (long)config::MAX_PUMP_MS);
  return pumpMs_;
}
