// This example uses asynchronous API using TaskManagerIO.
// It continuosly polls for data and detects new tags.
// Once a tag is detected, it prints new tag to the screen.
// Note: In order to make this work, you need to compile the
// library with the folowing flag.

#define USE_ASYNC_IO

#include <Arduino.h>
#include <ArduinoLog.h>
#include <TaskManager.h>
#include "RfidReader.h"

#define RO_PIN 5
#define DI_PIN 19
#define RE_PIN 18

char cardData[10] = {'\0'};
RfidReader reader = RfidReader(Serial2, RE_PIN);

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    delay(1000);
    Log.noticeln("Reading RFID tags...");
    Serial2.begin(19200, SERIAL_8E1, RO_PIN, DI_PIN);
}

void loop() {
    taskManager.runLoop();

    const char * newTag = reader.readCardData(1);

    if (strlen(newTag) > 0) {
        Log.noticeln("Received a new tag: %s", newTag);
    }
}