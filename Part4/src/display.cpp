#include "display.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

static Adafruit_ST7789 tft = Adafruit_ST7789(
    config::TFT_CS, config::TFT_DC, config::TFT_MOSI, config::TFT_SCLK, config::TFT_RST);

static const int SCREEN_W = 320;
static const int ROW_TOP = 45;   // first plant row, just below the title bar
static const int ROW_H = 30;     // Display::MAX_PLANTS rows end at y = 135

static const int RAIN_N = 5;
static const int RAIN_X0 = 135, RAIN_X1 = 185;
static const int RAIN_Y0 = 143, RAIN_Y1 = 170;   // between the rows and the plant
static int rainX[RAIN_N], rainY[RAIN_N];

static const float PLANT_SCALE = 0.6;   // full size is about 99 px tall
static const int PLANT_BASE_Y = 236;

// Scales a drawing offset or radius by PLANT_SCALE.
static int s(int v) { return (int)(v * PLANT_SCALE + 0.5); }

// Draws a little potted flower. (cx, baseY) is the centre of the pot's base.
static void drawPlant(int cx, int baseY) {
  uint16_t terracotta = tft.color565(198, 96, 54);
  uint16_t rim = tft.color565(168, 76, 40);
  uint16_t soil = tft.color565(70, 45, 30);
  uint16_t green = tft.color565(60, 170, 70);
  uint16_t greenDark = tft.color565(40, 130, 50);
  uint16_t petal = tft.color565(235, 95, 140);
  uint16_t middle = tft.color565(250, 205, 70);

  int potTopY = baseY - s(36);

  tft.fillTriangle(cx - s(30), potTopY, cx + s(30), potTopY, cx + s(21), baseY, terracotta);
  tft.fillTriangle(cx - s(30), potTopY, cx + s(21), baseY, cx - s(21), baseY, terracotta);
  tft.fillRoundRect(cx - s(36), potTopY - s(13), s(72), s(14), s(4), rim);
  tft.fillRect(cx - s(30), potTopY - s(11), s(60), s(5), soil);

  tft.fillRect(cx - s(3), potTopY - s(42), s(6), s(46), green);

  tft.fillCircle(cx - s(15), potTopY - s(22), s(10), green);
  tft.fillCircle(cx + s(15), potTopY - s(30), s(10), greenDark);

  int fy = potTopY - s(46);
  tft.fillCircle(cx, fy - s(11), s(6), petal);
  tft.fillCircle(cx + s(9), fy - s(5), s(6), petal);
  tft.fillCircle(cx + s(9), fy + s(5), s(6), petal);
  tft.fillCircle(cx, fy + s(11), s(6), petal);
  tft.fillCircle(cx - s(9), fy + s(5), s(6), petal);
  tft.fillCircle(cx - s(9), fy - s(5), s(6), petal);
  tft.fillCircle(cx, fy, s(6), middle);
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
void Display::drawWifi(bool connected) {
  const int cx = 300, dotY = 30;
  tft.fillRect(282, 4, 36, 32, ST77XX_BLUE); // clear the icon area (title-bar bg)
  if (!connected) return;

  uint16_t col = ST77XX_WHITE;
  tft.fillCircle(cx, dotY, 2, col); // the dot at the bottom
  drawArc(cx, dotY, 7, col);        // three waves fanning out above it
  drawArc(cx, dotY, 12, col);
  drawArc(cx, dotY, 17, col);
}

void Display::begin() {
  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // Title bar
  tft.fillRect(0, 0, SCREEN_W, 40, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 8);
  tft.print("WASSERKANONE");

  drawPlant(SCREEN_W / 2, PLANT_BASE_Y);
}

void Display::showPlant(int index, bool watering) {
  if (index < 0 || index >= MAX_PLANTS) return;
  int y = ROW_TOP + index * ROW_H;

  tft.fillRect(0, y, SCREEN_W, ROW_H, ST77XX_BLACK);

  tft.setTextSize(3);
  tft.setCursor(10, y + 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("BLATT ");
  tft.print(index + 1);
  tft.print(": ");
  if (watering) {
    tft.setTextColor(ST77XX_CYAN);
    tft.print("GEDEIHE");
  } else {
    tft.setTextColor(tft.color565(90, 90, 90));
    tft.print("LEBEND");
  }
}

void Display::rain(unsigned long durationMs) {
  uint16_t drop = tft.color565(110, 190, 255);

  unsigned long start = millis();
  while (millis() - start < durationMs) {
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
