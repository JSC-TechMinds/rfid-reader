#include "RfidReader.h"

RfidReader::RfidReader() {
}

const char * RfidReader::getSerialNumber() {
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