#include <ArduinoLog.h>
#include "RfidReader.h"

RfidReader::RfidReader(Stream& bus, uint8_t ioPin): bus(bus), ioPin(ioPin) {
    pinMode(ioPin, OUTPUT);
    setRs485ReceiveMode();
}

const char * RfidReader::getSerialNumber(uint8_t readerId) {
    static char serialNumber[10];
    RfidResponse response = requestData(RfidPacket::Function::READ_SERIAL_NUMBER, readerId, "");

    if (response.isValid()) {
        strncpy(serialNumber, response.getSerialNumber(), 10);
        return serialNumber;
    }

    return "";
}

int8_t RfidReader::getReaderId(const char * serialNumber) {
    RfidResponse response = requestData(RfidPacket::Function::READ_READER_ID, -1, serialNumber);

    if (response.isValid()) {
        return response.getReaderId();
    }

    return -1;
}

bool RfidReader::setReaderId(uint8_t readerId, const char * serialNumber) {
    RfidResponse response = requestData(RfidPacket::Function::SET_READER_ID, readerId, serialNumber);

    if (response.isValid()) {
        return true;
    }

    return false;
}

const char * RfidReader::readCardData(uint8_t readerId) {
    static char cardData[10];
    RfidResponse response = requestData(RfidPacket::Function::READ_CARD_DATA, readerId, "");

    if (response.isValid()) {
        strncpy(cardData, response.getCardData(), 10);
        return cardData;
    }

    return "";
}

const char * RfidReader::reReadCardData(uint8_t readerId) {
    static char cardData[10];
    RfidResponse response = requestData(RfidPacket::Function::RE_READ_CARD_DATA, readerId, "");

    if (response.isValid()) {
        strncpy(cardData, response.getCardData(), 10);
        return cardData;
    }

    return "";
}

void RfidReader::setRs485TransferMode() {
    // Activate RE pin
    digitalWrite(ioPin, HIGH);
}
        
void RfidReader::setRs485ReceiveMode() {
    // Activate DE pin
    digitalWrite(ioPin, LOW);
}

/*
 * Set proper delay to wait till all the data from the buffer are transmitted.
 * Values were defined by trial-and-error approach. Improvements are welcome.
*/
void RfidReader::waitForTransferFinish(RfidPacket::Function operation) {
    uint16_t delayMicro = 0;

    switch(operation) {
        case RfidPacket::Function::READ_READER_ID:
        case RfidPacket::Function::SET_READER_ID:
            delayMicro = 9000;
            break;
        default:
            delayMicro = 5000;
            break;

    }

    #ifdef USE_ASYNC_IO
    taskManager.yieldForMicros(delayMicro);
    #else
    delayMicroseconds(delayMicro);
    #endif
}

RfidResponse RfidReader::requestData(RfidPacket::Function operation, int8_t readerId, const char * serialNumber) {
    bus.flush();
    RfidRequest request = RfidRequest(readerId, operation, serialNumber);

    if (request.isValid()) {
        size_t requestSize = request.toWire(buffer, BUFFER_SIZE);

        setRs485TransferMode();
        bus.write(buffer, requestSize);
        waitForTransferFinish(operation);
        setRs485ReceiveMode();

        #ifdef USE_ASYNC_IO
        taskManager.yieldForMicros(100000);
        #else
        delay(100);
        #endif

        if (!bus.available()) {
            return RfidResponse::invalidResponse();
        }

        size_t responseSize = bus.available();
        bus.readBytes(buffer, responseSize);
        RfidResponse response = RfidResponse::fromWire(buffer, responseSize);

        return response;
    }

    return RfidResponse::invalidResponse();
}