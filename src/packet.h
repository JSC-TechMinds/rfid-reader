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

        String getSerialNumber();

        virtual bool isValid() = 0;

    protected:
        explicit RfidPacket(
            uint8_t readerId,
            RfidPacket::Function operation,
            String serialNumber = "");

        enum class Type : uint8_t {
            NUMBER = 'A'
        };

        uint8_t xorChecksum(uint8_t * buffer, uint8_t length);

        // Packet-specific control characters
        static constexpr const uint8_t END = 0x0D; // End

        // This value is used instead of actual reader ID when user
        // wants to set the reader ID or read it back.
        // The desired reader ID is appended as a last byte to the data payload.
        static constexpr const uint8_t READER_ID_PLACEHOLDER = 'X';

        uint8_t readerId;
        RfidPacket::Function operation;
        String serialNumber;
};

class RfidRequest: public RfidPacket {
    public:
        // Not all operations require reader ID or serial number
        explicit RfidRequest(
            uint8_t readerId,
            RfidPacket::Function operation,
            String serialNumber = "");

        size_t toWire(uint8_t * txBuffer, uint8_t bufferSize);

        bool isValid() override;

    private:
        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x09; // Start of heading

        void writeHeader(uint8_t * buffer);

        void writeFooter(uint8_t * buffer, uint8_t dataLength = 0);

        uint8_t calculatePacketSize(uint8_t dataLength = 0);

        static constexpr const uint8_t HEADER_SIZE = 4; // 4 bytes
        static constexpr const uint8_t FOOTER_SIZE = 3; // 3 bytes
        static constexpr const uint8_t BCC_SIZE = 2; // 2 bytes for checksum
};

class RfidResponse: public RfidPacket {
    public:
        static RfidResponse fromWire(uint8_t * rxBuffer, uint8_t length);

        bool isValid() override;

    private:
        // Used for wire de-serialization. Called by fromWire(...).
        explicit RfidResponse(uint8_t * rxBuffer, uint8_t length);

        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x0A; // Start of heading

        bool isValidPacket = false;
};

#endif // RFID_PACKET_H