#include <ArduinoUnitTests.h>
#include "../src/packet.h"

uint8_t buffer[64];

unittest(read_serial_number_packet_is_valid) {
    uint8_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', '1', 'B', '3', 'B', 0x0D };
    uint8_t expectedLength = 7;

    RfidPacket packet = RfidPacket(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = packet.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(read_serial_number_for_reader_2_packet_is_valid) {
    uint8_t length;
    uint8_t readerId = 2;

    uint8_t expectedData[] = { 0x09, 'A', '2', 'B', '3', '8', 0x0D };
    uint8_t expectedLength = 7;

    RfidPacket packet = RfidPacket(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = packet.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(set_reader_id_packet_is_valid) {
    uint8_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'C', '9', '9', '0', '8', '0', '0', '0', '1', '1', '6', 'B', 0x0D };
    uint8_t expectedLength = 16;

    RfidPacket packet = RfidPacket(readerId, RfidPacket::Function::SET_READER_ID, String("99080001"));

    length = packet.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(set_reader_id_for_reader_8_packet_is_valid) {
    uint8_t length;
    uint8_t readerId = 8;

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'C', '9', '9', '0', '8', '0', '0', '0', '1', '8', '6', '2', 0x0D };
    uint8_t expectedLength = 16;

    RfidPacket packet = RfidPacket(readerId, RfidPacket::Function::SET_READER_ID, String("99080001"));

    length = packet.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest_main()