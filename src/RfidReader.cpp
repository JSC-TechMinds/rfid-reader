#include <ArduinoLog.h>
#include "RfidReader.h"

RfidReader::RfidReader(Stream& bus, uint8_t rePin): bus(bus), rePin(rePin) {
    pinMode(rePin, OUTPUT);
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

uint8_t RfidReader::getReaderId(String serialNumber) {
    return 1;
}

bool RfidReader::setReaderId(uint8_t id, String serialNumber) {
    return true;
}

const char * RfidReader::readCardData(uint8_t reader_id) {
    return "";
}

const char * RfidReader::reReadCardData(uint8_t reader_id) {
    return "";
}

void RfidReader::setRs485TransferMode() {
    digitalWrite(rePin, HIGH);
    delay(10);
}
        
void RfidReader::setRs485ReceiveMode() {
    digitalWrite(rePin, LOW);
}

RfidResponse RfidReader::requestData(RfidPacket::Function operation, int8_t readerId = -1, const char * serialNumber = "") {
    bus.flush();
    setRs485TransferMode();
    RfidRequest request = RfidRequest(readerId, operation, serialNumber);
    size_t requestSize = request.toWire(buffer, BUFFER_SIZE);
    bus.write(buffer, requestSize);
    delay(5);
    setRs485ReceiveMode();

    while(!bus.available()) {
        delay(100);
    }

    size_t responseSize = bus.available();
    bus.readBytes(buffer, responseSize);
    RfidResponse response = RfidResponse::fromWire(buffer, responseSize);

    return response;
}