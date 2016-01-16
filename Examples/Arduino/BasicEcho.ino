#include <SoftwareSerial.h>

#define ARDUINO
#include "cModemStream.h"
#include "cModemInterface.h"
#include "cLTE.h"
#include "cSMS.h"

struct ModemStream* modem;
struct ModemInterface* interface;
SoftwareSerial* mySerial;

void setup() {
  Serial.begin(9600);
  mySerial = new SoftwareSerial(10,11);
  mySerial->begin(9600);
  interface = mmCreate(mySerial);
}

void loop() {
  while (mmAvailable(interface->modem)) {
    Serial.write(mmRead(interface->modem));
  }
  while (Serial.available()) {
    mmWriteByte(interface->modem,Serial.read());
  }
}
