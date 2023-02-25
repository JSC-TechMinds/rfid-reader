#include "RfidReader.h"

RfidReader::RfidReader() {

}

String RfidReader::getSerialNumber() {
    return String("");
}

uint8_t RfidReader::getReaderId(String serialNumber) {
    return 1;
}

bool RfidReader::setReaderId(uint8_t id, String serialNumber) {
    return true;
}

String RfidReader::readCardData(uint8_t reader_id) {
    return String("");
}

String RfidReader::reReadCardData(uint8_t reader_id) {
    return String("");
}