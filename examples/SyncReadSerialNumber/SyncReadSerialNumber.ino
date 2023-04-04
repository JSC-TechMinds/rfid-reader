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

    Log.noticeln("Reading serial number from reader 1.");

    Serial2.begin(19200, SERIAL_8E1, RO_PIN, DI_PIN);

    RfidReader reader = RfidReader(Serial2, RE_PIN);
    const char * serialNumber = reader.getSerialNumber(1);

    Log.noticeln("Reader 1 serial: %s.", serialNumber);
}

void loop() {
}
