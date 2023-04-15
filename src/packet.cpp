#include <ArduinoLog.h>
#include "packet.h"

RfidPacket::RfidPacket(): readerId(0), operation(RfidPacket::Function::READ_SERIAL_NUMBER) {
}

RfidPacket::RfidPacket(int8_t readerId, RfidPacket::Function operation, const char * serialNumber): readerId(readerId), operation(operation) {
    strncpy(serialNumberBuffer, serialNumber, SERIAL_NUMBER_LEN + 1);
}

int8_t RfidPacket::getReaderId() {
    return readerId;
}

RfidPacket::Function RfidPacket::getOperation() {
    return operation;
}

const char * RfidPacket::getSerialNumber() {
    return serialNumberBuffer;
}

uint8_t RfidPacket::xorChecksum(uint8_t * buffer, size_t size) {
    uint8_t checkSum = 0;

    for (int i = 0; i < size; i++) {
        checkSum ^= buffer[i];
    }

    return checkSum;
}

bool RfidPacket::isValid() {
    return isValidPacket;
}

RfidRequest::RfidRequest(int8_t readerId, RfidPacket::Function operation, const char * serialNumber): RfidPacket(readerId, operation, serialNumber) {
    // Validate packet data.
    isValidPacket = true;

    if (operation != RfidPacket::Function::READ_READER_ID && (readerId < 1 || readerId > 8)) {
        Log.errorln(F("RfidRequest: Invalid reader ID %d!"), readerId);
        isValidPacket = false;
    }

    // Serial number is used only in two operations. Ignore the check
    // if the operation doesn't need the serial number.
    if (
        (operation == RfidPacket::Function::SET_READER_ID ||
         operation == RfidPacket::Function::READ_READER_ID)
          && strlen(serialNumber) != SERIAL_NUMBER_LEN) {
        Log.errorln(F("RfidRequest: Invalid serial number %s. The serial number should have exactly %d characters!"), serialNumber, SERIAL_NUMBER_LEN);
        isValidPacket = false;
    }
}

size_t RfidRequest::toWire(uint8_t * txBuffer, size_t size) {
    uint8_t dataLength = 0;

    if (!isValidPacket) {
        Log.errorln(F("RfidRequest: Invalid packet!"));
        return -1;
    }

    if (size < HEADER_SIZE + FOOTER_SIZE) {
        Log.errorln(F("RfidRequest: Target buffer too small!"));
        return -1;
    }
   
    writeHeader(txBuffer);

    // Include data field.
    // Data field is an optional field with variable length.
    // It consists of a reader serial number, sometimes appended by the reader ID
    // (both in ASCII).
    if (operation == RfidPacket::Function::SET_READER_ID ||
            operation == RfidPacket::Function::READ_READER_ID) {
                char data[MAX_DATA_PAYLOAD_SIZE + 1];
                strncpy(data, serialNumberBuffer, SERIAL_NUMBER_LEN + 1);

                // Append reader ID to data field
                if (operation == RfidPacket::Function::SET_READER_ID) {
                    char readerIdStr[2];
                    itoa(readerId, readerIdStr, 10);
                    strncat(data, readerIdStr, READER_ID_LEN + 1);
                }

                    if (size < HEADER_SIZE + FOOTER_SIZE + strlen(data)) {
                        Log.errorln(F("RfidRequest: Payload won't fit the buffer!"));
                        return -1;
                    }

                memcpy(txBuffer + HEADER_SIZE, data, strlen(data));
                dataLength = strlen(data);
        }

        writeFooter(txBuffer, dataLength);
        return calculatePacketSize(dataLength);
    }

void RfidRequest::writeHeader(uint8_t * buffer) {
    buffer[0] = RfidRequest::SOH;
    buffer[1] = uint8_t(RfidPacket::Type::NUMBER);

    if (operation == RfidPacket::Function::SET_READER_ID ||
        operation == RfidPacket::Function::READ_READER_ID) {           
            buffer[2] = READER_ID_PLACEHOLDER;
    } else {
        itoa(readerId, (char *) (buffer + 2), 10);
    }

    buffer[3] = uint8_t(operation);
}

void RfidRequest::writeFooter(uint8_t * buffer, size_t size) {
    // BCC checksum
    // Note: Resulting checksum is converted into two ASCII characters.
    // For example: 0x3B is converted into '3', 'B'.

    uint8_t payloadLength = HEADER_SIZE + size;
    uint8_t checksum = xorChecksum(buffer, payloadLength);

    sprintf((char *) (buffer + payloadLength), "%02X", checksum);
    buffer[payloadLength + BCC_SIZE] = RfidPacket::END;
}

uint8_t RfidRequest::calculatePacketSize(size_t packetSize) {
    return HEADER_SIZE + packetSize + FOOTER_SIZE;
}

