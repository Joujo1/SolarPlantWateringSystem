#include <Arduino.h>

namespace WaterPumpCtrl
{
    class waterPumpCtrl
    {
    public:
        bool needsWater(int buttonPin);

        void runPump(int pumpPin, int durationMs);

        void pumpOn(int pumpPin);
        
        void pumpOff(int pumpPin);
    };
}
