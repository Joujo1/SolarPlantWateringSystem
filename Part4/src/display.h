#pragma once

#include <Arduino.h>

// The 320x240 ST7789 status screen.
class Display {
public:
  static constexpr int MAX_PLANTS = 3;   // status rows that fit on the screen

  void begin();

  // Shows "watering" or "idle" for a plant (0-based index).
  void showPlant(int index, bool watering);

  // Animates rain for durationMs. Blocks until done.
  void rain(unsigned long durationMs);

  void drawWifi(bool connected);
};