RfidResponse::RfidResponse(uint8_t * rxBuffer, size_t size): RfidPacket() {
    readerId = READER_ID_NOT_SET;
    operation = RfidPacket::Function::READ_SERIAL_NUMBER;
    serialNumberBuffer[0] = '\0';
    cardDataBuffer[0] = '\0';
    isValidPacket = true;

    if (size == 0) {
        isValidPacket = false;
        return;
    }

    if (rxBuffer[0] != RfidResponse::SOH || rxBuffer[1] != uint8_t(RfidPacket::Type::NUMBER) || rxBuffer[size-1] != RfidPacket::END) {
        Log.errorln(F("RfidResponse: Invalid packet."));
        isValidPacket = false;
        return;
    }

    char operationId = rxBuffer[3];
    if (operationId == (char) RfidPacket::Function::READ_SERIAL_NUMBER) {
        operation = RfidPacket::Function::READ_SERIAL_NUMBER;
    } else if (operationId == (char) RfidPacket::Function::SET_READER_ID) {
        operation = RfidPacket::Function::SET_READER_ID;
    } else if (operationId == (char) RfidPacket::Function::READ_READER_ID) {
        operation = RfidPacket::Function::READ_READER_ID;
    } else if (operationId == (char) RfidPacket::Function::READ_CARD_DATA) {
        operation = RfidPacket::Function::READ_CARD_DATA;
    } else if (operationId == (char) RfidPacket::Function::RE_READ_CARD_DATA) {
        operation = RfidPacket::Function::RE_READ_CARD_DATA;
    } else {
        Log.errorln(F("RfidResponse: Invalid operation %s."), operationId);
        operation = RfidPacket::Function::SET_READER_ID;
        isValidPacket = false;
        return;
    }

    bool hasDataPayload = size > (HEADER_SIZE + FOOTER_SIZE);
    char readerIdStr[2] = { '\0', '\0' };

    if (hasDataPayload) {
        uint8_t payloadBytes = size - (HEADER_SIZE + FOOTER_SIZE);

        switch (operation) {
            case RfidPacket::Function::READ_SERIAL_NUMBER:
                readerIdStr[0] = rxBuffer[2];

                if (payloadBytes < SERIAL_NUMBER_LEN || payloadBytes > SERIAL_NUMBER_LEN) {
                    Log.errorln(F("RfidResponse: Serial number has unexpected size."));
                    isValidPacket = false;
                    return;
                }

                strncpy(serialNumberBuffer, (const char *) (rxBuffer + HEADER_SIZE), SERIAL_NUMBER_LEN);
                serialNumberBuffer[SERIAL_NUMBER_LEN] = '\0';
                break;
            case RfidPacket::Function::SET_READER_ID:
                Log.errorln(F("RfidResponse: Unexpected payload!"));
                isValidPacket = false;
                return;
            case RfidPacket::Function::READ_READER_ID:
                readerIdStr[0] = rxBuffer[HEADER_SIZE];
                break;
            case RfidPacket::Function::READ_CARD_DATA:
            case RfidPacket::Function::RE_READ_CARD_DATA:
                readerIdStr[0] = rxBuffer[2];

                if (payloadBytes > CARD_DATA_LEN) {
                    Log.warningln(F("RfidResponse: Card data payload is longer than expected. It will be trimmed."));
                    payloadBytes = CARD_DATA_LEN;
                }

                strncpy(cardDataBuffer, (const char *) (rxBuffer + HEADER_SIZE), payloadBytes);
                cardDataBuffer[payloadBytes] = '\0';
                break;
            default:
                break;
        }
        readerId = atoi((const char *) readerIdStr);
    } else {
        switch (operation) {
            case RfidPacket::Function::READ_CARD_DATA:
            case RfidPacket::Function::RE_READ_CARD_DATA:
                readerIdStr[0] = rxBuffer[2];
                readerId = atoi((const char *) readerIdStr);
                break;
            case RfidPacket::Function::READ_SERIAL_NUMBER:
            case RfidPacket::Function::READ_READER_ID:
                Log.errorln(F("RfidResponse: Packet doesn't contain data payload."));
                isValidPacket = false;
                return;
            default:
                break;
        }
    }

    // Checksum
    uint8_t checksum = xorChecksum(rxBuffer, size - FOOTER_SIZE);
    uint8_t calculatedChecksum;
    sscanf((char *) (rxBuffer + (size - FOOTER_SIZE)), "%2hhx", &calculatedChecksum);

    if (checksum != calculatedChecksum) {
        Log.errorln(F("RfidResponse: Invalid checksum!"));
        isValidPacket = false;
    }
}

RfidResponse RfidResponse::fromWire(uint8_t * rxBuffer, size_t size) {
    return RfidResponse(rxBuffer, size);
}

RfidResponse RfidResponse::invalidResponse() {
    uint8_t buffer[1];
    return RfidResponse(buffer, 0);
}

const char * RfidResponse::getCardData() {
    return cardDataBuffer;
}