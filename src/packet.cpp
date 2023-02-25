#include "packet.h"

RfidPacket::RfidPacket(uint8_t readerId, RfidPacket::Function operation, String data): readerId(readerId), operation(operation), data(data) {

}

RfidPacket RfidPacket::fromWire(uint8_t * rxBuffer, uint8_t length) {
    return RfidPacket(1, RfidPacket::Function::READ_SERIAL_NUMBER);
}

uint8_t RfidPacket::toWire(uint8_t * txBuffer) {
    txBuffer[0] = RfidPacket::SOH;
    txBuffer[1] = uint8_t(RfidPacket::Type::NUMBER);
    itoa(readerId, (char *) (txBuffer + 2), 10);
    txBuffer[3] = uint8_t(operation);

    // BCC checksum
    // Note: Resulting checksum is converted into two ASCII characters.
    // For example: 0x3B is converted into '3', 'B'.
    uint8_t checksum = xorChecksum(txBuffer, 4);
    sprintf((char *) (txBuffer + 4), "%02X", checksum);

    txBuffer[6] = RfidPacket::END;

    return 7;
}

uint8_t RfidPacket::xorChecksum(uint8_t * array, uint8_t length) {
    uint8_t checkSum = 0;

    for (int i = 0; i < length; i++) {
        checkSum ^= array[i];
    }

    return checkSum;
}