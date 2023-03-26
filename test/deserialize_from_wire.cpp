#include <ArduinoUnitTests.h>
#include "../src/packet.h"

unittest(read_serial_number) {
    uint8_t responseData[] = { 0x0A, 'A', '1', 'B', '9', '9', '0', '8', '0', '0', '0', '1', '3', 'B', 0x0D };

    RfidResponse response = RfidResponse::fromWire(responseData, 15);
}

unittest_main()