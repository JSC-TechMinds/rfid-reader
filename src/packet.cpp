#include <ArduinoLog.h>
#include "packet.h"

RfidPacket::RfidPacket(): readerId(0), operation(RfidPacket::Function::READ_SERIAL_NUMBER) {
}

RfidPacket::RfidPacket(uint8_t readerId, RfidPacket::Function operation, const char * serialNumber): readerId(readerId), operation(operation) {
    strncpy(serialNumberBuffer, serialNumber, SERIAL_NUMBER_LEN + 1);
}

uint8_t RfidPacket::getReaderId() {
    return readerId;
}

RfidPacket::Function RfidPacket::getOperation() {
    return operation;
}

const char * RfidPacket::getSerialNumber() {
    return serialNumberBuffer;
}

uint8_t RfidPacket::xorChecksum(uint8_t * buffer, uint8_t length) {
    uint8_t checkSum = 0;

    for (int i = 0; i < length; i++) {
        checkSum ^= buffer[i];
    }

    return checkSum;
}

bool RfidPacket::isValid() {
    return isValidPacket;
}

RfidRequest::RfidRequest(uint8_t readerId, RfidPacket::Function operation, const char * serialNumber): RfidPacket(readerId, operation, serialNumber) {
    // Validate packet data.
    isValidPacket = true;

    if (readerId < 1 || readerId > 8) {
        Log.errorln("RfidRequest: Invalid reader ID %d!", readerId);
        isValidPacket = false;
    }

    // Serial number is used only in two operations. Ignore the check
    // if the operation doesn't need the serial number.
    if (
        (operation == RfidPacket::Function::SET_READER_ID ||
         operation == RfidPacket::Function::READ_READER_ID)
          && strlen(serialNumber) != SERIAL_NUMBER_LEN) {
        Log.errorln("RfidRequest: Invalid serial number %s. The serial number should have exactly %d characters!", serialNumber, SERIAL_NUMBER_LEN);
        isValidPacket = false;
    }
}

size_t RfidRequest::toWire(uint8_t * txBuffer, uint8_t bufferSize) {
    uint8_t dataLength = 0;

    if (!isValidPacket) {
        Log.errorln("RfidRequest: Invalid packet!");
        return -1;
    }
   
    writeHeader(txBuffer);

    // Include data field.
    // Data field is an optional field with variable length.
    // It consists of a reader serial number, sometimes appended by the reader ID
    // (both in ASCII).
    if (operation == RfidPacket::Function::SET_READER_ID ||
            operation == RfidPacket::Function::READ_READER_ID) {
                char data[MAX_DATA_PAYLOAD_SIZE];
                strncpy(data, serialNumberBuffer, SERIAL_NUMBER_LEN + 1);

                // Append reader ID to data field
                if (operation == RfidPacket::Function::SET_READER_ID) {
                    char readerIdStr[2];
                    itoa(readerId, readerIdStr, 10);
                    strncat(data, readerIdStr, READER_ID_LEN + 1);
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

void RfidRequest::writeFooter(uint8_t * buffer, uint8_t dataLength) {
    // BCC checksum
    // Note: Resulting checksum is converted into two ASCII characters.
    // For example: 0x3B is converted into '3', 'B'.

    uint8_t payloadLength = HEADER_SIZE + dataLength;
    uint8_t checksum = xorChecksum(buffer, payloadLength);

    sprintf((char *) (buffer + payloadLength), "%02X", checksum);
    buffer[payloadLength + BCC_SIZE] = RfidPacket::END;
}

uint8_t RfidRequest::calculatePacketSize(uint8_t dataLength) {
    return HEADER_SIZE + dataLength + FOOTER_SIZE;
}

RfidResponse::RfidResponse(uint8_t * rxBuffer, uint8_t length): RfidPacket() {
    isValidPacket = true;
    if (length == 0 || rxBuffer[0] != RfidResponse::SOH || rxBuffer[1] != uint8_t(RfidPacket::Type::NUMBER) || rxBuffer[length-1] != RfidPacket::END) {
        Log.errorln("RfidResponse: Invalid packet.");
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
        operation = RfidPacket::Function::READ_CARD_DATA;
    } else {
        Log.errorln("RfidResponse: Invalid operation %s.", operationId);
        operation = RfidPacket::Function::SET_READER_ID;
        isValidPacket = false;
        return;
    }

    bool dataPayload = length > (HEADER_SIZE + FOOTER_SIZE);

    if (operation == RfidPacket::Function::SET_READER_ID) {
        // Reader ID is not in the response packet. Set to 0.
        readerId = 0;
    } else {
        if (!dataPayload) {
            Log.errorln("RfidResponse: Packet doesn't contain data payload.");
            isValidPacket = false;
            return;
        }
    }

    if (dataPayload) {
        // We have data payload.
        uint8_t payloadBytes = length - (HEADER_SIZE + FOOTER_SIZE);

        if (payloadBytes > (MAX_DATA_PAYLOAD_SIZE - 1)) {
            Log.errorln("RfidResponse: Payload exceeds %d bytes.", MAX_DATA_PAYLOAD_SIZE - 1);
            isValidPacket = false;
            return;
        }

        if (operation == RfidPacket::Function::READ_READER_ID) {
            // Reader ID is set in the data payload.
            readerId = atoi((const char *) (rxBuffer + HEADER_SIZE));
        } else {
            readerId = atoi((const char *) (rxBuffer + 2));
            strncpy(serialNumberBuffer, (const char *) (rxBuffer + HEADER_SIZE), SERIAL_NUMBER_LEN);
            serialNumberBuffer[SERIAL_NUMBER_LEN] = '\0';
        }
    }

    // Checksum
    uint8_t checksum = xorChecksum(rxBuffer, length - FOOTER_SIZE);
    uint8_t calculatedChecksum;
    sscanf((char *) (rxBuffer + (length - FOOTER_SIZE)), "%2hhx", &calculatedChecksum);

    if (checksum != calculatedChecksum) {
        Log.errorln("RfidResponse: Invalid checksum!");
        isValidPacket = false;
    }
}

RfidResponse RfidResponse::fromWire(uint8_t * rxBuffer, uint8_t length) {
    return RfidResponse(rxBuffer, length);
}