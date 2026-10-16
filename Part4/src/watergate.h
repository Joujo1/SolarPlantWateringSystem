#pragma once

// Decides whether an automatic watering cycle may start. Kept free of Arduino
// calls so it can be unit tested.
struct WaterGate {
  unsigned long nextAllowedAt = 0;
  unsigned long hourStart = 0;
  int cyclesThisHour = 0;
};

const unsigned long ONE_HOUR_MS = 3600000UL;

// Returns true and books a cycle if the soak time since the last cycle has
// passed and the hourly cap is not reached. Safe across millis() rollover.
inline bool tryStartCycle(WaterGate& gate, unsigned long now, unsigned long soakMs, int maxCyclesPerHour) {
  if (now - gate.hourStart >= ONE_HOUR_MS) {
    gate.hourStart = now;
    gate.cyclesThisHour = 0;
  }
  if ((long)(now - gate.nextAllowedAt) < 0) return false;
  if (gate.cyclesThisHour >= maxCyclesPerHour) return false;

  gate.cyclesThisHour++;
  gate.nextAllowedAt = now + soakMs;
  return true;
}
