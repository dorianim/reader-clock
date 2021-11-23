#include "DisplayController.h"
#include "quotes.h"

// GDEW027W3 2.7" b/w
// https://www.waveshare.com/product/displays/e-paper/2.7inch-e-paper-hat.htm

DisplayController* displayController;

void setup()
{
  Serial.begin(115200);
  Serial.println("Lets goooo");
  displayController = new DisplayController();
}

void loop() {
  Serial.println("Updating display ...");
  displayController->showQuote(&quotes[0]);
  delay(500000);
};