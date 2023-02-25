#include <ArduinoUnitTests.h>
#include "../src/packet.h"

uint8_t buffer[64];

unittest(read_serial_number_packet_is_valid) {
    uint8_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', '1', 'B', '3', 'B', 0x0D };
    uint8_t expectedLength = 7;

    RfidPacket packet = RfidPacket(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = packet.toWire(buffer);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest_main()