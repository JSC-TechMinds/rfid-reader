#ifndef RFID_PACKET_H
#define RFID_PACKET_H

#include <Arduino.h>

class RfidPacket {
    public:
        virtual ~RfidPacket() = default;

        enum class Function : uint8_t {
            READ_SERIAL_NUMBER = 'B',
            SET_READER_ID = 'C',
            READ_READER_ID = 'D',
            READ_CARD_DATA = 'F',
            RE_READ_CARD_DATA = 'G'
        };

        uint8_t getReaderId();

        RfidPacket::Function getOperation();

        const char * getSerialNumber();

        bool isValid();

    protected:
        explicit RfidPacket();

        explicit RfidPacket(
            uint8_t readerId,
            RfidPacket::Function operation,
            const char * serialNumber = "");

        enum class Type : uint8_t {
            NUMBER = 'A'
        };

        static uint8_t xorChecksum(uint8_t * buffer, uint8_t length);

        // Packet-specific control characters
        static constexpr const uint8_t END = 0x0D; // End

        // Reader ID range: <1-8>
        // A single ASCII character is reserved in the header.
        static constexpr const uint8_t READER_ID_LEN = 1;

        // This value is used instead of actual reader ID when user
        // wants to set the reader ID or read it back.
        // The desired reader ID is appended as a last byte to the data payload.
        static constexpr const uint8_t READER_ID_PLACEHOLDER = 'X';

        static constexpr const uint8_t HEADER_SIZE = 4; // 4 bytes
        static constexpr const uint8_t FOOTER_SIZE = 3; // 3 bytes
        static constexpr const uint8_t BCC_SIZE = 2; // 2 bytes for checksum

        // Section 1 to 2 - The year of manufacture.
        // Section 3 to 4 - The week of manufacture.
        // Section 5 to 8 - Sequence number, 0001 to 9999.
        // Example: 99080001
        static constexpr const uint8_t SERIAL_NUMBER_LEN = 8;

        // Serial number, reader ID, 1 byte for null-terminating character
        static constexpr const uint8_t MAX_DATA_PAYLOAD_SIZE = SERIAL_NUMBER_LEN + READER_ID_LEN + 1;

        uint8_t readerId;
        RfidPacket::Function operation;
        char serialNumberBuffer[SERIAL_NUMBER_LEN + 1];
        bool isValidPacket;
};

class RfidRequest: public RfidPacket {
    public:
        // Not all operations require reader ID or serial number
        explicit RfidRequest(
            uint8_t readerId,
            RfidPacket::Function operation,
            const char * serialNumber = "");

        size_t toWire(uint8_t * txBuffer, uint8_t bufferSize);

    private:
        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x09; // Start of heading

        void writeHeader(uint8_t * buffer);

        void writeFooter(uint8_t * buffer, uint8_t dataLength = 0);

        uint8_t calculatePacketSize(uint8_t dataLength = 0);
};

class RfidResponse: public RfidPacket {
    public:
        static RfidResponse fromWire(uint8_t * rxBuffer, uint8_t length);

    private:
        // Not all operations require reader ID or serial number
        explicit RfidResponse(
            uint8_t * rxBuffer,
            uint8_t length);

        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x0A; // Start of heading
};

#endif // RFID_PACKET_H