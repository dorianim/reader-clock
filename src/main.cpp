#include "Arduino.h"
#include "ReaderClock.h"

ReaderClock* readerClock;

void setup()
{
  Serial.begin(115200);
  Serial.println("Lets goooo");
  readerClock = new ReaderClock();
}

void loop() {
  readerClock->loop();
};