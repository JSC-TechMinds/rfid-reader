#include <Arduino.h>
#include "packet.h"

void setup() {
    Serial.begin(115200);
    uint8_t buffer[64];
    uint8_t length;
    RfidPacket packet = RfidPacket(1, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = packet.toWire(buffer);
}

void loop() {
}
