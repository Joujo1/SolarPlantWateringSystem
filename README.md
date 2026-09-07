# Solar Plant Watering System

Plant watering system for the Arduino Uno R4 WiFi. It reads soil moisture, runs water pumps through a relay, shows status on a 2.0 inch ST7789 TFT display, and can be triggered from a phone with Blynk.

Hardware: Arduino Uno R4 WiFi, Adafruit 2.0 inch 320x240 ST7789 TFT display, capacitive soil moisture sensors, a 4 channel relay, and water pumps.

## Build

The project uses PlatformIO. The source code is in the Part4 folder. Build and upload with the uno_r4_wifi environment:

    pio run -e uno_r4_wifi -t upload

## Secrets

Part4/src/wifi_secrets.h is required to build but is not included in the repository because it is gitignored. Create it with your own values:

    #define SSID "your_wifi_name"
    #define PASSWORD "your_wifi_password"
    #define BLYNK_AUTH_TOKEN "your_blynk_token"
