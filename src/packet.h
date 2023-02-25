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
        uint8_t toWire(uint8_t * txBuffer, uint8_t bufferSize);

    private:
        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x09; // Start of heading
        static constexpr const uint8_t END = 0x0D; // End

        static constexpr const uint8_t HEADER_SIZE = 4; // 4 bytes
        static constexpr const uint8_t FOOTER_SIZE = 3; // 3 bytes
        static constexpr const uint8_t BCC_SIZE = 2; // 2 bytes for checksum

        // This value is used instead of actual reader ID when user
        // wants to set the reader ID or read it back.
        // The desired reader ID is appended as a last byte to the data payload.
        static constexpr const uint8_t READER_ID_PLACEHOLDER = 'X';

        uint8_t xorChecksum(uint8_t * buffer, uint8_t length);

        void writeHeader(uint8_t * buffer);

        void writeFooter(uint8_t * buffer, uint8_t dataLength = 0);

        uint8_t calculatePacketSize(uint8_t dataLength = 0);

        uint8_t readerId;
        RfidPacket::Function operation;
        String data;

};


#endif // RFID_PACKET_H