#include <Arduino.h>
#include "packet.h"

void setup() {
    Serial.begin(115200);
    uint8_t buffer[64];
    uint8_t length;
    RfidRequest request = RfidRequest(1, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = request.toWire(buffer, 64);
}

void loop() {
}
