#ifndef RS485_RFID_READER_H
#define RS485_RFID_READER_H

#include <Arduino.h>
#include "packet.h"

class RfidReader {
    private:
        void setRs485ReceiveMode();
        void setRs485TransferMode();
        RfidResponse requestData(RfidPacket::Function operation, int8_t readerId, const char * serialNumber);

        static constexpr const uint8_t BUFFER_SIZE = 64; // bytes

        uint8_t buffer[BUFFER_SIZE];
        Stream& bus;
        uint8_t rePin;

    public:
        explicit RfidReader(Stream& bus, uint8_t rePin);

        /**
         * @brief Get the reader serial number.
         * 
         * This method reads the card number within the reader factory, a total of eight digits.
         * Section 1 to 2 for the year of manufacture, the first 3 to 4 means weeks,
         * the 5 to 8 digits are sequence number, 0001 to 9999.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return String reader serial number.
         */
        const char * getSerialNumber(uint8_t readerId);

        /**
         * @brief Get the Reader ID.
         * 
         * Read the Reader ID using the reader factory serial number.
         * 
         * @param serialNumber Reader factory serial number
         * @return uint8_t Reader ID, value <1-8>.
         */
        uint8_t getReaderId(String serialNumber);

        /**
         * @brief Set the Reader ID.
         * 
         * This method sets the Reader ID using the reader factory serial number.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @param serialNumber Serial number of the reader we want to configure.
         * @return true if the operation was successful.
         * @return false if the operation has failed.
         */
        bool setReaderId(uint8_t readerId, String serialNumber);

        /**
         * @brief Read the RFID card data.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return String 
         */
        const char * readCardData(uint8_t readerId);

        /**
         * @brief Re-read the RFID card data.
         * 
         * This method is to re-read the card reader to read the card information.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return String 
         */
        const char * reReadCardData(uint8_t readerId);
};

#endif // RS485_RFID_READER_H