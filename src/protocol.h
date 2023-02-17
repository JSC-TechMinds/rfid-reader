#ifndef RFID_PROTOCOL_H
#define RFID_PROTOCOL_H

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
            uint8_t reader_id,
            RfidPacket::Function operation,
            /* Optional; it depends on operation. */
            byte data [] = nullptr);

    private:
        // Packet-specific control characters
        static constexpr const uint8_t SOH = 0x09; // Start of heading
        static constexpr const uint8_t END = 0x0D; // End

};


#endif // RFID_PROTOCOL_H