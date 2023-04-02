#include <ArduinoUnitTests.h>
#include "../src/packet.h"

unittest(no_data) {
    uint8_t responseData[] = { };
    RfidResponse response = RfidResponse::fromWire(responseData, 0);
    assertFalse(response.isValid());
}

unittest(invalid_response_soh_value) {
    uint8_t responseData[] = { 0x09, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', '2', 0x0D };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(invalid_packet_termination_value) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', '1', 0x0A };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(invalid_type) {
    uint8_t responseData[] = { 0x0A, 'B', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', '2', 0x0D };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(invalid_checksum) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '2', '1', 0x0D };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(invalid_operation) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'H', '9', '9', '0', '8', '0', '0', '0', '1', '3', 'B', 0x0D };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(payload_exceeds_limit) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '1', '0', '9', 0x0D };
    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertFalse(response.isValid());
}

unittest(read_serial_number) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', '1', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertEqual((uint8_t) RfidPacket::Function::READ_SERIAL_NUMBER, (uint8_t) response.getOperation());
    assertEqual(1, response.getReaderId());
    assertEqual("99080001", response.getSerialNumber());
    assertTrue(response.isValid());
}

unittest(read_serial_number_from_reader_8) {
    uint8_t responseData[] = { 0x0A, 'A', '8', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', '8', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 15);
    assertEqual((uint8_t) RfidPacket::Function::READ_SERIAL_NUMBER, (uint8_t) response.getOperation());
    assertEqual(8, response.getReaderId());
    assertEqual("99080001", response.getSerialNumber());
    assertTrue(response.isValid());
}

unittest(set_reader_id) {
    uint8_t responseData[] = { 0x0A, 'A', 'X', 'C', '5', '0', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 7);
    assertEqual((uint8_t) RfidPacket::Function::SET_READER_ID, (uint8_t) response.getOperation());
    assertEqual(-1, response.getReaderId());
    assertEqual("", response.getSerialNumber());
    assertTrue(response.isValid());
}

unittest(set_reader_id_with_payload_is_invalid) {
    uint8_t responseData[] = { 0x0A, 'A', 'X', 'C', '1', '6', '1', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 8);
    assertFalse(response.isValid());
}

unittest(read_reader_id) {
    uint8_t responseData[] = { 0x0A, 'A', 'X', 'D', '2', '6', '5', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 8);
    assertEqual((uint8_t) RfidPacket::Function::READ_READER_ID, (uint8_t) response.getOperation());
    assertEqual(2, response.getReaderId());
    assertEqual("", response.getSerialNumber());
    assertTrue(response.isValid());
}

unittest(read_card_data_no_card) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'F', '3', 'C', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 7);
    assertEqual((uint8_t) RfidPacket::Function::READ_CARD_DATA, (uint8_t) response.getOperation());
    assertEqual(1, response.getReaderId());
    assertEqual("", response.getSerialNumber());
    assertEqual("", response.getCardData());
    assertTrue(response.isValid());
}

unittest_main()