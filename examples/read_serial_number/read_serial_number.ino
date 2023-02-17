#include <Arduino.h>

#define RO_PIN 5
#define DI_PIN 19
#define RE_PIN 18

void init_rfid_reader();
void test_rfid_reader();

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing reader.");
  init_rfid_reader();

}

void loop() {
  // put your main code here, to run repeatedly:
  test_rfid_reader();
  sleep(10);

}

void init_rfid_reader() {
  Serial2.begin(19200, SERIAL_8E1, RO_PIN, DI_PIN);
  pinMode(RE_PIN, OUTPUT);
  digitalWrite(RE_PIN, LOW);
}

void test_rfid_reader() {
  int rxlen = 0;
  int rlen = 0;
  char buf[64];

  // command to get reader serial number
  byte txBuf[8] = {0x09, 0x41, 0x32, 0x42, 0x33, 0x38, 0x0D };

  while (rxlen == 0 ) {
    digitalWrite(RE_PIN, HIGH);
    delay(10);
    Serial2.write(txBuf, 7);
    delay(5); 
    #warning ("Timing for RE_PIN is critical.")
    digitalWrite(RE_PIN, LOW);
          
    rxlen = Serial2.available();
    Serial.print("rxlen: "); Serial.println(rxlen);
    delay(1000);
  }
    
  if (rxlen > 64) {
    rlen = 64;   
  } else {
    rlen = rxlen;
  }
    
  rlen = Serial2.readBytes(buf, rlen);
  Serial.print("Serial number: ");
  Serial.println(buf);
}
