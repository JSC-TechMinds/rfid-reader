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
    #ifdef USE_ASYNC_IO
    digitalWrite(ioPin, HIGH);
    taskManager.yieldForMicros(6000);
    #else
    digitalWrite(ioPin, HIGH);
    delay(5);
    #endif
}
        
void RfidReader::setRs485ReceiveMode() {
    // Activate DE pin
    #ifdef USE_ASYNC_IO
    taskManager.yieldForMicros(6000);
    digitalWrite(ioPin, LOW);
    #else
    delay(5);
    digitalWrite(ioPin, LOW);
    #endif
}

RfidResponse RfidReader::requestData(RfidPacket::Function operation, int8_t readerId, const char * serialNumber) {
    bus.flush();
    RfidRequest request = RfidRequest(readerId, operation, serialNumber);

    if (request.isValid()) {
        size_t requestSize = request.toWire(buffer, BUFFER_SIZE);

        setRs485TransferMode();
        bus.write(buffer, requestSize);
        setRs485ReceiveMode();

        for (int i=0; i<NUM_RETRIES; i++) {
            if (bus.available()) {
                break;
            }

            #ifdef USE_ASYNC_IO
            taskManager.yieldForMicros(100000);
            #else
            delay(100);
            #endif
        }

        if (!bus.available()) {
            Log.errorln(F("Request timed out!"));
            return RfidResponse::invalidResponse();
        }

        size_t responseSize = bus.available();
        bus.readBytes(buffer, responseSize);
        RfidResponse response = RfidResponse::fromWire(buffer, responseSize);

        return response;
    }

    return RfidResponse::invalidResponse();
}