#include <Arduino.h>
#include <ArduinoLog.h>
#include "packet.h"

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    delay(1000);

    uint8_t responseData[] = { 0x0A, 'A', '2', 'F', '0', '0', '0', '0', '0', '0', 'F', 'F', '1', 'A', '4', 'F', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 17);

    Log.noticeln("Operation READ_CARD_DATA: %T", (uint8_t) RfidPacket::Function::READ_CARD_DATA == (uint8_t) response.getOperation());
    Log.noticeln("Reader ID: %d", response.getReaderId());
    Log.noticeln("Tag: %s", response.getCardData());
    Log.noticeln("Is valid response: %T", response.isValid());
}

void loop() {
}
