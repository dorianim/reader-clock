#include "DisplayController.h"

DisplayController::DisplayController(): _quoteFont{&FreeSerif9pt7b}, _boldFont{&FreeSerifBold12pt7b} {
    // GDEW027W3 2.7" b/w
    // https://www.waveshare.com/product/displays/e-paper/2.7inch-e-paper-hat.htm

    #ifdef USE_GXEPD2
    this->_display = new GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT>(GxEPD2_270(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
    #else
        GxIO_Class* io = new GxIO_Class(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
        this->_display = new GxEPD_Class(*io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
    #endif
    this->_display->init();
    Serial.println("Init done");
    this->_display->fillScreen(GxEPD_WHITE);
    #ifdef USE_GXEPD2
    this->_display->display();
    #else
    this->_display->update();
    #endif
    Serial.println("Clearing done");
}

void DisplayController::showQuote(Quote* quote) {

  uint16_t box_x = 0;
  uint16_t box_y = 0;
  #ifdef USE_GXEPD2
  uint16_t box_w = GxEPD2_270::HEIGHT;
  uint16_t box_h = GxEPD2_270::WIDTH;
  #else
  uint16_t box_w = this->_display->width();
  uint16_t box_h = this->_display->height();
  #endif
  uint16_t cursor_y = box_y;

  this->_display->setRotation(45);
  this->_display->setFont(_quoteFont);
  this->_display->setTextColor(GxEPD_BLACK);

  this->_display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  this->_display->setCursor(box_x, cursor_y+15);
  this->_writeTextWithLinebraksAtSpaces(quote->textBeforeTime);
  this->_display->setFont(_boldFont);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_writeTextWithLinebraksAtSpaces(quote->timeText);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_display->setFont(_quoteFont);
  this->_writeTextWithLinebraksAtSpaces(quote->textAfterTime); 
  //this->display->updateWindow(box_x, box_y, box_w, box_h, true);
  #ifdef USE_GXEPD2
  this->_display->display();
  #else
  this->_display->update();
  #endif
}

void DisplayController::_writeTextWithLinebraksAtSpaces(const char* text) {

  //int16_t boxTopLeftX = 0;
  //int16_t boxToLeftY = 15;
  uint16_t boxWidth = this->_display->width();
  //uint16_t boxHeight = this->_display->height();

  char* saveptr;
  char* result = strdup("");
  char* tmpString = strdup(text);

  while (1) {
    result = strtok_r(tmpString, " ", &saveptr);
    delete tmpString;
    tmpString = NULL;
    if(result == NULL) break;

    int16_t currentX = this->_display->getCursorX();
    int16_t currentY = this->_display->getCursorY();
    int16_t x, y;
    uint16_t w, h;
    this->_display->getTextBounds(result, currentX, currentY, &x, &y, &w, &h);
    
    if(currentX + w > boxWidth) {
      this->_display->println();
    }

    this->_display->print(result);
    this->_display->print(" ");
  }

}