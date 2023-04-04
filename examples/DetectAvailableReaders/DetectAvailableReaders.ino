// This example uses synchronous blocking API.

#include <Arduino.h>
#include <ArduinoLog.h>
#include "RfidReader.h"

#define RO_PIN 5
#define DI_PIN 19
#define RE_PIN 18

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    delay(1000);

    Log.noticeln("Finding available readers...");

    Serial2.begin(19200, SERIAL_8E1, RO_PIN, DI_PIN);
    RfidReader reader = RfidReader(Serial2, RE_PIN);

    // Note: Maximum 8 readers is supported on the bus.
    // This constraint makes it fairly easy to detect readers on the bus.
    int total = 0;
    for (int i=1; i<=8; i++) {
        Log.noticeln("Querying for reader with ID %d.", i);
        if (strlen(reader.getSerialNumber(i)) > 0) {
            total += 1;
            Log.noticeln("Found reader with ID %d (%d total).", i, total);
        }
    }
}

void loop() {
}
