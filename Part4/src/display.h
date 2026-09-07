#include <Arduino.h>

namespace DisplayCtrl
{
    class tftDisplay
    {
    public:
        void begin();

        void showChannel(int channel, bool watering);

        void rain(int durationMs);

        void drawWifi(bool connected);
    };
}
