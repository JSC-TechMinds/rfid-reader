#ifndef RS485_RFID_READER_H
#define RS485_RFID_READER_H

#include <Arduino.h>

#ifndef RS485_BUFFER_SIZE
#define RS485_BUFFER_SIZE 64
#endif

class RfidReader {
    public:
        explicit RfidReader();

        /**
         * @brief Get the reader serial number.
         * 
         * This method reads the card number within the reader factory, a total of eight digits.
         * Section 1 to 2 for the year of manufacture, the first 3 to 4 means weeks,
         * the 5 to 8 digits are sequence number, 0001 to 9999.
         * 
         * @return String reader serial number.
         */
        const char * getSerialNumber();

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
         * @param reader_id ID of the reader. It can be any number in range <1, 8>.
         * @param serialNumber Serial number of the reader we want to configure.
         * @return true if the operation was successful.
         * @return false if the operation has failed.
         */
        bool setReaderId(uint8_t reader_id, String serialNumber);

        /**
         * @brief Read the RFID card data.
         * 
         * @param reader_id ID of the reader. It can be any number in range <1, 8>.
         * @return String 
         */
        const char * readCardData(uint8_t reader_id);

        /**
         * @brief Re-read the RFID card data.
         * 
         * This method is to re-read the card reader to read the card information.
         * 
         * @param reader_id ID of the reader. It can be any number in range <1, 8>.
         * @return String 
         */
        const char * reReadCardData(uint8_t reader_id);

    private:
        uint8_t rxBuffer[RS485_BUFFER_SIZE];
        uint8_t txBuffer[RS485_BUFFER_SIZE];
};

#endif // RS485_RFID_READER_H