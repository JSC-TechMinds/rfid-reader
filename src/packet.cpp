#include "packet.h"

RfidPacket::RfidPacket(uint8_t readerId, RfidPacket::Function operation, String serialNumber): readerId(readerId), operation(operation), serialNumber(serialNumber) {

}

uint8_t RfidPacket::getReaderId() {
    return readerId;
}

RfidPacket::Function RfidPacket::getOperation() {
    return operation;
}

String RfidPacket::getSerialNumber() {
    return serialNumber;
}

bool RfidPacket::isValid() {
    bool validReaderId = readerId >= 1 && readerId <= 8;
    bool validSerialNumber = (operation == RfidPacket::Function::SET_READER_ID ||
                                operation == RfidPacket::Function::READ_READER_ID) ?
                                 serialNumber.length() > 0 : true;

    return validReaderId && validSerialNumber;
}

RfidPacket RfidPacket::fromWire(uint8_t * rxBuffer, uint8_t length) {
    return RfidPacket(1, RfidPacket::Function::READ_SERIAL_NUMBER);
}

size_t RfidPacket::toWire(uint8_t * txBuffer, uint8_t bufferSize) {
    uint8_t dataLength = 0;

    // Validate packet data.
    //
    // Note: We can't do the validation inside the constructor
    // because Arduino doesn't support exceptions. The packet can
    // be built with invalid data.
    if (!isValid()) {
        return -1;
    }
   
    writeHeader(txBuffer);

    // Include data field.
    // Data field is an optional field with variable length.
    // It consists of a reader serial number, sometimes appended by the reader ID
    // (both in ASCII).
    if (operation == RfidPacket::Function::SET_READER_ID ||
        operation == RfidPacket::Function::READ_READER_ID) {
            String data = serialNumber;

            // Append reader ID to data field
            if (operation == RfidPacket::Function::SET_READER_ID) {
                data += String(readerId);
            }

            dataLength = data.length();
            data.getBytes(txBuffer + HEADER_SIZE, bufferSize - HEADER_SIZE, 0);
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