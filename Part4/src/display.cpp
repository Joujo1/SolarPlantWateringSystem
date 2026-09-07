#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

using namespace DisplayCtrl;

#define TFT_CS 10   // display "CS"
#define TFT_DC 8    // display "D/C"
#define TFT_RST 9   // display "RST"
#define TFT_MOSI 11 // display "MOSI"
#define TFT_SCLK 13 // display "SCK"

static Adafruit_ST7789 tft =
    Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

static const int SCREEN_W = 320;
static const int ROW_TOP = 55;

static const int RAIN_N = 5;
static const int RAIN_X0 = 120, RAIN_X1 = 198;
static const int RAIN_Y0 = 92, RAIN_Y1 = 127;
static int rainX[RAIN_N], rainY[RAIN_N];

// Draws a little potted flower. (cx, baseY) is the centre of the pot's base.
static void drawPlant(int cx, int baseY) {
  uint16_t terracotta = tft.color565(198, 96, 54);
  uint16_t rim = tft.color565(168, 76, 40);
  uint16_t soil = tft.color565(70, 45, 30);
  uint16_t green = tft.color565(60, 170, 70);
  uint16_t greenDark = tft.color565(40, 130, 50);
  uint16_t petal = tft.color565(235, 95, 140);
  uint16_t middle = tft.color565(250, 205, 70);

  int potTopY = baseY - 36;

  tft.fillTriangle(cx - 30, potTopY, cx + 30, potTopY, cx + 21, baseY, terracotta);
  tft.fillTriangle(cx - 30, potTopY, cx + 21, baseY, cx - 21, baseY, terracotta);
  tft.fillRoundRect(cx - 36, potTopY - 13, 72, 14, 4, rim);
  tft.fillRect(cx - 30, potTopY - 11, 60, 5, soil);

  tft.fillRect(cx - 3, potTopY - 42, 6, 46, green);

  tft.fillCircle(cx - 15, potTopY - 22, 10, green);
  tft.fillCircle(cx + 15, potTopY - 30, 10, greenDark);

  int fy = potTopY - 46;
  tft.fillCircle(cx, fy - 11, 6, petal);
  tft.fillCircle(cx + 9, fy - 5, 6, petal);
  tft.fillCircle(cx + 9, fy + 5, 6, petal);
  tft.fillCircle(cx, fy + 11, 6, petal);
  tft.fillCircle(cx - 9, fy + 5, 6, petal);
  tft.fillCircle(cx - 9, fy - 5, 6, petal);
  tft.fillCircle(cx, fy, 6, middle);
}

// Draws the upper part of an arc (used for the wifi "waves").
static void drawArc(int cx, int cy, int r, uint16_t color) {
  for (float a = 40; a <= 140; a += 2.5) {
    float rad = radians(a);
    int x = cx + (int)(r * cos(rad) + 0.5);
    int y = cy - (int)(r * sin(rad) + 0.5);
    tft.fillCircle(x, y, 1, color); // small blobs overlap into a thick arc
  }
}

// Small wifi symbol in the top-right of the title bar: drawn when connected,
// erased (left blank) when not.
void tftDisplay::drawWifi(bool connected) {
  const int cx = 300, dotY = 30;
  tft.fillRect(282, 4, 36, 32, ST77XX_BLUE); // clear the icon area (title-bar bg)
  if (!connected) return;

  uint16_t col = ST77XX_WHITE;
  tft.fillCircle(cx, dotY, 2, col); // the dot at the bottom
  drawArc(cx, dotY, 7, col);        // three waves fanning out above it
  drawArc(cx, dotY, 12, col);
  drawArc(cx, dotY, 17, col);
}

void tftDisplay::begin() {
  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // Title bar
  tft.fillRect(0, 0, SCREEN_W, 40, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 8);
  tft.print("WASSERKANONE");

  drawPlant(160, 234);
}

void tftDisplay::showChannel(int channel, bool watering) {
  int y = ROW_TOP;

  tft.fillRect(0, y, SCREEN_W, 36, ST77XX_BLACK);

  tft.setTextSize(3);
  tft.setCursor(10, y + 8);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("BLATT ");
  tft.print(channel + 1);
  tft.print(": ");
  if (watering) {
    tft.setTextColor(ST77XX_CYAN);
    tft.print("GEDEIHE");
  } else {
    tft.setTextColor(tft.color565(90, 90, 90));
    tft.print("LEBEND");
  }
}

void tftDisplay::rain(int durationMs) {
  uint16_t drop = tft.color565(110, 190, 255);

  unsigned long start = millis();
  while (millis() - start < (unsigned long)durationMs) {
    for (int i = 0; i < RAIN_N; i++) {
      rainX[i] = RAIN_X0 + random(RAIN_X1 - RAIN_X0);
      rainY[i] = RAIN_Y0 + random(RAIN_Y1 - RAIN_Y0);
      tft.fillCircle(rainX[i], rainY[i], 3, drop);
    }
    delay(150);

    for (int i = 0; i < RAIN_N; i++) {
      tft.fillCircle(rainX[i], rainY[i], 3, ST77XX_BLACK);
    }
    delay(120);
  }

  for (int i = 0; i < RAIN_N; i++) {
    tft.fillCircle(rainX[i], rainY[i], 3, ST77XX_BLACK);
  }
}
