#ifndef RS485_RFID_READER_H
#define RS485_RFID_READER_H

// Use async non-blocking mode.
// Requires TaskManagerIO as a dependency.
// #define USE_ASYNC_IO

#ifdef USE_ASYNC_IO
#include <TaskManagerIO.h>
#endif

#include <Arduino.h>
#include "packet.h"

class RfidReader {
    private:
        void setRs485ReceiveMode();
        void setRs485TransferMode();
        void waitForTransferFinish(RfidPacket::Function operation);
        RfidResponse requestData(RfidPacket::Function operation, int8_t readerId, const char * serialNumber);

        static constexpr const uint8_t BUFFER_SIZE = 64; // bytes
        uint8_t buffer[BUFFER_SIZE];
        Stream& bus;

        // enabled = RE pin active, disabled = DE pin active
        // https://www.cuidevices.com/blog/rs-485-serial-interface-explained
        uint8_t ioPin;

    public:
        explicit RfidReader(Stream& bus, uint8_t ioPin);

        /**
         * @brief Get the reader serial number.
         * 
         * This method reads the card number within the reader factory, a total of eight digits.
         * Section 1 to 2 for the year of manufacture, the first 3 to 4 means weeks,
         * the 5 to 8 digits are sequence number, 0001 to 9999.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return const char * reader serial number.
         */
        const char * getSerialNumber(uint8_t readerId);

        /**
         * @brief Get the Reader ID.
         * 
         * Read the Reader ID using the reader factory serial number.
         * 
         * @param serialNumber Reader factory serial number
         * @return int8_t Reader ID, value <1-8> or -1 if the call failed.
         */
        int8_t getReaderId(const char * serialNumber);

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
        bool setReaderId(uint8_t readerId, const char * serialNumber);

        /**
         * @brief Read the RFID card data.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return const char * card data. 
         */
        const char * readCardData(uint8_t readerId);

        /**
         * @brief Re-read the RFID card data.
         * 
         * This method is to re-read the card reader to read the card information.
         * 
         * @param readerId ID of the reader. It can be any number in range <1, 8>.
         * @return const char * card data. 
         */
        const char * reReadCardData(uint8_t readerId);
};

#endif // RS485_RFID_READER_H