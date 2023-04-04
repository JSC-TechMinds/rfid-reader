#include <ArduinoUnitTests.h>
#include "../src/packet.h"

uint8_t buffer[64];

unittest(read_serial_number_packet_is_valid) {
    size_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', '1', 'B', '3', 'B', 0x0D };
    size_t expectedLength = 7;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(read_serial_number_from_reader_2_packet_is_valid) {
    size_t length;
    uint8_t readerId = 2;

    uint8_t expectedData[] = { 0x09, 'A', '2', 'B', '3', '8', 0x0D };
    size_t expectedLength = 7;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(set_reader_id_packet_is_valid) {
    size_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'C', '9', '9', '0', '8', '0', '0', '0', '1', '1', '6', 'B', 0x0D };
    size_t expectedLength = 16;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::SET_READER_ID, "99080001");

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(set_reader_id_for_reader_8_packet_is_valid) {
    size_t length;
    uint8_t readerId = 8;

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'C', '9', '9', '0', '8', '0', '0', '0', '1', '8', '6', '2', 0x0D };
    size_t expectedLength = 16;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::SET_READER_ID, "99080001");

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(read_reader_id_packet_is_valid) {
    size_t length;
    uint8_t readerId = 1; // Ignored

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'D', '9', '9', '0', '8', '0', '0', '0', '1', '5', 'D', 0x0D };
    size_t expectedLength = 15;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_READER_ID, "99080001");

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(read_card_data_packet_is_valid) {
    size_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', '1', 'F', '3', 'F', 0x0D };
    size_t expectedLength = 7;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_CARD_DATA);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(read_card_data_from_reader_4_packet_is_valid) {
    size_t length;
    uint8_t readerId = 4;

    uint8_t expectedData[] = { 0x09, 'A', '4', 'F', '3', 'A', 0x0D };
    size_t expectedLength = 7;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_CARD_DATA);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(re_read_card_data_packet_is_valid) {
    size_t length;
    uint8_t readerId = 1;

    uint8_t expectedData[] = { 0x09, 'A', '1', 'G', '3', 'E', 0x0D };
    size_t expectedLength = 7;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::RE_READ_CARD_DATA);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest(reader_id_out_of_range_write_no_data) {
    size_t length;
    uint8_t readerId = 10;

    size_t expectedLength = -1;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
}

unittest(target_buffer_too_small) {
    uint8_t smallBuffer[5];
    size_t length;
    uint8_t readerId = 1;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_SERIAL_NUMBER);

    length = request.toWire(smallBuffer, 5);

    assertEqual(-1, length);
}

unittest(target_buffer_too_small_for_payload) {
    uint8_t smallBuffer[9];
    size_t length;
    uint8_t readerId = 1; // Ignored

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_READER_ID, "99080001");

    length = request.toWire(smallBuffer, 9);

    assertEqual(-1, length);
}

unittest(read_reader_id_no_id_validation) {
    size_t length;
    uint8_t readerId = -1; // Ignored, out of range ID value

    uint8_t expectedData[] = { 0x09, 'A', 'X', 'D', '9', '9', '0', '8', '0', '0', '0', '1', '5', 'D', 0x0D };
    size_t expectedLength = 15;

    RfidRequest request = RfidRequest(readerId, RfidPacket::Function::READ_READER_ID, "99080001");

    length = request.toWire(buffer, 64);

    assertEqual(expectedLength, length);
    assertEqual(0, memcmp(buffer, expectedData, expectedLength));
}

unittest_main()