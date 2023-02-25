#include "packet.h"

RfidPacket::RfidPacket(uint8_t readerId, RfidPacket::Function operation, String data): readerId(readerId), operation(operation), data(data) {

}

RfidPacket RfidPacket::fromWire(uint8_t * rxBuffer, uint8_t length) {
    return RfidPacket(1, RfidPacket::Function::READ_SERIAL_NUMBER);
}

uint8_t RfidPacket::toWire(uint8_t * txBuffer, uint8_t bufferSize) {
    uint8_t dataLength = 0;
    
    writeHeader(txBuffer);

    // Append data
    // Data is used only in two operations. Ignore the data otherwise.
    if (operation == RfidPacket::Function::SET_READER_ID ||
        operation == RfidPacket::Function::READ_READER_ID) {
            dataLength = data.length();
            data.getBytes(txBuffer + HEADER_SIZE, bufferSize - HEADER_SIZE, 0);

            // Append reader ID to data
            if (operation == RfidPacket::Function::SET_READER_ID) {
                itoa(readerId, (char *) (txBuffer + HEADER_SIZE + dataLength), 10);
                dataLength += 1;
            }
    }

    writeFooter(txBuffer, dataLength);
    return calculatePacketSize(dataLength);
}

void RfidPacket::writeHeader(uint8_t * buffer) {
    buffer[0] = RfidPacket::SOH;
    buffer[1] = uint8_t(RfidPacket::Type::NUMBER);

    if (operation == RfidPacket::Function::SET_READER_ID ||
        operation == RfidPacket::Function::READ_READER_ID) {           
            buffer[2] = READER_ID_PLACEHOLDER;
    } else {
        itoa(readerId, (char *) (buffer + 2), 10);
    }

    buffer[3] = uint8_t(operation);
}

void RfidPacket::writeFooter(uint8_t * buffer, uint8_t dataLength) {
    // BCC checksum
    // Note: Resulting checksum is converted into two ASCII characters.
    // For example: 0x3B is converted into '3', 'B'.

    uint8_t payloadLength = HEADER_SIZE + dataLength;
    uint8_t checksum = xorChecksum(buffer, payloadLength);

    sprintf((char *) (buffer + payloadLength), "%02X", checksum);
    buffer[payloadLength + BCC_SIZE] = RfidPacket::END;
}

uint8_t RfidPacket::calculatePacketSize(uint8_t dataLength) {
    return HEADER_SIZE + dataLength + FOOTER_SIZE;
}

uint8_t RfidPacket::xorChecksum(uint8_t * buffer, uint8_t length) {
    uint8_t checkSum = 0;

    for (int i = 0; i < length; i++) {
        checkSum ^= buffer[i];
    }

    return checkSum;
}