#include <Arduino.h>
#include <ArduinoLog.h>

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL, &Serial);
}

void loop() {
}
