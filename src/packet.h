#ifndef RFID_PACKET_H
#define RFID_PACKET_H

#include <Arduino.h>

class RfidPacket {
    public:
        enum class Type : uint8_t {
            NUMBER = 'A'
        };

        enum class Function : uint8_t {
            READ_SERIAL_NUMBER = 'B',
            SET_READER_ID = 'C',
            READ_READER_ID = 'D',
            READ_CARD_CODE_DATA = 'F',
            RE_READ_CARD = 'G'
        };

        explicit RfidPacket(
            uint8_t readerId,
            RfidPacket::Function operation,
            String data = "");

        static RfidPacket fromWire(uint8_t * rxBuffer, uint8_t length);
        uint8_t toWire(uint8_t * txBuffer);

    private:
        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x09; // Start of heading
        static constexpr const uint8_t END = 0x0D; // End

        uint8_t xorChecksum(uint8_t * array, uint8_t length);

        uint8_t readerId;
        RfidPacket::Function operation;
        String data;

};


#endif // RFID_PACKET_H